#pragma once

#include "ESPNowStruct.hpp";

#include <esp_now.h>
#include <WiFi.h>

// #include "TaskControler.hpp"
// #include "PositionTaskBuilder.hpp"

class ESPNowSlave
{
private:
public:
    static TaskControler *taskControler_esp;
    static PositionTaskBuilder *positionTaskBuilder_esp;

    ESPNowSlave(TaskControler *taskControler_, PositionTaskBuilder *positionTaskBuilder_)
    {
        taskControler_esp = taskControler_;
        positionTaskBuilder_esp = positionTaskBuilder_;
    }

    static void ProcessCMD(command cmd)
    {

        Serial.print("Processing command: ");
        Serial.println(cmd);

        switch (cmd)
        {
        case START:
            taskControler_esp->Start();
            break;
        case AUTO_ON:
            taskControler_esp->SetAutoMode(true);
            break;
        case AUTO_OFF:
            taskControler_esp->SetAutoMode(false);
            break;
        case YELLOW_ONE:
        {
            Point points[2] = {
                {0, 0},
                {10, 100},
            };

            BasicTask *task = positionTaskBuilder_esp->CreateTasksFromPoints(points, 2);

            taskControler_esp->AddTask(task);
            break;
        }
        case YELLOW_TWO:
        {
            Point points[3] = {
                {0, 0},
                {10, 0},
                {95, -160},
            };

            BasicTask *task = positionTaskBuilder_esp->CreateTasksFromPoints(points, 3);

            taskControler_esp->AddTask(task);
            break;
        }
        case BLUE_ONE:
        {
            Point points[2] = {
                {0, 0},
                {10, -100},
            };

            BasicTask *task = positionTaskBuilder_esp->CreateTasksFromPoints(points, 2);

            taskControler_esp->AddTask(task);
            break;
        }
        case BLUE_TWO:
        {
            Point points[3] = {
                {0, 0},
                {10, 0},
                {95, 160},
            };

            BasicTask *task = positionTaskBuilder_esp->CreateTasksFromPoints(points, 3);

            taskControler_esp->AddTask(task);
            break;
        }

        default:
            break;
        }
    }

    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
    {
        command_data myData;
        memcpy(&myData, incomingData, sizeof(myData));

        ProcessCMD(myData.cmd_1);
        ProcessCMD(myData.cmd_2);
        ProcessCMD(myData.cmd_3);
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

        Serial.println("ESPNowSlave initialized");
    }
};

TaskControler *ESPNowSlave::taskControler_esp = nullptr; // Initialize to nullptr
PositionTaskBuilder *ESPNowSlave::positionTaskBuilder_esp = nullptr;