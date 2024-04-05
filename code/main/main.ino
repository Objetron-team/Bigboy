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

Radar radar(TRIGGER_PIN, ECHO_PIN);
#endif

BluetoothSerial SerialBT;

#define RXp2 16
#define TXp2 17

const int BUFFER_SIZE = 64;  // Adjust the buffer size as needed
char buffer[BUFFER_SIZE];
int bufferIndex = 0;



PIDMotor motorL(MOTOR_L_PIN_1, MOTOR_L_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);
PIDMotor motorR(MOTOR_R_PIN_1, MOTOR_R_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);

ValueConverter valueConverter(ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

DriveControler driveControler(& motorL, & motorR);
PositionControler positionControler(& driveControler, ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

TaskControler taskControler(& positionControler, & driveControler, & valueConverter);

PositionTaskBuilder positionTaskBuilder(& positionControler, & driveControler, & valueConverter);

void setup() { 
    
    #if IS_MAIN
        myClaw.Init();
    // Radar setup
    Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
    #endif
    
    // Motor setup
    motorL.InitEncoder(ENCODER_L_PIN_A, ENCODER_L_PIN_B, ENCODER_MAX_FREQ, ENCODER_RESOLUTION, WHEEL_DIAMETER);
    motorR.InitEncoder(ENCODER_R_PIN_A, ENCODER_R_PIN_B, ENCODER_MAX_FREQ, ENCODER_RESOLUTION, WHEEL_DIAMETER);
    
    // PID setup
    motorL.InitPID(LS_PID_KP, LS_PID_KI, LS_PID_KD, SAMPLE_TIME);
    motorR.InitPID(RS_PID_KP, RS_PID_KI, RS_PID_KD, SAMPLE_TIME);
    
    // DriveControler setup
    driveControler.InitPid(DISTANCE_KP, DISTANCE_KI, DISTANCE_KD, ANGLE_KP, ANGLE_KI, ANGLE_KD, SAMPLE_TIME);
    
    Serial.begin(115200);
    SerialBT.begin(device_name); //Bluetooth device name
    
    
    
    taskControler.SetAutoMode(false);
    
}

double global_target = 0;
double global_target_2 = 0;

void SerialCommande() {
    
    if (Serial.available() > 0) {
        //if(Serial.available() > 0) {
        
        String commande = Serial.readStringUntil('\n');
        
        // convert the incoming command to number
        int number = atoi(commande.c_str());
        
        // convert to double
        global_target = (double) number;
        
        driveControler.SetDistance(global_target);
        
        if (global_target == 0) {
            driveControler.Reset();
        }
    }
    
}




void loop() {    
    SerialCommande();
    
    //motorL.SetSpeed(100);
    //motorR.SetSpeed(100);
    
    //motorL.Update();
    //motorR.Update();
    
    Serial.print("L:");
    Serial.print(motorL.GetSpeed());
    Serial.print(",");
    Serial.print("R:");
    Serial.println(motorR.GetSpeed());
    
    
    delay(5);
}