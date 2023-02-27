#ifndef DMA_H 
	#define DMA_H 
	#include "dma.h"
#endif

void init_DMA(uint8_t endianess){
	LPC_SC->PCONP |= (1 << 29);

	//page 599:		Enable DMA controller and little endian mode
	LPC_GPDMA->DMACConfig = (1 << 0) |
							(endianess << 1);

	LPC_GPDMA->DMACSync = 0;

	NVIC_EnableIRQ(DMA_IRQn);
}

