#define SAMPLE_TIME 15

#define IS_MAIN true
#define PAMI_TYPE 0 // 0 -> noir 1 -> gris



#if IS_MAIN
    
    #define DEATH_TIME 90 // in s
    #define LEVEL_PIN 23
    
    #include "settings/main/motor_def.h";
#include "settings/main/drive_def.h";
#include "settings/main/radar.h";
#include "settings/main/claw.h";
#include "settings/main/arm.h";
#else
    #define DEATH_TIME 10 // in s
    
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


#include "ESPNowStruct.hpp"
#include "ESPNowMaster.hpp"
#include "ESPNowSlave.hpp"

#include <LCD-I2C.h>
#include <Wire.h>

LCD_I2C lcd(0x27, 16, 2);
int points = 0;



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

#if IS_MAIN
    Arm myArm(PIN_ARM, ARM_TIME);
Claw myClaw(PIN_CLAW_1, PIN_CLAW_2, CLAW_TIME);
ESPNowMaster espNowMaster;

#else
    Radar radar(TRIGGER_PIN, ECHO_PIN);
ESPNowSlave espNowSlave(& taskControler, & positionTaskBuilder);

#endif

void setup()
{
    Serial.begin(115200);
    
    #if IS_MAIN
        
        pinMode(LEVEL_PIN, INPUT);
    
    if (digitalRead(LEVEL_PIN) == HIGH) //bleu
    {
        command_data pami_gris_cmd;
        pami_gris_cmd.cmd_1 = BLUE_ONE;
        pami_gris_cmd.cmd_2 = AUTO_ON;
        pami_gris_cmd.cmd_3 = START;
        
        uint8_t mac_gris[] = {0xE4, 0x65, 0xB8, 0x79, 0x83, 0x14};
        
        slave_data pami_gris;
        memcpy(pami_gris.mac_address, mac_gris, 6);
        pami_gris.cmd_data = pami_gris_cmd;
        
        command_data pami_noir_cmd;
        pami_noir_cmd.cmd_1 = BLUE_TWO;
        pami_noir_cmd.cmd_2 = AUTO_ON;
        pami_noir_cmd.cmd_3 = START;
        
        uint8_t mac_noir[] = {0xE4, 0x65, 0xB8, 0x76, 0x94, 0x44};
        
        slave_data pami_noir;
        memcpy(pami_noir.mac_address, mac_noir, 6);
        pami_noir.cmd_data = pami_noir_cmd;
        
        
        espNowMaster.AddSlave(pami_gris);
        espNowMaster.AddSlave(pami_noir);
        
    }
    else    //yellow
    {
        command_data pami_gris_cmd;
        pami_gris_cmd.cmd_1 = YELLOW_ONE;
        pami_gris_cmd.cmd_2 = AUTO_ON;
        pami_gris_cmd.cmd_3 = START;
        
        uint8_t mac_gris[] = {0xE4, 0x65, 0xB8, 0x79, 0x83, 0x14};
        
        slave_data pami_gris;
        memcpy(pami_gris.mac_address, mac_gris, 6);
        pami_gris.cmd_data = pami_gris_cmd;
        
        command_data pami_noir_cmd;
        pami_noir_cmd.cmd_1 = YELLOW_TWO;
        pami_noir_cmd.cmd_2 = AUTO_ON;
        pami_noir_cmd.cmd_3 = START;
        
        uint8_t mac_noir[] = {0xE4, 0x65, 0xB8, 0x76, 0x94, 0x44};
        
        slave_data pami_noir;
        memcpy(pami_noir.mac_address, mac_noir, 6);
        pami_noir.cmd_data = pami_noir_cmd;
        
        
        espNowMaster.AddSlave(pami_gris);
        espNowMaster.AddSlave(pami_noir);
    }
    
    espNowMaster.Init();
    
    
    myClaw.Init();
    pinMode(35, INPUT);
    lcd.begin();
    lcd.display();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Nombre de points:");
    lcd.setCursor(0, 1);
    lcd.print("32");
    
    myArm.Init();
    Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
    
    #else
        radar.Init();
    
    espNowSlave.Init();
    
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
    
    taskControler.SetAutoMode(false);
    
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
            }
            
            // Move remaining data to the beginning of the buffer
            memmove(buffer, buffer + i + 1, bufferIndex - i - 1);
            bufferIndex -= i + 1;
            i = -1; // Reset i to start from the beginning of the buffer
        }
    }
    return parsed_number;
}
void loop()
{
    
    #if IS_MAIN
        
        while(digitalRead(35) != LOW) {
        Serial.println("attente");
    }
    
    
    if (competition == true && ok ==  1) {
        
        espNowMaster.Start();
        //part 1 -> go to the line and rotate -> open claw
        taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 95));
        taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, -90));
        taskControler.AddTask(new ClawTask(& myClaw, OPEN));
        
        //part 2 -> Recolt the flowers
        taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 95)); //94, 90
        taskControler.AddTask(new ClawTask(& myClaw, CLOSE));
        taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, -50)); // AJOUT strat2
        //taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 55));Strat1
        taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 65 ));
        
 
        //part 3 -> Cash the flowers
        // taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, -90));Strat1
        //taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 50));Strat1
        taskControler.AddTask(new ClawTask(& myClaw, OPEN));
        //taskControler.AddTask(new ReverseTask(& driveControler, & valueConverter, 50));Strat1
        taskControler.AddTask(new ReverseTask(& driveControler, & valueConverter, 30)); //Ajout strat2
        
        //part 4 -> Go to next circle
        //taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, -152)); Strat1
        taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, 140)); // ajout strat 2
        //taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 76)); //Strat1
        taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 76)); // Ajout start 2
        //taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, -10)); //Strat1
        taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, 45));// Ajout start 2
        //taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 62));// strat1
        taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 53));// Ajout start 2
        taskControler.AddTask(new ClawTask(& myClaw, CLOSE));
        
        
        //part 5 -> Align for cash in
        //taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, 72));// Strat 1
        taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, 60));// Ajout start 2
        //taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 22));//Strat 1
        taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 35));// Ajout start 2
        //taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, -90)); //Strat1
        taskControler.AddTask(new RotationTask(& driveControler, & valueConverter, -90));// Ajout start 2
        //taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 42)); // strat1
        taskControler.AddTask(new ForwardTask(& driveControler, & valueConverter, 75));// Ajout start 2
        taskControler.AddTask(new ClawTask(& myClaw, OPEN));
        //taskControler.AddTask(new ReverseTask(& driveControler, & valueConverter, 25));//Strat1
        taskControler.AddTask(new ReverseTask(& driveControler, & valueConverter, 15));// Ajout start 2
        
        
        taskControler.SetAutoMode(true);
        taskControler.Start();
        ok = 0;
    }
    
    while(Serial2.available() > 0)
    {
        char incomingByte = Serial2.read();
        
        // Store incoming byte in the buffer
        buffer[bufferIndex++] = incomingByte;
        
        // Check if the buffer is full
        if (bufferIndex >= BUFFER_SIZE)
        {
            processBuffer(); // Process the buffer when it's full
        }
    }
    
    // Process remaining data in the buffer
    if (bufferIndex > 0)
    {
        processBuffer();
    }
    
    
    
    
    espNowMaster.Update();
    
    lcd.setCursor(0, 0);
    lcd.print("Nombre de points: ");
    lcd.print("32");
    lcd.setCursor(0,1);
    lcd.print("X:");
    lcd.print(positionControler.GetCurrentPoint().x);
    lcd.print(" Y:");
    lcd.print(positionControler.GetCurrentPoint().y);
    lcd.print(" A:");
    lcd.print(positionControler.GetCurrentAngle());
    
    #endif
    
    taskControler.Update();
    
    delay(5);
}