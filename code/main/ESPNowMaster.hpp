#pragma once

#include <vector>

#include "ESPNowStruct.hpp";

#include <esp_now.h>
#include <WiFi.h>

class ESPNowMaster
{
private:
    const int SEND_INTERVAL = 2000;
    const int WAIT_TIME = 90000;

    long start_time = 0;
    long last_send_time = 0;

    esp_now_peer_info_t peerInfo;

    // key is the slave mac address and value is the command data
    std::vector<slave_data> slaves_data;

    bool has_init_been_called = false;
    bool data_has_been_sent = false;

    void RegisterSlaves()
    {

        for (auto slave : slaves_data)
        {
            uint8_t *mac_address = slave.mac_address;

            memcpy(peerInfo.peer_addr, mac_address, 6);
            if (esp_now_add_peer(&peerInfo) != ESP_OK)
            {
                Serial.println("Failed to add peer");
                return;
            }
        }
    }

    void SendData()
    {
        for (auto slave : slaves_data)
        {
            esp_err_t result = esp_now_send(slave.mac_address, (uint8_t *)&slave.cmd_data, sizeof(slave.cmd_data));
            if (result != ESP_OK)
            {
                Serial.println("Failed to send data");
            }
        }
    }

public:
    ESPNowMaster(/* args */) {}

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        char macStr[18];
        Serial.print("Packet to: ");
        // Copies the sender mac address to a string
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        Serial.print(macStr);
        Serial.print(" send status:\t");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    }

    void AddSlave(slave_data slave)
    {
        if (has_init_been_called)
        {
            throw std::runtime_error("Cannot add slave after init has been called");
        }

        slaves_data.push_back(slave);
    }

    void Init()
    {
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

        esp_now_register_send_cb(OnDataSent);

        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        Serial.println("Registering slaves");

        RegisterSlaves();

        has_init_been_called = true;

        Serial.println("ESPNowMaster initialized");
    }

    void Start()
    {
        start_time = millis();
        last_send_time = millis();
    }

    void Update()
    {
        long current_time = millis();
        // don't send data if it's too soon or we just sent data
        if (current_time - last_send_time < SEND_INTERVAL || current_time - start_time < WAIT_TIME)
        {
            return;
        }

        // send the data one time
        if (!data_has_been_sent)
        {
            SendData();
            last_send_time = current_time;
            data_has_been_sent = true;
        }
    }
};
