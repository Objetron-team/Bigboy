#include "PIDMotor.cpp";
#include "DriveControler.cpp";
#include "PositionControler.cpp";
#include "BluetoothSerial.h";
#include "Claw.cpp";

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



#define SAMPLE_TIME 15

PIDMotor motorL(MOTOR_L_PIN_1, MOTOR_L_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);
PIDMotor motorR(MOTOR_R_PIN_1, MOTOR_R_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);


DriveControler driveControler(&motorL, &motorR);
PositionControler positionControler(&driveControler);

void setup () { 

    pinMode(TRIGGER_PIN,OUTPUT);
    digitalWrite(ECHO_PIN, LOW);

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
            default:
                break;
        }
        
    }

}

void loop () {    
    SerialCommande();

    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
    if(measure == 0){
        measure = 1000000;
    }

    float distance_mm = measure / 2.0 * SOUND_SPEED;

    driveControler.SetDistance(global_target);
    driveControler.SetAngle(global_target_2);

    Serial.print("Distance_echo:");
    Serial.print(distance_mm);
    Serial.print(",");

    Serial.print("Distance:");
    Serial.print(driveControler.GetDistance());
    Serial.print(",");

    Serial.print("Angle:");
    Serial.print(driveControler.GetAngle());
    Serial.print(",");

    Serial.print("Target:");
    Serial.println(global_target);

    if(distance_mm < 100){
        driveControler.UrgentStop();
    }

    driveControler.Update();

    //positionControler.Update();

    delay(5);
}