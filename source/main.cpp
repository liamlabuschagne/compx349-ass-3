#include "MicroBit.h"

MicroBit uBit;
int bias = 0; // Default left bias

void motorRun(int index, int direction, int speed)
{
    uint8_t buf[3];
    switch (index)
    {
    case 0: // Left
        buf[0] = 0x00;
        buf[1] = direction; // 0 forward,  1 backward
        buf[2] = speed;

        uBit.i2c.write(
            0x20, buf,
            3); // device address is 0x10 but must be left shifted for Micro:bit libraries.
        break;
    case 1: // Right
        buf[0] = 0x02;
        buf[1] = direction; // 0 forward,  1 backward
        buf[2] = speed;

        uBit.i2c.write(
            0x20, buf,
            3); // device address is 0x10 but must be left shifted for Micro:bit libraries.
        break;
    case 2: // Both
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

void motorStop(int index)
{
    motorRun(index, 0, 0);
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

// void checkLeftFiber()
// {
//     while (1)
//     {
//         if (readGreyscale(0) == 0)
//         {
// 			MicroBitEvent evt(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_RISE); 
//             uBit.messageBus.send(evt);
//         }
// 		uBit.sleep(10);
//     }
// }

void onCollisionLeft(Event evt)
{
	if (bias == 0) {
		motorStop(0);
		motorRun(1,0,0x30);
	}
}

void postCollisionLeft(Event evt){
	if (bias == 0) {
		motorStop(1);
		motorRun(0,0,0x30);
		bias = 1;
	}
}

void onCollisionRight(Event evt)
{
	if (bias == 1) {
		motorStop(1);
		motorRun(0,0,0x30);
	}
}

void postCollisionRight(Event evt){
	if (bias == 1) {
		motorStop(0);
		motorRun(1,0,0x30);
		bias = 0;
	}
}

int main()
{
    uBit.init();

	uBit.io.P13.eventOn(MICROBIT_PIN_EVENT_ON_EDGE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_FALL, onCollisionLeft, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_RISE, postCollisionLeft, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P14, MICROBIT_PIN_EVT_FALL, onCollisionRight, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P14, MICROBIT_PIN_EVT_RISE, postCollisionRight, MESSAGE_BUS_LISTENER_IMMEDIATE);

	// motorRun(0,0,0x30);
	// while(true){
	// 	if(bias == 0){
	// 		motorRun(0,0,0x30);
	// 	}else {
	// 		motorRun(1,0,0x30);
	// 	}
	// }	
}
