#include <movi.h>

#define MMC_CHANNEL        2
#define SD_START_BLOCK     45
//#define SD_BLOCK_CNT       64 //64 block = 32KB，32KB对目前的cc_bootloader已经有点小了
#define SD_BLOCK_CNT       256

#define DDR_TARTGET_ADDR   0x20000000 //cc_bootloader的链接地址

typedef unsigned int(*copy_sd_mmc_to_mem)
(unsigned int channel, unsigned int start_block, unsigned short block_size, unsigned int *trg, unsigned int init);

void movi_bl2_copy(void)
{
	copy_sd_mmc_to_mem copy_bl2 = 
		(copy_sd_mmc_to_mem)(*(unsigned int*) (0xD0037F98));

	copy_bl2(MMC_CHANNEL, SD_START_BLOCK, SD_BLOCK_CNT, (unsigned int *)DDR_TARTGET_ADDR, 0);
}
