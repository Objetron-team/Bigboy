#pragma once
#include <ESP32Servo.h>

class Claw
{
private:
    Servo *servo_1;
    Servo *servo_2;

    int claw_time;

    int pin_1;
    int pin_2;

    int direction;

    long open_time;
    long current_time;

public:
    Claw(int pin_1, int pin_2, int time)
    {
        servo_1 = new Servo();
        servo_2 = new Servo();

        this->pin_1 = pin_1;
        this->pin_2 = pin_2;

        claw_time = time;
    }

    void Init()
    {
        servo_1->attach(pin_1); // droite
        servo_2->attach(pin_2); // gauche
    }

    void Open()
    {
        servo_1->write(105);
        servo_2->write(80);
        delay(1000);
        servo_1->write(90);
        servo_2->write(90);
    }

    void Close()
    {
        servo_1->write(77);
        servo_2->write(102);
        delay(1000);
        servo_1->write(90);
        servo_2->write(90);
    }

    void Update()
    {
        current_time = millis();
    }
};
