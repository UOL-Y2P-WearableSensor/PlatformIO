//
// Created by 沈逸潇 on 10/02/2023.
//

#include "Sensor.h"

#include "../lib/DFRobot_I2C_Multiplexer-main/DFRobot_I2C_Multiplexer.h"
#include <Arduino_LSM6DS3.h>
#include <Wire.h>

#define MAX_CALIBRATING_NUM 500
#define IMU_onboard IMU_LSM6DS3
ICM20600 IMU_external(true);
//AK09918 ak09918;
//AK09918_err_type_t err;
DFRobot_I2C_Multiplexer I2CMulti(&Wire, 0x70);

void Sensor::init() {
    if (!IMU_LSM6DS3.begin()) {
        Serial.println("Failed to initialize IMU onboard!");
        while (true);
    }
    Serial.println("initialization of sensor class");
    Serial.print("onboardIMU acceleration sampleRate = ");
    Serial.println(IMU_onboard.accelerationSampleRate());
    Serial.print("onboardIMU gyroscope sampleRate = ");
    Serial.println(IMU_onboard.gyroscopeSampleRate());
    calibrating_onboard();

    I2CMulti.begin();
    for (int i = 0; i < 8; i++) {
        if (i == IMU_LEFT_FEMUR
            || i == IMU_RIGHT_FEMUR
            || i == IMU_LEFT_TIBIA
            || i == IMU_RIGHT_TIBIA) {
            I2CMulti.selectPort(i);
            IMU_external.initialize();
            IMU_external_data[i].filter.begin(100);
            delay(100);
        }
    }
    calibrating_external();

}


void Sensor::calibrating_onboard() {
    Serial.print("IMU_LSM6DS3 GYRO calibration, ETA=");
    Serial.print(CALI_TIME_MS);
    Serial.println("ms");

    int num = 0;
    memset(IMU_onboard_data.gyro_cali, 0, 3);
    unsigned long start_time = millis();
    while (millis() <= start_time + CALI_TIME_MS) {
        if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;

        num++;
        memset(IMU_onboard_data.gyro, 0, 3);
        IMU_onboard.readGyroscope(IMU_onboard_data.gyro[0],
                                  IMU_onboard_data.gyro[1],
                                  IMU_onboard_data.gyro[2]);
        for (int i = 0; i < 3; ++i) {
            IMU_onboard_data.gyro_cali[i] -= IMU_onboard_data.gyro[i];
        }
    }
    for (int i = 0; i < 3; ++i) {
        IMU_onboard_data.gyro_cali[i] = IMU_onboard_data.gyro_cali[i] / num;
    }
    Serial.print("IMU_onboard_data.gyro_cali\n");
    Serial.print("\t\t[x]=");
    Serial.println(IMU_onboard_data.gyro_cali[0]);
    Serial.print("\t\t[y]=");
    Serial.println(IMU_onboard_data.gyro_cali[1]);
    Serial.print("\t\t[z]=");
    Serial.println(IMU_onboard_data.gyro_cali[2]);
}

void Sensor::calibrating_external() {

    Serial.print("IMU_external GYRO calibration, ETA=");
    Serial.print(CALI_TIME_MS);
    Serial.println("ms");

    unsigned long start_time = millis();
    int num = 0;

    for (int port = 0; port < 8; ++port) {
        memset(IMU_external_data[port].gyro_cali, 0, 3);
    }

    while (millis() <= start_time + CALI_TIME_MS) {
        num++;

        for (int port = 0; port < 8; port++) {
            if (port == IMU_LEFT_FEMUR
                || port == IMU_RIGHT_FEMUR
                || port == IMU_LEFT_TIBIA
                || port == IMU_RIGHT_TIBIA) {
                I2CMulti.selectPort(port);
                memset(IMU_external_data[port].gyro, 0, 3);
                IMU_external.getGyroscope(&IMU_external_data[port].gyro[0],
                                          &IMU_external_data[port].gyro[1],
                                          &IMU_external_data[port].gyro[2]);
                for (int i = 0; i < 3; ++i) {
                    IMU_external_data[port].gyro_cali[i] -= (IMU_external_data[port].gyro[i]);
                }
            }
        }
    }
    Serial.print("cali data num: ");
    Serial.println(num);
    for (int port = 0; port < 8; port++) {
        if (port == IMU_LEFT_FEMUR
            || port == IMU_RIGHT_FEMUR
            || port == IMU_LEFT_TIBIA
            || port == IMU_RIGHT_TIBIA) {
            for (int i = 0; i < 3; ++i) {
                IMU_external_data[port].gyro_cali[i] = IMU_external_data[port].gyro_cali[i] / num;
            }

            Serial.print("IMU_external[");
            Serial.print(port);
            Serial.println("].gyro_cali");
            Serial.print("\t\t[x]=");
            Serial.println(IMU_external_data[port].gyro_cali[0]);
            Serial.print("\t\t[y]=");
            Serial.println(IMU_external_data[port].gyro_cali[1]);
            Serial.print("\t\t[z]=");
            Serial.println(IMU_external_data[port].gyro_cali[2]);

        }
    }



}

