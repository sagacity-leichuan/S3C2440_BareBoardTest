
#define		WTCON		(0xE2700000)
#define		WTDAT		(0xE2700004)
#define		WTCNT		(0xE2700008)
#define 	WTCLRINT	(0xE270000C)

#define 	rWTCON		(*(volatile unsigned int *)WTCON)
#define 	rWTDAT		(*(volatile unsigned int *)WTDAT)
#define 	rWTCNT		(*(volatile unsigned int *)WTCNT)
#define 	rWTCLRINT	(*(volatile unsigned int *)WTCLRINT)


// ��ʼ��WDTʹ֮���Բ����ж�
void wdt_init_reset(void)
{
	// ��һ�������ú�Ԥ��Ƶ���ͷ�Ƶ�����õ�ʱ��������128us
	rWTCON &= ~(0xff<<8);
	rWTCON |= (65<<8);				// 1MHz
	
	rWTCON &= ~(3<<3);
	rWTCON |= (3<<3);				// 1/128 MHz, T = 128us
	
	// �ڶ����������жϺ͸�λ�źŵ�ʹ�ܻ��ֹ
	rWTCON &= ~(1<<2);				// disable wdt interrupt
	rWTCON |= (1<<0);				// enable wdt reset
	
	// �����������ö�ʱʱ��
	// WDT��ʱ�������������ն�ʱʱ��Ϊ�����ֵ��ʱ������
	rWTDAT = 10000;					// ��ʱ1.28s
	rWTCNT = 10000;					// ��ʱ1.28s
	
	// ��ʵWTDAT�е�ֵ�����Զ�ˢ��WTCNT��ȥ���������ʽ����WTCON�е�ֵ������ֵ����
	// Ĭ��ֵ��Ȼ�������Ĭ��ֵ��ʼ�������������ʱ��ȽϾá���������Լ���ʽ��
	// ������WTCNT��WTDATһ����ֵ�����һ�εĶ�ʱֵ�ͺͺ����һ���ˡ�
	//rWTDAT = 1000;					// ��ʱ0.128s
	//rWTCNT = 1000;					// ��ʱ0.128s
	
	// ���Ĳ����Ȱ����мĴ��������ú�֮����ȥ�����Ź�
	rWTCON |= (1<<5);				// enable wdt
}

