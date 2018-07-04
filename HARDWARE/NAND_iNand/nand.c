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

#define MAX_NAND_BLOCK  			  8192 			/*定义nand最大块数：8192块 	*/
#define NAND_PAGE_SIZE  			  2048 			/*定义一页的容量:2048 byte 	*/
#define NAND_BLOCK_SIZE 			  64  			/*定义block大小：64页		*/

#define TACLS    					  1		// 12ns		/* 时序相关的设置 			*/
#define TWRPH0   					  4
#define TWRPH1   					  1

#define NAND_CMD_READ_1st             0x00			/* 命令						*/
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

// 复位  
void nand_reset(void)
{
	nand_select_chip();
	nand_send_cmd(NAND_CMD_RESET);
	nand_wait_idle();
	nand_deselect_chip();
}

// 等待就绪  
void nand_wait_idle(void)
{
	unsigned long i;
	while( !(rNFSTAT & (1<<4)) )
		for(i=0; i<10; i++);
}

// 发片选  
void nand_select_chip(void)
{
	unsigned long i;
	rNFCONT &= ~(1<<1);
	for(i=0; i<10; i++);
}

// 取消片选  
void nand_deselect_chip(void)
{
	unsigned long i = 0;
	rNFCONT |= (1<<1);
	for(i=0; i<10; i++);
}

// 发命令  
void nand_send_cmd(unsigned long cmd)
{
	unsigned long i = 0;

	rNFCMMD = cmd;
	for(i=0; i<10; i++);
}

// 发地址  
void nand_send_addr(unsigned long addr)
{
	unsigned long i;
	unsigned long col, row;

	// 列地址，即页内地址  
	col = addr % NAND_PAGE_SIZE;		
	// 行地址，即页地址  			
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

// 读一个字节的数据  
unsigned char nand_read8(void)
{
	return rNFDATA8;
}

// 写一个字节的数据  
void nand_write8(unsigned char data)
{
	rNFDATA8 = data;
}

unsigned char nand_read_status(void)
{
	unsigned char ch;
	int i;

	// 1. 发出片选信号  
	nand_select_chip();

	// 2. 读状态  
	nand_send_cmd(NAND_CMD_READ_STATUS);
	for(i=0; i<10; i++);
	ch = nand_read8();

	// 3. 取消片选  
	nand_deselect_chip();
	return ch;
}

// nandflash 初始化  
void nand_init(void)
{

	// 1. 配置nandflash  
	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0);
	rNFCONT = (0<<18)|(0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0x3<<1)|(1<<0);

	// 2. 配置引脚  
	rMP0_1CON = 0x22333322;
	rMP0_2CON = 0x00002222;
	rMP0_3CON = 0x22222222;

	// 3. 复位  
	nand_reset();
}

// 读芯片ID  
void nand_read_id(void)
{
	nand_id_info nand_id;
	// 1. 发片选  
	nand_select_chip();

	// 2. 读ID  
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

// 擦除块，参数为块号（0 ~ MAX_NAND_BLOCK-1）  
int nand_block_erase(unsigned long block_num)
{
	unsigned long i = 0;

	// 获得row地址，即页地址  
	unsigned long row = block_num * NAND_BLOCK_SIZE;

	// 1. 发出片选信号  
	nand_select_chip();
	// 2. 擦除：第一个周期发命令0x60，第二个周期发块地址，第三个周期发命令0xd0  
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
	// 3. 等待就绪  
	nand_wait_idle();

	// 4. 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// statas[0] = 1，表示擦除失败，详见NAND Flash数据手册中 READ STATUS一节的描述
		// 取消片选信号  
		nand_deselect_chip();						
		printf("masking bad block %d\r\n", block_num);
		return -1;
	}
	else
	{
		// status[0] = 0，表示擦除成功，返回0
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
	// 1. 发出片选信号  
	nand_select_chip();
	// 2. 擦除：第一个周期发命令0x60，第二个周期发块地址，第三个周期发命令0xd0  
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_1st);
	for(i=0; i<10; i++);
	
	// Row Address A12~A19	
	rNFADDR = row & 0xff;							
	for(i=0; i<10; i++);
	// Row Address A20~A27  
	rNFADDR = (row >> 8) & 0xff;
	for(i=0; i<10; i++);
	// Row Address A28~A30  
	rNFADDR = (row >> 16) & 0x01;	// 只要最低1bit为 A28
	for(i=0; i<10; i++);
	
	rNFSTAT |= (1<<4);			// clear RnB bit 
	
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_2st);
	for(i=0; i<10; i++);
	// 3. 等待就绪  
	nand_wait_idle();

	// 4. 读状态  
	unsigned char status = nand_read_status();
	if (status & 1)
	{
		// statas[0] = 1，表示擦除失败，详见NAND Flash数据手册中 READ STATUS一节的描述
		// 取消片选信号  
		nand_deselect_chip();						
		printf("masking bad block %d\r\n", block_addr);
		return -1;
	}
	else
	{
		// status[0] = 0，表示擦除成功，返回0
		nand_deselect_chip();
		return 0;
	}
}

