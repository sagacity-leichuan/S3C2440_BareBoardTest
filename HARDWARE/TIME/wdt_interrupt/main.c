#include "stdio.h"
#include "int.h"
#include "main.h"

void uart_init(void);


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
	wdt_init_interrupt();
	
	system_init_exception();
	
	printf("-------------wdt interrupt test--------------");
	
	intc_setvectaddr(NUM_WDT, isr_wdt);

	
	intc_enable(NUM_WDT);

	
	while (1);

	return 0;
}