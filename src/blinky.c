#include "nrf.h"

/* Pin definitions - as masks assumed to be on Port 0 */
#define PIN_ROW5    (1u << 19)
#define PIN_COL1    (1u << 28)
#define PIN_COL2    (1u << 11)
#define PIN_COL3    (1u << 31)
#define PIN_COL4    (1u << 5) // On P1
#define PIN_COL5    (1u << 30)

#define BTN_A 		14
#define BTN_B		23
#define BTN_A_BIT	(1u << BTN_A)  
#define BTN_B_BIT	(1u << BTN_B)

/**
 * A simple busy-loop delay
 *
 * \param loops the number of iterations of the busy loop. Note that
 * this routine is not calibrated to any particular timebase - the
 * number of loops is just that, and I've not measured how long a loop
 * takes. It also makes no attempt to compensate for any interrupts
 * that may fire while in the loop (in fact, it is oblivious to them).
 */
static void
delay(unsigned loops)
{
    volatile unsigned count = loops;
    while (count--)
        /* Nada */;
}

void flash(){
	NRF_P0->OUTSET = PIN_ROW5;
	NRF_P1->OUTSET = PIN_ROW5;
	delay(4000000);
    NRF_P0->OUTCLR = PIN_ROW5;
    NRF_P1->OUTCLR = PIN_ROW5;
	delay(4000000);
}

void setCol(unsigned int pin, unsigned int val){
	switch(pin){
		case PIN_COL1:
		case PIN_COL2:
		case PIN_COL3:
		case PIN_COL5:
			if(!val)
				NRF_P0->OUTSET = pin;
			else
				NRF_P0->OUTCLR = pin;
			break;
		case PIN_COL4:
			if(!val)
				NRF_P1->OUTSET = pin;
			else
				NRF_P1->OUTCLR = pin;
			break;
	}
}

int moveLeft(int x){
	if(x > 0) return x-1;
	return x;
}

int moveRight(int x){
	if(x < 4) return x+1;
	return x;
}

int
main(void)
{
    /* Set our GPIOs as output */
    NRF_P0->DIRSET = (PIN_ROW5 | PIN_COL1 | PIN_COL2 | PIN_COL3 | PIN_COL5);
    NRF_P1->DIRSET = (PIN_ROW5 | PIN_COL4);
	NRF_P0->PIN_CNF[BTN_A] = (GPIO_PIN_CNF_SENSE_Msk | GPIO_PIN_CNF_PULL_Msk);
	NRF_P0->PIN_CNF[BTN_B] = (GPIO_PIN_CNF_SENSE_Msk | GPIO_PIN_CNF_PULL_Msk);
	int cols[] = {PIN_COL1, PIN_COL2, PIN_COL3, PIN_COL4, PIN_COL5};
	
	int x = 0;	
	for (;;)
    {
		// clear the previous column
		setCol(PIN_COL1,0);
		setCol(PIN_COL2,0);
		setCol(PIN_COL3,0);
		setCol(PIN_COL4,0);
		setCol(PIN_COL5,0);

		// set the new column
		setCol(cols[x],1);

		flash();
		
		if(NRF_P0->LATCH & BTN_A_BIT){
			x = moveLeft(x);
			NRF_P0->LATCH = BTN_A_BIT;
		}
		
		if(NRF_P0->LATCH & BTN_B_BIT){
			x = moveRight(x);
			NRF_P0->LATCH = BTN_B_BIT;
		}
		
    }

    return 0;
}
