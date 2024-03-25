#include "PIDMotor.cpp";
#include "DriveControler.cpp";
#include "PositionControler.cpp";
#include "BluetoothSerial.h";
#include "Claw.cpp";
#include "Radar.cpp";

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


DriveControler driveControler(&motorL, &motorR);
PositionControler positionControler(&driveControler, ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

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

    positionControler.SetAutoMode(false);

}

float global_target = 0;
float global_target_2 = 0;

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
            case 'e':

                #if IS_MAIN
                    myClaw.Open();
                #endif

                break;
            case 'a':
                
                #if IS_MAIN
                    myClaw.Close();
                #endif

                break;

            case 'b':

                positionControler.AddPoint({20, 0});
                positionControler.AddPoint({20, 20});
                //positionControler.AddPoint({40, 20});
                //positionControler.AddPoint({20, 20});
                //positionControler.AddPoint({20, 0});

                break;

            case 'y':
                    break;

            case 'o':
                positionControler.Start();
                break;
            case 'p':
                positionControler.Stop();
                break;
            case 'n':
                positionControler.NextTask();
                break;
            case 'r':
                positionControler.Reset();
                break;

            default:
                break;
        }
        
    }

}

void loop () {    
    SerialCommande();

    float distance_mm = radar.GetDistance();
    if(distance_mm < 100){
        driveControler.UrgentStop();
    }

    Task* currentTask = positionControler.GetCurrentTask();

    Serial.print("Tasks:");
    Serial.print(positionControler.GetNumberOfTask());
    Serial.print(",");

    Serial.print("Distance_E:");
    Serial.print(currentTask->MoveError(positionControler.GetCurrentPoint()));
    Serial.print(",");

    Serial.print("Angle_E:");
    Serial.print(currentTask->RotateError(positionControler.GetCurrentPoint()));
    Serial.print(",");

    Serial.print("Distance:");
    Serial.print(currentTask->GetDistance());
    Serial.print(",");

    Serial.print("Angle:");
    Serial.print(currentTask->GetAngle());
    Serial.print(",");

    Serial.print("X:");
    Serial.print(positionControler.GetCurrentPoint().x);
    Serial.print(",");

    Serial.print("Y:");
    Serial.print(positionControler.GetCurrentPoint().y);
    Serial.print(",");

    Serial.print("cur_angle:");
    Serial.print(positionControler.GetCurrentAngle());
    Serial.print(",");



    Serial.print("Current:");
    Serial.println(positionControler.GetTaskId());
    //Serial.print(",");

    //driveControler.Update();

    positionControler.Update();


    delay(5);
}