#pragma once

#include "BasicTask.hpp"


#define WAIT_TIME_MS 500
#define DO_STOP_ROBOT false

class WaitTask : public BasicTask{

private:
    PositionControler* positionControler;
    DriveControler* driveControler;
    ValueConverter* valueConverter;

    long start_time = 0;
public:

    WaitTask(PositionControler* positionControler,DriveControler* driveControler ,ValueConverter* valueConverter)  {
        this->positionControler = positionControler;
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;
    }

    void Update() override {

        if(DO_STOP_ROBOT){
            driveControler->UrgentStop();
        }

    }

    void Compute() override {
        start_time = millis();
    }

    bool IsDone() override {
        return millis() - start_time > WAIT_TIME_MS;
    }

    void Debug() override {
        Serial.print("Start_time:");
        Serial.print(start_time);
        Serial.print(",");

        long current_time = millis();
        Serial.print("Current_time:");
        Serial.print(current_time);
        Serial.print(",");

        long error = current_time - start_time - WAIT_TIME_MS;

        Serial.print("Wait:");
        Serial.println(error);

    }

};