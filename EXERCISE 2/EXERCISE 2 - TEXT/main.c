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
#include "password.h"
#include <stdbool.h>

/* ----------------- APPLICATION GLOBALS ------------------ */

/** YOU CAN ONLY MODIFY THESE 3 LINES **/

#define SOURCEADDRESS 0x00000000
#define TRANFSIZE 0
char password[5] = "    ";

/***************************************/

static OS_TCB APP_TSK_TCB;

static CPU_STK_SIZE APP_TSK_STACK[APP_CFG_TASK_STK_SIZE];

/* Global Variables*/
int down_KEY1;
int down_KEY2;
int down_KEY3;

uint8_t val1[16];
uint8_t DMAbuffer[100];

uint32_t data = 0x0;

struct dma_transfer dmaRecvTransfer = {
    .channel = DMA_CHANNEL_1,
    .size = TRANFSIZE,
    .dest = (uint32_t)&DMAbuffer,
    .source = (uint32_t)SOURCEADDRESS,
    .source_burst_size = DMA_BURST_SIZE_1,
    .dest_burst_size = DMA_BURST_SIZE_1,
    .source_increment = DMA_INCREMENT,
    .dest_increment = DMA_INCREMENT,
    .source_transfer_width = DMA_TRANSFER_WIDTH_8,
    .dest_transfer_width = DMA_TRANSFER_WIDTH_8,
    .terminal_count_enable = DMA_TERMINAL_COUNT_ENABLE,
    .transfer_type = DMA_M2M,
    .source_peripheral = NULL,
    .dest_peripheral = NULL,
    .lli_head = 0,
};

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/* Generic function */
static void tostring2hex(uint8_t *str, int num);
static void showbuffer();
static void App_TaskCreate(void);

/* Task functions */
static void APP_TSK(void *p_arg);

void DMA_IRQHandler(void) { NVIC_DisableIRQ(DMA_IRQn); }

int main(void) {

  OS_ERR err;

  SystemInit();  /*initialize the clock */
  BSP_IntInit(); /*initiialize interrupt*/
  BSP_Start();   /*initialize kernel system tick timer */
  CPU_IntDis();  /*disable interrupt*/
  CPU_Init();    /*init cpu*/
  Mem_Init();    /*Memory initialization*/

  OSInit(&err); /* Initialize "uC/OS-III, The Real-Time Kernel"         */

  CPU_IntEn();

  OSTaskCreate((OS_TCB *)&APP_TSK_TCB, /* Create the start task */
               (CPU_CHAR *)"APP_TASK", (OS_TASK_PTR)APP_TSK, (void *)0,
               (OS_PRIO)APP_CFG_TASK_PRIO, (CPU_STK *)&APP_TSK_STACK[0],
               (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10u),
               (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE, (OS_MSG_QTY)0, (OS_TICK)0,
               (void *)0, (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR *)&err);

  OSStart(&err); /* Start multitasking (i.e. give control to uC/OS-III). */

  while (DEF_ON) { /* Should Never Get Here	*/
  };
}

static void APP_TSK(void *p_arg) {
  (void)p_arg; /* See Note #1                                          */
  OS_ERR os_err;

  BSP_OS_TickEnable(); /* Enable the tick timer and interrupt */
  LED_init();
  LCD_Initialization();

  joystick_init();
  BUTTON_init();

  LCD_Clear(Black);

  init_DMA(DMA_LITTLE_ENDIAN);

  while (DEF_TRUE) {
    // Init the DMA trasnfer
    if (down_KEY1 == 1) {
      down_KEY1 = 0;
    }

    // Show the buffer on the screen
    if (down_KEY2 == 1){
      showbuffer();
      down_KEY2 = 0;
    }

    // Check the value of the password
    if (down_KEY3 == 1) {
      LCD_Clear(Black);
      if (passwordcheck(password)) {
        GUI_Text(120 - 28, 160 - 8, (uint8_t *)"CORRECT", Black, Green);
      } else {
        GUI_Text(120 - 28, 160 - 8, (uint8_t *)"WRONG", Black, Red);
      }
      down_KEY3 = 0;
    }
  }
}

static void tostring2hex(uint8_t *str, int num) {
  int i;
  for (i = 1; i >= 0; i--) {
    int digit = (num >> (4 * i)) & 0xF; // Extract the i-th hex digit from the number
    str[1 - i] = (digit < 10)
            ? '0' + digit
            : 'A' + (digit - 10); // Convert digit to ASCII character and store in str
  }
  str[2] = '\0'; // Add null terminator to end of string
}

static void showbuffer() {
  LCD_Clear(Black);
  int line = 0;
  for (int i = 0; i < TRANFSIZE; i = i + 2) {
    tostring2hex(val1, DMAbuffer[i]);
    GUI_Text(120 - 16, line, (uint8_t *)val1, White, Black);
    tostring2hex(val1, DMAbuffer[i + 1]);
    GUI_Text(120, line, (uint8_t *)val1, White, Black);
    line = line + 16;
  }
}
