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


/*
    - adruino mega pins

    interrup -> 18,19,20,21,    2 and 3

    analogue write -> 2..13, 44..46

*/


Motor motorRight;
Motor motorLeft;

const bool PAMI_DUAL_INPUT = false;

const int MOTOR_RIGHT_SPEED_PIN = 2;
const int MOTOR_RIGHT_DIRECTION_PIN = 3;

const int MOTOR_LEFT_SPEED_PIN = 4;
const int MOTOR_LEFT_DIRECTION_PIN = 5;

const int acceleration_right = 50;  // % per sec
const int acceleration_left = 50;  // % per sec

float max_speed_left = 19.5f;
float max_speed_right = 19.5f;

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
const int WHEEL_ENCODER_PIN_LEFT_A = 20;
const int WHEEL_ENCODER_PIN_LEFT_B = 12;

Encoder encoderRight;
Encoder encoderLeft;

void InitEncoder(){

    encoderRight.Init(&motorRight,WHEEL_ENCODER_PIN_RIGHT_A,WHEEL_ENCODER_PIN_RIGHT_B);
    encoderLeft.Init(&motorLeft,WHEEL_ENCODER_PIN_LEFT_A,WHEEL_ENCODER_PIN_LEFT_B);

    attachInterrupt(digitalPinToInterrupt(WHEEL_ENCODER_PIN_RIGHT_A), CouterRight, CHANGE);

    //attachInterrupt(digitalPinToInterrupt(WHEEL_ENCODER_PIN_RIGHT_B), CouterRightB, CHANGE);


    attachInterrupt(digitalPinToInterrupt(WHEEL_ENCODER_PIN_LEFT_A), CouterLeft, FALLING);
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

    //pinMode(ULTRASOUND_ECHO_PIN,INPUT);


    //pinMode(ULTRASOUND_TRIGGER_PIN,OUTPUT);
    //digitalWrite(ULTRASOUND_TRIGGER_PIN, LOW);
    ultrasound.Init(ULTRASOUND_ECHO_PIN,ULTRASOUND_TRIGGER_PIN);
}


DriveController driveController;
PositionController positionController;

void setup() {

    Serial.begin(38400);

    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
    pinMode(ECHO_PIN, INPUT);

    InitUltrasound();

    InitMotor();
    InitEncoder();
    

    driveController.Init(&motorRight,&motorLeft,&encoderRight,&encoderLeft);
    positionController.Init(&driveController,&ultrasound);


    //define the path to follow
    //positionController.AddPoint({100,100});
    //positionController.AddPoint({200,100});
    //positionController.AddPoint({100,200});

    //delay(1000);

    //DebugPath();
}

void SerialCommande(){

    if(Serial.available() > 0){

        String commande = Serial.readStringUntil('\n');


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
                positionController.AddPoint({20,});
                positionController.AddPoint({20,-90});
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

void DebugPath(){
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

void DebugEncoder(){
    Serial.print("Encoder R:");
    Serial.print(encoderRight.GetRotationSpeed());
    Serial.print(",");

    Serial.print("Encoder R Counter:");
    Serial.print(encoderRight.GetCounter());
    Serial.print(",");

    Serial.print("Encoder R Direction:");
    Serial.print(encoderRight.GetDirection());
    Serial.print(",");

    Serial.print("Encoder L:");
    Serial.println(encoderLeft.GetRotationSpeed());
    //Serial.print(",");
}

void loop() {


    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
    if(measure == 0){
        measure = 1000000;
    }

    //if timeout -> measure = 1000000 
    
    float distance_mm = measure / 2.0 * SOUND_SPEED;
    
    //Debug(distance_mm / 10);
    DebugEncoder();
    //encoderRight.GetRotationSpeed();


    SerialCommande();

    positionController.Update(distance_mm / 10);

    //delay(5);

    //DebugUltrasound();

}
