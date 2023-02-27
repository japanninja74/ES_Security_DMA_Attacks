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
#include "LPC17xx.H"    /* LPC17xx definitions */
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

/**********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

static void APP_TSK(void *p_arg);

/**********************************************************************************************************
 *                                                main()
 *
 * Description : This is the standard entry point for C code.  It is assumed
 *that your code will call main() once you have performed all necessary
 *initialization.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : This the main standard entry point.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

#define BUFFER_SIZE 7

struct parameters { // represents the data sent by the sensor: temperature,
                    // Pressure and CRC(cyclic redundancy check)
  int T;
  short P;
  uint8_t CRC;
};
struct parameters myparam;

uint8_t buffer[BUFFER_SIZE];

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

  init_DMA(DMA_LITTLE_ENDIAN);

  // *** EXERCISE PART 1 ***

  //    uart2Init.uart_n = ;
  //    uart2Init.clock_selection = ;
  //    uart2Init.DLL = ;
  //    uart2Init.DLM = ;
  //    uart2Init.DivAddVal = ;
  //    uart2Init.MulVal = ;
  //    uart2Init.fifo_enable =	;
  //    uart2Init.dma_mode = ;
  //    uart2Init.trigger_level = ;
  //    uart2Init.rdv_int = ;
  //    uart2Init.word_size = ;
  //    uart2Init.stop_bit_n = ;
  //    uart2Init.parity_active = ;
  //    uart2Init.parity_type = ;
  
  // *** END OF PART 1 ***

  // Enable UART
  init_uart(&uart2Init);

  // Wait unti resume
  OSTaskSuspend(&APP_TSK_TCB, &os_err);

  do {

    // *** EXERCISE PART 2 ***

    //    	dmaRecvTransfer.channel = ;
    //    	dmaRecvTransfer.size = ;
    //    	dmaRecvTransfer.dest = ;
    //    	dmaRecvTransfer.source = ;
    //    	dmaRecvTransfer.source_burst_size = ;
    //    	dmaRecvTransfer.dest_burst_size = ;
    //    	dmaRecvTransfer.source_increment = ;
    //    	dmaRecvTransfer.dest_increment = ;
    //    	dmaRecvTransfer.source_transfer_width = ;
    //    	dmaRecvTransfer.dest_transfer_width = ;
    //    	dmaRecvTransfer.terminal_count_enable = ;
    //    	dmaRecvTransfer.transfer_type = ;
    //    	dmaRecvTransfer.source_peripheral = ;
    //    	dmaRecvTransfer.dest_peripheral = ;
    //    	dmaRecvTransfer.lli_head = ;

    // *** END OF PART 2 ***

    transfer_DMA(&dmaRecvTransfer);

    OSTaskSuspend(&APP_TSK_TCB, &os_err);

  } while (1);
}

// Code for the DISPALY_TSK
void DISPLAY_TSK(void *p_arg) {
  OS_ERR os_err;

  while (1) {
    char Temp[5];
    char Pressure[5];

    tostring((uint8_t *)Temp, myparam.T / 10000);

    // *** EXERCISE PART 3 ***

    // Show the temperature on the screen

    // *** END OF PART 3 ***

    tostring((uint8_t *)Pressure, myparam.P);

    // *** EXERCISE PART 4 ***

    // Show the pressure on the screen

    // *** END OF PART 4 ***
  }
}

void LOCK_TSK(void *p_arg) {
  OS_ERR os_err;

  LCD_Initialization();
  LCD_Clear(Black);

  while (1) {

    // *** EXERCISE PART 5 ***

    // Compute the CRC of the input temperature and 
    // pressure, and compare it to the received value.
    // Show a message such as "The OBC will now be shut down!" if the 
    // CRC is validated and a certain condition of the values
    // of tempearature and pressure is respected (for 
    // instance, temp+pressure > 300). Otherwise, display a message
    // showing that everythign is safe

    // *** END OF PART 5 ***

    OSTaskResume(&APP_TSK_TCB, &os_err);
    OSTaskSuspend(&LOCK_TSK_TCB, &os_err);
  }

}

void DMA_IRQHandler(void) {

  OS_ERR os_err;
  LCD_Clear(Black);

    // *** EXERCISE PART 6 ***

    // Deal with the semaphores in here in a way
    // that all the tasks can behave correctly

    // *** END OF PART 6 ***

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
  OSInit(&err);  /* Initialize "uC/OS-III, The Real-Time Kernel" */

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
