#define SAMPLE_TIME 15

#define IS_MAIN true
#define PAMI_TYPE 0 // 0 -> noir 1 -> gris



#if IS_MAIN
    #include "settings/main/motor_def.h";
#include "settings/main/drive_def.h";
#include "settings/main/radar.h";
#include "settings/main/claw.h";
#include "settings/main/arm.h";
#else
    #include "settings/pami/motor_def.h";
#include "settings/pami/drive_def.h";
#include "settings/pami/radar.h";
#endif

#include "PIDMotor.hpp"
#include "DriveControler.hpp"
#include "PositionControler.hpp"
#include "TaskControler.hpp"


#include "Arm.hpp"
#include "Claw.hpp"
#include "Radar.hpp"
#include "PositionTaskBuilder.hpp"

#include < LCD - I2C.h>
#include < Wire.h>

LCD_I2C lcd(0x27, 16, 2);
int points = 0;


#if IS_MAIN
    Arm myArm(PIN_ARM, ARM_TIME);
Claw myClaw(PIN_CLAW_1, PIN_CLAW_2, CLAW_TIME);
#else
    Radar radar(TRIGGER_PIN, ECHO_PIN);
#endif



#define RXp2 16
#define TXp2 17

const int BUFFER_SIZE = 64; // Adjust the buffer size as needed
char buffer[BUFFER_SIZE];
int bufferIndex = 0;
int number = 100;
bool competition = true;
int ok = 1;

PIDMotor motorL(MOTOR_L_PIN_1, MOTOR_L_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);
PIDMotor motorR(MOTOR_R_PIN_1, MOTOR_R_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);

ValueConverter valueConverter(ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

DriveControler driveControler(& motorL, & motorR);
PositionControler positionControler(& driveControler, ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

TaskControler taskControler(& positionControler, & driveControler, & valueConverter);

PositionTaskBuilder positionTaskBuilder(& positionControler, & driveControler, & valueConverter);

void setup()
{
    
    #if IS_MAIN
        myClaw.Init();
    pinMode(35, INPUT);
    lcd.begin();
    lcd.display();
    lcd.backlight();
    //lcd.setCursor(0, 0);
    //lcd.print("nombre de points:");
    //lcd.setCursor(0, 1);
    //lcd.print(points);
    myArm.Init();
    // Radar setup
    Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
    #else
        radar.Init();
    driveControler.AddRadar( & radar, FRONT);
    #endif
    
    // Motor setup
    motorL.InitEncoder(ENCODER_L_PIN_A, ENCODER_L_PIN_B, ENCODER_MAX_FREQ_L, ENCODER_RESOLUTION, WHEEL_DIAMETER);
    motorR.InitEncoder(ENCODER_R_PIN_A, ENCODER_R_PIN_B, ENCODER_MAX_FREQ_R, ENCODER_RESOLUTION, WHEEL_DIAMETER);
    
    // PID setup
    motorL.InitPID(LS_PID_KP, LS_PID_KI, LS_PID_KD, SAMPLE_TIME);
    motorR.InitPID(RS_PID_KP, RS_PID_KI, RS_PID_KD, SAMPLE_TIME);
    
    // DriveControler setup
    driveControler.InitPid(DISTANCE_KP, DISTANCE_KI, DISTANCE_KD, ANGLE_KP, ANGLE_KI, ANGLE_KD, SAMPLE_TIME);
    
    Serial.begin(115200);
    
    taskControler.SetAutoMode(false);
}

double global_target = 0;
double global_target_2 = 0;

void SerialCommande() {
    
    if (Serial.available() > 0) {
        
        String commande = Serial.readStringUntil('\n');
        
        //commande fomart  -> {letter}{number}
        // ex: z50 -> avancer de 50 cm
        
        char letter = commande.charAt(0);
        double number = commande.substring(1).toDouble();
        
        switch(letter)
        {
            case'z':
            global_target = number;
            break;
            case's':
            global_target = -number;
            break;
            case'a':
            global_target_2 = number;
            break;
            case'd':
            global_target_2 = -number;
            break;
        }
        
        driveControler.Reset();
        driveControler.SetDistance(global_target);
        driveControler.SetAngle(global_target_2);
    }
}

int processBuffer()
    {
    int parsed_number = 0;
    // Process the data stored in the buffer
    // For example, you can print it or perform any other desired operation
    for (int i = 0; i < bufferIndex; i++)
        {
        if (buffer[i] == '\n')
            {
            // If newline character is encountered, print the message and reset buffer
            
            parsed_number = atoi(buffer); // Convert the buffer content to an integer
            if (parsed_number != 0)
                {
                number = parsed_number; // Update the global variable only if a valid number is parsed
            }
            
            if (number < 15)
                {
                driveControler.UrgentStop();
                taskControler.Stop();
            }
            else
                {
                taskControler.Start();
            }
            Serial.println();
            Serial.write(buffer, i + 1); // Print the message until the newline character
            Serial.println();
            
            // Move remaining data to the beginning of the buffer
            memmove(buffer, buffer + i + 1, bufferIndex - i - 1);
            bufferIndex -= i + 1;
            i = -1; // Reset i to start from the beginning of the buffer
        }
    }
    return parsed_number;
}
unsignedlong startTime = 0;
void loop()
    {
    positionControler.Update();
    
    lcd.setCursor(0, 0);
    lcd.print("x:");
    lcd.print(positionControler.GetCurrentPoint().x);
    lcd.setCursor(0, 1);
    lcd.print("y:");
    lcd.print(positionControler.GetCurrentPoint().y);
    lcd.setCursor(0, 2);
    lcd.print("current angle:");
    lcd.print(positionControler.GetCurrentAngle());
    // driveControler.Debug();
    
    delay(5);
}