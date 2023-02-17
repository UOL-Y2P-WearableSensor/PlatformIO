//
// Created by 沈逸潇 on 15/02/2023.
//

#include "socket.h"
#include <SPI.h>
#include <WiFiNINA.h>
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