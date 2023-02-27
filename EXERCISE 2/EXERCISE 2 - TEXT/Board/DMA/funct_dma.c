#ifndef DMA_H 
	#define DMA_H 
	#include "dma.h"
#endif

void lli_add_elem(struct dma_lli_element** head, struct dma_lli_element* to_add){
	to_add->next_lli = 0;
	if((*head)==0)
		(*head) = to_add;

	else{
		struct dma_lli_element* pointer = (*head);
		while(pointer->next_lli != 0) pointer = pointer->next_lli;
		pointer->next_lli = to_add;
	}
}

int  transfer_DMA(struct dma_transfer* transfer){
	if(!transfer) return 1;
	if(transfer->channel >=8) return 1;
	if(transfer->source_burst_size >= 8) return 1;
	if(transfer->dest_burst_size >= 8) return 1;
	if(transfer->source_increment >= 2) return 1;
	if(transfer->dest_increment >= 2) return 1;
	if(transfer->source_transfer_width >= 3) return 1;
	if(transfer->dest_transfer_width >= 3) return 1;
	if(transfer->terminal_count_enable >= 2) return 1;
	if(transfer->transfer_type >= 4) return 1;

	// select the correct channel
	LPC_GPDMACH_TypeDef* channel = 
		(LPC_GPDMACH_TypeDef*)
		((LPC_AHB_BASE  + 0x04100 + 0x20 * transfer->channel));

	//page 596:		Clears the error interrupt requests
	LPC_GPDMA->DMACIntErrClr |= 0xFF;

	//page 595:		Clears the terminal counter interrupt request register
	LPC_GPDMA->DMACIntTCClear |= 0xFF;

	//page 601:		Set dest address 
	channel->DMACCDestAddr = transfer->dest;

	//page 601:		Set source address 
	channel->DMACCSrcAddr = transfer->source;

	//page 601: 	Scatter Gather
	channel->DMACCLLI = transfer->lli_head;
	
	//page 603: 	[11:0]	TransferSize 
	//				[14:12]	SourceBurstSize 
	//				[17_15]	DestBurstSize 
	//				[20:18]	Source transfer width 
	//				[23:21] Dest transfer width 
	//				[26]	Source address increment 
	//				[27]	Dest address   increment 
	//				[28]	Access to the peripheral in user mode
	//				[29]	Access to the peripheral is not bufferable
	//				[30]	Access is                   not cachable
	//				[31]	Terminal count interrupt 
	channel->DMACCControl = (
		(transfer->terminal_count_enable << 31) |
		(1<< 28)| //aadded for priviledge mode
		(transfer->dest_increment << 27) | 
		(transfer->source_increment << 26 ) |
		(transfer->dest_transfer_width << 21) |
		(transfer->source_transfer_width << 18) |
		(transfer->dest_burst_size << 15) |
		(transfer->source_burst_size << 12) | 
		(transfer->size << 0)
	);

	//page 605:		[0]		Channel enabled 
	//				[5:1]	Source peripheral
	//				[10:6]	Dest peripheral
	//				[13:11]	Transfer Type
	//				[14]	Interrupt error maks cleared
	//				[15]	terminal counter interrupt mask cleared
	//				[16]	Not used in LPC1768
	//				[17]	There is no data in the FIFO of the channel
	//				[18]	Enable DMA request
	channel->DMACCConfig = (
		(transfer->terminal_count_enable << 15) |
		(transfer->transfer_type << 11) |
		(transfer->dest_peripheral << 6) |
		(transfer->source_peripheral << 1) |
		(1 << 0)
	);

	return 0;
}
