/*---------------------------------------------------------------------------
 * lab 2-1     RL-ARM - RTX  --  example code for "RTOS" lab, CPE-312
 *      lab#2 example : PB7 -> PB6 -> both OFF
 *            option 1: scan PA11. if PA11 is pressed, both ON
*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * lab 2-2     RL-ARM - RTX  --  example code for "RTOS" lab, CPE-312
 *      lab#2 example : PB7 -> PB6 -> both OFF
 *            option 2: PA11 interrupt, then both ON
*---------------------------------------------------------------------------*/

#include <RTL.h>
#include "stm32l1xx.h"
#include "stdio.h"
#include "additional_config.h"

#define r 1
#define p 0

/* id1, id2 will contain task identifications at run-time. */
OS_TID id0, id1, id2, id3, id4, id5;
int sw=0, state=r;

/* Forward declaration of tasks. */
__task void task0 (void);
__task void task1 (void);
__task void task2 (void);
__task void task3 (void);
__task void task4 (void);
__task void task5 (void);

/*---------------------------------------------------------------------------
  Tasks : Implement tasks
 *---------------------------------------------------------------------------*/
__task void task0 (void){
	/* Obtain own system task identification number and its priority. */
  id0 = os_tsk_self();
	os_tsk_prio_self(5);

	/* Create task2,3 and obtain its task identification number. */
  id1 = os_tsk_create (task1, 2);
  id2 = os_tsk_create (task2, 2);
  id3 = os_tsk_create (task3, 2);
	id4 = os_tsk_create (task4, 2);
//	id5 = os_tsk_create (task5, 1);
	os_tsk_delete_self();
}
	
__task void task1 (void){
  for (;;) {
		/* task1 activity :-- turn on "PB7" */
		GPIO_SetBits(GPIOB,GPIO_Pin_7);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		
		/* Signal to task2 that task1 has completed. */
    os_evt_set(0x0002, id2);

    /* Wait for completion of task2 activity. */
    /*  0xFFFF makes it wait without timeout. */
    /*  0x0004 represents bit 2. */
    os_evt_wait_or(0x0001, 0xFFFF);

    /* Wait for 50+30 clock ticks before restarting task1 activity. */
    os_dly_wait(20);
  }
}

__task void task2 (void) {

  for (;;) {
		/* Wait for completion of task1 activity. */
		/*  0xFFFF makes it wait without timeout. */
		/*  0x0004 represents bit 2. */
    os_evt_wait_or(0x0002, 0xFFFF);

		/* Wait for 30+20 clock ticks before starting task2 activity. */
    os_dly_wait(50);

		/* task2 activity :-- turn off "PB7" */
		GPIO_ResetBits(GPIOB,GPIO_Pin_7);
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		
		/* Signal to task1 that task2 has completed. */
    os_evt_set(0x0004, id3);
  }
}

__task void task3 (void) {

  for (;;) {
		/* Wait for completion of task1 activity. */
		/*  0xFFFF makes it wait without timeout. */
		/*  0x0004 represents bit 2. */
    os_evt_wait_or(0x0004, 0xFFFF);

		/* Wait for 20+50 clock ticks before starting task2 activity. */
    os_dly_wait(30);

		/* task2 activity :-- turn off "PB7" */
		GPIO_ResetBits(GPIOB,GPIO_Pin_7);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		
		/* Signal to task1 that task2 has completed. */
    os_evt_set(0x0001, id1);
  }
}

__task void task4 (void) {
  for (;;) {
		os_evt_wait_or(0x0008, 0xFFFF);
		GPIO_SetBits(GPIOB,GPIO_Pin_7);
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
    os_tsk_pass();
  }
}

//__task void task5 (void) {
//  for (;;) {
//		sw = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);
//		if( (sw != 1) && (state == r)) {
//			state = p;
//    	GPIO_ToggleBits(GPIOC,GPIO_Pin_12);
//			GPIO_SetBits(GPIOB,GPIO_Pin_7);
//			GPIO_SetBits(GPIOB,GPIO_Pin_6);
//		}
//		else if ( (sw == 1) && (state == p)) {
//			state = r;
//		}
//  }
//}

/*----------------------------------------------------------------------------
  Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	
	PB6_7_OUTPUT();							/* configure PB6 and PB7 as OUTPUT */
	PC12_OUTPUT();							/* configure PC12 as OUTPUT */
	PD2_OUTPUT();								/* configure PD2 as OUTPUT */
	PA0_INPUT_INT();						/* configure PA0 for external interrupt */
	PA11_INPUT_INT();						/* configure PA11 for external interrupt */
//	PA11_INPUT();								/* configure PA11 as INPUT */
	
	/* Start the RTX kernel, and then create and execute task1. */
  os_sys_init(task0);

}

void EXTI15_10_IRQHandler (void) {
	if(EXTI_GetITStatus(EXTI_Line11) == SET)
  {
    GPIO_ToggleBits(GPIOC,GPIO_Pin_12);
		isr_evt_set(0x0008, id4);
    EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

void EXTI0_IRQHandler (void) {
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
  {
    GPIO_ToggleBits(GPIOD,GPIO_Pin_2);
    isr_evt_set(0x0008, id4);
    EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
