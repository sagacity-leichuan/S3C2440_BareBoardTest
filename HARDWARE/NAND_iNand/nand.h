void nand_init(void);
void nand_read_id(void);
int nand_block_erase(unsigned long block_num);
int copy_nand_to_sdram(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length);
int copy_sdram_to_nand(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length);
int nand_page_read(unsigned int pgaddr, unsigned char *buf, unsigned int length);
int nand_page_write(unsigned int pgaddr, const unsigned char *buf, unsigned int length);
int nand_random_read(unsigned long pgaddr,unsigned short offset, unsigned char *data);
int nand_random_write(unsigned long pgaddr,unsigned short offset,unsigned char wrdata);

