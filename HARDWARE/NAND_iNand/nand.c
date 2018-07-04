#include "nand.h"
#include "stdio.h"

#define rNFCONF 			( *((volatile unsigned long *)0xB0E00000) )
#define rNFCONT 			( *((volatile unsigned long *)0xB0E00004) )
#define rNFCMMD 			( *((volatile unsigned long *)0xB0E00008) )
#define rNFADDR 			( *((volatile unsigned long *)0xB0E0000C) )
#define rNFDATA 			( *((volatile unsigned long *)0xB0E00010) )
#define rNFDATA8 			( *((volatile unsigned char *)0xB0E00010) )
#define rNFSTAT 			( *((volatile unsigned long *)0xB0E00028) )

#define rMP0_1CON 		( *((volatile unsigned long *)0xE02002E0) )
#define rMP0_2CON 		( *((volatile unsigned long *)0xE0200300) )
#define rMP0_3CON 		( *((volatile unsigned long *)0xE0200320) )

#define MAX_NAND_BLOCK  			  8192 			/*����nand��������8192�� 	*/
#define NAND_PAGE_SIZE  			  2048 			/*����һҳ������:2048 byte 	*/
#define NAND_BLOCK_SIZE 			  64  			/*����block��С��64ҳ		*/

#define TACLS    					  1		// 12ns		/* ʱ����ص����� 			*/
#define TWRPH0   					  4
#define TWRPH1   					  1

#define NAND_CMD_READ_1st             0x00			/* ����						*/
#define NAND_CMD_READ_2st             0x30

#define NAND_CMD_READ_CB_1st          0x00
#define NAND_CMD_READ_CB_2st          0x35

#define NAND_CMD_RANDOM_WRITE         0x85
#define NAND_CMD_RANDOM_READ_1st      0x05
#define NAND_CMD_RANDOM_READ_2st      0xe0

#define NAND_CMD_READ_ID              0x90
#define NAND_CMD_RESET                0xff
#define NAND_CMD_READ_STATUS          0x70

#define NAND_CMD_WRITE_PAGE_1st       0x80
#define NAND_CMD_WRITE_PAGE_2st       0x10

#define NAND_CMD_BLOCK_ERASE_1st      0x60
#define NAND_CMD_BLOCK_ERASE_2st      0xd0


#define ECC_EN						  (1<<4)
#define CONTROL_EN					  (1<<0)


static void nand_reset(void);
static void nand_wait_idle(void);
static void nand_select_chip(void);
static void nand_deselect_chip(void);
static void nand_send_cmd(unsigned long cmd);
static void nand_send_addr(unsigned long addr);
static unsigned char nand_read8(void);
static void nand_write8(unsigned char data);
static unsigned int nand_read32(void);
static void nand_write32(unsigned int data);

typedef struct nand_id_info
{
	//marker code
	unsigned char IDm; 
	//device code
	unsigned char IDd; 
	unsigned char ID3rd;
	unsigned char ID4th;
	unsigned char ID5th;
}nand_id_info;

// ��λ  
void nand_reset(void)
{
	nand_select_chip();
	nand_send_cmd(NAND_CMD_RESET);
	nand_wait_idle();
	nand_deselect_chip();
}

// �ȴ�����  
void nand_wait_idle(void)
{
	unsigned long i;
	while( !(rNFSTAT & (1<<4)) )
		for(i=0; i<10; i++);
}

// ��Ƭѡ  
void nand_select_chip(void)
{
	unsigned long i;
	rNFCONT &= ~(1<<1);
	for(i=0; i<10; i++);
}

// ȡ��Ƭѡ  
void nand_deselect_chip(void)
{
	unsigned long i = 0;
	rNFCONT |= (1<<1);
	for(i=0; i<10; i++);
}

// ������  
void nand_send_cmd(unsigned long cmd)
{
	unsigned long i = 0;

	rNFCMMD = cmd;
	for(i=0; i<10; i++);
}

// ����ַ  
void nand_send_addr(unsigned long addr)
{
	unsigned long i;
	unsigned long col, row;

	// �е�ַ����ҳ�ڵ�ַ  
	col = addr % NAND_PAGE_SIZE;		
	// �е�ַ����ҳ��ַ  			
	row = addr / NAND_PAGE_SIZE;

	// Column Address A0~A7  
	rNFADDR = col & 0xff;			
	for(i=0; i<10; i++);		

	// Column Address A8~A11  
	rNFADDR = (col >> 8) & 0x0f; 		
	for(i=0; i<10; i++);

	// Row Address A12~A19	
	rNFADDR = row & 0xff;			
	for(i=0; i<10; i++);

	// Row Address A20~A27	
	rNFADDR = (row >> 8) & 0xff;
	for(i=0; i<10; i++);

	// Row Address A28~A30	
	rNFADDR = (row >> 16) & 0xff;
	for(i=0; i<10; i++);
}

