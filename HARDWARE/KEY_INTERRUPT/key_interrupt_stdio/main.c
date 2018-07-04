#include "stdio.h"
#include "int.h"
#include "main.h"

void uart_init(void);

#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	

void delay(int i)
{
	volatile int j = 10000;
	while (i--)
		while(j--);
}


int main(void)
{
	uart_init();
	//key_init();
	key_init_interrupt();
	

	system_init_exception();
	
	printf("-------------key interrypt test--------------");

	intc_setvectaddr(KEY_EINT2, isr_eint2);
	intc_setvectaddr(KEY_EINT3, isr_eint3);
	intc_setvectaddr(KEY_EINT16_19, isr_eint16171819);

	intc_enable(KEY_EINT2);
	intc_enable(KEY_EINT3);
	intc_enable(KEY_EINT16_19);
	
	while (1)
	{
		printf("A ");
		delay(10000);
	}

	return 0;
}

