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

int main()
{
	uBit.init();
	while(1){
		motorRun(0,0,0x80);
		uBit.sleep(1000);
		motorStop(0);
		motorRun(0,1,0x80);
		uBit.sleep(1000);
		motorStop(0);
		motorRun(1,0,0x80);
		uBit.sleep(1000);
		motorStop(1);
		motorRun(1,1,0x80);
		uBit.sleep(1000);
		motorStop(1);
		motorRun(2,0,0x80);
		uBit.sleep(1000);
		motorStop(2);
		motorRun(2,1,0x80);
		uBit.sleep(1000);
		motorStop(2);
	}
}
