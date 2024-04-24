#pragma once

#include "Point.hpp"
#include "DriveControler.hpp"
#include "ValueConverter.hpp"

class PositionControler
{
private:
    DriveControler *driveControler;

    Point current_point;
    Point target_point;

    double current_angle = 0;

    double last_distance = 0;
    double last_angle = 0;

    bool started = false;
    bool auto_mode = false;

    ValueConverter *valueConverter;

public:
    PositionControler(DriveControler *driveControler, int pulse_per_turn, double wheel_diameter, double wheel_distance)
    {
        this->driveControler = driveControler;

        valueConverter = new ValueConverter(pulse_per_turn, wheel_diameter, wheel_distance);

        current_point.x = 0;
        current_point.y = 0;
    }

    void Update()
    {

        // integrate the position
        double distance = valueConverter->PulseToDistanceCM(driveControler->GetDistance()); // in centimeter
        double angle = valueConverter->PulseToAngle(driveControler->GetAngle());            // in degree

        double d_distance = distance - last_distance; // in centimeter

        current_angle = last_angle + angle;

        // angle can't be bigger than 180 degree
        if (current_angle > 180)
        {
            current_angle -= 360;
        }
        else if (current_angle < -180)
        {
            current_angle += 360;
        }

        // convert into cm
        current_point.x += d_distance * cos(current_angle * M_PI / 180);
        current_point.y += d_distance * sin(current_angle * M_PI / 180);

        last_distance = distance;

        driveControler->Update();
    }

    void SoftReset()
    {
        driveControler->UrgentStop();
        driveControler->Reset(); // Reset all counter to avoid drift and overflows

        last_angle = current_angle;
        last_distance = 0;
    }

    Point GetCurrentPoint()
    {
        return current_point;
    }

    double GetCurrentAngle()
    {
        return current_angle;
    }

    void Reset()
    {
        driveControler->UrgentStop();
        driveControler->Reset(); // Reset all counter to avoid drift and overflows

        current_point.x = 0;
        current_point.y = 0;
        current_angle = 0;

        last_angle = 0;
        last_distance = 0;
    }
};
