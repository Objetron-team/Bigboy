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

        float measurement_interval_ms = 5;

    public:

        int encoder_resolution;
        float wheel_diameter;
        float wheel_circumference = wheel_diameter * PI;
        float degrees_per_pulse = 360.0 / encoder_resolution;

        void Init(Motor *motor_,int pin_a_,int pin_b_, float wheel_diameter_ = 0.026, int encoder_resolution_ = 64, float measurement_interval_ms_ = 5);

        void DebouncedCount();
        void ResetCounter();
        int GetCounter();
        int GetDirection();
        float GetSpeed();
        float GetDistance();
        float GetRotationSpeed();
};

#endif
