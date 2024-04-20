/*
    DriveControler.hpp - Library for controling a robot with two motors


        Control the robot with two motors using PID controler for distance and angle.

            Two PID controler are used:
                - One for the distance
                - One for the angle

            Use the motor encoder to get the distance and the angle of the robot.

*/

#pragma once

#include <map>
#include <vector>

#include <PID_v1.h>
#include "PIDMotor.hpp"
#include "Radar.hpp"

enum MovementType
{
    MOVEMENT_FORWARD,
    MOVEMENT_BACKWARD,
    MOVEMENT_LEFT,
    MOVEMENT_RIGHT,
    MOVEMENT_STOP
};

class DriveControler
{
private:
    // Motor parameters
    PIDMotor *motorL;
    PIDMotor *motorR;

    // PID
    PID *pid_distance;
    PID *pid_angle;

    bool isUrgentStop = false;

    // List of Radars
    std::map<RadarPosition, std::vector<Radar *>> radars;

    double setpoint_distance, input_distance, output_distance;
    double setpoint_angle, input_angle, output_angle;

    bool radar_enable = true;

    void CheckRadar(RadarPosition position)
    {
        for (Radar *radar : radars[position])
        {

            // Serial.print("Radar:");
            // Serial.print(position);
            // Serial.print(",");

            // Serial.print("Distance:");
            // Serial.print(radar->GetDistance());
            // Serial.print(",");

            if (radar->GetDistance() < RADAR_MIN_DISTANCE)
            {
                UrgentStop();
                break;
            }
        }
    }

    void CheckRadars()
    {
        if (!radar_enable)
        {
            return;
        }

        this->isUrgentStop = false;
        MovementType movement = RequestedMovement();

        switch (movement)
        {
        case MOVEMENT_FORWARD:
        {

            if (CHECK_LEFT_RADAR_ON_FRONT_FORWARD)
            {
                CheckRadar(LEFT);
            }

            if (CHECK_RIGHT_RADAR_ON_FRONT_FORWARD)
            {
                CheckRadar(RIGHT);
            }

            if (CHECK_FRONT_RADAR_ON_FRONT_FORWARD)
            {
                CheckRadar(FRONT);
            }

            break;
        }
        case MOVEMENT_LEFT:
        {

            if (CHECK_LEFT_RADAR_ON_LEFT_ROTATION)
            {
                CheckRadar(LEFT);
            }

            if (CHECK_RIGHT_RADAR_ON_LEFT_ROTATION)
            {
                CheckRadar(RIGHT);
            }

            if (CHECK_FRONT_RADAR_ON_LEFT_ROTATION)
            {
                CheckRadar(FRONT);
            }

            break;
        }
        case MOVEMENT_RIGHT:
        {

            if (CHECK_LEFT_RADAR_ON_RIGHT_ROTATION)
            {
                CheckRadar(LEFT);
            }

            if (CHECK_RIGHT_RADAR_ON_RIGHT_ROTATION)
            {
                CheckRadar(RIGHT);
            }

            if (CHECK_FRONT_RADAR_ON_RIGHT_ROTATION)
            {
                CheckRadar(FRONT);
            }

            break;
        }

        default:
            break;
        }

        // Serial.println();
    }

public:
    DriveControler(PIDMotor *motorL, PIDMotor *motorR)
    {
        this->motorL = motorL;
        this->motorR = motorR;
    }

    void InitPid(double kp_distance, double ki_distance, double kd_distance, double kp_angle, double ki_angle, double kd_angle, double sample_time)
    {
        pid_distance = new PID(&input_distance, &output_distance, &setpoint_distance, kp_distance, ki_distance, kd_distance, DIRECT);
        pid_angle = new PID(&input_angle, &output_angle, &setpoint_angle, kp_angle, ki_angle, kd_angle, DIRECT);

        pid_distance->SetOutputLimits(-100, 100);
        pid_angle->SetOutputLimits(-100, 100);

        pid_distance->SetMode(AUTOMATIC);
        pid_angle->SetMode(AUTOMATIC);

        pid_distance->SetSampleTime(sample_time);
        pid_angle->SetSampleTime(sample_time);
    }

