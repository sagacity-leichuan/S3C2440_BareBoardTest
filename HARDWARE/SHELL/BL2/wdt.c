#include "int.h"
#include "shell.h"

#define		WTCON		(0xE2700000)
#define		WTDAT		(0xE2700004)
#define		WTCNT		(0xE2700008)
#define 	WTCLRINT	(0xE270000C)

#define 	rWTCON		(*(volatile unsigned int *)WTCON)
#define 	rWTDAT		(*(volatile unsigned int *)WTDAT)
#define 	rWTCNT		(*(volatile unsigned int *)WTCNT)
#define 	rWTCLRINT	(*(volatile unsigned int *)WTCLRINT)


// ��ʼ��WDTʹ֮���Բ����ж�
void wdt_init_interrupt(void)
{
	// ��һ�������ú�Ԥ��Ƶ���ͷ�Ƶ�����õ�ʱ��������128us
	rWTCON &= ~(0xff<<8);
	rWTCON |= (65<<8);				// 1MHz
	
	rWTCON &= ~(3<<3);
	rWTCON |= (3<<3);				// 1/128 MHz, T = 128us
	
	// �ڶ����������жϺ͸�λ�źŵ�ʹ�ܻ��ֹ
	rWTCON |= (1<<2);				// enable wdt interrupt
	rWTCON &= ~(1<<0);				// disable wdt reset
	
	// �����������ö�ʱʱ��
	// WDT��ʱ�������������ն�ʱʱ��Ϊ�����ֵ��ʱ������
	//rWTDAT = 10000;					// ��ʱ1.28s
	//rWTCNT = 10000;					// ��ʱ1.28s
	
	// ��ʵWTDAT�е�ֵ�����Զ�ˢ��WTCNT��ȥ���������ʽ����WTCON�е�ֵ������ֵ����
	// Ĭ��ֵ��Ȼ�������Ĭ��ֵ��ʼ�������������ʱ��ȽϾá���������Լ���ʽ��
	// ������WTCNT��WTDATһ����ֵ�����һ�εĶ�ʱֵ�ͺͺ����һ���ˡ�
	//rWTDAT = 1000;					// ��ʱ0.128s
	//rWTCNT = 1000;					// ��ʱ0.128s
	
	rWTDAT = 7812;						// ��ʱ1s
	rWTCNT = 7812;						// ��ʱ1s

	// ���Ĳ����Ȱ����мĴ��������ú�֮����ȥ�����Ź�
	rWTCON |= (1<<5);				// enable wdt
}


// wdt���жϴ������
void isr_wdt(void)
{
	// ��ʱ��Ȼ��ʱ��û����ʱ������Ļ�ϴ�ӡ����������ʱ�䵽���Զ�ִ������
	// ִ�����������shell����ѭ��
	g_bootdelay--;
	putchar('\b');
	printf("%d", g_bootdelay);
	
	if (g_bootdelay == 0)
	{
		g_isgo = 1;
		
		// ��Ҫ�Զ�ִ�е�������ӵ���������������жϴ�����򣬲��ʺ�ִ�г�����
		// ���Է�������Ҫ��һЩ
		
		//printf("g_isgo = 1.\n");
		// �ر�wdt
		//intc_disable(NUM_WDT);
	}

	// ���ж�
	intc_clearvectaddr();
	rWTCLRINT = 1;
}


// wdt��Ϊ��ʱ���õ����г�ʼ��
void wdt_timer_init(void)
{
	wdt_init_interrupt();
	// ���������Ҫʹ���жϣ���Ҫ�����жϳ�ʼ����������ʼ���жϿ�����
	system_init_exception();
	intc_setvectaddr(NUM_WDT, isr_wdt);
	// ʹ���ж�
	intc_enable(NUM_WDT);
}













