/*
    DriveControler.cpp - Library for controling a robot with two motors


        Control the robot with two motors using PID controler for distance and angle.

            Two PID controler are used:
                - One for the distance
                - One for the angle

            Use the motor encoder to get the distance and the angle of the robot.

*/

#include <PID_v1.h>
#include "PIDMotor.cpp"

#ifndef DriveControler_class

#define DriveControler_class

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

        void InitPid(float kp_distance, float ki_distance, float kd_distance, float kp_angle, float ki_angle, float kd_angle, float sample_time){
            pid_distance = new PID(&input_distance, &output_distance, &setpoint_distance, kp_distance, ki_distance, kd_distance, DIRECT);
            pid_angle    = new PID(&input_angle, &output_angle, &setpoint_angle, kp_angle, ki_angle, kd_angle, DIRECT);

            pid_distance->SetOutputLimits(-100, 100);
            pid_angle->SetOutputLimits(-100, 100);

            pid_distance->SetMode(AUTOMATIC);
            pid_angle->SetMode(AUTOMATIC);

            pid_distance->SetSampleTime(sample_time);
            pid_angle->SetSampleTime(sample_time);
        }

        void SetAngle(float angle){
            setpoint_angle = angle; // in pulse
        }

        void SetDistance(float distance){
            setpoint_distance = distance;   // in pulse
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

        float GetDistance(){
            return (float)input_distance;
        }

        float GetAngle(){
            return (float)input_angle;
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

            motorL->Reset();
            motorR->Reset();

            setpoint_distance = 0;
            setpoint_angle = 0;

            input_distance = 0;
            input_angle = 0;

            output_distance = 0;
            output_angle = 0;

            pid_distance->SetMode(AUTOMATIC);
            pid_angle->SetMode(AUTOMATIC);
        }
        
        void UrgentStop(){
            motorL->UrgentStop();
            motorR->UrgentStop();
        }
};

#endif