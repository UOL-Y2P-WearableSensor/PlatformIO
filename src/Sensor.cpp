//
// Created by 沈逸潇 on 10/02/2023.
//

#include "Sensor.h"

#define IMU_onboard IMU_LSM6DS3

ICM20600 IMU_external(true);
DFRobot_I2C_Multiplexer I2CMulti(&Wire, 0x70);

void Sensor::init() {
    if (!IMU_LSM6DS3.begin()) {
        Serial.println("Failed to initialize IMU onboard!");
        while (true);
    }
    Serial.println("initialization of sensor class");

    I2CMulti.begin();
    for (int i = 0; i < 8; i++) {
        if (Sensor::port_schedule[i]) {
            I2CMulti.selectPort(i);
            IMU_external.initialize();
            delay(100);
        }
    }


}

///**
// * for onboard_IMU only
// */
//    void Sensor::calibrating() {
//        Serial.println("IMU_LSM6DS3 calibration, ETA=10s...");
//        int idx = 0;
//        int max_num = 600;
//        while (idx < max_num) {
//            if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;
//            memset(data_onboardIMU, 0, sizeof data_onboardIMU / sizeof(float));
//            idx++;
//            IMU_onboard.readGyroscope(data_onboardIMU[0 + 3], data_onboardIMU[1 + 3], data_onboardIMU[2 + 3]);
//            for (int i = 0; i < 3; ++i) {
//                calibration[i] -= (data_onboardIMU[3 + i] / max_num);
//            }
//        }
//        Serial.print("gyro_calibration[x]=");
//        Serial.println(calibration[0]);
//        Serial.print("gyro_calibration[y]=");
//        Serial.println(calibration[1]);
//        Serial.print("gyro_calibration[z]=");
//        Serial.println(calibration[2]);
//
//    }

void Sensor::get_const_data(char *buffer) {
    strcat(buffer, "Hello server, this is Arduino-Uno-Wifi-Rev2 speaking");
}


void Sensor::get_onboard_data(char *input) {
    char buff[100];
    memset(data_onboardIMU, 0, sizeof data_onboardIMU / sizeof(float));
    while (true) {
        if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;

        IMU_onboard.readAcceleration(data_onboardIMU[0], data_onboardIMU[1], data_onboardIMU[2]);
        IMU_onboard.readGyroscope(data_onboardIMU[0 + 3], data_onboardIMU[1 + 3], data_onboardIMU[2 + 3]);

//            //calibrating
//            for (int i = 0; i < 3; ++i) {
//                data_onboardIMU[3 + i]+=calibration[i];
//            }

        strcat(input, "[onboard]");
        for (int i = 0; i < 6; ++i) {
            memset(buff, 0, sizeof buff / sizeof(char));
            dtostrf(data_onboardIMU[i], 8, 3, buff);
            strcat(buff, " ");
            strcat(input, buff);
        }
        break;

    }

}


void Sensor::get_multiplexer_data(char *buffer) {
    int max_port_num = 8;
    for (int i = 0; i < max_port_num; i++) {
        if (port_schedule[i]) {
            I2CMulti.selectPort(i);
            get_single_data(buffer, i);
            strcat(buffer, "|");
        }
    }

}

void Sensor::get_single_data(char *input, int idx) {
    char buff[100];
    I2CMulti.selectPort(idx);
    memset(data_externalIMU, 0, sizeof data_externalIMU / sizeof(int16_t));

    IMU_external.getGyroscope(&data_externalIMU[0],
                              &data_externalIMU[1],
                              &data_externalIMU[2]);

    IMU_external.getAcceleration(&data_externalIMU[3],
                                 &data_externalIMU[4],
                                 &data_externalIMU[5]);

    for (int i = 0; i < 6; ++i) {
        memset(buff, 0, sizeof buff / sizeof(char));
        itoa(data_externalIMU[i], buff, 10);
        strcat(buff, " ");
        strcat(input, buff);
    }
    I2CMulti.selectPort(8);

}

Sensor::Sensor() {

}