// 从nand中读数据到sdram  
int copy_nand_to_sdram(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length)
{
	unsigned long i = 0;

	// 1. 发出片选信号  
	nand_select_chip();

	// 2. 从nand读数据到sdram，第一周期发命令0x00，第二周期发地址nand_addr,第三个周期发命令0x30，可读一页(2k)的数据  
	while(length)
	{
		nand_send_cmd(NAND_CMD_READ_1st);
		nand_send_addr(nand_addr);
		rNFSTAT = (rNFSTAT)|(1<<4);
		nand_send_cmd(NAND_CMD_READ_2st);
		nand_wait_idle();

		// 列地址，即页内地址  
		unsigned long col = nand_addr % NAND_PAGE_SIZE;
		i = col;
		// 读一页数据，每次拷1byte，共拷2048次(2k)，直到长度为length的数据拷贝完毕 
		for(; i<NAND_PAGE_SIZE && length!=0; i++,length--)
		{
			*sdram_addr = nand_read8();
			sdram_addr++;
			nand_addr++;
		}
	}

	// 3. 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// 取消片选信号  
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

// 从sdram中写数据到nand  
int copy_sdram_to_nand(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length)
{
	unsigned long i = 0;

	// 1. 发出片选信号  
	nand_select_chip();

	// 2. 从sdram读数据到nand，第一周期发命令0x80，第二周期发地址nand_addr,第三个周期写一页(2k)数据，第四周期发0x10  
	while(length)
	{
		nand_send_cmd(NAND_CMD_WRITE_PAGE_1st);
		nand_send_addr(nand_addr);
		// 列地址，即页内地址  
		unsigned long col = nand_addr % NAND_PAGE_SIZE;
		i = col;
		// 写一页数据，每次拷1byte，共拷2048次(2k)，直到长度为length的数据拷贝完毕 
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
	// 3. 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// 取消片选信号  
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
	
	// 1 发出片选信号  
	nand_select_chip();

	// 2 写页读命令1st  
	nand_send_cmd(NAND_CMD_READ_1st);
	
	// 3 写入页地址
	rNFADDR = 0;										
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	
	// 4 clear RnB
	rNFSTAT |= (1<<4);
	
	// 5  写页读命令2st 
	nand_send_cmd(NAND_CMD_READ_2st);
	
	// 6 等待空闲
	nand_wait_idle();
	
	// 7 连续读取2KB的Page main区数据 （继续读取可读出64B的spare area数据）
	for (i=0; (i<NAND_PAGE_SIZE) && (length!=0); i++,length--)
		*buf++ = nand_read8();
	
	// 8 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// 读出错，取消片选信号，返回错误码-1  
		nand_deselect_chip();
		printf("nand random read fail\r\n");
		return -1;
	}
	else
	{
		// 读正确，取消片选，返回0
		nand_deselect_chip();
		return 0;
	}
}


int nand_page_read32(unsigned int pgaddr, unsigned int *buf, unsigned int lengthB)
{
	int i = 0;
	
	// 1 发出片选信号  
	nand_select_chip();

	// 2 写页读命令1st  
	nand_send_cmd(NAND_CMD_READ_1st);
	
	// 3 写入页地址
	rNFADDR = 0;										
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	
	// 4 clear RnB
	rNFSTAT |= (1<<4);
	
	// 5  写页读命令2st 
	nand_send_cmd(NAND_CMD_READ_2st);
	
	// 6 等待空闲
	nand_wait_idle();
	
	// 7 连续读取2KB的Page main区数据 （继续读取可读出64B的spare area数据）
	for (i=0; (i<NAND_PAGE_SIZE/4) && (lengthB!=0); i++,lengthB--)
		*buf++ = nand_read32();
	
	// 8 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// 读出错，取消片选信号，返回错误码-1  
		nand_deselect_chip();
		printf("nand random read fail\r\n");
		return -1;
	}
	else
	{
		// 读正确，取消片选，返回0
		nand_deselect_chip();
		return 0;
	}
}


