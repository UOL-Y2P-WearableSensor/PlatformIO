//
// Created by 沈逸潇 on 10/02/2023.
//

#include "Sensor.h"

#define port0   ENABLE
#define port1   DISABLE
#define port2   ENABLE
#define port3   DISABLE
#define port4   ENABLE
#define port5   DISABLE
#define port6   ENABLE
#define port7   DISABLE
#define ENABLE 1
#define DISABLE 0

#define IMU_onboard IMU_LSM6DS3

namespace DigitalTwin {

    int Sensor::port_schedule[8] = {port0, port1,
                                    port2, port3,
                                    port4, port5,
                                    port6, port7};
    Sensor *Sensor::sensor_instance = nullptr;

    DFRobot_I2C_Multiplexer Sensor::I2CMulti(&Wire, 0x70);
    ICM20600 Sensor::IMU_external(true);
    char Sensor::buff[100];
    int16_t Sensor::data_externalIMU[6];
    float Sensor::data_onboardIMU[6];
    float Sensor::calibration[3] = { 0, 0, 0};

    Sensor *Sensor::get_share_sensor_instance() {
        if (sensor_instance == nullptr)
            sensor_instance = new Sensor();
        return sensor_instance;
    }

    Sensor::Sensor() {

        if (!IMU_LSM6DS3.begin()) {
            Serial.println("Failed to initialize IMU onboard!");
            while (1);
        }

        I2CMulti.begin();
        for (int i = 0; i < 8; i++) {
            if (DigitalTwin::Sensor::port_schedule[i]) {
                I2CMulti.selectPort(i);
                IMU_external.initialize();
                delay(100);
            }
        }
//        calibrating();
    }

/**
 * for onboard_IMU only
 */
    void Sensor::calibrating() {
        Serial.println("IMU_LSM6DS3 calibration, ETA=10s...");
        int idx = 0;
        int max_num = 600;
        while (idx < max_num) {
            if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;
            memset(data_onboardIMU, 0, sizeof data_onboardIMU / sizeof(float));
            idx++;
            IMU_onboard.readGyroscope(data_onboardIMU[0 + 3], data_onboardIMU[1 + 3], data_onboardIMU[2 + 3]);
            for (int i = 0; i < 3; ++i) {
                calibration[i] -= (data_onboardIMU[3 + i] / max_num);
            }
        }
        Serial.print("gyro_calibration[x]=");
        Serial.println(calibration[0]);
        Serial.print("gyro_calibration[y]=");
        Serial.println(calibration[1]);
        Serial.print("gyro_calibration[z]=");
        Serial.println(calibration[2]);

    }

    void Sensor::get_const_data(char *buffer) {
        strcat(buffer, "Hello server, this is Arduino-Uno-Wifi-Rev2 speaking");
    }


    void Sensor::get_onboard_data(char *buffer) {
        memset(data_onboardIMU, 0, sizeof data_onboardIMU / sizeof(float));

        while (true) {
            if (!(IMU_onboard.accelerationAvailable() && IMU_onboard.gyroscopeAvailable())) continue;

            IMU_onboard.readAcceleration(data_onboardIMU[0], data_onboardIMU[1], data_onboardIMU[2]);
            IMU_onboard.readGyroscope(data_onboardIMU[0 + 3], data_onboardIMU[1 + 3], data_onboardIMU[2 + 3]);

            //calibrating
            for (int i = 0; i < 3; ++i) {
                data_onboardIMU[3 + i]+=calibration[i];
            }

            strcat(buffer, "[onboard]");
            for (int i = 0; i < 6; ++i) {
                memset(buff, 0, sizeof buff / sizeof(char));
                dtostrf(data_onboardIMU[i], 8, 3, buff);
                strcat(buff, " ");
                strcat(buffer, buff);
            }
            break;

        }

    }


    void Sensor::get_multiplexer_data(char* buffer) {
        int max_port_num = 1;
        for (int i = 0; i < max_port_num; i++) {
            if (DigitalTwin::Sensor::port_schedule[i]) {
                I2CMulti.selectPort(i);
                get_single_data(buffer);
                strcat(buffer, " ");
            }
        }
    }

    void Sensor::get_single_data(char *input) {
        memset(data_externalIMU, 0, sizeof data_externalIMU / sizeof(int16_t));
        memset(buff, 0, sizeof buff / sizeof(char));

        IMU_external.getGyroscope(&data_externalIMU[0],
                              &data_externalIMU[1],
                              &data_externalIMU[2]);

        IMU_external.getAcceleration(&data_externalIMU[3],
                              &data_externalIMU[4],
                              &data_externalIMU[5]);

        sprintf(buff, "%d %d %d %d %d %d",
                data_externalIMU[0], data_externalIMU[1], data_externalIMU[2],
                data_externalIMU[3], data_externalIMU[4], data_externalIMU[5]
        );

        strcat(input, buff);
    }


}

