//
// Created by 沈逸潇 on 10/02/2023.
//

#ifndef ARDUINO_UNO_WIFI_REV2_SENSOR_H
#define ARDUINO_UNO_WIFI_REV2_SENSOR_H

#include "../lib/Seeed_ICM20600_AK09918-master/ICM20600.h"
#include <Wire.h>
#include "../lib/DFRobot_I2C_Multiplexer-main/DFRobot_I2C_Multiplexer.h"
#include <Arduino_LSM6DS3.h>

namespace DigitalTwin {

    class Sensor {

    public:
        Sensor();
        static Sensor *get_share_sensor_instance();
        static void get_multiplexer_data(char*);
        static void get_onboard_data(char*);
        static void get_const_data(char*);
        static void get_single_data(char*);
    private:
        static int port_schedule[];
        static DFRobot_I2C_Multiplexer I2CMulti;
        static Sensor *sensor_instance;
        static ICM20600 IMU_external;
        static char buff[];
        static int16_t data_externalIMU[];
        static float data_onboardIMU[];
        static float calibration[];
        static void calibrating();


    };

}


#define SENSOR DigitalTwin::Sensor::get_share_sensor_instance()
#endif //ARDUINO_UNO_WIFI_REV2_SENSOR_H
