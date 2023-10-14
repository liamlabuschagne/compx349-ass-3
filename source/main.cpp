#include "MicroBit.h"
#define SPEED 50

void intersection();
void rightTurn();

enum Direction {
	FORWARD,
	BACKWARD
};

enum Motor {
	LEFT,
	RIGHT,
	ALL
};

MicroBit uBit;
int bias = RIGHT; // Default right bias

void motorRun(Motor index, Direction direction, int speed)
{
    uint8_t buf[3];
    switch (index)
    {
    case LEFT:
        buf[0] = 0x00;
        buf[1] = direction; // 0 forward,  1 backward
        buf[2] = speed;

        uBit.i2c.write(
            0x20, buf,
            3); // device address is 0x10 but must be left shifted for Micro:bit libraries.
        break;
    case RIGHT:
        buf[0] = 0x02;
        buf[1] = direction; // 0 forward,  1 backward
        buf[2] = speed;

        uBit.i2c.write(
            0x20, buf,
            3); // device address is 0x10 but must be left shifted for Micro:bit libraries.
        break;
    case ALL:
        buf[0] = 0x00;
        buf[1] = direction; // 0 forward,  1 backward
        buf[2] = speed;

        uBit.i2c.write(
            0x20, buf,
            3); // device address is 0x10 but must be left shifted for Micro:bit libraries.
        buf[0] = 0x02;
        uBit.i2c.write(0x20, buf, 3);
    }
}

void motorStop(Motor index)
{
    motorRun(index, FORWARD, 0);
}

void writeLED(int led, int ledSwitch)
{
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

int readGreyscale(int side)
{ // return of 1 is white, 0 is black, -1 is invalid input
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

int readUltrasonic(){
    int d;

    uBit.io.P1.setDigitalValue(1);
    uBit.sleep(1);
    uBit.io.P1.setDigitalValue(0);

    if (uBit.io.P2.getDigitalValue() == 0) {
        uBit.io.P1.setDigitalValue(0);
        uBit.io.P1.setDigitalValue(1);
        uBit.sleep(20);
        uBit.io.P1.setDigitalValue(0);

        uBit.io.P2.setPolarity(1);
        
        d = uBit.io.P2.getPulseUs(500*58);
    } else {
        uBit.io.P1.setDigitalValue(1);
        uBit.io.P1.setDigitalValue(0);
        uBit.sleep(20);
        uBit.io.P1.setDigitalValue(0);

        uBit.io.P2.setPolarity(0);

        d = uBit.io.P2.getPulseUs(500*58);
    }

    return (d/59);
}

void wiggle(int direction){
	if(direction == 0){
		motorRun(RIGHT,FORWARD,SPEED);
	}else {
		motorRun(LEFT,FORWARD,SPEED);
	}	
}

void leftOnWhite(Event evt)
{
	motorStop(ALL);

	if(readGreyscale(1) == 0){ // right on black
	}else {
        // turn left
        wiggle(0);
	}
}

void leftOnBlack(Event evt){
	motorStop(ALL);

	if(readGreyscale(1) == 1){ // right on white
        // turn right
        wiggle(1);
	}else {
        // turn right
        rightTurn();
	}
}

void rightOnBlack(Event evt){
	motorStop(ALL);

	if(readGreyscale(LEFT) == 0){ // left on black
		create_fiber(rightTurn);
	}else {
		create_fiber(intersection);
	}
}

void enableGreyscaleEvents(){
	uBit.io.P13.eventOn(MICROBIT_PIN_EVENT_ON_EDGE);
	uBit.io.P14.eventOn(MICROBIT_PIN_EVENT_ON_EDGE);

	uBit.messageBus.listen(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_FALL, leftOnBlack, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_RISE, leftOnWhite, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P14, MICROBIT_PIN_EVT_FALL, rightOnBlack, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

void disableGreyscaleEvents(){
	uBit.messageBus.ignore(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_FALL, leftOnBlack);
	uBit.messageBus.ignore(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_RISE, leftOnWhite);
	uBit.messageBus.ignore(MICROBIT_ID_IO_P14, MICROBIT_PIN_EVT_FALL, rightOnBlack);
}

void rightTurn() {
    uBit.display.scroll("R");
    disableGreyscaleEvents();
    motorRun(LEFT,FORWARD,SPEED);
    uBit.sleep(950*50/SPEED);
    enableGreyscaleEvents();
    motorRun(ALL,FORWARD,SPEED);
}

void intersection(){
	disableGreyscaleEvents();
	if(bias == 1){
		motorRun(LEFT,FORWARD,SPEED);
		bias = 0;
        uBit.sleep(1000*50/SPEED);
	}else {
		motorRun(RIGHT,FORWARD,SPEED);
		bias = 1;
        uBit.sleep(600*50/SPEED);
	}
	enableGreyscaleEvents();
	motorRun(ALL,FORWARD,SPEED);
}

int main()
{
    uBit.init();
	enableGreyscaleEvents();	

	wiggle(RIGHT);
    
    int d;
    while(1){
        d = readUltrasonic();
        if (d <= 15 && d > 0) {
			motorStop(ALL);
            uBit.io.P12.setDigitalValue(1);
        } else {
            uBit.io.P12.setDigitalValue(0);
        }
        uBit.sleep(1000);
    }
}
