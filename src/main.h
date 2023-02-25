//
// Created by 沈逸潇 on 18/02/2023.
//

#ifndef ARDUINO_UNO_WIFI_REV2_MAIN_H
#define ARDUINO_UNO_WIFI_REV2_MAIN_H

//# define SECRET_SSID "OpenWrt"
# define SECRET_SSID "CastleBlack"
# define SECRET_PASS "123123123"

#define PORT 3490
#define MAX_POSES_PER_REQUEST 1
#define FPS 100
#define CALI_TIME_MS 3000
struct Euler_angle {
    float yaw;
    float pitch;
    float roll;
};
//const std::string name[5]={"L_F","R_F","L_T", "R_T", "OnBrd"};
//port number
enum Wearable_sensor {
    IMU_LEFT_FEMUR = 2,
    IMU_RIGHT_FEMUR = 6,
    IMU_LEFT_TIBIA = 5,
    IMU_RIGHT_TIBIA = 7
};

#endif //ARDUINO_UNO_WIFI_REV2_MAIN_H
