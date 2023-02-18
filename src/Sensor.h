//
// Created by 沈逸潇 on 10/02/2023.
//

#ifndef ARDUINO_UNO_WIFI_REV2_SENSOR_H
#define ARDUINO_UNO_WIFI_REV2_SENSOR_H

#include "../lib/Seeed_ICM20600_AK09918-master/ICM20600.h"
#include "../lib/Seeed_ICM20600_AK09918-master/AK09918.h"
#include "../lib/MahonyAHRS-master/src/MahonyAHRS.h"
#include "main.h"

struct IMU_externalData{
    int16_t gyro[3];    //unit: dps
    int16_t acc[3];     //unit: mg
    int32_t mag[3];     //unit: uT
    float gyro_cali[3];
    Mahony filter;
};

struct IMU_onboardData{
    float gyro[3];
    float acc[3];
    float gyro_cali[3];
    Mahony filter;
};

    class Sensor {

    public:
        Sensor();

        void init();
        void get_multiplexer_data(char*);
        void get_onboard_data(char*);
        static void get_const_data(char*);
        void get_single_data(Euler_angle*, int);

    private:
        void calibrating_onboard();
        void calibrating_external();
        void phase2char_6dof(char*, int);
        void phase2char_Euler(char*, int);
        IMU_externalData IMU_external_data[8];
        IMU_onboardData IMU_onboard_data;

    };



#endif //ARDUINO_UNO_WIFI_REV2_SENSOR_H
