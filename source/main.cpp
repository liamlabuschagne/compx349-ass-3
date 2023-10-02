#include "MicroBit.h"

MicroBit uBit;

void motorRun(int index, int direction, int speed){
	uint8_t buf[3]; 
	switch(index){
		case 0: // Left
			buf[0] = 0x00;
			buf[1] = direction;  // 0 forward,  1 backward
			buf[2] = speed;  

			uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
			break;	
		case 1: // Right
			buf[0] = 0x02;
			buf[1] = direction;  // 0 forward,  1 backward
			buf[2] = speed;  

			uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
			break;
		case 2: // Both
			buf[0] = 0x00;
			buf[1] = direction;  // 0 forward,  1 backward
			buf[2] = speed;  

			uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
			buf[0] = 0x02;
			uBit.i2c.write( 0x20, buf, 3);
	}
}

void motorStop(int index){
	motorRun(index,0,0);
}

void writeLED(int led, int ledSwitch) {
	switch (led)
	{
	case 0:
		uBit.io.P8.setDigitalValue(ledSwitch);
		break;
	
	case 1:
		uBit.io.P12.setDigitalValue(ledSwitch);
		break;
	}
}

int readGreyscale(int side) { // return of 1 is white, 0 is black, -1 is invalid input
	switch (side)
	{
		case 0:
			return uBit.io.P13.getDigitalValue();
			break;
		case 1:
			return uBit.io.P14.getDigitalValue();
			break;
	}
	return -1;
}

int main()
{
	uBit.init();
	int ret;
	while(1){
		if (readGreyscale(0)) {
			motorRun(0, 0, 0x50);
			uBit.sleep(500);
			motorStop(0);
		}

		if (readGreyscale(1)) {
			motorRun(1, 0, 0x50);
			uBit.sleep(500);
			motorStop(1);
		}
	}
}