unsigned int nand_read32(void)
{
	return rNFDATA;
}

void nand_write32(unsigned int data)
{
	rNFDATA = data;
}

// ��һ���ֽڵ�����  
unsigned char nand_read8(void)
{
	return rNFDATA8;
}

// дһ���ֽڵ�����  
void nand_write8(unsigned char data)
{
	rNFDATA8 = data;
}

unsigned char nand_read_status(void)
{
	unsigned char ch;
	int i;

	// 1. ����Ƭѡ�ź�  
	nand_select_chip();

	// 2. ��״̬  
	nand_send_cmd(NAND_CMD_READ_STATUS);
	for(i=0; i<10; i++);
	ch = nand_read8();

	// 3. ȡ��Ƭѡ  
	nand_deselect_chip();
	return ch;
}

// nandflash ��ʼ��  
void nand_init(void)
{

	// 1. ����nandflash  
	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0);
	rNFCONT = (0<<18)|(0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0x3<<1)|(1<<0);

	// 2. ��������  
	rMP0_1CON = 0x22333322;
	rMP0_2CON = 0x00002222;
	rMP0_3CON = 0x22222222;

	// 3. ��λ  
	nand_reset();
}

// ��оƬID  
void nand_read_id(void)
{
	nand_id_info nand_id;
	// 1. ��Ƭѡ  
	nand_select_chip();

	// 2. ��ID  
	nand_send_cmd(NAND_CMD_READ_ID);
	nand_send_addr(0x00);
	nand_wait_idle();
	
	nand_id.IDm = 	nand_read8();
	nand_id.IDd = 	nand_read8();
	nand_id.ID3rd = nand_read8();
	nand_id.ID4th = nand_read8();
	nand_id.ID5th = nand_read8();

	printf("nandflash: makercode = %x\r\n devicecode = %x\r\n ID3rd = %x\r\n ID4rd = %x\r\n ID5rd = %x\r\n", nand_id.IDm, nand_id.IDd, nand_id.ID3rd, nand_id.ID4th, nand_id.ID5th);
	nand_deselect_chip();
}

// �����飬����Ϊ��ţ�0 ~ MAX_NAND_BLOCK-1��  
int nand_block_erase(unsigned long block_num)
{
	unsigned long i = 0;

	// ���row��ַ����ҳ��ַ  
	unsigned long row = block_num * NAND_BLOCK_SIZE;

	// 1. ����Ƭѡ�ź�  
	nand_select_chip();
	// 2. ��������һ�����ڷ�����0x60���ڶ������ڷ����ַ�����������ڷ�����0xd0  
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_1st);
	for(i=0; i<10; i++);
	
	// Row Address A12~A19	
	rNFADDR = row & 0xff;							
	for(i=0; i<10; i++);
	// Row Address A20~A27  
	rNFADDR = (row >> 8) & 0xff;
	for(i=0; i<10; i++);
	// Row Address A28~A30  
	rNFADDR = (row >> 16) & 0xff;	
	
	rNFSTAT |= (1<<4);			// clear RnB bit 
	
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_2st);
	for(i=0; i<10; i++);
	// 3. �ȴ�����  
	nand_wait_idle();

	// 4. ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// statas[0] = 1����ʾ����ʧ�ܣ����NAND Flash�����ֲ��� READ STATUSһ�ڵ�����
		// ȡ��Ƭѡ�ź�  
		nand_deselect_chip();						
		printf("masking bad block %d\r\n", block_num);
		return -1;
	}
	else
	{
		// status[0] = 0����ʾ�����ɹ�������0
		nand_deselect_chip();
		return 0;
	}
}

