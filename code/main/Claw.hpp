#pragma once
#include <ESP32Servo.h>

class Claw{
private:

    Servo* servo_1;
    Servo* servo_2;

    int claw_time;

    int pin_1;
    int pin_2;

    int direction;

    long open_time;
    long current_time;

    

public:
    Claw(int pin_1, int pin_2, int time){
        servo_1 = new Servo();
        servo_2 = new Servo();

        this->pin_1 = pin_1;
        this->pin_2 = pin_2;

        claw_time = time;
    }


    void Init(){
        servo_1->attach(pin_1);
        servo_2->attach(pin_2);
    }

    void Open(){
        servo_1->write(110);
        servo_2->write(83);
        delay(1000);
        servo_1->write(90);
        servo_2->write(90);
    }

    void Close(){
        servo_1->write(75);
        servo_2->write(103);
        delay(1000);
        servo_2->write(90);
        servo_1->write(90);

    }   

    void Update(){
        current_time = millis();
    }

};
