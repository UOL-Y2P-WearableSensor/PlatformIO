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

namespace DigitalTwin {
    int Sensor::port_schedule[8] = {port0, port1,
                                    port2, port3,
                                    port4, port5,
                                    port6, port7};
    Sensor *Sensor::sensor_instance = nullptr;
    DFRobot_I2C_Multiplexer Sensor::I2CMulti(&Wire, 0x70);
    ICM20600 Sensor::icm20600(true);

    Sensor *Sensor::get_share_sensor_instance() {
        if (sensor_instance == nullptr)
            sensor_instance = new Sensor();
        return sensor_instance;
    }

    Sensor::Sensor() {
        Wire.begin();

        for (int i = 0; i < 8; i++) {
            if (this->port_schedule[i]) {
                I2CMulti.selectPort(i);
                icm20600.initialize();
                delay(100);
            }
        }

    }

    void Sensor::get_all_data(char* buffer) {
        for (int i = 0; i < 8; i++) {
            if (this->port_schedule[i]) {
                get_single_data(i, buffer);
                strcat(buffer, "\n");
            }
        }
    }

    void Sensor::get_single_data(int idx, char *buffer) {
        char tmp[100];
        int16_t data[6];


        for (int i = 0; i < 8; i++) {
            if (this->port_schedule[i] && idx == i) {

                memset(tmp, 0, sizeof tmp / sizeof(char));
                memset(data, 0, sizeof data / sizeof(int16_t));

                I2CMulti.selectPort(idx);
                icm20600.getGyroscope(&data[0], &data[1], &data[2]);
                icm20600.getAcceleration(&data[3], &data[4], &data[5]);
                sprintf(tmp, "port_id_%d[%d][%d][%d][%d][%d][%d]",
                        idx, data[0], data[1], data[2], data[3], data[4], data[5]
                );
                strcat(buffer, tmp);
            }
        }
    }
}

