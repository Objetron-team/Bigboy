/*
    Class to control a motor with a PID controller and an encoder.
        - Control the motor is used with a ramp to avoid high current draw
        - The encoder is used to get the speed of the motor
        - The PID controller is used to control the speed of the motor

        In dual Mode,
            - The motor is controlled with two pins, one for the speed in one direction and one for the speed in the other direction
            - direction_pin -> speed in forward direction
            - speed_pin -> speed in backward direction
                ! both pin are analogWrite pins

        In single Mode,
            - The motor is controlled with one pin for the speed and one pin for the direction
            - direction_pin -> direction of the motor
                ! direction_pin is a digitalWrite pin   -> HIGH for forward, LOW for backward
            - speed_pin -> speed of the motor
                ! speed_pin is an analogWrite pin
*/

#pragma once

#include <Arduino.h>;
#include <ESP32Encoder.h>
#include <PID_v1.h>

#define NBR_OF_SPEED_MEASUREMENTS 10


class PIDMotor{
    private:

        //Type of driver
        bool dual_input;

        //Motor pins
        int pin_direction;
        int pin_speed;

        //Motor parameters
        float current_speed;
        float threshold_speed;
        float min_speed;    
        float max_speed;

        //Motor timing
        long last_update_time;
        float max_acceleration; //in power/seconde -> 0.1 is 10% increase per seconde

        //Encoder
        ESP32Encoder encoder;

        int MAX_FREQUENCY = 1000; // in Hz
        int PULSES_PER_TURN = 1;
        float WHEEL_DIAMETER = 1;   // in meter

        float VFilt = 0;
        float VPrev = 0;
        float speed_measurements[NBR_OF_SPEED_MEASUREMENTS];

        long last_encoder_time;
        long last_encoder_value;

        //PID
        PID* pid;
        
        double setpoint, input, output;


        void SetSpeed_with_ramp(float target){
                //Increase or decrease the current speed to reach the target speed by the max acceleration
            float delta_time = (millis() - last_update_time) / 1000.0;
            last_update_time = millis();

            int target_sign = abs(target) / target;

            if(abs(target) < threshold_speed){
                target = 0;
            }

            if(abs(target) > max_speed){
                target = target_sign * max_speed;
            }


            float delta_speed = delta_time * max_acceleration;

            if(target > current_speed){
                current_speed += delta_speed;
                if(current_speed > target){
                    current_speed = target;
                }
            }
            if(target < current_speed){
                current_speed -= delta_speed;
                if(current_speed < target){
                    current_speed = target;
                }
            }

            SetMotorSpeendAndDir(current_speed);
        }

        void SetMotorSpeendAndDir(float speed){

            if(abs(speed) < min_speed && speed != 0){
                speed = speed / abs(speed) * min_speed;
            }

            //Convert the speed in % to a value between 0 and 255
            float value = abs(speed) * 255 / 100.0;

            if(dual_input){

                // in dual_input mode, the pin_direction is used a a second speed pin, the two speed pins are used to control the speed of the motor and direction.
                // pin_direction -> speed in forward direction
                // pin_speed -> speed in backward direction

                if(speed < 0){
                    analogWrite(pin_speed, value);
                    analogWrite(pin_direction, 0);
                }else if(speed > 0){
                    analogWrite(pin_speed, 0);
                    analogWrite(pin_direction, value);
                }else{
                    analogWrite(pin_speed, 0);
                    analogWrite(pin_direction, 0);
                }

                return;
            }

            if(speed < 0){
                digitalWrite(pin_direction, HIGH);
            }
            if(speed >= 0){
                digitalWrite(pin_direction, LOW);
            }

            analogWrite(pin_speed, value);
        }

    public:

