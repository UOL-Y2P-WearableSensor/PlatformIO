//
// Created by 沈逸潇 on 10/02/2023.
//

#include "Sensor.h"


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

    void Sensor::get_all_data() {
        String output;
        for (int i = 0; i < 8; i++) {
//            output+=get_single_data(i);
        }


    }

    void Sensor::get_single_data(int idx, char * buffer) {
        char tmp[50];
        memset(tmp, 0, sizeof tmp/sizeof (char ));
        for (int i = 0; i < 8; i++) {
            if (this->port_schedule[i] && idx == i) {

                I2CMulti.selectPort(idx);
                sprintf(tmp, "port_id_%d-%d",
                        idx
                        ,icm20600.getAccelerationX()
                        );
                Serial.print(" x = ");
                Serial.println( icm20600.getAccelerationX() );

                strcat(buffer, tmp);
//                sprintf(buffer, "port_id_%d-%d-%d-%d-%d-%d-%d",
//                        idx
//                        icm20600.getGyroscopeX();
//                        ,icm20600.getGyroscopeY()
//                        ,icm20600.getGyroscopeZ()
//                        ,icm20600.getAccelerationX()
//                        ,icm20600.getAccelerationY()
//                        ,icm20600.getAccelerationZ()
//                        );
            }
        }
    }
}

