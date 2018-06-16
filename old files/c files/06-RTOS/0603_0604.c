/*---------------------------------------------------------------------------
 * lab 2-3     RL-ARM - RTX  --  example code for "RTOS" lab, CPE-312
 * 			sw1 : LED PD2 ON
 *			sw2 : LED PC12 ON
 *			sw3 : both PD2 and PC12 ON
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * lab 2-4     RL-ARM - RTX  --  example code for "RTOS" lab, CPE-312
 * 			sw1 : LED PD2 ON
 *			sw2 : LED PC12 ON
 *			sw3 : both PD2 and PC12 ON
 *			PA11 (interrupt) : switch ON/OFF (LED PA12 indicate ON/OFF status
 *													both PD2 and PC12 OFF
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include "stm32l1xx.h"
#include "stdio.h"
#include "additional_config.h"

#define r 0
#define p 1
#define off 0
#define on 1
#define blink 2

/* id1, id2 will contain task identifications at run-time. */
OS_TID id0, id1, id2, id3, id4, id5, id6, id7, id8;
int sw1_3=0, state=r, power=on;

/* Forward declaration of tasks. */
__task void task0 (void);
__task void task1 (void);
__task void task2 (void);
__task void task3 (void);
__task void task4 (void);
__task void task5 (void);
__task void task6 (void);
__task void task7 (void);
__task void task8 (void);

/*---------------------------------------------------------------------------
  Tasks : Implement tasks
 *---------------------------------------------------------------------------*/
__task void task0 (void){
	/* Obtain own system task identification number and its priority. */
  id0 = os_tsk_self();
	os_tsk_prio_self(5);

	/* Create task2,3 and obtain its task identification number. */
  id1 = os_tsk_create (task1, 2);
//  id2 = os_tsk_create (task2, 2);
  id3 = os_tsk_create (task3, 2);
//  id4 = os_tsk_create (task4, 2);
  id5 = os_tsk_create (task5, 2);
//  id6 = os_tsk_create (task6, 2);
	id7 = os_tsk_create (task7, 3);
	id8 = os_tsk_create (task8, 1);

	os_tsk_delete_self();
}
	
__task void task1 (void){
  for (;;) {
    os_evt_wait_or(0x0001, 0xFFFF);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
  }
}

__task void task3 (void){
  for (;;) {
    os_evt_wait_or(0x0002, 0xFFFF);
	GPIO_SetBits(GPIOC,GPIO_Pin_12);
  }
}

__task void task5 (void){
  for (;;) {
    os_evt_wait_or(0x0004, 0xFFFF);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	GPIO_SetBits(GPIOC,GPIO_Pin_12);
  }
}

__task void task7 (void) {
  for (;;) {
		os_evt_wait_or(0x0008, 0xFFFF);

		GPIO_ToggleBits(GPIOA,GPIO_Pin_12);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
  }
}

__task void task8 (void) {
	GPIO_SetBits(GPIOA, GPIO_Pin_12);

	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);

  for (;;) {
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_12)==SET)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==RESET)
		{
			os_evt_set(0x0001, id1);
		}
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==RESET)
		{
			os_evt_set(0x0002, id3);
		}
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==RESET)
		{
			os_evt_set(0x0004, id5);
		}
	}
  }
}

/*----------------------------------------------------------------------------
  Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	
	PB6_7_OUTPUT();							/* configure PB6 and PB7 */
//	PA0_INPUT_INT();						/* configure PA0 for external interrupt */
	PA12_OUTPUT();							/* configure PA12 as OUTPUT */
	PC12_OUTPUT();							/* configure PC12 as OUTPUT */
	PD2_OUTPUT();								/* configure PD2 as OUTPUT */
	PA0_4_5_INPUT();						/* configure PA0, PA4 and PA5 as INPUT */
	PA11_INPUT_INT();						/* configure PA11 for external interrupt */
//	PA11_INPUT();								/* configure PA11 as INPUT */

	/* Start the RTX kernel, and then create and execute task1. */
  os_sys_init(task0);

}

void EXTI15_10_IRQHandler (void) {
	if(EXTI_GetITStatus(EXTI_Line11) == SET)
  {
    isr_evt_set(0x0008, id7);
    EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

//void EXTI0_IRQHandler (void) {
//	if(EXTI_GetITStatus(EXTI_Line0) == SET)
//  {
////    GPIO_ToggleBits(GPIOB,GPIO_Pin_7);
//    isr_evt_set(0x0008, id7);
//    EXTI_ClearITPendingBit(EXTI_Line0);
//	}
//}
