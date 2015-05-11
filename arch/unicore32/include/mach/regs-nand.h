/*
                                    
 */
/*
                      
 */
#define NAND_IDR0	(PKUNITY_NAND_BASE + 0x0000)
/*
                      
 */
#define NAND_IDR1	(PKUNITY_NAND_BASE + 0x0004)
/*
                      
 */
#define NAND_IDR2	(PKUNITY_NAND_BASE + 0x0008)
/*
                      
 */
#define NAND_IDR3	(PKUNITY_NAND_BASE + 0x000C)
/*
                               
 */
#define NAND_PAR0	(PKUNITY_NAND_BASE + 0x0010)
/*
                               
 */
#define NAND_PAR1	(PKUNITY_NAND_BASE + 0x0014)
/*
                               
 */
#define NAND_PAR2	(PKUNITY_NAND_BASE + 0x0018)
/*
                            
 */
#define NAND_ECCEN	(PKUNITY_NAND_BASE + 0x001C)
/*
                      
 */
#define NAND_BUF	(PKUNITY_NAND_BASE + 0x0020)
/*
                            
 */
#define NAND_ECCSR	(PKUNITY_NAND_BASE + 0x0024)
/*
                       
 */
#define NAND_CMD	(PKUNITY_NAND_BASE + 0x0028)
/*
                               
 */
#define NAND_DMACR	(PKUNITY_NAND_BASE + 0x002C)
/*
                        
 */
#define NAND_IR		(PKUNITY_NAND_BASE + 0x0030)
/*
                              
 */
#define NAND_IMR	(PKUNITY_NAND_BASE + 0x0034)
/*
                              
 */
#define NAND_CHIPEN	(PKUNITY_NAND_BASE + 0x0038)
/*
                        
 */
#define NAND_ADDR	(PKUNITY_NAND_BASE + 0x003C)

/*
                                 
 */
#define NAND_CMD_CMD_MASK		FMASK(4, 4)
#define NAND_CMD_CMD_READPAGE		FIELD(0x0, 4, 4)
#define NAND_CMD_CMD_ERASEBLOCK		FIELD(0x6, 4, 4)
#define NAND_CMD_CMD_READSTATUS		FIELD(0x7, 4, 4)
#define NAND_CMD_CMD_WRITEPAGE		FIELD(0x8, 4, 4)
#define NAND_CMD_CMD_READID		FIELD(0x9, 4, 4)
#define NAND_CMD_CMD_RESET		FIELD(0xf, 4, 4)
