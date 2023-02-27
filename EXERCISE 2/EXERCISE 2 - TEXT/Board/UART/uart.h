#ifndef lpc17xx_H 
	#define lpc17xx_H 
	#include "lpc17xx.h"
#endif

#define UART0 0
#define UART2 2
#define UART3 3

#define UART_CCLK_DIV_4 0
#define UART_CCLK_DIV_1 1
#define UART_CCLK_DIV_2 2
#define UART_CCLK_DIV_8 3

#define UART_EN_FIFO 1
#define UART_NO_FIFO 0

#define UART_TR_LEVEL_1		0
#define UART_TR_LEVEL_4		1
#define UART_TR_LEVEL_8		2
#define UART_TR_LEVEL_14	3

#define UART_RECV_DV_INT	1
#define UART_NO_INT			0

#define UART_WORD_5	0
#define UART_WORD_6	1
#define UART_WORD_7	2
#define UART_WORD_8	3

#define UART_STOP_1	0
#define UART_STOP_2	1

#define UART_PARITY		1
#define UART_NO_PARITY	0

#define UART_DMA 		1
#define UART_NO_DMA 	0

#define UART_ODD_PARITY		0
#define UART_EVEN_PARITY	1

struct uart_parameters{
	uint32_t	uart_n;
	uint32_t	clock_selection;
	uint32_t	DLL;
	uint32_t	DLM;
	uint32_t	DivAddVal;
	uint32_t	MulVal;
	uint32_t	fifo_enable;
	uint32_t	dma_mode;
	uint32_t	trigger_level;
	uint32_t	rdv_int;
	uint32_t	word_size;
	uint32_t	stop_bit_n;
	uint32_t	parity_active;
	uint32_t	parity_type;
};

int init_uart(struct uart_parameters*);
void UART2_IRQHandler(void);
