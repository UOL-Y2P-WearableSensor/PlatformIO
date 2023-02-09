/*
  Arduino LSM6DS3 - Simple Gyroscope

  This example reads the gyroscope values from the LSM6DS3
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Uno Wi-Fi Rev 2 or Arduino Nano 33 IoT

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include <Arduino_LSM6DS3.h>
#include <UART.h>

void setup() {
    Serial.begin(67200);
    while (!Serial);

    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");

        while (1);
    }

    Serial.print("Gyroscope sample rate = ");
    Serial.print(IMU.gyroscopeSampleRate());
    Serial.print(" Hz\n");
    Serial.print("Accelerometer sample rate = ");
    Serial.print(IMU.accelerationSampleRate());
    Serial.print(" Hz\n\n");
    Serial.print("Idx   Gyroscope in degrees/second  Acceleration in g/second \n");
    Serial.print("\t\tX\tY\tZ\t\tX\tY\tZ\n");
}
int time_idx=0;

void output_format1(int time_idx, const float* data, size_t length){
    String output = StringSumHelper(time_idx);

    for (int i=0; i<length ; i++) {
        output += "\t"+ StringSumHelper(*data) ;
        data++;
    }
    output+="\n";
    Serial.print(output);
}

void output_format2(int time_idx, const float* data, size_t length){
    Serial.print(time_idx);
    for (int i=0; i<length ; i++) {
        Serial.print("\t");
        Serial.print(data[i]);
    }
    Serial.print("\n");
}





void loop() {

    if (time_idx >= 100){
        Serial.print("100 rounds over\n");
        time_idx=1;
    } else{
        time_idx++;
    }
    float data[6];
    memset(data, 0, sizeof(data));

    if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable() ) {
        IMU.readGyroscope(data[0], data[1], data[2]);
        IMU.readAcceleration(data[3], data[4], data[5]);
        output_format2(time_idx, data, sizeof data /sizeof (float ));
    }



}