    void SetAngle(double angle)
    {
        // convert double to double
        setpoint_angle = (double)angle; // in pulse
    }

    void SetDistance(double distance)
    {
        // convert double to double
        setpoint_distance = distance; // in pulse
    }

    void AddRadar(Radar *radar, RadarPosition position)
    {
        radars[position].push_back(radar);
    }

    MovementType GetTypeOfMovement()
    {
        if (output_distance > 0)
        {
            return MOVEMENT_FORWARD;
        }
        else if (output_distance < 0)
        {
            return MOVEMENT_BACKWARD;
        }
        else if (output_angle > 0)
        {
            return MOVEMENT_LEFT;
        }
        else if (output_angle < 0)
        {
            return MOVEMENT_RIGHT;
        }
        else
        {
            return MOVEMENT_STOP;
        }
    }

    MovementType RequestedMovement()
    {
        if (setpoint_distance > 0)
        {
            return MOVEMENT_FORWARD;
        }
        else if (setpoint_distance < 0)
        {
            return MOVEMENT_BACKWARD;
        }
        else if (setpoint_angle > 0)
        {
            return MOVEMENT_LEFT;
        }
        else if (setpoint_angle < 0)
        {
            return MOVEMENT_RIGHT;
        }
        else
        {
            return MOVEMENT_STOP;
        }
    }

    void Update()
    {

        CheckRadars();
        if (isUrgentStop)
        {
            return;
        }

        input_distance = (motorL->GetCount() + motorR->GetCount()) / 2; // in pulse;
        input_angle = (motorR->GetCount() - motorL->GetCount()) / 2;    // in pulse;

        pid_distance->Compute();
        pid_angle->Compute();

        motorL->SetSpeed(output_distance - output_angle);
        motorR->SetSpeed(output_distance + output_angle);

        motorL->Update();
        motorR->Update();
    }

    double GetDistance()
    {
        return (double)input_distance;
    }

    double GetAngle()
    {
        return (double)input_angle;
    }

    double GetPID_distance_output()
    {
        return output_distance;
    }

    double GetPID_angle_input()
    {
        return input_angle;
    }

    double GetPID_angle_output()
    {
        return output_angle;
    }

    void Reset()
    {
        pid_distance->SetMode(MANUAL);
        pid_angle->SetMode(MANUAL);

        setpoint_distance = 0;
        setpoint_angle = 0;

        input_distance = 0;
        input_angle = 0;

        output_distance = 0;
        output_angle = 0;

        motorL->Reset();
        motorR->Reset();

        pid_distance->SetMode(AUTOMATIC);
        pid_angle->SetMode(AUTOMATIC);
    }

    void UrgentStop()
    {
        // pid_distance->SetMode(MANUAL);
        // pid_angle->SetMode(MANUAL);

        motorL->UrgentStop();
        motorR->UrgentStop();

        isUrgentStop = true;
    }

    void DisableRadar()
    {
        radar_enable = false;
    }

    void Debug()
    {

        // Serial.print("Movement:");
        // Serial.print(GetTypeOfMovement());
        // Serial.print(",");
        //
        // Serial.print("Movement_request:");
        // Serial.print(RequestedMovement());
        // Serial.print(",");
        //
        // Serial.print("UrgentStop:");
        // Serial.print(isUrgentStop);
        // Serial.print(",");

        Serial.print("Distance_target:");
        Serial.print(setpoint_distance);
        Serial.print(",");

        Serial.print("Angle_target:");
        Serial.print(setpoint_angle);
        Serial.print(",");

        Serial.print("Distance:");
        Serial.print(input_distance);
        Serial.print(",");

        Serial.print("Angle:");
        Serial.print(input_angle);
        Serial.println(",");
        //
        // Serial.print("Distance_output:");
        // Serial.print(output_distance);
        // Serial.print(",");
        //
        // Serial.print("Angle_output:");
        // Serial.println(output_angle);
    }
};