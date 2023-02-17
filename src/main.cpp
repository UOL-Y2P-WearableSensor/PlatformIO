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

//# define SECRET_SSID "OpenWrt"
# define SECRET_SSID "CastleBlack"
# define SECRET_PASS "123123123"
#define PORT 3490
#define MSG_LENGTH 1024
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
IPAddress server(192, 168, 1, 141);  // numeric IP for Google (no DNS)
WiFiClient client;
Sensor sensor;

const char Authentication_header[] = "Authentication-RpLE44NHZx7WUwuUJFQY hello server, this is exhausted Yixiao...\r\n";
char socket_buff[MSG_LENGTH];


unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 20; // delay between updates, in milliseconds
const unsigned long TIMEOUT = 2000; // delay between updates, in milliseconds
int time_idx = 0;


void setup() {
    Wire.begin();
    Serial.begin(9600);
    while (!Serial);
    sensor.init();


    Serial.println("welcome");

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }


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
    printWifiStatus();

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


// this method makes an HTTP connection to the server:
void sendRequest(char *content) {
    char tmp[30];
    //make the request
    memset(tmp, 0, sizeof tmp / sizeof(char));

    //add one header to indicate time
    if (time_idx == 1000) time_idx = 0;
    client.write("[time_idx = ");
    itoa(time_idx++, tmp, 10);
    client.write(tmp);
    client.write("]");


    //write the request
    client.write(content);
    client.write("\n");
}


void loop() {

    int i = 0;
    memset(socket_buff, 0, sizeof socket_buff / sizeof(char));

    while (client.connected()) {

        if (i == 5) {
            sendRequest(socket_buff);
            memset(socket_buff, 0, sizeof socket_buff / sizeof(char));
            i = 0;
        }

        if (millis() - lastConnectionTime >= postingInterval) {
            i++;
            sensor.get_multiplexer_data(socket_buff);
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