int nand_erase(unsigned long block_addr)
{
	int i = 0;
	if((nand_read_status() & 0x80) == 0) 
	{
	    printf("Write protected.\n");
	    return -1;
	}
	
	unsigned long row = block_addr >> 18;
	// 1. ����Ƭѡ�ź�  
	nand_select_chip();
	// 2. ��������һ�����ڷ�����0x60���ڶ������ڷ����ַ�����������ڷ�����0xd0  
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_1st);
	for(i=0; i<10; i++);
	
	// Row Address A12~A19	
	rNFADDR = row & 0xff;							
	for(i=0; i<10; i++);
	// Row Address A20~A27  
	rNFADDR = (row >> 8) & 0xff;
	for(i=0; i<10; i++);
	// Row Address A28~A30  
	rNFADDR = (row >> 16) & 0x01;	// ֻҪ���1bitΪ A28
	for(i=0; i<10; i++);
	
	rNFSTAT |= (1<<4);			// clear RnB bit 
	
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_2st);
	for(i=0; i<10; i++);
	// 3. �ȴ�����  
	nand_wait_idle();

	// 4. ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1)
	{
		// statas[0] = 1����ʾ����ʧ�ܣ����NAND Flash�����ֲ��� READ STATUSһ�ڵ�����
		// ȡ��Ƭѡ�ź�  
		nand_deselect_chip();						
		printf("masking bad block %d\r\n", block_addr);
		return -1;
	}
	else
	{
		// status[0] = 0����ʾ�����ɹ�������0
		nand_deselect_chip();
		return 0;
	}
}

// ��nand�ж����ݵ�sdram  
int copy_nand_to_sdram(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length)
{
	unsigned long i = 0;

	// 1. ����Ƭѡ�ź�  
	nand_select_chip();

	// 2. ��nand�����ݵ�sdram����һ���ڷ�����0x00���ڶ����ڷ���ַnand_addr,���������ڷ�����0x30���ɶ�һҳ(2k)������  
	while(length)
	{
		nand_send_cmd(NAND_CMD_READ_1st);
		nand_send_addr(nand_addr);
		rNFSTAT = (rNFSTAT)|(1<<4);
		nand_send_cmd(NAND_CMD_READ_2st);
		nand_wait_idle();

		// �е�ַ����ҳ�ڵ�ַ  
		unsigned long col = nand_addr % NAND_PAGE_SIZE;
		i = col;
		// ��һҳ���ݣ�ÿ�ο�1byte������2048��(2k)��ֱ������Ϊlength�����ݿ������ 
		for(; i<NAND_PAGE_SIZE && length!=0; i++,length--)
		{
			*sdram_addr = nand_read8();
			sdram_addr++;
			nand_addr++;
		}
	}

	// 3. ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// ȡ��Ƭѡ�ź�  
		nand_deselect_chip();
		printf("copy nand to sdram fail\r\n");
		return -1;
	}
	else
	{
		nand_deselect_chip();
		return 0;
	}
}

// ��sdram��д���ݵ�nand  
int copy_sdram_to_nand(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length)
{
	unsigned long i = 0;

	// 1. ����Ƭѡ�ź�  
	nand_select_chip();

	// 2. ��sdram�����ݵ�nand����һ���ڷ�����0x80���ڶ����ڷ���ַnand_addr,����������дһҳ(2k)���ݣ��������ڷ�0x10  
	while(length)
	{
		nand_send_cmd(NAND_CMD_WRITE_PAGE_1st);
		nand_send_addr(nand_addr);
		// �е�ַ����ҳ�ڵ�ַ  
		unsigned long col = nand_addr % NAND_PAGE_SIZE;
		i = col;
		// дһҳ���ݣ�ÿ�ο�1byte������2048��(2k)��ֱ������Ϊlength�����ݿ������ 
		for(; i<NAND_PAGE_SIZE && length!=0; i++,length--)
		{
			nand_write8(*sdram_addr);
			sdram_addr++;
			nand_addr++;
		}
		rNFSTAT = (rNFSTAT)|(1<<4);
		nand_send_cmd(NAND_CMD_WRITE_PAGE_2st);
		nand_wait_idle();
	}
	// 3. ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// ȡ��Ƭѡ�ź�  
		nand_deselect_chip();
		printf("copy sdram to nand fail\r\n");
		return -1;
	}
	else
	{
		nand_deselect_chip();
		return 0;
	}
}

