#pragma once

#include "ESPNowStruct.hpp";

#include <esp_now.h>
#include <WiFi.h>

class ESPNowSlave
{
private:
public:
    ESPNowSlave(/* args */) {}

    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
    {
        command_data myData;

        memcpy(&myData, incomingData, sizeof(myData));
        Serial.print("Bytes received: ");
        Serial.println(len);
        Serial.print("cmd_1 : ");
        Serial.println(myData.cmd_1);
        Serial.print("cmd_2 : ");
        Serial.println(myData.cmd_2);
        Serial.print("cmd_3 : ");
        Serial.println(myData.cmd_3);
    }

    void Init()
    {
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

        esp_now_register_recv_cb(OnDataRecv);

        Serial.println("ESPNowMaster initialized");
    }
};