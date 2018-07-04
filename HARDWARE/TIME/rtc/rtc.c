#include "main.h"

#define 	RTC_BASE	 (0xE2800000)
#define		rINTP      	 (*((volatile unsigned long *)(RTC_BASE + 0x30)))
#define		rRTCCON    	 (*((volatile unsigned long *)(RTC_BASE + 0x40)))
#define		rTICCNT    	 (*((volatile unsigned long *)(RTC_BASE + 0x44)))
#define		rRTCALM    	 (*((volatile unsigned long *)(RTC_BASE + 0x50)))
#define		rALMSEC    	 (*((volatile unsigned long *)(RTC_BASE + 0x54)))
#define		rALMMIN    	 (*((volatile unsigned long *)(RTC_BASE + 0x58)))
#define		rALMHOUR  	 (*((volatile unsigned long *)(RTC_BASE + 0x5c)))
#define		rALMDATE     (*((volatile unsigned long *)(RTC_BASE + 0x60)))
#define		rALMMON    	 (*((volatile unsigned long *)(RTC_BASE + 0x64)))
#define		rALMYEAR  	 (*((volatile unsigned long *)(RTC_BASE + 0x68)))
#define		rRTCRST      (*((volatile unsigned long *)(RTC_BASE + 0x6c)))
#define		rBCDSEC    	 (*((volatile unsigned long *)(RTC_BASE + 0x70)))
#define		rBCDMIN   	 (*((volatile unsigned long *)(RTC_BASE + 0x74)))
#define		rBCDHOUR     (*((volatile unsigned long *)(RTC_BASE + 0x78)))
#define		rBCDDATE     (*((volatile unsigned long *)(RTC_BASE + 0x7c)))
#define		rBCDDAY      (*((volatile unsigned long *)(RTC_BASE + 0x80)))
#define		rBCDMON      (*((volatile unsigned long *)(RTC_BASE + 0x84)))
#define		rBCDYEAR     (*((volatile unsigned long *)(RTC_BASE + 0x88)))
#define		rCURTICCNT   (*((volatile unsigned long *)(RTC_BASE + 0x90)))
#define		rRTCLVD    	 (*((volatile unsigned long *)(RTC_BASE + 0x94)))


// �������ܣ���ʮ����numת��bcd�룬Ʃ���56ת��0x56
static unsigned int num_2_bcd(unsigned int num)
{
	// ��һ������56��ֳ�5��6 
	// �ڶ�������5��6��ϳ�0x56
	return (((num / 10)<<4) | (num % 10));
}

// �������ܣ���bcd��bcdת��ʮ���ƣ�Ʃ���0x56ת��56
static unsigned int bcd_2_num(unsigned int bcd)
{
	// ��һ������0x56��ֳ�5��6 
	// �ڶ�������5��6��ϳ�56
	return (((bcd & 0xf0)>>4)*10 + (bcd & (0x0f)));
}


void rtc_set_time(const struct rtc_time *p)
{
	// ��һ������RTC��д����
	rRTCCON |= (1<<0);
	
	// �ڶ�����дRTCʱ��Ĵ���
	rBCDYEAR = num_2_bcd(p->year - 2000);
	rBCDMON = num_2_bcd(p->month);
	rBCDDATE = num_2_bcd(p->date);
	rBCDHOUR = num_2_bcd(p->hour);
	rBCDMIN = num_2_bcd(p->minute);
	rBCDSEC = num_2_bcd(p->second);
	rBCDDAY = num_2_bcd(p->day);
	
	// ���һ��������RTC�Ķ�д����
	rRTCCON &= ~(1<<0);
}

void rtc_get_time(struct rtc_time *p)
{
	// ��һ������RTC��д����
	rRTCCON |= (1<<0);
	
	// �ڶ�������RTCʱ��Ĵ���
	p->year = bcd_2_num(rBCDYEAR) + 2000;
	p->month = bcd_2_num(rBCDMON);
	p->date = bcd_2_num(rBCDDATE);
	p->hour = bcd_2_num(rBCDHOUR);
	p->minute = bcd_2_num(rBCDMIN);
	p->second = bcd_2_num(rBCDSEC);
	p->day = bcd_2_num(rBCDDAY);
	
	// ���һ��������RTC�Ķ�д����
	rRTCCON &= ~(1<<0);
}

void rtc_set_alarm(void)
{
	rALMSEC = num_2_bcd(23);
	rRTCALM |= 1<<0;
	rRTCALM |= 1<<6;
}

void isr_rtc_alarm(void)
{
	static int i = 0; 
	printf("rtc alarm, i = %d...", i++);
	
	rINTP |= (1<<1);
	intc_clearvectaddr();
}

