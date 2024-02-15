#include "encoder.h";
#include <Arduino.h>;
#include "../motor/motor.h";


void Encoder::Init(Motor *motor_,int pin_a_,int pin_b_){

    motor = motor_;

    pin_a = pin_a_;
    pin_b = pin_b_;

    counter = 0;
    last_wheel_interrupt = 0;

    last_wheel_speed_measurement = 0;
    counter_back = 0;

    for(int i = 0; i < 5; i++){
        rotation_speeds[i] = 0;
    }

    pinMode(pin, INPUT_PULLUP);
}

void Encoder::DebouncedCount(){

    int MSB = digitalRead(this->pin_a); // Read the A and B encoder pins
    int LSB = digitalRead(this->pin_b);
    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded << 2) | encoded;
    
    if ((sum == 0b1101) || (sum == 0b0100) || (sum == 0b0010) || (sum == 0b1011))
        counter++;
    else if ((sum == 0b1110) || (sum == 0b0111) || (sum == 0b0001) || (sum == 0b1000))
        counter--;

    last_wheel_interrupt = micros();
}

void Encoder::ResetCounter(){
    counter = 0;
}

float Encoder::GetSpeed(){
    return motor->GetSpeed();
}

int Encoder::GetCounter(){
    return counter;
}

float Encoder::GetDistance(){
    return (counter * wheel_circumference) / encoder_resolution;
}

float Encoder::GetRotationSpeed(){

    long current_time = millis();

    if (current_time - last_wheel_speed_measurement > measurement_interval_ms){

        //shift the array
        for(int i = 4; i > 0; i--){
            rotation_speeds[i] = rotation_speeds[i-1];
        }

        float delta_counter = counter - counter_back;
        float direction = delta_counter > 0 ? 1 : -1;

        //add the new measurement to the array -> tick per second
        rotation_speeds[0] = ((delta_counter) * direction) * 1000 / (current_time - last_wheel_speed_measurement);
        
        counter_back = counter;
        last_wheel_speed_measurement = current_time;
    }

    //smooth out the rotation speed by taking the average of the last 5 measurements
    float rotation_speed = 0;
    for(int i = 0; i < 5; i++){
        rotation_speed += rotation_speeds[i];
    }
    rotation_speed /= 5;

    return rotation_speed;
}
