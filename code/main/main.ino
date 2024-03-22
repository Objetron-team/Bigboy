#include "PIDMotor.cpp";
#include "DriveControler.cpp";
#include "PositionControler.cpp";

#include "settings/pami_motor_def.h";
#include "settings/pami_drive_def.h";

#define SAMPLE_TIME 5

PIDMotor motorL(MOTOR_L_PIN_1, MOTOR_L_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);
PIDMotor motorR(MOTOR_R_PIN_1, MOTOR_R_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);

DriveControler driveControler(&motorL, &motorR);

PositionControler positionControler(&driveControler);

void setup () { 

    // Motor setup
    motorL.InitEncoder(ENCODER_L_PIN_A, ENCODER_L_PIN_B, ENCODER_MAX_FREQ, ENCODER_RESOLUTION, WHEEL_DIAMETER);
    motorR.InitEncoder(ENCODER_R_PIN_A, ENCODER_R_PIN_B, ENCODER_MAX_FREQ, ENCODER_RESOLUTION, WHEEL_DIAMETER);

    // PID setup
    motorL.InitPID(LS_PID_KP, LS_PID_KI, LS_PID_KD, SAMPLE_TIME);
    motorR.InitPID(RS_PID_KP, RS_PID_KI, RS_PID_KD, SAMPLE_TIME);

    // DriveControler setup
    driveControler.InitPid(DISTANCE_KP, DISTANCE_KI, DISTANCE_KD, ANGLE_KP, ANGLE_KI, ANGLE_KD, SAMPLE_TIME);

    Serial.begin ( 115200 );
}

bool PID_ENABLE = false;

float global_target = 0;
float global_target_2 = 0;

void SerialCommande(){

    if(Serial.available() > 0){

        String commande = Serial.readStringUntil('\n');
        char commande_type = commande.charAt(0);
        
        switch(commande_type){
            case 'z':
                
                positionControler.AddPoint({20, 0});
                positionControler.AddPoint({40, 0});
                positionControler.AddPoint({40, 20});
                positionControler.AddPoint({20, 20});


                break;
            case 's':
                break;
            case 'q':
                break;
            case 'd':
                break;
            case 'e':
                break;
            default:
                break;
        }
        
    }

}

void loop () {    
    SerialCommande();

    Serial.print("Task_id:");
    Serial.print(positionControler.GetCurrentTask()->id);
    Serial.print(",");
    Serial.print("Task_error_m:");
    Serial.print(positionControler.GetCurrentTask()->MoveError(positionControler.GetCurrentPoint()));
    Serial.print(",");
    Serial.print("Task_error_r:");
    Serial.print(positionControler.GetCurrentTask()->RotateError(positionControler.GetCurrentPoint()));
    Serial.print(",");
    Serial.print("Task_distance:");
    Serial.print(positionControler.GetCurrentTask()->GetDistance()  );
    Serial.print(",");
    Serial.print("Task_angle:");
    Serial.println(positionControler.GetCurrentTask()->GetAngle());


    positionControler.Update();

    delay(SAMPLE_TIME);
}