#define SAMPLE_TIME 15

#define IS_MAIN true
#define PAMI_TYPE 0 // 0 -> noir 1 -> gris



#if IS_MAIN
    
    #define DEATH_TIME 100 // in s
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

DriveControler driveControler( & motorL, & motorR);
PositionControler positionControler( & driveControler, ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

TaskControler taskControler( & positionControler, & driveControler, & valueConverter);

PositionTaskBuilder positionTaskBuilder( & positionControler, & driveControler, & valueConverter);

#if IS_MAIN
    Arm myArm(PIN_ARM, ARM_TIME);
Claw myClaw(PIN_CLAW_1, PIN_CLAW_2, CLAW_TIME);
ESPNowMaster espNowMaster;

#else
    Radar radar(TRIGGER_PIN, ECHO_PIN);
ESPNowSlave espNowSlave( & taskControler, & positionTaskBuilder);

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
    
    myArm.Init();
    Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
    
    #else
        radar.Init();
    
    espNowSlave.Init();
    
    driveControler.AddRadar(& radar, FRONT);
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

double global_target = 0;
double global_target_2 = 0;


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
            else
            {
                
            }
            //Serial.println();
            //Serial.write(buffer, i + 1); // Print the message until the newline character
            //Serial.println();
            
            // Move remaining data to the beginning of the buffer
            memmove(buffer, buffer + i + 1, bufferIndex - i - 1);
            bufferIndex -= i + 1;
            i = -1; // Reset i to start from the beginning of the buffer
        }
    }
    return parsed_number;
}
unsigned long startTime = 0;
void loop()
{
    
    
    #if IS_MAIN
        
        if (ok == 1) {
            unsigned long startTime = millis();
            myArm.Open();
    }
    
    while(digitalRead(35) != LOW) {
        Serial.println("attente");
    }
    
    unsigned long currentTime = millis();
    long TimeUntilStop = currentTime - startTime;
    if (TimeUntilStop < 90000) {
    } else {
        Serial.print("stop");
        driveControler.UrgentStop();
        taskControler.Stop();
    }
    
    //SerialCommande();
    if (competition == true && ok ==  1) {
        
        espNowMaster.Start();
        Point points1[4] = { 
            {0, 0},
            {87, 0},
            {87 , -145},
            {35 , -145},
        };
        BasicTask *task1 = positionTaskBuilder.CreateTasksFromPoints(points1, 4); 
        //ClawTask *task2 = new ClawTask(&myClaw);


        //BasicTask *task3 = positionTaskBuilder.CreateTasksFromPoints(points3, 2); 
        ReverseTask *task4 = new ReverseTask(&driveControler, &valueConverter,30);
        
        Point points5[6] = { 
            {60,-42},
            {100,-22},
            {120,0},
            {180,0},
            {190,0},
            {230,5},
        };
        
        BasicTask * task5 = positionTaskBuilder.CreateTasksFromPoints(points5, 6); 
        taskControler.AddTask(task1);
        //taskControler.AddTask(task2);
        //taskControler.AddTask(task3);
        taskControler.AddTask(task4);
        taskControler.AddTask(task5);
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
    
    lcd.setCursor(0, 0);
    lcd.print("x:");
    lcd.print(positionControler.GetCurrentPoint().x);
    lcd.setCursor(0, 1);
    lcd.print("y:");
    lcd.print(positionControler.GetCurrentPoint().y);
    lcd.setCursor(0, 2);
    lcd.print("current angle:");
    lcd.print(positionControler.GetCurrentAngle());
    
    
    espNowMaster.Update();
    
    #endif
    
    taskControler.Update();
    //taskControler.Debug();
    
    
    delay(5);
}