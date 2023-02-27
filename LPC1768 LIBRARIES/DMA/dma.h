#ifndef lpc17xx_H 
	#define lpc17xx_H 
	#include "lpc17xx.h"
#endif

#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2 
#define DMA_CHANNEL_3 3 
#define DMA_CHANNEL_4 4
#define DMA_CHANNEL_5 5
#define DMA_CHANNEL_6 6
#define DMA_CHANNEL_7 7

#define DMA_NO_INCREMENT 	0
#define DMA_INCREMENT 		1

#define DMA_LITTLE_ENDIAN	0
#define DMA_BIG_ENDIAN 		1

#define DMA_M2M	0
#define	DMA_M2P 1
#define DMA_P2M 2
#define DMA_P2P 3

#define DMA_NO_PERIPHERAL 	0
#define DMA_UART2_RX 		13 
#define DMA_UART2_TX 		12

#define DMA_BURST_SIZE_1	0
#define DMA_BURST_SIZE_4	1
#define DMA_BURST_SIZE_8	2
#define DMA_BURST_SIZE_16	3
#define DMA_BURST_SIZE_32	4
#define DMA_BURST_SIZE_64	5
#define DMA_BURST_SIZE_128	6
#define DMA_BURST_SIZE_256	7

#define DMA_TRANSFER_WIDTH_8	0
#define DMA_TRANSFER_WIDTH_16	1
#define DMA_TRANSFER_WIDTH_32	2

#define DMA_TERMINAL_COUNT_ENABLE 		1
#define DMA_TERMINAL_COUNT_NOT_ENABLE 	0

struct dma_transfer{
	uint32_t	channel;
	uint32_t 	size;
	uint32_t 	dest;
	uint32_t 	source;
	uint32_t	source_burst_size;
	uint32_t	dest_burst_size;
	uint32_t 	source_increment;
	uint32_t 	dest_increment;
	uint32_t	source_transfer_width;
	uint32_t	dest_transfer_width;
	uint32_t	terminal_count_enable;
	uint32_t 	transfer_type;
	uint32_t 	source_peripheral;
	uint32_t	dest_peripheral;
	uint32_t 	lli_head;
};

struct dma_lli_element{
	uint32_t 					src;
	uint32_t 					dest;
	struct dma_lli_element* 	next_lli;
	uint32_t					ctrl;
};

void init_DMA(uint8_t);
int  transfer_DMA(struct dma_transfer*);
extern void DMA_IRQHandler(void);
void lli_add_elem(struct dma_lli_element**, struct dma_lli_element* );
