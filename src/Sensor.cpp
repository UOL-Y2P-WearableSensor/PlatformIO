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
//    calibrating_onboard();

    I2CMulti.begin();
    for (int i = 0; i < 8; i++) {
        if (Sensor::port_schedule[i]) {
            I2CMulti.selectPort(i);
            IMU_external.initialize();
            IMU_external_data[i].filter.begin(100);
            calibrating_external(i);
            delay(100);
        }
    }

}


void Sensor::calibrating_onboard() {
    Serial.println("IMU_LSM6DS3 GYRO calibration, ETA=10s...");
    int idx = 0;
    memset(IMU_onboard_data.gyro_cali, 0, 3);
    while (idx < MAX_CALIBRATING_NUM) {
        if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;

        idx++;
        memset(IMU_onboard_data.gyro, 0, 3);
        IMU_onboard.readGyroscope(IMU_onboard_data.gyro[0],
                                  IMU_onboard_data.gyro[1],
                                  IMU_onboard_data.gyro[2]);
        for (int i = 0; i < 3; ++i) {
            IMU_onboard_data.gyro_cali[i] -= (IMU_onboard_data.gyro[i] / MAX_CALIBRATING_NUM);
        }
    }
    Serial.print("IMU_onboard_data.gyro_cali\n");
    Serial.print("\t[x]=");
    Serial.println(IMU_onboard_data.gyro_cali[0]);
    Serial.print("\t[y]=");
    Serial.println(IMU_onboard_data.gyro_cali[1]);
    Serial.print("\t[z]=");
    Serial.println(IMU_onboard_data.gyro_cali[2]);
}

void Sensor::calibrating_external(int port) {
    Serial.print("IMU_external[");
    Serial.print(port);
    Serial.println("].gyro_cali");

    int idx = 0;
    memset(IMU_external_data[port].gyro_cali, 0, 3);

    while (idx < MAX_CALIBRATING_NUM) {
        idx++;
        memset(IMU_external_data[port].gyro, 0, 3);
        IMU_external.getGyroscope(&IMU_external_data[port].gyro[0],
                                  &IMU_external_data[port].gyro[1],
                                  &IMU_external_data[port].gyro[2]);
        for (int i = 0; i < 3; ++i) {
            IMU_external_data[port].gyro_cali[i] -= (IMU_external_data[port].gyro[i] );
        }
    }
    for (int i = 0; i < 3; ++i) {
        IMU_external_data[port].gyro_cali[i] = IMU_external_data[port].gyro_cali[i] / MAX_CALIBRATING_NUM;
    }

    Serial.print("\t[x]=");
    Serial.println(IMU_external_data[port].gyro_cali[0]);
    Serial.print("\t[y]=");
    Serial.println(IMU_external_data[port].gyro_cali[1]);
    Serial.print("\t[z]=");
    Serial.println(IMU_external_data[port].gyro_cali[2]);


}

void Sensor::get_const_data(char *buffer) {
    strcat(buffer, "Hello server, this is Arduino-Uno-Wifi-Rev2 speaking");
}


//void Sensor::get_onboard_data(char *input) {
//    char buff[100];
//    memset(data_onboardIMU, 0, sizeof data_onboardIMU / sizeof(float));
//    while (true) {
//        if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;
//
//        IMU_onboard.readGyroscope(data_onboardIMU[0], data_onboardIMU[1], data_onboardIMU[2]);
//        IMU_onboard.readAcceleration(data_onboardIMU[3], data_onboardIMU[4], data_onboardIMU[5]);
//
////            //calibrating
////            for (int i = 0; i < 3; ++i) {
////                data_onboardIMU[i]+=onboardIMU_gyro_cali[i];
////            }
//
//        strcat(input, "[onboard]");
//        for (int i = 0; i < 6; ++i) {
//            memset(buff, 0, sizeof buff / sizeof(char));
//            dtostrf(data_onboardIMU[i], 8, 3, buff);
//            strcat(buff, " ");
//            strcat(input, buff);
//        }
//        break;
//
//    }
//}


void Sensor::get_multiplexer_data(char *buffer) {
    int max_port_num = 8;
    char buff[100];
    for (int i = 0; i < max_port_num; i++) {
        if (port_schedule[i]) {
            memset(buff, 0, sizeof buff/ sizeof(char));
            sprintf(buff, "[%d]", i);
            strcat(buffer, buff);
            get_single_data(buffer, i);
        }
    }

}



void Sensor::get_single_data(char *input, int idx) {

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
            (float)IMU_external_data[idx].gyro[0] + IMU_external_data[idx].gyro_cali[0],
            (float)IMU_external_data[idx].gyro[1] + IMU_external_data[idx].gyro_cali[1],
            (float)IMU_external_data[idx].gyro[2] + IMU_external_data[idx].gyro_cali[2],
            IMU_external_data[idx].acc[0],
            IMU_external_data[idx].acc[1],
            IMU_external_data[idx].acc[2]);

    phase2char_Euler(input, idx);
//    Serial.print("Roll: ");
//    Serial.print(IMU_external_data[idx].filter.getRoll());
//    Serial.print("\tPitch: ");
//    Serial.print(IMU_external_data[idx].filter.getPitch());
//    Serial.print("\tYaw: ");
//    Serial.println(IMU_external_data[idx].filter.getYaw());

    I2CMulti.selectPort(8);
}
void Sensor::phase2char_6dof(char *input, int idx) {
    char buff[100];
    for (int i: IMU_external_data[idx].gyro) {
        memset(buff, 0, sizeof buff / sizeof(char));
        itoa(i, buff, 10);
        strcat(buff, " ");
        strcat(input, buff);
    }
    for (int i: IMU_external_data[idx].acc) {
        memset(buff, 0, sizeof buff / sizeof(char));
        itoa(i, buff, 10);
        strcat(buff, " ");
        strcat(input, buff);
    }
}

void Sensor::phase2char_Euler(char * input, int port) {
    char buff[100];

    memset(buff, 0, sizeof buff/ sizeof(char ));
    dtostrf(IMU_external_data[port].filter.getRoll(), 8, 3, buff);
    strcat(input, buff);
    strcat(input, " ");

    memset(buff, 0, sizeof buff/ sizeof(char ));
    dtostrf(IMU_external_data[port].filter.getPitch(), 8, 3, buff);
    strcat(input, buff);
    strcat(input, " ");

    memset(buff, 0, sizeof buff/ sizeof(char ));
    dtostrf(IMU_external_data[port].filter.getYaw(), 8, 3, buff);
    strcat(input, buff);

}
Sensor::Sensor() = default;



