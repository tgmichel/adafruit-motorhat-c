main: AdafruitMotorHat.o
	gcc example.c AdafruitMotorHat.o -o example -lwiringPi

AdafruitMotorHat.o: AdafruitMotorHat.c
	gcc -c AdafruitMotorHat.c

clean:
	rm *o main
