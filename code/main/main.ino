

#include "motor/motor.h";
#include "motor/motor.cpp";

#include "encoder/encoder.h";
#include "encoder/encoder.cpp";

#include "pid/pid.h";
#include "pid/pid.cpp";

#include "driveController/driveController.h";
#include "driveController/driveController.cpp";

#include "positionController/positionController.h";
#include "positionController/positionController.cpp";

#include "ultrasound/ultrasound.h";
#include "ultrasound/ultrasound.cpp";

#include "BluetoothSerial.h"

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "MyLittlePami";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;


/*
    - adruino mega pins

    interrup -> 18,19,20,21,    2 and 3

    analogue write -> 2..13, 44..46


    - esp 32 pins

    interrup -> all

    analogue write -> All except 34,35,36,39

*/


Motor motorRight;
Motor motorLeft;

const bool PAMI_DUAL_INPUT = true;

const int MOTOR_RIGHT_SPEED_PIN = 33;
const int MOTOR_RIGHT_DIRECTION_PIN = 32;

const int MOTOR_LEFT_SPEED_PIN = 26;
const int MOTOR_LEFT_DIRECTION_PIN = 25;

const int acceleration_right = 50;  // % per sec
const int acceleration_left = 50;  // % per sec

float max_speed_left = 100.0f;
float max_speed_right = 100.0f;

float threshold_speed_left = 3.0f;
float threshold_speed_right = 3.0f;

float min_speed_left = 0;//11.0f;
float min_speed_right = 0;//11.0f;

void InitMotor(){

    motorRight.Init(MOTOR_RIGHT_DIRECTION_PIN,MOTOR_RIGHT_SPEED_PIN,acceleration_right,threshold_speed_right,min_speed_right, max_speed_right,PAMI_DUAL_INPUT);
    motorLeft.Init(MOTOR_LEFT_DIRECTION_PIN,MOTOR_LEFT_SPEED_PIN ,acceleration_left,threshold_speed_left,min_speed_left, max_speed_left,PAMI_DUAL_INPUT);

    motorRight.SetSpeed(0);
    motorLeft.SetSpeed(0);
}

const int WHEEL_ENCODER_PIN_RIGHT_A = 18;
const int WHEEL_ENCODER_PIN_RIGHT_B = 19;

const int WHEEL_ENCODER_PIN_LEFT_A = 5;
const int WHEEL_ENCODER_PIN_LEFT_B = 17;

const float WHEEL_DIAMETER = 0.026; //m
const float ENCODER_RESOLUTION = 35750.04; //pulse per rotation
const float DEBOUNCED_TIME = 50.0f; //ms    

Encoder encoderRight;
Encoder encoderLeft;

void InitEncoder(){

    encoderRight.Init(&motorRight,WHEEL_ENCODER_PIN_RIGHT_A,WHEEL_ENCODER_PIN_RIGHT_B,WHEEL_DIAMETER,ENCODER_RESOLUTION,DEBOUNCED_TIME);
    encoderLeft.Init(&motorLeft,WHEEL_ENCODER_PIN_LEFT_A,WHEEL_ENCODER_PIN_LEFT_B,WHEEL_DIAMETER,ENCODER_RESOLUTION,DEBOUNCED_TIME);

    attachInterrupt(digitalPinToInterrupt(WHEEL_ENCODER_PIN_RIGHT_A), CouterRight, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(WHEEL_ENCODER_PIN_RIGHT_B), CouterRight, CHANGE);

    attachInterrupt(digitalPinToInterrupt(WHEEL_ENCODER_PIN_LEFT_A), CouterLeft, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(WHEEL_ENCODER_PIN_LEFT_B), CouterLeft, CHANGE);
}

void CouterRight(){
    encoderRight.DebouncedCount();
}

void CouterLeft(){
    encoderLeft.DebouncedCount();
}

const byte ULTRASOUND_TRIGGER_PIN = 9;
const byte ULTRASOUND_ECHO_PIN = 8;

const byte TRIGGER_PIN = 9; // Broche TRIGGER
const byte ECHO_PIN = 8;   // Broche ECHO
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s

/* Vitesse du son dans l'air en mm/us */
const float SOUND_SPEED = 340.0 / 1000;


Ultrasound ultrasound;

void InitUltrasound(){

    pinMode(ULTRASOUND_ECHO_PIN,INPUT);


    pinMode(ULTRASOUND_TRIGGER_PIN,OUTPUT);
    digitalWrite(ULTRASOUND_TRIGGER_PIN, LOW);
    ultrasound.Init(ULTRASOUND_ECHO_PIN,ULTRASOUND_TRIGGER_PIN);
}


DriveController driveController;
PositionController positionController;

