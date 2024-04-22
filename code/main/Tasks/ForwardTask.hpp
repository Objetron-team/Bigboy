#pragma once

#include "BasicTask.hpp"

#define DISTANCE_THRESHOLD 2 // cm
#define TTL_FACTOR 300

class ForwardTask : public BasicTask
{

private:
    DriveControler *driveControler;
    ValueConverter *valueConverter;

    double distance_to_target_cm = 0;
    double distance_to_target_pulse = 0;

    void _Update() override
    {
        driveControler->SetDistance(distance_to_target_pulse);
        driveControler->SetAngle(0);
    }

    bool _IsDone() override
    {

        double current_distance_pulse = driveControler->GetDistance();
        double distance_error_pulse = (distance_to_target_pulse)-current_distance_pulse;

        double distance_error_cm = valueConverter->PulseToDistanceCM(distance_error_pulse);

        return (abs(distance_error_cm) < DISTANCE_THRESHOLD);
    }

    void _Debug() override
    {
        Serial.print("TASK:");
        Serial.print("ForwardTask");

        double current_distance_pulse = driveControler->GetDistance();
        double distance_error_pulse = (distance_to_target_pulse)-current_distance_pulse;

        double distance_error_cm = valueConverter->PulseToDistanceCM(distance_error_pulse);

        Serial.print("Distance_error_cm:");
        Serial.print(distance_error_cm);
        Serial.print(",");

        Serial.print("Current_distance_pulse:");
        Serial.print(current_distance_pulse);
        Serial.print(",");

        Serial.print("Target_distance_pulse:");
        Serial.println(distance_to_target_pulse);
    }

    void _Compute() override
    {
        time_to_live = (distance_to_target_pulse / (MAX_SPEED_PULSE)) * TTL_FACTOR;
    }

public:
    ForwardTask(DriveControler *driveControler, ValueConverter *valueConverter, double distance_cm)
    {
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;

        distance_to_target_cm = distance_cm;
        distance_to_target_pulse = valueConverter->DistanceCMToPulse(distance_to_target_cm);
    }
};