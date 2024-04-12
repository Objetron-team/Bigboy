#pragma once

#include <Arduino.h>;

#define SOUND_SPEED 340.0 / 1000 // mm/s
#define MEASURE_TIMEOUT 25000UL
#define MEASURE_INTERVAL 10
#define OUT_OF_RANGE 1000000

enum RadarPosition
{
    LEFT,
    RIGHT,
    FRONT
};

class Radar
{
private:
    /* data */

    int trigger_pin = 0;
    int echo_pin = 0;

public:
    Radar(int trigger_pin, int echo_pin)
    {
        this->trigger_pin = trigger_pin;
        this->echo_pin = echo_pin;
    }

    void Init()
    {
        pinMode(trigger_pin, OUTPUT);
        digitalWrite(echo_pin, LOW);
    }

    double GetDistance()
    {

        digitalWrite(trigger_pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigger_pin, LOW);

        long measure = pulseIn(echo_pin, HIGH, MEASURE_TIMEOUT);
        if (measure == 0)
        {
            return OUT_OF_RANGE;
        }

        double distance_mm = measure / 2.0 * SOUND_SPEED;
        return distance_mm;
    }
};
