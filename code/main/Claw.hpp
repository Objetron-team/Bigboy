
#include <ESP32Servo.h>

class Claw{
private:

    Servo* servo_1;
    Servo* servo_2;

    int CLAW_TIME;

    int pin_1;
    int pin_2;

    int direction;

    long open_time;
    long current_time;

    void RunMotor(int direction){

        if(current_time - open_time < CLAW_TIME){

            servo_1->write(200 * direction);
            servo_2->write(200 * direction);

        }
        else{
            servo_1->write(0);
            servo_2->write(0);
        }
    }

public:
    Claw(int pin_1, int pin_2, int time){
        servo_1 = new Servo();
        servo_2 = new Servo();

        this->pin_1 = pin_1;
        this->pin_2 = pin_2;

        CLAW_TIME = time;
    }


    void Init(){
        servo_1->attach(pin_1);
        servo_2->attach(pin_2);
    }

    void Open(){
        /*
        direction = 1;
        open_time = millis();
        RunMotor(direction);
        
        */

        servo_1->write(110);
        delay(1000);
        servo_1->write(90);
        delay(1000);
        servo_1->write(75);
        delay(1000);
        servo_1->write(90);
        

    }

    void Close(){

        servo_2->write(80);
        delay(1000);
        servo_2->write(90);
        delay(1000);
        servo_2->write(103);
        delay(1000);
        servo_2->write(90);

    }   

    void Update(){
        current_time = millis();
        RunMotor(direction);
    }

};
