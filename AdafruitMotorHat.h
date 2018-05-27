#define word    unsigned short

#define ADAFRUIT_MOTORHAT 0x60

#define PWM_FREQUENCY 3200.0
#define PWM_PRESCALE 0xFE

#define PWM_MODE1 0x00
#define PWM_MODE2 0x01

#define PWM_RESTART 0x80
#define PWM_SLEEP 0x10
#define PWM_ALLCALL 0x01
#define PWM_INVRT 0x10
#define PWM_OUTDRV 0x04

#define MOTOR_FORWARD 1
#define MOTOR_BACK 2
#define MOTOR_BRAKE 3
#define MOTOR_RELEASE 4

#define PWMA 8
#define AIN2 9
#define AIN1 10
#define PWMB 13
#define BIN2 12
#define BIN1 11

#define CIN2 3
#define CIN1 4
#define DIN2 6
#define DIN1 5

#define PWM_ALL_LED_ON_L 0xFA
#define PWM_ALL_LED_ON_H 0xFB
#define PWM_ALL_LED_OFF_L 0xFC
#define PWM_ALL_LED_OFF_H 0xFD

#define PWM_LED0_ON_L 0x06
#define PWM_LED0_ON_H 0x07
#define PWM_LED0_OFF_L 0x08
#define PWM_LED0_OFF_H 0x09

#define MODE_SINGLE_STEP 1
#define MODE_DOUBLE_STEP 2
#define MODE_INTERLEAVE_STEP 3
#define MODE_MICRO_STEP 4

/*
  Exports
*/
typedef struct { 
  int (* const oneStep)(int,word,int,int);
  void (* const releaseMotors)(word);
  word (* const init)(void);
} AdafruitMotorHat_container;

extern AdafruitMotorHat_container const AdafruitMotorHat;