PID SpeedLeft;
PID SpeedRight;

float lk = 0.1;
float li = 0.0;
float ld = 0.0;

float rk = 0.1;
float ri = 0.0;
float rd = 0.0;

void setup() {

    //disableCore0WDT();
    //disableCore1WDT();

    Serial.begin(115200);

    Serial.println("Start init");

    SerialBT.begin(device_name); //Bluetooth device name
    Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
    //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
    #ifdef USE_PIN
        SerialBT.setPin(pin);
        Serial.println("Using PIN");
    #endif

    SpeedLeft.Init(lk,li,ld,-100,100);
    SpeedRight.Init(rk,ri,rd,-100,100);

    Serial.println("InitUltrasound");

    //InitUltrasound();

    Serial.println("InitMotor");

    InitMotor();

    Serial.println("InitEncoder");

    InitEncoder();
    
    Serial.println("InitDriveController");

    driveController.Init(&motorRight,&motorLeft,&encoderRight,&encoderLeft);
    
    Serial.println("InitPositionController");

    positionController.Init(&driveController,&ultrasound);

    Serial.println("Init done");

}

float speed = 0;
float target_angle = 0;
float target_distance = 0;

float distance_to_add = 10 * ENCODER_RESOLUTION / (WHEEL_DIAMETER * 100 * PI);   // 10 cm to tick

float angle_rad = 10 * PI / 180;
float arc_length = angle_rad * ENCODER_RESOLUTION / PI;

void SerialCommande(){

    if(SerialBT.available() > 0){

        String commande = SerialBT.readStringUntil('\n');


        /*
            format commande :
            {commande} {param1} {param2} {param3} {param4}
        
            commande :
            - m : move
                param1 : distance
            - r : rotate
                param1 : angle
        */
    
        char commande_type = commande.charAt(0);

        /*
            {side}{param}{nbr} -> side : l or r, param : k,i,d ; nbr : 0..9
                nbr -> float

            exemple :   lk3 -> left k = 3
                        ri5 -> right i = 5

        */

        if(commande_type == 'l' || commande_type == 'r'){
            char side = commande.charAt(0);
            char param = commande.charAt(1);
            float nbr = commande.substring(2).toFloat();

            if(side == 'l'){
                if(param == 'p'){
                    lk = nbr;
                }else if(param == 'i'){
                    li = nbr;
                }else if(param == 'd'){
                    ld = nbr;
                }

                SpeedLeft.UpdateKpKiKd(lk,li,ld);

                SerialBT.print("Left PID Updated");
                SerialBT.print("Kp:");
                SerialBT.print(lk);
                SerialBT.print("Ki:");
                SerialBT.print(li);
                SerialBT.print("Kd:");
                SerialBT.println(ld);

            }else if(side == 'r'){
                if(param == 'p'){
                    rk = nbr;
                }else if(param == 'i'){
                    ri = nbr;
                }else if(param == 'd'){
                    rd = nbr;
                }

                SpeedRight.UpdateKpKiKd(rk,ri,rd);

                SerialBT.print("Right PID Updated");
                SerialBT.print("Kp:");
                SerialBT.print(rk);
                SerialBT.print("Ki:");
                SerialBT.print(ri);
                SerialBT.print("Kd:");
                SerialBT.println(rd);
            }

            return;
        }
        

        switch(commande_type){
            case 'z':
                positionController.Start();
                break;
            case 's':
                positionController.Stop();
                break;
            case 'n':
                positionController.Next();
                break;
            case 'b':
                positionController.AddPoint({20,0});
                positionController.AddPoint({20,90});
                break;
            case 'y':
                positionController.AddPoint({20,0});
                positionController.AddPoint({20,-90});
                break;
            
            case 'f':
                speed += 10;
                break;
            case 'g':
                speed -= 10;
                break;
            case 'h':
                speed = 0;
                break;
            
            default:
                break;
        }
    }

}

void Debug(float distance_mm){

    Serial.print("Tasks:");
    Serial.print(positionController.GetTaskCount());
    Serial.print(",");

    Serial.print("Current_task_id:");
    Serial.print(positionController.current_task.id);
    Serial.print(",");
    /*
    Serial.print("State:");
    Serial.print(positionController.start);
    Serial.print(",");
    */
    Serial.print("Task_completed:");
    Serial.print(positionController.IsTaskCompleted());
    Serial.print(",");

    Serial.print("target_distance:");
    Serial.print(driveController.GetTargetDistance());
    Serial.print(",");
 
    Serial.print("target_angle:");
    Serial.print(driveController.GetTargetAngle());
    Serial.print(",");

    Serial.print("collision_distance:");
    Serial.print(distance_mm);
    Serial.print(",");

    Serial.print("x:");
    Serial.print(driveController.GetX());
    Serial.print(",");

    Serial.print("y:");
    Serial.print(driveController.GetY());
    Serial.print(",");

    Serial.print("theta:");
    Serial.println(driveController.GetTheta());
    //Serial.print(",");

}

