# Adafruit MotoHAT C Port

This a rather limited C port - only Stepper code and oneStep method - of [Adafruit's official Python Library](https://github.com/adafruit/Adafruit-Motor-HAT-Python-Library). The [Motorhat](https://www.adafruit.com/product/234) uses the [TB6612 Motor driver](https://cdn-shop.adafruit.com/datasheets/TB6612FNG_datasheet_en_20121101.pdf) so you can probably use this code with it.

Tested on a Raspberry Pi 3 B+.

# Depedencies

  - [WiringPi](http://wiringpi.com/download-and-install/)

# Configuration

  - Probably a good idea to increase the default i2c communication speed in "/boot/config.txt" (i.e. dtparam=i2c_arm_baudrate=3200000)
  - PWM_FREQUENCY is setup to 3200.00Hz by default in "AdafruitMotorHat.h"
  - MODE_SINGLE_STEP, MODE_DOUBLE_STEP, MODE_INTERLEAVE_STEP and MODE_MICRO_STEP are implemented.
  - MODE_MICRO_STEP can operate on 1/8 and 1/16

# Run the example

  - make
  - ./example

In the example code a microswitch in GPIO 17 (WiginPi GPIO 0) is expected to be HIGH by default. When LOW will run the motor.

[Original library](https://github.com/adafruit/Adafruit-Motor-HAT-Python-Library) written by Limor Fried for Adafruit Industries. MIT license, all text above must be included in any redistribution.
