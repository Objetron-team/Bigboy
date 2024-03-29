#include "PIDMotor.hpp"
#include "DriveControler.hpp"
#include "PositionControler.hpp"
#include "TaskControler.hpp"
#include "BluetoothSerial.h"
#include "Claw.hpp"
#include "Radar.hpp"
#include "PositionTaskBuilder.hpp"

#define SAMPLE_TIME 15

#define IS_MAIN false

#if IS_MAIN
    #include "settings/main/motor_def.h";
    #include "settings/main/drive_def.h";
    #include "settings/main/bluetooth_config.h";
    #include "settings/main/radar.h";
    #include "settings/main/claw.h";
    
    Claw myClaw(PIN_CLAW_1, PIN_CLAW_2,CLAW_TIME);

#else
    #include "settings/pami/motor_def.h";
    #include "settings/pami/drive_def.h";
    #include "settings/pami/bluetooth_config.h";
    #include "settings/pami/radar.h";
#endif

BluetoothSerial SerialBT;

Radar radar(TRIGGER_PIN, ECHO_PIN);

PIDMotor motorL(MOTOR_L_PIN_1, MOTOR_L_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);
PIDMotor motorR(MOTOR_R_PIN_1, MOTOR_R_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);

ValueConverter valueConverter(ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

DriveControler driveControler(&motorL, &motorR);
PositionControler positionControler(&driveControler, ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

TaskControler taskControler(&positionControler,&driveControler, &valueConverter);

PositionTaskBuilder positionTaskBuilder(&positionControler, &driveControler, &valueConverter);

void setup () { 

    radar.Init();


    #if IS_MAIN
        myClaw.Init();
    #endif

    // Motor setup
    motorL.InitEncoder(ENCODER_L_PIN_A, ENCODER_L_PIN_B, ENCODER_MAX_FREQ, ENCODER_RESOLUTION, WHEEL_DIAMETER);
    motorR.InitEncoder(ENCODER_R_PIN_A, ENCODER_R_PIN_B, ENCODER_MAX_FREQ, ENCODER_RESOLUTION, WHEEL_DIAMETER);

    // PID setup
    motorL.InitPID(LS_PID_KP, LS_PID_KI, LS_PID_KD, SAMPLE_TIME);
    motorR.InitPID(RS_PID_KP, RS_PID_KI, RS_PID_KD, SAMPLE_TIME);

    // DriveControler setup
    driveControler.InitPid(DISTANCE_KP, DISTANCE_KI, DISTANCE_KD, ANGLE_KP, ANGLE_KI, ANGLE_KD, SAMPLE_TIME);

    Serial.begin ( 115200 );
    SerialBT.begin(device_name); //Bluetooth device name

    taskControler.SetAutoMode(false);

}

double global_target = 0;
double global_target_2 = 0;

void SerialCommande(){

    if(SerialBT.available() > 0){

        String commande = SerialBT.readStringUntil('\n');
        char commande_type = commande.charAt(0);
        
        switch(commande_type){
            case 'z':
                
                global_target += 5000;

                break;
            case 's':

                global_target -= 5000;

                break;
            case 'q':

                global_target_2 += 1000;

                break;
            case 'd':

                global_target_2 -= 1000;

                break;
            // case 'e':

            //     #if IS_MAIN
            //         myClaw.Open();
            //     #endif

            //     break;
            // case 'a':
                
            //     #if IS_MAIN
            //         myClaw.Close();
            //     #endif

            //     break;

            case 'b':
                {
                    // create Points array
                    Point points[7] = {
                        {0,0},
                        {40,0},
                        {80,0},
                        {80,40},
                        {40,40},
                        {40,0},
                        {0,0}
                    };

                    BasicTask* task = positionTaskBuilder.CreateTasksFromPoints(points,7);

                    taskControler.AddTask(task);
                    break;
                }
            case 'y':
                taskControler.SetAutoMode(true);
                    break;
            case 'h':
                taskControler.SetAutoMode(false);
                break;
            case 'o':
                taskControler.Start();
                break;
            case 'p':
                taskControler.Stop();
                break;
            case 'n':
                taskControler.NextTask();
                break;
            case 'r':
                taskControler.Reset();
                break;

            default:
                break;
        }
        
    }

}

void loop () {    
    SerialCommande();

    double distance_mm = radar.GetDistance();
    if(distance_mm < 100){
        driveControler.UrgentStop();
    }

    taskControler.Update();
    //taskControler.Debug();

    //driveControler.Debug();

    delay(5);
}