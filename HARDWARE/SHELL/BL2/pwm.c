
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
#define 	rTCMPB2		(*(volatile unsigned int *)TCMPB2)

#define 	GPD0DAT		(0xE02000A4)
#define 	rGPD0DAT	(*(volatile unsigned int *)GPD0DAT)



/*
// ��ʼ��PWM timer2��ʹ�����PWM���Σ�Ƶ����2KHz��dutyΪ50%
void timer2_pwm_init(void)
{
	// ����GPD0_2���ţ���������ΪXpwmTOUT_2
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (2<<8);
	
	// ����PWM��ʱ����һ�ɼĴ�����ʹ�乤��
	rTCFG0 &= ~(0xff<<8);
	rTCFG0 |= (65<<8);			// prescaler1 = 65, Ԥ��Ƶ��Ƶ��Ϊ1MHz
	
	rTCFG1 &= ~(0x0f<<8);
	rTCFG1 |= (1<<8);			// MUX2����Ϊ1/2,��Ƶ��ʱ������Ϊ500KHz
	// ʱ�����úã����ǵ�ʱ��Ƶ����500KHz����Ӧ��ʱ��������2us��Ҳ����˵ÿ��2us
	// ��һ���������Ҫ����ʱ����x����TCNTB��Ӧ��д��x/2us
	
	rCON |= (1<<15);		// ʹ��auto-reload��������ʱ���ܷ���PWM����
	//rTCNTB2 = 250;			// 0.5ms/2us = 500us/2us = 250
	//rTCMPB2 = 125;			// duty = 50%
	
	rTCNTB2 = 50;			
	rTCMPB2 = 25;	
	
	// ��һ����Ҫ�ֹ���TCNTB�е�ֵˢ�µ�TCNT��ȥ���Ժ�Ϳ���auto-reload��
	rCON |= (1<<13);		// ���Զ�ˢ�¹���
	rCON &= ~(1<<13);		// �ر��Զ�ˢ�¹���
	
	rCON |= (1<<12);		// ��timer2��ʱ����Ҫ�Ȱ����������úò��ܿ���ʱ��
}

*/

/*
void buzzer_on(void)
{
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (2<<8);
}

// �ط�������˼·��TCON�����ùأ�GPIO���ó�����ģʽ
void buzzer_off(void)
{
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (0<<8);
}
*/

// ��������ֱ�����ó����ģʽ��Ȼ������ߵ�ƽ��buzzer���ˡ�����͵�ƽ�͹���
// �ŵ����SOC������pwm���ܣ�ֻҪ������ߵ͵�ƽ���ܿ��Ʒ�����
// ȱ���Ƿ����������е�Ƶ���޷��Ķ�
// ������ԣ���ô���ط������ͱ����GPIO����ߵ͵�ƽ�ˡ�

/*
// ���Խ��ۣ����ܹرշ����������Ǵ򿪺͹ر��Զ�װ��ǰ�󣬷���������ɫ���
// Ӧ����Ӱ�������PWM���ε�Ƶ�ʵ��µ�
void buzzer_on(void)
{
	rCON |= (1<<13);		// ���Զ�ˢ�¹���
}

void buzzer_off(void)
{
	rCON &= ~(1<<13);		// �ر��Զ�ˢ�¹���
}
*/

/*
// ���Խ��ۣ����Ϸ��������ؿ��ƺ�on��off�����Է���������Ӱ���ˣ�����ɫҲ������
void buzzer_on(void)
{
	rCON |= (1<<13);		// ���Զ�ˢ�¹���
	rCON |= (1<<12);
}

void buzzer_off(void)
{
	rCON &= ~(1<<13);		// �ر��Զ�ˢ�¹���
	rCON &= ~(1<<12);
}
*/


// ���Խ��ۣ�����ֱ�����ó����ģʽȷʵ������������������ߵ�ƽ�������죬�͵�ƽ����
// �����졣
void timer2_pwm_init(void)
{
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (1<<8);			// ���ó�outputģʽ
	rGPD0DAT |= (1<<2);			// ����ߵ�ƽ
}

void buzzer_on(void)
{
	rGPD0DAT |= (1<<2);	
}

void buzzer_off(void)
{
	rGPD0DAT &= ~(1<<2);	
}