void DriveDebug(){
    Serial.print("Target Distance:");
    Serial.print(driveController.GetTargetDistance());
    Serial.print(",");

    Serial.print("Current Distance:");
    Serial.print(driveController.GetCurrentDistance());
    Serial.print(",");

    Serial.print("Target Angle:");
    Serial.print(driveController.GetTargetAngle());
    Serial.print(",");

    Serial.print("Current Angle:");
    Serial.print(driveController.GetCurrentAngle());
    Serial.print(",");

    Serial.print("Target Speed:");
    Serial.print(driveController.GetTargetSpeed());
    Serial.print(",");

    Serial.print("Current Speed:");
    Serial.print(driveController.GetSpeed());
    Serial.print(",");

    Serial.print("X:");
    Serial.print(driveController.GetX());
    Serial.print(",");

    Serial.print("Y:");
    Serial.print(driveController.GetY());
    Serial.print(",");

    Serial.print("Theta:");
    Serial.println(driveController.GetTheta());

}

int DebugPath(){
    int count = 1;
    Path *temp_task = positionController.current_task.next_task;
    while(temp_task != NULL){
        count++;

        Serial.print("Task:");
        Serial.print(temp_task->id);
        Serial.print(",");
        Serial.print("x:");
        Serial.print(temp_task->point.x);
        Serial.print(",");
        Serial.print("y:");
        Serial.print(temp_task->point.y);
        Serial.print(",");
        Serial.print("angle:");
        Serial.println(temp_task->angle);

        temp_task = temp_task->next_task;
    }
    return count;
}
/*
    Task:1 ,x:0.00      ,y:0.00     ,angle:45.00
    Task:2 ,x:100.00    ,y:100.00   ,angle:45.00
    Task:3 ,x:0.00      ,y:0.00     ,angle:0.00
    Task:4 ,x:200.00    ,y:100.00   ,angle:0.00
    Task:5 ,x:0.00      ,y:0.00     ,angle:135.00
    Task:6 ,x:100.00    ,y:200.00   ,angle:135.00

*/

void DebugUltrasound(){
    Serial.print("Distance:");
    Serial.println(ultrasound.GetDistance());
}

float speed_right;
float speed_left;

void DebugEncoder(){

    Serial.print("Request Speed:");
    Serial.print(speed);
    Serial.print(",");

    Serial.print("Encoder R:");
    Serial.print(encoderRight.GetRotationSpeed());
    Serial.print(",");

    Serial.print("PID Speed R:");
    Serial.print(speed_right);
    Serial.print(",");

    Serial.print("Real Speed R:");
    Serial.print(encoderRight.GetSpeed());
    Serial.print(",");

    // Serial.print("Encoder R Counter:");
    // Serial.print(encoderRight.GetCounter());
    // Serial.print(",");

    // Serial.print("Encoder R Direction:");
    // Serial.print(encoderRight.GetDirection());
    // Serial.print(",");

    // Serial.print("Encoder R Distance:");
    // Serial.print(encoderRight.GetDistance());
    // Serial.print(",");

    Serial.print("Encoder L:");
    Serial.print(encoderLeft.GetRotationSpeed());
    Serial.print(",");

    Serial.print("PID Speed L:");
    Serial.print(speed_left);
    Serial.print(",");

    Serial.print("Real Speed L:");
    Serial.println(encoderLeft.GetSpeed());
    //Serial.print(",");

    // Serial.print("Encoder L Counter:");
    // Serial.print(encoderLeft.GetCounter());
    // Serial.print(",");

    // Serial.print("Encoder L Direction:");
    // Serial.print(encoderLeft.GetDirection());
    // Serial.print(",");

    // Serial.print("Encoder L Distance:");
    // Serial.println(encoderLeft.GetDistance());
    //Serial.print(",");

}



void loop() {

    speed_left = SpeedLeft.Compute(speed,encoderLeft.GetSpeed());
    speed_right = SpeedRight.Compute(speed,encoderRight.GetSpeed());
    
    //Debug(10000 / 10);
    //DebugPath();
    DebugEncoder();
    //encoderRight.GetRotationSpeed();
    //DriveDebug();
    motorRight.SetSpeed(speed_right);
    motorLeft.SetSpeed(speed_left);

    SerialCommande();

    //positionController.Update(100000 / 10);


    //driveController.Update(100000 / 10);

    //DebugUltrasound();

}
