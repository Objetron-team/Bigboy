#pragma once

#include "BasicTask.hpp"

#define DISTANCE_THRESHOLD 5 // cm
#define TTL_FACTOR 300

class ReverseTask : public BasicTask
{

private:
    PositionControler *positionControler;
    DriveControler *driveControler;
    ValueConverter *valueConverter;

    double distance_to_target_cm = 0;
    double distance_to_target_pulse = 0;

    void _Update() override
    {
        driveControler->SetDistance(-distance_to_target_pulse);
        driveControler->SetAngle(0);
    }

    bool _IsDone() override
    {
        double current_distance_pulse = driveControler->GetDistance();
        double distance_error_pulse = distance_to_target_pulse - current_distance_pulse;

        double distance_error_cm = valueConverter->PulseToDistanceCM(distance_error_pulse);

        return (distance_error_cm < DISTANCE_THRESHOLD);
    }

    void _Debug() override
    {
    }

    void _Compute() override
    {
        time_to_live = (distance_to_target_pulse / (MAX_SPEED_PULSE)) * TTL_FACTOR;
    }

public:
    ReverseTask(DriveControler *driveControler, ValueConverter *valueConverter, double distance_cm)
    {
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;

        distance_to_target_cm = distance_cm;
        distance_to_target_pulse = valueConverter->DistanceCMToPulse(distance_to_target_cm);
    }
};