#pragma once
#include <ESP32Servo.h>

class Arm{
private:

    Servo* servo;

    int arm_time;

    int pin;

    int direction;

    long open_time;
    long current_time;
    

public:
    Arm(int pin, int time){
        servo = new Servo();

        this->pin = pin;

        arm_time = time;
    }


    void Init(){
        servo->attach(pin);
    }
    void Close(){
        servo->write(110);
        delay(1000);
        servo->write(90);
    }

    void Open(){
        servo->write(70);
        delay(1000);
        servo->write(90);        
    }

    void Update(){
        current_time = millis();
    }

};
