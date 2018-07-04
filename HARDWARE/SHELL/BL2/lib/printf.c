#include "vsprintf.h"
#include "string.h"
#include "printf.h"

extern void putc(unsigned char c);
extern unsigned char getc(void);

#define	OUTBUFSIZE	1024
#define	INBUFSIZE	1024

// �Լ�������2��ȫ�ֱ������飬�ֱ���Ϊ����/���ջ�������
// ��������ʱ�Ƚ�Ҫ���͵���Ϣ��ʽ�����뷢�ͻ�������Ȼ��putc����ֱ�Ӵӷ��ͻ�����ȡ
// ���ݷ��ͳ�ȥ��
static char g_pcOutBuf[OUTBUFSIZE];
static char g_pcInBuf[INBUFSIZE];


// putc��������������豸�󶨣����������Ҫ�����Լ�ȥʵ�֣��������ֲ�Ĺؼ�
// printf("a = %d, b = %s.\n", a, p);
int printf(const char *fmt, ...)
{
	int i;
	int len;
	va_list args;

	va_start(args, fmt);
	len = vsprintf(g_pcOutBuf,fmt,args);
	va_end(args);
	for (i = 0; i < strlen(g_pcOutBuf); i++)
	{
		putc(g_pcOutBuf[i]);
	}
	return len;
}



int scanf(const char * fmt, ...)
{
	int i = 0;
	unsigned char c;
	va_list args;
	
	while(1)
	{
		c = getc();
		putc(c);
		if((c == 0x0d) || (c == 0x0a))
		{
			g_pcInBuf[i] = '\0';
			break;
		}
		else
		{
			g_pcInBuf[i++] = c;
		}
	}
	
	va_start(args,fmt);
	i = vsscanf(g_pcInBuf,fmt,args);
	va_end(args);

	return i;
}