int nand_page_read(unsigned int pgaddr, unsigned char *buf, unsigned int length)
{
	int i = 0;
	
	// 1 ����Ƭѡ�ź�  
	nand_select_chip();

	// 2 дҳ������1st  
	nand_send_cmd(NAND_CMD_READ_1st);
	
	// 3 д��ҳ��ַ
	rNFADDR = 0;										
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	
	// 4 clear RnB
	rNFSTAT |= (1<<4);
	
	// 5  дҳ������2st 
	nand_send_cmd(NAND_CMD_READ_2st);
	
	// 6 �ȴ�����
	nand_wait_idle();
	
	// 7 ������ȡ2KB��Page main������ ��������ȡ�ɶ���64B��spare area���ݣ�
	for (i=0; (i<NAND_PAGE_SIZE) && (length!=0); i++,length--)
		*buf++ = nand_read8();
	
	// 8 ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// ������ȡ��Ƭѡ�źţ����ش�����-1  
		nand_deselect_chip();
		printf("nand random read fail\r\n");
		return -1;
	}
	else
	{
		// ����ȷ��ȡ��Ƭѡ������0
		nand_deselect_chip();
		return 0;
	}
}


int nand_page_read32(unsigned int pgaddr, unsigned int *buf, unsigned int lengthB)
{
	int i = 0;
	
	// 1 ����Ƭѡ�ź�  
	nand_select_chip();

	// 2 дҳ������1st  
	nand_send_cmd(NAND_CMD_READ_1st);
	
	// 3 д��ҳ��ַ
	rNFADDR = 0;										
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	
	// 4 clear RnB
	rNFSTAT |= (1<<4);
	
	// 5  дҳ������2st 
	nand_send_cmd(NAND_CMD_READ_2st);
	
	// 6 �ȴ�����
	nand_wait_idle();
	
	// 7 ������ȡ2KB��Page main������ ��������ȡ�ɶ���64B��spare area���ݣ�
	for (i=0; (i<NAND_PAGE_SIZE/4) && (lengthB!=0); i++,lengthB--)
		*buf++ = nand_read32();
	
	// 8 ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// ������ȡ��Ƭѡ�źţ����ش�����-1  
		nand_deselect_chip();
		printf("nand random read fail\r\n");
		return -1;
	}
	else
	{
		// ����ȷ��ȡ��Ƭѡ������0
		nand_deselect_chip();
		return 0;
	}
}


int nand_page_write(unsigned int pgaddr, const unsigned char *buf, unsigned int length)
{
	int i = 0;
	
	// 1 ����Ƭѡ�ź�  
	nand_select_chip();

	// 2 write cmd 1st  
	nand_send_cmd(NAND_CMD_WRITE_PAGE_1st);
	
	// 3 write page addr
	rNFADDR = 0;
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;

	// 4 д��һҳ����
	for(; i<NAND_PAGE_SIZE && length!=0; i++,length--)
		nand_write8(*buf++);
	
	// 5 clear RnB
	rNFSTAT = (rNFSTAT)|(1<<4);
	
	// 6 write cmd 2
	nand_send_cmd(NAND_CMD_WRITE_PAGE_2st);
	
	// 7 wait idle
	nand_wait_idle();
	
	// 8 ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// ȡ��Ƭѡ�ź�  
		nand_deselect_chip();
		printf("nand random write fail\r\n");
		return -1;
	}
	else
	{
		nand_deselect_chip();
		return 0;
	}
	
}
 
/*
 * �������ܣ�	��������� 
 * ������		pgaddr Ϊҳ��ַ, offsetΪҳ��ƫ�Ƶ�ַ��dataΪ����ֵ
 * ����ֵ��		0��ʾ��ȡ�ɹ���1��ʾ��ȡʧ��
*/ 
int nand_random_read(unsigned long pgaddr,unsigned short offset, unsigned char *data)
{
	unsigned char readdata;

	// 1. ����Ƭѡ�ź�  
	nand_select_chip();

	// 2. �����ҳ��ĳ����ַ��ֵ  
	nand_send_cmd(NAND_CMD_READ_1st);
	//д��ҳ��ַ
	rNFADDR = 0;										
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	rNFSTAT |= (1<<4);
	nand_send_cmd(NAND_CMD_READ_2st);
	nand_wait_idle();

	nand_send_cmd(NAND_CMD_RANDOM_READ_1st);
	//д��ҳ��ƫ�Ƶ�ַ
	rNFADDR = offset&0xff; 							
	rNFADDR = (offset>>8)&0xff;
	rNFSTAT = (rNFSTAT)|(1<<4);
	nand_send_cmd(NAND_CMD_RANDOM_READ_2st);

	readdata = nand_read8();

	// 3. ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// ȡ��Ƭѡ�ź�  
		nand_deselect_chip();
		printf("nand random read fail\r\n");
		return -1;
	}
	else
	{
		nand_deselect_chip();
		*data = readdata;
		return 0;
	}
}

