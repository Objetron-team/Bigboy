#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "../motor/motor.h";



#ifndef Encoder_h

#define Encoder_h

class Encoder {
    private:
        Motor *motor;

        int pin_a;
        int pin_b;
        
        long last_wheel_speed_measurement;

        long last_wheel_interrupt;
        const long last_wheel_interrupt_debounced_micros = 500;

        int counter;
        int counter_back;
        int direction;
        float rotation_speeds[5];

        const int encoder_resolution = 64;
        const float wheel_diameter = 0.026;
        const float wheel_circumference = wheel_diameter * PI;
        const float degrees_per_pulse = 360.0 / encoder_resolution;
        const float measurement_interval_ms = 5;


    public:




        void Init(Motor *motor_,int pin_a_,int pin_b_);

        void DebouncedCount();
        void ResetCounter();
        int GetCounter();
        int GetDirection();
        float GetSpeed();
        float GetDistance();
        float GetRotationSpeed();
};

#endif
