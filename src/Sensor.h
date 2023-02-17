//
// Created by 沈逸潇 on 10/02/2023.
//

#ifndef ARDUINO_UNO_WIFI_REV2_SENSOR_H
#define ARDUINO_UNO_WIFI_REV2_SENSOR_H

#include "../lib/Seeed_ICM20600_AK09918-master/ICM20600.h"
#include <Wire.h>
#include "../lib/DFRobot_I2C_Multiplexer-main/DFRobot_I2C_Multiplexer.h"
#include <Arduino_LSM6DS3.h>
#include <MahonyAHRS.h>

#define port0   ENABLE
#define port1   DISABLE
#define port2   DISABLE
#define port3   DISABLE
#define port4   DISABLE
#define port5   DISABLE
#define port6   DISABLE
#define port7   ENABLE
#define ENABLE 1
#define DISABLE 0



    class Sensor {

    public:
        Sensor();
        void init();
        void get_multiplexer_data(char*);
        void get_onboard_data(char*);
        static void get_const_data(char*);
        void get_single_data(char*, int);

    private:
        int port_schedule[8]= {port0, port1,
                               port2, port3,
                               port4, port5,
                               port6, port7};
        int16_t data_externalIMU[6];
        float data_onboardIMU[6];


    };



#endif //ARDUINO_UNO_WIFI_REV2_SENSOR_H