/*
 * �������ܣ�	���д����
 * ������		pgaddr Ϊҳ��ַ, offsetΪҳ��ƫ�Ƶ�ַ��wrdataΪҪд�������
 * ����ֵ��		0��ʾд��ɹ���1��ʾд��ʧ��
 * ���Խ��ۣ�	1��random writeһ��ֻ��д��һ���ֽڣ�����ڲ�ֻ��ʹ�� nand_write8��ʹ��nand_write32�ͻ����
*/ 
int nand_random_write(unsigned long pgaddr,unsigned short offset,unsigned char wrdata)
{
	// 1. ����Ƭѡ�ź�  
	nand_select_chip();

	// 2. ���дҳ��ĳ����ַ��ֵ  
	nand_send_cmd(NAND_CMD_WRITE_PAGE_1st);
	rNFADDR = 0;
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	nand_send_cmd(NAND_CMD_RANDOM_WRITE);
	//д��ҳ��ƫ�Ƶ�ַ
	rNFADDR = offset&0xff; 					
	rNFADDR = (offset>>8)&0xff;
	nand_write8(wrdata);
	rNFSTAT = (rNFSTAT)|(1<<4);
	nand_send_cmd(NAND_CMD_WRITE_PAGE_2st);
	nand_wait_idle();

	// 3. ��״̬  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// ȡ��Ƭѡ�ź�  
		nand_deselect_chip();
		printf("nand random write fail\r\n");
		return -1;
	}
	else
	{
		nand_deselect_chip();
		return 0;
	}
}


// nand_test ʹ�õ���BLOCK_NO���е���ҳ
#define BLOCK_NO	10
#define PAGE_NO		(BLOCK_NO * NAND_BLOCK_SIZE)
#define PAGE_ADDR	(PAGE_NO * NAND_PAGE_SIZE)


#define OFFSET0 4
#define OFFSET1 5
#define OFFSET2 6
#define OFFSET3 7

void nand_test(void)
{
	int ret = 0;
	unsigned char data1, data2, data3, data4;
	unsigned char buf[8];
	unsigned int bufInt[2];
	
	nand_init();
	nand_read_id();
	

	#if 0
	// ���Ȳ�������д��
	if ((ret = nand_erase(PAGE_ADDR)) == 0)
		printf("success to erase block %d\r\n", BLOCK_NO);
	else
		printf("fail to erase block %d\r\n", BLOCK_NO);
	#endif
	
	
	// �ȸ�Ѿrandomд4��byte
	#if 1
	nand_random_write(PAGE_ADDR, OFFSET0, 'a');
	nand_random_write(PAGE_ADDR, OFFSET1, 'b');
	nand_random_write(PAGE_ADDR, OFFSET2, 'c');
	nand_random_write(PAGE_ADDR, OFFSET3, 'd');
	#endif
	
	#if 0
	nand_random_write(PAGE_ADDR, OFFSET0, 0xba);
	nand_random_write(PAGE_ADDR, OFFSET1, 0xde);
	nand_random_write(PAGE_ADDR, OFFSET2, 0xc0);
	nand_random_write(PAGE_ADDR, OFFSET3, 0xde);
	
	#endif

	// Ȼ���������ַ����������������Բ��Ե���
	nand_random_read(PAGE_ADDR, OFFSET0, &data1);
	nand_random_read(PAGE_ADDR, OFFSET1, &data2);
	nand_random_read(PAGE_ADDR, OFFSET2, &data3);
	nand_random_read(PAGE_ADDR, OFFSET3, &data4);
	
	printf("PAGE_ADDR: \r\n", PAGE_ADDR);
	printf("4 byte data from nand_random_read: %x, %x, %x, %x\r\n", data1, data2, data3, data4);
	
	ret = nand_page_read(PAGE_ADDR, buf, sizeof(buf));
	if (ret != 0)
		printf("nand_page_read error!\r\n");
	else
		printf("4 byte data form nand_page_read: %x, %x, %x, %x\r\n", buf[OFFSET0], buf[OFFSET1], buf[OFFSET2], buf[OFFSET3]);
	
	ret = nand_page_read32(PAGE_ADDR, bufInt, sizeof(bufInt)/sizeof(unsigned int));
	if (ret != 0)
		printf("nand_page_read32 error!\r\n");
	else	
		printf("1 word data form nand_page_read32: %x\r\n", bufInt[OFFSET0]);
	
	
}