int nand_page_write(unsigned int pgaddr, const unsigned char *buf, unsigned int length)
{
	int i = 0;
	
	// 1 发出片选信号  
	nand_select_chip();

	// 2 write cmd 1st  
	nand_send_cmd(NAND_CMD_WRITE_PAGE_1st);
	
	// 3 write page addr
	rNFADDR = 0;
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;

	// 4 写入一页内容
	for(; i<NAND_PAGE_SIZE && length!=0; i++,length--)
		nand_write8(*buf++);
	
	// 5 clear RnB
	rNFSTAT = (rNFSTAT)|(1<<4);
	
	// 6 write cmd 2
	nand_send_cmd(NAND_CMD_WRITE_PAGE_2st);
	
	// 7 wait idle
	nand_wait_idle();
	
	// 8 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// 取消片选信号  
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
 * 函数功能：	随机读数据 
 * 参数：		pgaddr 为页地址, offset为页内偏移地址，data为返回值
 * 返回值：		0表示读取成功，1表示读取失败
*/ 
int nand_random_read(unsigned long pgaddr,unsigned short offset, unsigned char *data)
{
	unsigned char readdata;

	// 1. 发出片选信号  
	nand_select_chip();

	// 2. 随机读页内某个地址的值  
	nand_send_cmd(NAND_CMD_READ_1st);
	//写入页地址
	rNFADDR = 0;										
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	rNFSTAT |= (1<<4);
	nand_send_cmd(NAND_CMD_READ_2st);
	nand_wait_idle();

	nand_send_cmd(NAND_CMD_RANDOM_READ_1st);
	//写入页内偏移地址
	rNFADDR = offset&0xff; 							
	rNFADDR = (offset>>8)&0xff;
	rNFSTAT = (rNFSTAT)|(1<<4);
	nand_send_cmd(NAND_CMD_RANDOM_READ_2st);

	readdata = nand_read8();

	// 3. 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// 取消片选信号  
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
 * 函数功能：	随机写数据
 * 参数：		pgaddr 为页地址, offset为页内偏移地址，wrdata为要写入的数据
 * 返回值：		0表示写入成功，1表示写入失败
 * 测试结论：	1、random write一次只能写入一个字节，因此内部只能使用 nand_write8，使用nand_write32就会出错
*/ 
int nand_random_write(unsigned long pgaddr,unsigned short offset,unsigned char wrdata)
{
	// 1. 发出片选信号  
	nand_select_chip();

	// 2. 随机写页内某个地址的值  
	nand_send_cmd(NAND_CMD_WRITE_PAGE_1st);
	rNFADDR = 0;
	rNFADDR = 0;
	rNFADDR = pgaddr&0xff;
	rNFADDR = (pgaddr>>8)&0xff;
	rNFADDR = (pgaddr>>16)&0xff;
	nand_send_cmd(NAND_CMD_RANDOM_WRITE);
	//写入页内偏移地址
	rNFADDR = offset&0xff; 					
	rNFADDR = (offset>>8)&0xff;
	nand_write8(wrdata);
	rNFSTAT = (rNFSTAT)|(1<<4);
	nand_send_cmd(NAND_CMD_WRITE_PAGE_2st);
	nand_wait_idle();

	// 3. 读状态  
	unsigned char status = nand_read_status();
	if (status & 1 )
	{
		// 取消片选信号  
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


// nand_test 使用的是BLOCK_NO块中的首页
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
	// 得先擦除才能写啊
	if ((ret = nand_erase(PAGE_ADDR)) == 0)
		printf("success to erase block %d\r\n", BLOCK_NO);
	else
		printf("fail to erase block %d\r\n", BLOCK_NO);
	#endif
	
	
	// 先给丫random写4个byte
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

	// 然后再用三种方法，读出来看看对不对得上
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