void Sensor::get_const_data(char *buffer) {
    strcat(buffer, "Hello server, this is Arduino-Uno-Wifi-Rev2 speaking");
}


void Sensor::get_onboard_data(Euler_angle *input) {
    while (true) {
        if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;

        IMU_onboard.readGyroscope(IMU_onboard_data.gyro[0], IMU_onboard_data.gyro[1], IMU_onboard_data.gyro[2]);
        IMU_onboard.readAcceleration(IMU_onboard_data.acc[0],IMU_onboard_data.acc[1],IMU_onboard_data.acc[2]);

        IMU_onboard_data.filter.updateIMU(
                (float) IMU_onboard_data.gyro[0] + IMU_onboard_data.gyro_cali[0],
                (float) IMU_onboard_data.gyro[1] + IMU_onboard_data.gyro_cali[1],
                (float) IMU_onboard_data.gyro[2] + IMU_onboard_data.gyro_cali[2],
                IMU_onboard_data.acc[0],
                IMU_onboard_data.acc[1],
                IMU_onboard_data.acc[2]);

        input->pitch = IMU_onboard_data.filter.getPitch();
        input->yaw = IMU_onboard_data.filter.getYaw();
        input->roll = IMU_onboard_data.filter.getRoll();


        break;

    }
}


void Sensor::get_single_data(Euler_angle *input, int idx) {
    I2CMulti.selectPort(idx);
    memset(IMU_external_data[idx].gyro, 0, 3);
    memset(IMU_external_data[idx].acc, 0, 3);


    IMU_external.getGyroscope(&IMU_external_data[idx].gyro[0],
                              &IMU_external_data[idx].gyro[1],
                              &IMU_external_data[idx].gyro[2]);

    IMU_external.getAcceleration(&IMU_external_data[idx].acc[0],
                                 &IMU_external_data[idx].acc[1],
                                 &IMU_external_data[idx].acc[2]);


    IMU_external_data[idx].filter.updateIMU(
            (float) IMU_external_data[idx].gyro[0] + IMU_external_data[idx].gyro_cali[0],
            (float) IMU_external_data[idx].gyro[1] + IMU_external_data[idx].gyro_cali[1],
            (float) IMU_external_data[idx].gyro[2] + IMU_external_data[idx].gyro_cali[2],
            IMU_external_data[idx].acc[0],
            IMU_external_data[idx].acc[1],
            IMU_external_data[idx].acc[2]);

    input->pitch = IMU_external_data[idx].filter.getPitch();
    input->yaw = IMU_external_data[idx].filter.getYaw();
    input->roll = IMU_external_data[idx].filter.getRoll();

    I2CMulti.selectPort(8);
}

//void Sensor::phase2char_6dof(char *input, int idx) {
//    char buff[100];
//    for (int i: IMU_external_data[idx].gyro) {
//        memset(buff, 0, sizeof buff / sizeof(char));
//        itoa(i, buff, 10);
//        strcat(buff, " ");
//        strcat(input, buff);
//    }
//    for (int i: IMU_external_data[idx].acc) {
//        memset(buff, 0, sizeof buff / sizeof(char));
//        itoa(i, buff, 10);
//        strcat(buff, " ");
//        strcat(input, buff);
//    }
//}
//
//void Sensor::phase2char_Euler(char *input, int port) {
//    char buff[100];
//
//    memset(buff, 0, sizeof buff / sizeof(char));
//    dtostrf(IMU_external_data[port].filter.getRoll(), 8, 3, buff);
//    strcat(input, buff);
//    strcat(input, " ");
//
//    memset(buff, 0, sizeof buff / sizeof(char));
//    dtostrf(IMU_external_data[port].filter.getPitch(), 8, 3, buff);
//    strcat(input, buff);
//    strcat(input, " ");
//
//    memset(buff, 0, sizeof buff / sizeof(char));
//    dtostrf(IMU_external_data[port].filter.getYaw(), 8, 3, buff);
//    strcat(input, buff);
//
//}

Sensor::Sensor() = default;



