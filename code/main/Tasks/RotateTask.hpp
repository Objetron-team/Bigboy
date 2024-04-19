#pragma once

#include "BasicTask.hpp"


#define ANGLE_THRESHOLD 2


class RotateTask : public BasicTask
{

private:
    PositionControler *positionControler;
    DriveControler *driveControler;
    ValueConverter *valueConverter;

    Point target_position;

    double target_angle_degree = 0;
    double target_angle_pulse = 0;

    double distance_to_target_cm = 0;
    double distance_to_target_pulse = 0;

    void _Update() override
    {

        driveControler->SetDistance(0);
        driveControler->SetAngle(target_angle_pulse);
    }

    bool _IsDone() override
    {

        double current_angle = valueConverter->PulseToAngle(driveControler->GetAngle()); // in degree

        double angle_diff = target_angle_degree - current_angle;

        return abs(angle_diff) < ANGLE_THRESHOLD;
    }

    void _Debug() override
    {
        Serial.print("Target_angle:");
        Serial.print(target_angle_degree);
        Serial.print(",");
        Serial.print("Target_angle_pulse:");
        Serial.print(target_angle_pulse);
        Serial.print(",");
        Serial.print("Current_angle_pulse:");
        Serial.print(driveControler->GetAngle());
        Serial.print(",");

        double current_angle = valueConverter->PulseToAngle(driveControler->GetAngle()); // in degree

        double angle_diff = target_angle_degree - current_angle;

        if (angle_diff > 180)
        {
            angle_diff -= 360;
        }
        else if (angle_diff < -180)
        {
            angle_diff += 360;
        }

        Serial.print("Error_angle:");
        Serial.println(angle_diff);
    }

    void _Compute() override
    {
        double current_angle = positionControler->GetCurrentAngle(); // in degree

        Point current_position = positionControler->GetCurrentPoint();

        double angle_between_points = atan2(target_position.y - current_position.y, target_position.x - current_position.x) * 180 / M_PI;

        double angle_to_rotate = angle_between_points - current_angle;

        if (angle_to_rotate > 180)
        {
            angle_to_rotate -= 360;
        }
        else if (angle_to_rotate < -180)
        {
            angle_to_rotate += 360;
        }

        target_angle_degree = angle_to_rotate;
        target_angle_pulse = valueConverter->AngleToPulse(target_angle_degree);
    }

public:
    RotateTask(PositionControler *positionControler, DriveControler *driveControler, ValueConverter *valueConverter, Point target_position)
    {
        this->positionControler = positionControler;
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;

        this->target_position = target_position;
    }
};