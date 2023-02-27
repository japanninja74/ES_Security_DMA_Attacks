/*----------------------------------------------------------------------------
 * Name:    main.c
 * Purpose:
 * Note(s):
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "BSP.h"
#include "LPC17xx.H"    /* LPC17xx definitions                */
#include "RTE_Device.h" // Keil::Device:Startup
#include "core_cm3.h"
#include "includes.h"

/* ----------------- APPLICATION GLOBALS ------------------ */

static OS_TCB APP_TSK_TCB;
static OS_TCB LOCK_TSK_TCB;
static OS_TCB DISPLAY_TSK_TCB;
static CPU_STK_SIZE APP_TSK_STACK[APP_CFG_TASK_STK_SIZE];
static CPU_STK_SIZE LOCK_TSK_STACK[APP_CFG_TASK_STK_SIZE];
static CPU_STK_SIZE DISPLAY_TSK_STACK[APP_CFG_TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void APP_TSK(void *p_arg);


#define BUFFER_SIZE 7

struct parameters { // represents the data sent by the sensor: temperature,
                    // Pressure and CRC(cyclic redundancy check)
  int T;
  short P;
  uint8_t CRC;
};

struct parameters myparam;

uint8_t buffer[BUFFER_SIZE];

uint8_t myFlag = 0x00;

int last_state;

uint8_t T1, T2, T3, T4, P1, P2;

// Function to get a string from a number
static int tostring(uint8_t *str, int num) {
  int i, rem, len = 0, n;
  // handling the case 0
  if (num == 0) {
    str[0] = '0';
    str[1] = '\0';
    return 1;
  }
  n = num;
  while (n != 0) {
    len++;
    n /= 10;
  }
  for (i = 0; i < len; i++) {
    rem = num % 10;
    num = num / 10;
    str[len - (i + 1)] = rem + '0';
  }
  str[len] = '\0';
  return len - 1;
}

// code for the APP_TSK
void APP_TSK(void *p_arg) {
  OS_ERR os_err;
  uint8_t i = 0;

  struct dma_transfer dmaRecvTransfer;
  struct uart_parameters uart2Init;

  // Initializing some parameters
  myparam.T = 1;
  myparam.P = 1;
  myparam.CRC = 0;

  CPU_IntEn();

  // Enable DMA
  init_DMA(DMA_LITTLE_ENDIAN);

  // Configuring UART properly
  uart2Init.uart_n = UART2;
  uart2Init.clock_selection = UART_CCLK_DIV_4;
  uart2Init.DLL = 72;
  uart2Init.DLM = 0;
  uart2Init.DivAddVal = 1;
  uart2Init.MulVal = 2;
  uart2Init.fifo_enable = UART_EN_FIFO;
  uart2Init.dma_mode = UART_DMA;
  uart2Init.trigger_level = UART_TR_LEVEL_1;
  uart2Init.rdv_int = UART_NO_INT;
  uart2Init.word_size = UART_WORD_8;
  uart2Init.stop_bit_n = UART_STOP_1;
  uart2Init.parity_active = UART_PARITY;
  uart2Init.parity_type = UART_EVEN_PARITY;

  // Enable UART
  init_uart(&uart2Init);

  // Wait until a resume
  OSTaskSuspend(&APP_TSK_TCB, &os_err);

  do {
    dmaRecvTransfer.channel = DMA_CHANNEL_1;
    dmaRecvTransfer.size = BUFFER_SIZE;
    dmaRecvTransfer.dest = (uint32_t)&myparam;
    dmaRecvTransfer.source = (uint32_t)&LPC_UART2->RBR;
    dmaRecvTransfer.source_burst_size = DMA_BURST_SIZE_1;
    dmaRecvTransfer.dest_burst_size = DMA_BURST_SIZE_1;
    dmaRecvTransfer.source_increment = DMA_NO_INCREMENT;
    dmaRecvTransfer.dest_increment = DMA_INCREMENT;
    dmaRecvTransfer.source_transfer_width = DMA_TRANSFER_WIDTH_8;
    dmaRecvTransfer.dest_transfer_width = DMA_TRANSFER_WIDTH_8;
    dmaRecvTransfer.terminal_count_enable = DMA_TERMINAL_COUNT_ENABLE;
    dmaRecvTransfer.transfer_type = DMA_P2M;
    dmaRecvTransfer.source_peripheral = DMA_UART2_RX;
    dmaRecvTransfer.dest_peripheral = DMA_NO_PERIPHERAL;
    dmaRecvTransfer.lli_head = 0;

    transfer_DMA(&dmaRecvTransfer);

    OSTaskSuspend(&APP_TSK_TCB, &os_err);

  } while (1);
}

// Code for the DISPALY_TSK
void DISPLAY_TSK(void *p_arg) {
  OS_ERR os_err;

  while (1) {
    // Values shown on the screen
    char Temp[5];
    char Pressure[5];

    tostring((uint8_t *)Temp, myparam.T / 10000);

    // Show something different depending on the recevied values
    if (myparam.T / 10000 > 1000) {
      GUI_Text(30, 200, (uint8_t *)"Temperature(C): ", Black, Red);
      GUI_Text(154, 200, (uint8_t *)Temp, Black, Red);
    } else if (myparam.T / 10000 > 100) {
      GUI_Text(30, 200, (uint8_t *)"Temperature(C): ", Black, Yellow);
      GUI_Text(154, 200, (uint8_t *)Temp, Black, Yellow);
    } else {
      GUI_Text(30, 200, (uint8_t *)"Temperature(C): ", Black, Green);
      GUI_Text(154, 200, (uint8_t *)Temp, Black, Green);
    }

    tostring((uint8_t *)Pressure, myparam.P);

    if (myparam.P > 1000) {
      GUI_Text(30, 225, (uint8_t *)"Pressure(Pa): ", Black, Red);
      GUI_Text(140, 225, (uint8_t *)Pressure, Black, Red);
    } else if (myparam.P > 100) {
      GUI_Text(30, 225, (uint8_t *)"Pressure(Pa): ", Black, Yellow);
      GUI_Text(140, 225, (uint8_t *)Pressure, Black, Yellow);
    } else {
      GUI_Text(30, 225, (uint8_t *)"Pressure(Pa): ", Black, Green);
      GUI_Text(140, 225, (uint8_t *)Pressure, Black, Green);
    }
  }
}

// Code for the LOCK_TSK
void LOCK_TSK(void *p_arg) {
  OS_ERR os_err;

  uint8_t i = 0;

  LCD_Initialization();
  LCD_Clear(Black);

  while (1) {

    // Perform the CRC check
    T1 = myparam.T >> 24;

    T2 = myparam.T >> 16;
    T2 &= 0x000000FF;

    T3 = myparam.T >> 8;
    T3 &= 0x000000FF;

    T4 = myparam.T & 0x000000FF;

    P1 = myparam.P >> 8;

    P2 = myparam.P & 0x00FF;

    T1 = T1 ^ T2 ^ T3 ^ T4 ^ P1 ^ P2;

    if (T1 == myparam.CRC) {            // check if CRC is satisfied
      if (myparam.T + myparam.P > 300){ // Random condition
        myFlag = 1;
      }

      if (myFlag) {
        GUI_Text(0, 20, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 36, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 20, (uint8_t *)"The OBC will now be shut down!", Black, Red);
        GUI_Text(0, 36, (uint8_t *)"Too harsh conditions", Black, Red);
        last_state = 1;
      } 
      else {
        GUI_Text(0, 20, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 36, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 20, (uint8_t *)"The system is working", Black, Green);
        GUI_Text(0, 36, (uint8_t *)"under normal conditions", Black, Green);
        last_state = 0;
      }
    }

    OSTaskResume(&APP_TSK_TCB, &os_err);
    OSTaskSuspend(&LOCK_TSK_TCB, &os_err);
  }
}

void DMA_IRQHandler(void) {
  OS_ERR os_err;
  myFlag = 0x00;
  LCD_Clear(Black);
  if (T1 != myparam.CRC) { // If CRC is not satisfied, leave screen as is
    if (last_state){
        GUI_Text(0, 20, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 36, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 20, (uint8_t *)"The OBC will now be shut down!", Black, Red);
        GUI_Text(0, 36, (uint8_t *)"Too harsh conditions", Black, Red);
    }
    else{
        GUI_Text(0, 20, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 36, (uint8_t *)"                              ", Black, Black);
        GUI_Text(0, 20, (uint8_t *)"The system is working", Black, Green);
        GUI_Text(0, 36, (uint8_t *)"under normal conditions", Black, Green);
    }
  }
  // OSTaskSuspend(&DISPLAY_TSK_TCB, &os_err);
  OSTaskResume(&LOCK_TSK_TCB, &os_err);
  LPC_GPDMA->DMACIntTCClear = 0xFF;
}

int main(void) {
  OS_ERR err;

  SystemInit();  /*initialize the clock */
  BSP_IntInit(); /*initiialize interrupt*/
  BSP_Start();   /*initialize kernel system tick timer */
  CPU_IntDis();  /*disable interrupt*/
  CPU_Init();    /*init cpu*/
  Mem_Init();    /*Memory initialization*/
  OSInit(&err);  /* Initialize "uC/OS-III, The Real-Time Kernel"         */

  OSTaskCreate(
      (OS_TCB *)&APP_TSK_TCB, (CPU_CHAR *)"APP_TSK", (OS_TASK_PTR)APP_TSK,
      (void *)0, (OS_PRIO)APP_CFG_TASK_PRIO, (CPU_STK *)&APP_TSK_STACK[0],
      (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10),
      (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE, (OS_MSG_QTY)0, (OS_TICK)0, (void *)0,
      (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), (OS_ERR *)&err);

  OSTaskCreate(
      (OS_TCB *)&LOCK_TSK_TCB, (CPU_CHAR *)"LOCK_TSK", (OS_TASK_PTR)LOCK_TSK,
      (void *)0, (OS_PRIO)LOCK_CFG_TASK_PRIO, (CPU_STK *)&LOCK_TSK_STACK[0],
      (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10),
      (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE, (OS_MSG_QTY)0, (OS_TICK)0, (void *)0,
      (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), (OS_ERR *)&err);

  OSTaskCreate(
      (OS_TCB *)&DISPLAY_TSK_TCB, (CPU_CHAR *)"DISPLAY_TSK",
      (OS_TASK_PTR)DISPLAY_TSK, (void *)0, (OS_PRIO)DISPLAY_CFG_TASK_PRIO,
      (CPU_STK *)&DISPLAY_TSK_STACK[0], (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10),
      (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE, (OS_MSG_QTY)0, (OS_TICK)0, (void *)0,
      (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), (OS_ERR *)&err);

  OSStart(&err); /* Start multitasking (i.e. give control to uC/OS-III). */

  while (DEF_ON) {
  };
}
