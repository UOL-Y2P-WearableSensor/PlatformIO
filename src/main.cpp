/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using the WiFi module.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and UNO WiFi Rev.2)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFiNINA.h>
#include "Sensor.h"
#include "socket.h"
#include "main.h"


char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
IPAddress server(192, 168, 1, 141);  // numeric IP for Google (no DNS)
WiFiClient client;
Sensor sensor;
Euler_angle eulerAngle;


long time_idx = 0;
const char Authentication_header[] = "Authentication-RpLE44NHZx7WUwuUJFQY hello server, this is exhausted Yixiao...\r\n";
unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1000 / FPS; // delay between updates, in milliseconds
const unsigned long TIMEOUT = 10000; // delay between updates, in milliseconds


void setup() {
    Wire.begin();
    Serial.begin(9600);
    while (!Serial) {};
    sensor.init();

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }

    //check the port number
    char check[300];
    sprintf(check,
            "Check port configuration:\n"
            "\t\tIMU_LEFT_FEMUR at Port.%d\n"
            "\t\tIMU_RIGHT_FEMUR at Port.%d\n"
            "\t\tIMU_LEFT_TIBIA at Port.%d\n"
            "\t\tIMU_RIGHT_TIBIA at Port.%d",
            IMU_LEFT_FEMUR, IMU_RIGHT_FEMUR, IMU_LEFT_TIBIA, IMU_RIGHT_TIBIA);
    Serial.println(check);

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 2 seconds for connection:
        delay(2000);
    }
    Serial.println("Connected to WiFi");


    while (true) {
        if (client.connect(server, PORT)) {
            //make the request
            client.write(Authentication_header);
            Serial.println("Connected to server");
            // note the time that the connection was made:
            lastConnectionTime = millis();
            break;
        }
        Serial.println("do you open the server? go & check it");
        delay(500);
    }

}


void loop() {

    int pose_per_request = 0;

    while (client.connected()) {

        if (pose_per_request == MAX_POSES_PER_REQUEST) {
            client.write("\n");
            pose_per_request = 0;
        }

        if (millis() - lastConnectionTime >= postingInterval) {
            pose_per_request++;
            client.write("[");
            client.write(String(time_idx++).c_str());
            client.write("]");
            client.write(" ");

            for (int i = 0; i < 8; ++i) {
                if (i == IMU_LEFT_FEMUR
                    || i == IMU_RIGHT_FEMUR
//                    || i == IMU_LEFT_TIBIA
//                    || i == IMU_RIGHT_TIBIA
                    ) {

                    sensor.get_single_data(&eulerAngle, i);
                    client.write(String(eulerAngle.yaw, 2).c_str());
                    client.write(" ");
                    client.write(String(eulerAngle.roll, 2).c_str());
                    client.write(" ");
                    client.write(String(eulerAngle.pitch, 2).c_str());
                    client.write(" ");

                    }
            }

            lastConnectionTime = millis();

        }

    }

    Serial.println("Disconnected! Try reconnection...(TIMEOUT set to be 2s)");
    client.stop();

    while (!client.connected()) {

        if (millis() - lastConnectionTime > TIMEOUT) {
            Serial.println("re-connected failed---TIMEOUT---into the dead loop, QWQ");
            while (true);
        }
        if (client.connect(server, PORT)) {
            //make the request
            client.write(Authentication_header);
            Serial.println("re-connected");

            // note the time that the connection was made:
            lastConnectionTime = millis();
            break;
        }
        delay(60);

    }


}
