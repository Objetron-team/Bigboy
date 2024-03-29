
// Motor definition

//  Left motor pins
#define MOTOR_L_PIN_1 32
#define MOTOR_L_PIN_2 33

//  Right motor pins
#define MOTOR_R_PIN_1 25
#define MOTOR_R_PIN_2 26

//  Global motor parameters
#define MOTOR_ACCELERATION 10
#define MOTOR_MAX_SPEED 40
#define MOTOR_MIN_SPEED 0
#define MOTOR_THRESHOLD_SPEED 3

//  Dual mode, depending on the motor driver
#define PAMI_DUAL_MODE true

// Encoder definition

#define ENCODER_L_PIN_A 18
#define ENCODER_L_PIN_B 19

#define ENCODER_R_PIN_A 39
#define ENCODER_R_PIN_B 36

#define ENCODER_RESOLUTION 1786  // pulses per revolution
#define ENCODER_MAX_FREQ 6400   // Hz
#define WHEEL_DIAMETER 0.035    // meters    
#define WHEEL_DISTANCE 0.112 / 2   // meters


// PID definition

#define LS_PID_KP 1
#define LS_PID_KI 0
#define LS_PID_KD 0

#define RS_PID_KP 1
#define RS_PID_KI 0
#define RS_PID_KD 0