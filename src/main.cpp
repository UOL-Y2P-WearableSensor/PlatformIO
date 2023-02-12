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
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192, 168, 1, 141);  // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)
#define PORT 3490
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

const char Authentication_header[] = "Authentication-RpLE44NHZx7WUwuUJFQY hello server, this is exhausted Yixiao...\r\n";
const char PUT_header[] = "PUT /IMU_data.json HTTP/1.1\r\n";
const char ending[] = "ending\r\n\r\n";
char buffer[500];
char tmp[30];

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 50L; // delay between updates, in milliseconds
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
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
    }

    SENSOR;

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

    Serial.println("\nStarting connection to server...");

    if (client.connect(server, PORT)) {
        //make the request
        client.write(Authentication_header);

        // note the time that the connection was made:
        lastConnectionTime = millis();
    } else {
        // if you couldn't make a connection:
        Serial.println("initial connection failed");
        while (true);
    }
}


// this method makes an HTTP connection to the server:
void sendRequest(const char *header) {

    //make the request
    memset(buffer, 0, sizeof buffer / sizeof(char));
//    strcat(buffer, header);

    memset(tmp, 0, sizeof tmp / sizeof(char));
    sprintf(tmp, "time_idx = %d\n", time_idx++);
    strcat(buffer, tmp);

//    strcat(buffer, "123123123123123123123123321123231123312123123123231123123123132231231321312231232312312312312312312312312312332112323112331212312312323112312312313223123132131223123231231231231231231231231231233211232311233121231231232311231231231322312313213122312323123");
    SENSOR->get_all_data(buffer);
    Serial.println(buffer);

    //write the request
    client.write(buffer);

    // note the time that the connection was made:
    lastConnectionTime = millis();

}

void loop() {

    if (!client.connected()) {
        Serial.println("disconnect from server");
        client.stop();

        while (true);


    }


    while (client.connected()) {
        if (millis() - lastConnectionTime > postingInterval) {
            sendRequest(PUT_header);
        }
//        while (client.available()) {
//            char c = client.read();
//            Serial.write(c);
//        }
    }


}
