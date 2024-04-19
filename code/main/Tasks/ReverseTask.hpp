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

    Point target_position;

    double distance_to_target_cm = 0;
    double distance_to_target_pulse = 0;

    void _Update() override
    {
        driveControler->SetDistance(-distance_to_target_pulse);
        driveControler->SetAngle(0);
    }

    bool _IsDone() override
    {
        // check if the current position is close enough to the target position
        double error = driveControler->GetDistance();
        double distance_to_target = valueConverter->PulseToDistanceCM(error);
        return ((-35 < distance_to_target) && (distance_to_target < -25));
    }
    
    void _Debug() override
    {
        
    }

    void _Compute() override
    {
        distance_to_target_cm = 30;
        distance_to_target_pulse = valueConverter->DistanceCMToPulse(distance_to_target_cm);

        // compute the time to live using MAX_SPEED (m/s) and distance to target (cm), add time to accelerate and decelerate factor
        time_to_live = (distance_to_target_pulse / (MAX_SPEED_PULSE)) * TTL_FACTOR;
    }

public:
    ReverseTask( DriveControler *driveControler, ValueConverter *valueConverter)
    {
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;

    }
};