        PIDMotor(int pin_direction,int pin_speed,float max_acceleration, float max_speed, float min_speed,float threshold_speed, bool dual_input){
            this->pin_direction = pin_direction;
            this->pin_speed = pin_speed;
            this->max_acceleration = max_acceleration;
            this->threshold_speed = threshold_speed;
            this->min_speed = min_speed;
            this->max_speed = max_speed;
            this->dual_input = dual_input;
            this->current_speed = 0;
            this->last_update_time = millis();
        }

        void InitEncoder(int pinA, int pinB, int max_frequency, int pulses_per_turn, float wheel_diameter){
            encoder.attachFullQuad(pinA, pinB);
            encoder.setCount(0);

            this->MAX_FREQUENCY = max_frequency;
            this->PULSES_PER_TURN = pulses_per_turn;
            this->WHEEL_DIAMETER = wheel_diameter;
        }

        void InitPID(float Kp, float Ki, float Kd, float sample_time_ms){
            pid = new PID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);
            pid->SetMode(AUTOMATIC);
            pid->SetOutputLimits(-100, 100);
            pid->SetSampleTime(sample_time_ms);
        }

        void SetSpeed(float speed){
            setpoint = speed;   //Set the setpoint of the PID, in % of the max speed
        }

        float GetSpeed(){
            long currentMicros = micros();
            long currentCount = encoder.getCount();

            long deltaMicros = currentMicros - last_encoder_time;
            long deltaCount = currentCount - last_encoder_value;

            last_encoder_time = currentMicros;
            last_encoder_value = currentCount;

            if (deltaMicros == 0) {
                return 0;
            }

            float speed = deltaCount * 1000000 / deltaMicros;

            // add low pass filter 25hz before adding to the array
            this->VFilt = 0.854 * this->VFilt + 0.0728 * speed + 0.0728 * this->VPrev;
            this->VPrev = speed;

            for (int i = 0; i < NBR_OF_SPEED_MEASUREMENTS - 1; i++) {
                speed_measurements[i] = speed_measurements[i + 1];
            }

            speed_measurements[NBR_OF_SPEED_MEASUREMENTS - 1] = VFilt;

            float sum = 0;
            for (int i = 0; i < NBR_OF_SPEED_MEASUREMENTS; i++) {
                sum += speed_measurements[i];
            }

            return sum / NBR_OF_SPEED_MEASUREMENTS;
        }

        float GetSpeedRelative(){

            float absolute_relative_speed = GetSpeed() / MAX_FREQUENCY * 100;

            float real_relative_speed = absolute_relative_speed / (max_speed / 100);

            return real_relative_speed;
        }

        float GetSpeedMeterPerSeconde(){
            return GetSpeed() * WHEEL_DIAMETER * PI / PULSES_PER_TURN;
        }

        int GetDirection(){
            return GetSpeed() >= 0 ? 1 : -1;
        }

        void Update(){
            input = GetSpeedRelative();
            pid->Compute();
            SetSpeed_with_ramp(output);
        }

        void Reset(){

            pid->SetMode(MANUAL);

            // Reset the encoder count to 0 and stop the motor
            encoder.setCount(0);
            SetSpeed(0);

            // Reset the speed
            current_speed = 0;

            // Reset the time
            last_update_time = millis();

            // Reset the encoder time
            last_encoder_time = micros();
            last_encoder_value = 0;

            // Reset the speed measurements
            for(int i = 0; i < NBR_OF_SPEED_MEASUREMENTS; i++){
                speed_measurements[i] = 0;
            }
            VFilt = 0;
            VPrev = 0;

            // Reset the PID
            pid->SetMode(AUTOMATIC);
        }

        void UrgentStop(){
            SetMotorSpeendAndDir(0);
            SetSpeed(0);
            SetSpeed_with_ramp(0);
        }

        double GetPID_target(){
            return setpoint;
        }

        double GetPID_output(){
            return output;
        }

        double GetPID_input(){
            return input;
        }

        double GetCount(){
            return encoder.getCount();
        }

        void SetHardSpeed(float speed){
            SetMotorSpeendAndDir(speed);
        }

};