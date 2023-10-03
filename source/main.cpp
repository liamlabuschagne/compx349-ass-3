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

void leftOnWhite(Event evt)
{
	motorStop(2);
	motorRun(0,0,48);
	motorRun(1,1,48);
}

void leftOnBlack(Event evt){
	motorRun(2,0,48);
}

void rightOnWhite(Event evt)
{
	motorStop(2);
	motorRun(1,0,48);
	motorRun(0,1,48);
}

void rightOnBlack(Event evt){
	motorRun(2,0,48);
}

int main()
{
    uBit.init();

	uBit.io.P13.eventOn(MICROBIT_PIN_EVENT_ON_EDGE);
	uBit.io.P14.eventOn(MICROBIT_PIN_EVENT_ON_EDGE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_FALL, leftOnBlack, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P13, MICROBIT_PIN_EVT_RISE, leftOnWhite, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P14, MICROBIT_PIN_EVT_FALL, rightOnBlack, MESSAGE_BUS_LISTENER_IMMEDIATE);
	uBit.messageBus.listen(MICROBIT_ID_IO_P14, MICROBIT_PIN_EVT_RISE, rightOnWhite, MESSAGE_BUS_LISTENER_IMMEDIATE);
	
	motorRun(2,0,0x30);
}
