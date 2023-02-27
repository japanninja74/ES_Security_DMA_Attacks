#ifndef UART_H
	#define UART_H
	#include "uart.h"
#endif

int init_uart(struct uart_parameters* up){
	LPC_UART_TypeDef* uart;
	switch(up->uart_n){
		case UART0:
			LPC_SC->PCONP |= (1 << 3);
			LPC_SC->PCLKSEL0 |= (up->clock_selection << 6);
			uart = (LPC_UART_TypeDef*) (LPC_APB0_BASE  + 0x0C000);
			LPC_PINCON->PINSEL0 |= (
				(1 << 4) | 
				(1 << 6)
			);
			if(up->dma_mode == UART_DMA){
				LPC_SC->DMAREQSEL &= 0b11111100;
			}
			break;
		case UART2:
			// Enable uart peripheral
			LPC_SC->PCONP |= (1 << 24);
			// Select clock
			LPC_SC->PCLKSEL0 |= (up->clock_selection << 16);
			// Get uart structure
			uart = (LPC_UART_TypeDef*) (LPC_APB1_BASE  + 0x18000);
			// Select pins for uart
			LPC_PINCON->PINSEL0 |= (
				(1 << 20) | 
				(1 << 22)
			);
			// Select lines for uart2
			if(up->dma_mode == UART_DMA){
				LPC_SC->DMAREQSEL &= 0b11001111;
			}
			break;
		case UART3:
			LPC_SC->PCONP |= (1 << 25);
			LPC_SC->PCLKSEL0 |= (up->clock_selection << 18);
			uart = (LPC_UART_TypeDef*) (LPC_APB1_BASE  + 0x1C000);
			LPC_PINCON->PINSEL0 |= (
				(2 << 0) | 
				(2 << 2)
			);
			if(up->dma_mode == UART_DMA){
				LPC_SC->DMAREQSEL &= 0b00111111;
			}
			break;
		default:
			return 1;
	}

	// Set baud rate
	uart->LCR |= (1 << 7);
	uart->DLL = up->DLL;
	uart->DLM = up->DLM;
	uart->FDR = (
		(up->DivAddVal << 0) |
		(up->MulVal << 4)
	);
	uart->LCR &= ~(1 << 7);

	// Set fifo
	uart->FCR = (
		(up->fifo_enable << 0) |
		(1 << 1) |
		(1 << 2) |
		(up->dma_mode << 3) |
		(up->trigger_level << 6)
	);
	
	// Enable interrupt
	uart->IER = (
		(up->rdv_int << 0)
	);

	// Set uart parameters
	uart->LCR = (
		(up->word_size << 0) |
		(up->stop_bit_n << 2) |
		(up->parity_active << 3) |
		(up->parity_type << 4)
	);

	return 0;
}

void UART2_IRQHandler(void){
	return;
}

