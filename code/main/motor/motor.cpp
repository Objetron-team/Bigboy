#include "motor.h";
#include <Arduino.h>;


void Motor::Init(int pin_direction_,int pin_speed_,float max_acceleration_, float threshold_speed, float min_speed,float max_speed, bool dual_input_){

    this->dual_input = dual_input_;
    this->pin_direction = pin_direction_;
    this->pin_speed = pin_speed_;

    this->max_acceleration = max_acceleration_;
    this->threshold_speed = threshold_speed;
    this->min_speed = min_speed;
    this->max_speed = max_speed;

    pinMode(pin_direction, OUTPUT);
    pinMode(pin_speed, OUTPUT);

    current_speed = 0;
}

float Motor::GetSpeed(){
    return current_speed;
}

/*
    Set the target speed of the motor
    param: target speed in % (between -100 and 100)
*/
void Motor::SetSpeed(float target){

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

void Motor::UrgentStop(){
    current_speed = 0;

    SetMotorSpeendAndDir(0);
}

/*
    Convert the speed in % to a value between 0 and 255 and set the motor speed and direction
    param: speed in % (between -100 and 100)
*/
void Motor::SetMotorSpeendAndDir(float speed){
    int sign = GetDirection();

    if(abs(speed) < min_speed && speed != 0){
        speed = sign * min_speed;
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



void Motor::SetMaxSpeed(float max_speed){
    this->max_speed = max_speed;
}


int Motor::GetDirection(){
    if(current_speed < 0){
        return -1;
    }
    if(current_speed == 0){
        return 1;
    }
    if(current_speed > 0){
        return 1;
    }
}