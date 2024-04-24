
// Motor definition

//  Left motor pins
#define MOTOR_L_PIN_1 25
#define MOTOR_L_PIN_2 26

//  Right motor pins
#define MOTOR_R_PIN_1 32
#define MOTOR_R_PIN_2 33

//  Global motor parameters
#define MOTOR_ACCELERATION 20
#define MOTOR_MAX_SPEED 100
#define MOTOR_MIN_SPEED 0
#define MOTOR_THRESHOLD_SPEED 0

//  Dual mode, depending on the motor driver
#define PAMI_DUAL_MODE false

// Encoder definition

#define ENCODER_L_PIN_A 39
#define ENCODER_L_PIN_B 36

#define ENCODER_R_PIN_A 19
#define ENCODER_R_PIN_B 18

#define ENCODER_RESOLUTION 8400  // pulses per revolution
#define ENCODER_MAX_FREQ_L 12200 // Hz
#define ENCODER_MAX_FREQ_R 12200 // Hz
#define WHEEL_DIAMETER 0.0805    // meters
#define WHEEL_DISTANCE 0.227 / 2 // meters

#define MAX_SPEED_PULSE ((ENCODER_MAX_FREQ_L + ENCODER_MAX_FREQ_R) / 2) * (MOTOR_MAX_SPEED / 100)

// PID definition

#define LS_PID_KP 1
#define LS_PID_KI 0
#define LS_PID_KD 0

#define RS_PID_KP 1
#define RS_PID_KI 0
#define RS_PID_KD 0