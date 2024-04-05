/*
    DriveControler.hpp - Library for controling a robot with two motors


        Control the robot with two motors using PID controler for distance and angle.

            Two PID controler are used:
                - One for the distance
                - One for the angle

            Use the motor encoder to get the distance and the angle of the robot.

*/

#pragma once

#include <PID_v1.h>
#include "PIDMotor.hpp"

class DriveControler{
    private:

        // Motor parameters
        PIDMotor* motorL;
        PIDMotor* motorR;

        // PID
        PID* pid_distance;
        PID* pid_angle;

        double setpoint_distance, input_distance, output_distance;
        double setpoint_angle, input_angle, output_angle;

    public:
        DriveControler(PIDMotor* motorL, PIDMotor* motorR){
            this->motorL = motorL;
            this->motorR = motorR;
        }

        void InitPid(double kp_distance, double ki_distance, double kd_distance, double kp_angle, double ki_angle, double kd_angle, double sample_time){
            pid_distance = new PID(&input_distance, &output_distance, &setpoint_distance, kp_distance, ki_distance, kd_distance, DIRECT);
            pid_angle    = new PID(&input_angle, &output_angle, &setpoint_angle, kp_angle, ki_angle, kd_angle, DIRECT);

            pid_distance->SetOutputLimits(-100, 100);
            pid_angle->SetOutputLimits(-100, 100);

            pid_distance->SetMode(AUTOMATIC);
            pid_angle->SetMode(AUTOMATIC);

            pid_distance->SetSampleTime(sample_time);
            pid_angle->SetSampleTime(sample_time);
        }

        void SetAngle(double angle){
            //convert double to double
            setpoint_angle = (double)angle; // in pulse
        }

        void SetDistance(double distance){
            //convert double to double
            setpoint_distance = (double)distance; // in pulse
        }

        void Update(){

            input_distance = (motorL->GetCount() + motorR->GetCount()) / 2 ; // in pulse;
            input_angle = (motorR->GetCount() - motorL->GetCount()) / 2 ;    // in pulse;

            pid_distance->Compute();
            pid_angle->Compute();

            motorL->SetSpeed(output_distance - output_angle);
            motorR->SetSpeed(output_distance + output_angle);

            motorL->Update();
            motorR->Update();
        }

        double GetDistance(){
            return (double)input_distance;
        }

        double GetAngle(){
            return (double)input_angle;
        }

        double GetPID_distance_output(){
            return output_distance;
        }

        double GetPID_angle_input(){
            return input_angle;
        }

        double GetPID_angle_output(){
            return output_angle;
        }

        void Reset(){
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
        
        void UrgentStop(){
            motorL->UrgentStop();
            motorR->UrgentStop();
        }

        void Debug(){

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
            Serial.print(",");

            Serial.print("Distance_output:");
            Serial.print(output_distance);
            Serial.print(",");

            Serial.print("Angle_output:");
            Serial.println(output_angle);
        }
};