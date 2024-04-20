#pragma once

#include "BasicTask.hpp"

#define ANGLE_THRESHOLD 2 // degree
#define TTL_FACTOR 300

class RotationTask : public BasicTask
{
private:
    DriveControler *driveControler;
    ValueConverter *valueConverter;

    double target_angle_degree = 0;
    double target_angle_pulse = 0;

    void _Update() override
    {
        driveControler->SetDistance(0);
        driveControler->SetAngle(target_angle_pulse);
    }

    bool _IsDone() override
    {
        double current_angle_pulse = driveControler->GetAngle();
        double angle_error_pulse = (target_angle_pulse)-current_angle_pulse;

        double angle_error_degree = valueConverter->PulseToAngle(angle_error_pulse);

        return (abs(angle_error_degree) < ANGLE_THRESHOLD);
    }

    void _Debug() override
    {
        Serial.print("TASK:");
        Serial.print("RotationTask");
    }

    void _Compute() override
    {
        time_to_live = (target_angle_pulse / (MAX_SPEED_PULSE)) * TTL_FACTOR;
    }

public:
    RotationTask(DriveControler *driveControler, ValueConverter *valueConverter, double angle_degree)
    {
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;

        target_angle_degree = angle_degree;
        target_angle_pulse = valueConverter->AngleToPulse(target_angle_degree);
    }
};