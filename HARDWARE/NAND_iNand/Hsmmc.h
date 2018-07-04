#ifndef __HSMMC_H__
#define __HSMMC_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include "stdint.h"

// SD协议规定的命令码	
#define	CMD0	0
#define	CMD1	1
#define	CMD2	2	
#define	CMD3	3	
#define	CMD6	6
#define	CMD7	7
#define	CMD8	8
#define	CMD9	9
#define	CMD13	13
#define	CMD16	16
#define	CMD17	17
#define	CMD18	18
#define	CMD23	23	
#define	CMD24	24
#define	CMD25	25	
#define	CMD32	32	
#define	CMD33	33	
#define	CMD38	38	
#define	CMD41	41	
#define CMD51	51
#define	CMD55	55	

// 卡类型
#define UNUSABLE		0
#define SD_V1			1	
#define	SD_V2			2	
#define	SD_HC			3
#define	MMC				4
	
// 卡状态
#define CARD_IDLE		0			// 空闲态
#define CARD_READY		1			// 准备好
#define CARD_IDENT		2
#define CARD_STBY		3
#define CARD_TRAN		4
#define CARD_DATA		5
#define CARD_RCV		6
#define CARD_PRG		7			// 卡编程状态
#define CARD_DIS		8			// 断开连接

// 卡回复类型	
#define CMD_RESP_NONE	0			// 无回复
#define CMD_RESP_R1		1
#define CMD_RESP_R2		2
#define CMD_RESP_R3		3
#define CMD_RESP_R4		4
#define CMD_RESP_R5		5
#define CMD_RESP_R6		6
#define CMD_RESP_R7		7
#define CMD_RESP_R1B	8
	
typedef struct {
	uint32_t RESERVED1;
	uint32_t RESERVED2 : 16;	
	uint32_t SD_BUS_WIDTHS : 4;
	uint32_t SD_SECURITY : 3;	
	uint32_t DATA_STAT_AFTER_ERASE : 1;
	uint32_t SD_SPEC : 4;	
	uint32_t SCR_STRUCTURE : 4;
} SD_SCR;	

int32_t Hsmmc_Init(void);
int32_t Hsmmc_GetCardState(void);
int32_t Hsmmc_GetSdState(uint8_t *pStatus);
int32_t Hsmmc_Get_SCR(SD_SCR *pSCR);
int32_t Hsmmc_Get_CSD(uint8_t *pCSD);
int32_t Hsmmc_EraseBlock(uint32_t StartBlock, uint32_t EndBlock);
int32_t Hsmmc_WriteBlock(uint8_t *pBuffer, uint32_t BlockAddr, uint32_t BlockNumber);
int32_t Hsmmc_ReadBlock(uint8_t *pBuffer, uint32_t BlockAddr, uint32_t BlockNumber);
	
#ifdef __cplusplus
}
#endif

#endif /*__HSMMC_H__*/
