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

# define SECRET_SSID "CastleBlack"
# define SECRET_PASS "123123123"
#define PORT 3490
#define MSG_LENGTH 300
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
IPAddress server(192, 168, 1, 141);  // numeric IP for Google (no DNS)
WiFiClient client;

const char Authentication_header[] = "Authentication-RpLE44NHZx7WUwuUJFQY hello server, this is exhausted Yixiao...\r\n";
const char PUT_header[] = "PUT /IMU_data.json HTTP/1.1\r\n";
const char ending[] = "ending\r\n\r\n";
char buffer[MSG_LENGTH];
char tmp[30];


unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 20; // delay between updates, in milliseconds
const unsigned long TIMEOUT = 2000; // delay between updates, in milliseconds
int time_idx = 0;

void sendRequest(const char *);

void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

void setup() {
    Wire.begin();
    Serial.begin(38400);
    while (!Serial){};

    SENSOR;

    Serial.println("hello");

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

    while (1){
        if (client.connect(server, PORT)) {
            //make the request
            client.write(Authentication_header);
            Serial.println("Connected to server");
            // note the time that the connection was made:
            lastConnectionTime = millis();
            break;
        }
        Serial.println("do you open the server? go, check it");
        delay(500);
    }



}


/**
 * fill the rest of buffer with '\0'
 * @param buffer
 * @param buffer_size
 */
void stable_length(char * buffer, int buffer_size, char filling){
    for (int i = 0; i < buffer_size; ++i) {
        if (buffer[i]=='\0'){
            for (int j = i; j < buffer_size; ++j) {
                buffer[j]=filling;
            }
            return;
        }
    }
}
char buffer_test[500];
void print_externalIMU_with_MUX(){
    memset(buffer_test, 0, sizeof buffer_test / sizeof(char));
    SENSOR->get_multiplexer_data(buffer_test);
    Serial.println(buffer_test);       //serial will block the speed
}

// this method makes an HTTP connection to the server:
void sendRequest(const char *header) {

    //make the request
    memset(buffer, 0, sizeof buffer / sizeof(char));

    memset(tmp, 0, sizeof tmp / sizeof(char));

    if (time_idx==1000) time_idx=0;
    itoa(time_idx++, tmp, 10);

    strcat(buffer, "[time_idx = ");
    strcat(buffer, tmp);
    strcat(buffer, "]");

//    SENSOR->get_const_data(buffer);   //const char array
    SENSOR->get_onboard_data(buffer);
    strcat(buffer, "\n");
    stable_length(buffer, MSG_LENGTH-1, '\0');

    //write the request
    client.write(buffer);
//    Serial.println(buffer);       //serial will block the speed

    // note the time that the connection was made:
    lastConnectionTime = millis();

}


void loop() {


    while (client.connected()) {
        if (millis() - lastConnectionTime > postingInterval) {
            sendRequest(PUT_header);
//            print_externalIMU_with_MUX();
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
