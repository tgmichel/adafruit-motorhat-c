#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <wiringPi.h>
#include "AdafruitMotorHat.h"

void main(){

	// init
	word i2c = AdafruitMotorHat.init();

	// delay between oneStep calls
	int stepInMicroSecs = 1800;
	// 16 (1/16) by default. Can be also set to 8.
	int MICROSTEPS = 16;
	// MODE_SINGLE_STEP,MODE_DOUBLE_STEP,MODE_INTERLEAVE_STEP and MODE_MICRO_STEP can be used
	int mode = MODE_DOUBLE_STEP;

	int currentStep = 0;

	wiringPiSetup();

	pinMode(0, INPUT);
	pullUpDnControl(0,PUD_UP);
	int isOff = 1;
	int needToRelease = 0;

	while(1){

		isOff = digitalRead(0);

		if (isOff == 0) {
			currentStep = AdafruitMotorHat.oneStep(mode,i2c,currentStep,MICROSTEPS);
			needToRelease = 1;
		}
		else if (isOff == 1 && needToRelease == 1) {
			printf("Release\n");
			// once the microswitch is HIGH again, make sure you release the motors and reset the currentStep
			AdafruitMotorHat.releaseMotors(i2c);
			needToRelease = 0;
			currentStep = 0;
		}
		delayMicroseconds(stepInMicroSecs);
	}
}