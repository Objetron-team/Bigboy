#include <Wire.h>

#ifndef Motor_h

#define Motor_h

class Motor {
    private:
        bool dual_input;
        int pin_direction;
        int pin_speed;

        float target_speed;
        float current_speed;

        float threshold_speed;
        float min_speed;
        float max_speed;

        long last_update_time;
        float max_acceleration; //in power/seconde -> 0.1 is 10% increase per seconde

        void SetMotorSpeendAndDir(float speed); //value between -100 and 100

    public:
        void Init(int pin_direction,int pin_speed,float max_acceleration_, float threshold_speed, float min_speed,float max_speed, bool dual_input_ = false);
        void SetSpeed(float target);
        void UrgentStop();
        int GetDirection();
        float GetSpeed();

        //void SetThresholdSpeedAndDeadzone(float threshold_speed, float speed_deadzone);
        void SetMaxSpeed(float max_speed);
};

#endif
