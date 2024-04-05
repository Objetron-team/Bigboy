
#include <ESP32Servo.h>

class Claw{
private:

    Servo* servo_1;
    Servo* servo_2;
    Servo* servo_3;

    int CLAW_TIME;

    int pin_1;
    int pin_2;
    int pin_3;

    int direction;

    long open_time;
    long current_time;
    

public:
    Claw(int pin_1, int pin_2, int pin_3,int time){
        servo_1 = new Servo();
        servo_2 = new Servo();
        servo_3 = new Servo();

        this->pin_1 = pin_1;
        this->pin_2 = pin_2;
        this->pin_3 = pin_3;

        CLAW_TIME = time;
    }


    void Init(){
        servo_1->attach(pin_1);
        servo_2->attach(pin_2);
        servo_3->attach(pin_3);
    }

    void Open(){

        servo_1->write(110);
        servo_2->write(80);
        delay(1000);
        servo_1->write(90);
        servo_2->write(90);
    }

    void Close(){

        servo_2->write(103);
        servo_1->write(75);
        delay(1000);
        servo_2->write(90);
        servo_1->write(90);

    }   

    void Boubou(){
        servo_3->write(70);
        delay(1000);
        servo_3->write(90);
        delay(1000);
        servo_3->write(100);
        delay(1000);
        servo_3->write(90);
        
    }

    void Update(){
        current_time = millis();
    }

};
