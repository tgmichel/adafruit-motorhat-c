#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "AdafruitMotorHat.h"

void setAllPWM(word i2c, word on, word off){
        wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_ON_L, on & 0xFF);
        wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_ON_H, on >> 8);
        wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_OFF_L, off & 0xFF);
        wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_OFF_H, off >> 8);
}

void setPWM(word i2c, word pin, word on, word off){
        wiringPiI2CWriteReg8(i2c, PWM_LED0_ON_L + 4 * pin, on & 0xFF);
        wiringPiI2CWriteReg8(i2c, PWM_LED0_ON_H + 4 * pin, on >> 8);
        wiringPiI2CWriteReg8(i2c, PWM_LED0_OFF_L + 4 * pin, off & 0xFF);
        wiringPiI2CWriteReg8(i2c, PWM_LED0_OFF_H + 4 * pin, off >> 8);
}

void setPin(word i2c, word pin, word value){
    if(pin < 0 || pin > 15){
        printf("PWM pin must be between 0 and 15 inclusive.  Received '%d'\n", pin);
        return;
    }

    switch(value){
        case 0:
                setPWM(i2c, pin, 0, 4096);
                break;
        case 1:
                setPWM(i2c, pin, 4096, 0);
                break;
        default:
                printf("PWM pin value must be 0 or 1.  Received '%d'\n", pin);
                return;
    }
}

static int oneStep(int mode,word i2c,int currentStep,int MICROSTEPS){
	int pwm_a = 255;
	int pwm_b = 255;

	if(mode == MODE_SINGLE_STEP) {
	    int isOdd = (int)(currentStep/(MICROSTEPS/2)) % 2;
	    if(isOdd == 0) {
	    	int correction = (int)MICROSTEPS/2;
	    	currentStep = currentStep + correction;
	    }
	    else {
	    	currentStep = currentStep + MICROSTEPS;
	    }
	}

	if(mode == MODE_DOUBLE_STEP) {
	    int isEven = (int)(currentStep/(MICROSTEPS/2)) % 2;
	    if(isEven != 0) {
	    	int correction = (int)MICROSTEPS/2;
	    	currentStep = currentStep + correction;
	    }
	    else {
	    	currentStep = currentStep + MICROSTEPS;
	    }
	}

	if(mode == MODE_INTERLEAVE_STEP) {
	    int inter = (int) MICROSTEPS / 2;
	    currentStep = currentStep + inter;
	}

	if(mode == MODE_MICRO_STEP) {
		int *MICROSTEP_CURVE;

		if(MICROSTEPS == 16) {
			int tmp_curve[18] = {0, 12, 25, 50, 74, 98, 120, 142, 160, 180, 196, 212, 224, 236, 243, 250, 252, 255};
			MICROSTEP_CURVE = &tmp_curve;
		}
		else if(MICROSTEPS == 8) {
			int tmp_curve[9] = {0, 50, 98, 142, 180, 212, 236, 250, 255};
			MICROSTEP_CURVE = &tmp_curve;
		}
		

		currentStep = currentStep + 1;
		pwm_a = 0;
		pwm_b = 0;
		if ((currentStep >= 0) && (currentStep < MICROSTEPS)){
			pwm_a = MICROSTEP_CURVE[MICROSTEPS - currentStep];
            pwm_b = MICROSTEP_CURVE[currentStep];
		} else if ((currentStep >= MICROSTEPS) && (currentStep < MICROSTEPS*2)) {
			pwm_a = MICROSTEP_CURVE[currentStep - MICROSTEPS];
            pwm_b = MICROSTEP_CURVE[MICROSTEPS*2 - currentStep];
		} else if ((currentStep >= MICROSTEPS*2) && (currentStep < MICROSTEPS*3)) {
			pwm_a = MICROSTEP_CURVE[MICROSTEPS*3 - currentStep];
            pwm_b = MICROSTEP_CURVE[currentStep - MICROSTEPS*2];
		} else if ((currentStep >= MICROSTEPS*3) && (currentStep < MICROSTEPS*4)) {
			pwm_a = MICROSTEP_CURVE[currentStep - MICROSTEPS*3];
            pwm_b = MICROSTEP_CURVE[MICROSTEPS*4 - currentStep];
		}

	}

    currentStep = currentStep + (MICROSTEPS * 4);
    currentStep = currentStep % (MICROSTEPS * 4);

    setPWM(i2c,PWMA,0,pwm_a*16);
    setPWM(i2c,PWMB,0,pwm_b*16);

    int *coil;

	if(mode == MODE_MICRO_STEP) {
		if ((currentStep>= 0) && (currentStep < MICROSTEPS)) {
			int tmp[4] = {1, 1, 0, 0};
			coil = &tmp;
		} else if ((currentStep >= MICROSTEPS) && (currentStep < MICROSTEPS*2)) {
			int tmp[4] = {0, 1, 1, 0};
			coil = &tmp;
		} else if ((currentStep >= MICROSTEPS*2) && (currentStep < MICROSTEPS*3)) {
			int tmp[4] = {0, 0, 1, 1};
			coil = &tmp;
		} else if ((currentStep >= MICROSTEPS*3) && (currentStep < MICROSTEPS*4)) {
			int tmp[4] = {1, 0, 0, 1};
			coil = &tmp;
		}
	}
	else {
		int coilMatrix[8][4]={
		    {1, 0, 0, 0},
		    {1, 1, 0, 0},
		    {0, 1, 0, 0},
		    {0, 1, 1, 0},
		    {0, 0, 1, 0},
		    {0, 0, 1, 1},
		    {0, 0, 0, 1},
		    {1, 0, 0, 1}
		};

		int coilKey = (int) currentStep/(MICROSTEPS/2);
		coil = &coilMatrix[coilKey];
	}

	setPin(i2c,AIN2,coil[0]);
	setPin(i2c,BIN1,coil[1]);
	setPin(i2c,AIN1,coil[2]);
	setPin(i2c,BIN2,coil[3]);

    return currentStep;
}

static void releaseMotors(word i2c) {
	delay(100);
	setPin(i2c,AIN1,0);
	setPin(i2c,AIN2,0);
	setPin(i2c,BIN1,0);
	setPin(i2c,BIN2,0);
	setPin(i2c,CIN1,0);
	setPin(i2c,CIN2,0);
	setPin(i2c,DIN1,0);
	setPin(i2c,DIN2,0);
}

static word init(){
    //Setup I2C
    word i2c = wiringPiI2CSetup(ADAFRUIT_MOTORHAT);

    //Setup PWM
    setAllPWM(i2c, 0, 0);
    wiringPiI2CWriteReg8(i2c, PWM_MODE2, PWM_OUTDRV);
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, PWM_ALLCALL);
    delay(5);
    word mode1 = wiringPiI2CReadReg8(i2c, PWM_MODE1) & ~PWM_SLEEP;
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, mode1);
    delay(5);

    //Set PWM frequency
    word prescale = (int)(25000000.0 / 4096.0 / PWM_FREQUENCY - 1.0);
    word oldmode = wiringPiI2CReadReg8(i2c, PWM_MODE1);
    word newmode = oldmode & 0x7F | 0x10;
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, newmode);
    wiringPiI2CWriteReg8(i2c, PWM_PRESCALE, prescale);
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, oldmode);
    delay(5);
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, oldmode | 0x80);

    return i2c;
}

AdafruitMotorHat_container const AdafruitMotorHat = { oneStep,releaseMotors,init };