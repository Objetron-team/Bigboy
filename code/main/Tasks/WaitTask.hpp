#pragma once

#include "BasicTask.hpp"

#define WAIT_TIME_MS 500
#define DO_STOP_ROBOT true

class WaitTask : public BasicTask
{

private:
    DriveControler *driveControler;

    long start_time_second = 0;

    bool _IsDone() override
    {
        return millis() - start_time_second > WAIT_TIME_MS;
    }

    void _Update() override
    {

        if (DO_STOP_ROBOT)
        {
            driveControler->UrgentStop();
        }
    }

    void _Debug() override
    {
        Serial.print("Start_time:");
        Serial.print(start_time_second);
        Serial.print(",");

        long current_time = millis();
        Serial.print("Current_time:");
        Serial.print(current_time);
        Serial.print(",");

        long error = current_time - start_time_second - WAIT_TIME_MS;

        Serial.print("Wait:");
        Serial.println(error);
    }

    void _Compute() override
    {
        start_time_second = millis();
    }

public:
    WaitTask(DriveControler *driveControler)
    {
        this->driveControler = driveControler;
    }
};