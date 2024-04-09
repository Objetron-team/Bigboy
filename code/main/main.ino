#define SAMPLE_TIME 15

#define IS_MAIN true
#define PAMI_TYPE 0 // 0 -> noir 1 -> gris

#if IS_MAIN
    #include "settings/main/motor_def.h";
    #include "settings/main/drive_def.h";
    #include "settings/main/bluetooth_config.h";
    #include "settings/main/radar.h";
    #include "settings/main/claw.h";
    #include "settings/main/arm.h";
#else
    #include "settings/pami/motor_def.h";
    #include "settings/pami/drive_def.h";
    #include "settings/pami/bluetooth_config.h";
    #include "settings/pami/radar.h";
#endif

#include "PIDMotor.hpp"
#include "DriveControler.hpp"
#include "PositionControler.hpp"
#include "TaskControler.hpp"
#include "BluetoothSerial.h"
#include "Arm.hpp"
#include "Claw.hpp"
#include "Radar.hpp"
#include "PositionTaskBuilder.hpp"


#if IS_MAIN
    Arm myArm(PIN_ARM, ARM_TIME);
    Claw myClaw(PIN_CLAW_1, PIN_CLAW_2, CLAW_TIME);
#else
    Radar radar(TRIGGER_PIN, ECHO_PIN);
#endif

BluetoothSerial SerialBT;

#define RXp2 16
#define TXp2 17

const int BUFFER_SIZE = 64; // Adjust the buffer size as needed
char buffer[BUFFER_SIZE];
int bufferIndex = 0;
int number = 100;

PIDMotor motorL(MOTOR_L_PIN_1, MOTOR_L_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);
PIDMotor motorR(MOTOR_R_PIN_1, MOTOR_R_PIN_2, MOTOR_ACCELERATION, MOTOR_MAX_SPEED, MOTOR_MIN_SPEED, MOTOR_THRESHOLD_SPEED, PAMI_DUAL_MODE);

ValueConverter valueConverter(ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

DriveControler driveControler(&motorL, &motorR);
PositionControler positionControler(&driveControler, ENCODER_RESOLUTION, WHEEL_DIAMETER, WHEEL_DISTANCE);

TaskControler taskControler(&positionControler, &driveControler, &valueConverter);

PositionTaskBuilder positionTaskBuilder(&positionControler, &driveControler, &valueConverter);

void setup()
{

#if IS_MAIN
    myClaw.Init();
    myArm.Init();
    // Radar setup
    Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
#else
    radar.Init();
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
    SerialBT.begin(device_name); // Bluetooth device name

    taskControler.SetAutoMode(false);
}

double global_target = 0;
double global_target_2 = 0;

void SerialCommande()
{

    if (SerialBT.available() > 0)
    {
        // if(Serial.available() > 0) {

        String commande = SerialBT.readStringUntil('\n');
        // String commande = Serial.readStringUntil('\n');
        char commande_type = commande.charAt(0);

        switch (commande_type)
        {
        case 'z':
        {
            // create Points array  blue 1
            Point points[3] = {
                {0, 0},
                {20, 0},
                {20, -90},
            };

            BasicTask *task = positionTaskBuilder.CreateTasksFromPoints(points, 3);

            taskControler.AddTask(task);
            break;
        }
        case 's':
        {
            // create Points array bleu 2
            Point points[3] = {
                {0, 0},
                {75, 0},
                {75, 140},
            };

            BasicTask *task = positionTaskBuilder.CreateTasksFromPoints(points, 3);

            taskControler.AddTask(task);
            break;
        }
        case 'q':
        {
            // create Points array yellow 1
            Point points[3] = {
                {0, 0},
                {20, 0},
                {20, 90},
            };

            BasicTask *task = positionTaskBuilder.CreateTasksFromPoints(points, 3);

            taskControler.AddTask(task);
            break;
        }
        case 'd':
        {
            // create Points array yellow 2
            Point points[3] = {
                {0, 0},
                {75, 0},
                {75, -140},
            };

            BasicTask *task = positionTaskBuilder.CreateTasksFromPoints(points, 3);

            taskControler.AddTask(task);
            break;
        }
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
        {
            // create Points array
            Point points[7] = {
                {0, 0},
                //{40,0},
                {80, 0},
                {80, 40},
                {40, 40},
                {40, 0},
                {0, 0}};

            BasicTask *task = positionTaskBuilder.CreateTasksFromPoints(points, 7);

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

        case 't':
            myArm.Open();
            break;
        case 'g':
            myArm.Close();
            break;
        default:
            break;
        }
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

            if (number < 30)
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

void loop()
{
    SerialCommande();

#if IS_MAIN

    while (Serial2.available() > 0)
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

    #else

        double distance_mm = radar.GetDistance();

        if (distance_mm < 200)
        {
            driveControler.UrgentStop();
        }
    #endif

    taskControler.Update();
    // taskControler.Debug();

    // Serial.println(MAX_SPEED_PULSE);

    // driveControler.Debug();

    delay(5);
}