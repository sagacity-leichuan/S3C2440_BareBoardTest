
#define 	GPD0CON		(0xE02000A0)
#define 	TCFG0		(0xE2500000)
#define 	TCFG1		(0xE2500004)
#define 	CON			(0xE2500008)
#define 	TCNTB2		(0xE2500024)
#define 	TCMPB2		(0xE2500028)

#define 	rGPD0CON	(*(volatile unsigned int *)GPD0CON)
#define 	rTCFG0		(*(volatile unsigned int *)TCFG0)
#define 	rTCFG1		(*(volatile unsigned int *)TCFG1)
#define 	rCON		(*(volatile unsigned int *)CON)
#define 	rTCNTB2		(*(volatile unsigned int *)TCNTB2)
#define 	rTCMPB2		 (*(volatile unsigned int *)TCMPB2)


// 初始化PWM timer2，使其输出PWM波形：频率是2KHZ，duty为50%
void timer2_pwm_init(void)
{
	// 设置GPD0_2，将其设置为XpwmTOUT_2
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (2<<8);
	
	// 设置PWM定时器的一干寄存器，使其工作
	rTCFG0 &= ~(0xff<<8);
	rTCFG0 |= (65<<8);			// prescaler1 = 65, 预分频只有的频率为1MHZ
	
	rTCFG1 &= ~(0x0f<<8);
	rTCFG1 |= (1<<8);			// MUX设置为1/2，为500Khz，分频之后的时钟周期为2us
	// 时钟设置好，我们的时钟频率是500KHZ，对应的时钟周期为2us，也就是说没个2us计一次数
	// 如果要定时的时间是x，则TCNIB中应该写入x/2us
	
	rCON |= (1<<15);		//	 使能auto-reload ，反复想定时才能产生PWM波形
	
	//rTCNTB2 = 250;			// 0.5ms/2us = 500us/2us = 250
	//rTCMPB2 = 125;			// duty = 50%
	
	rTCNTB2 = 50;			
	rTCMPB2 = 25;	
	
	// 第一次需要手动将TCNTB中的值刷新到TCNT中去，以后就可以atuo-reload了
	rCON |= (1<<13);		// 打开自动刷新功能
	//如果硬件速度不够快，可以加延时
	rCON &= ~(1<<13);		// 关闭自动刷新功能
	
	rCON |= (1<<12);		// 开timer2定时器，要先把其他寄存器设置好才能开定时器
}

