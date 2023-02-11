//
// Created by 沈逸潇 on 10/02/2023.
//

#ifndef ARDUINO_UNO_WIFI_REV2_SENSOR_H
#define ARDUINO_UNO_WIFI_REV2_SENSOR_H

#include "../lib/Seeed_ICM20600_AK09918-master/ICM20600.h"
#include <Wire.h>
#include "../lib/DFRobot_I2C_Multiplexer-main/DFRobot_I2C_Multiplexer.h"



namespace DigitalTwin {

    class Sensor {

    public:
        Sensor();
        static Sensor *get_share_sensor_instance();
        void get_all_data(char*);
        void get_single_data(int, char*);

    private:
        static int port_schedule[8];
        static DFRobot_I2C_Multiplexer I2CMulti;
        static Sensor *sensor_instance;
        static ICM20600 icm20600;
    };


}

#define SENSOR DigitalTwin::Sensor::get_share_sensor_instance()
#endif //ARDUINO_UNO_WIFI_REV2_SENSOR_H
