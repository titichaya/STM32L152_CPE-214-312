/*---------------------------------------------------------------------------
 *      RL-ARM - RTX  --  example code for "RTOS" lab, CPE-312
 *      lab#3 example : mutex usage (share buffer "bank_ac")
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include "stm32l1xx.h"
#include "stdio.h"
#include "discover_board.h"
#include "stm32l_discovery_lcd.h"
#include "additional_config.h"

int data = 3;
char strDisp[6] ;
unsigned int bank_ac = 0;

/* id1, id2 will contain task identifications at run-time. */
OS_TID plus5, plus2, plus1, showBal, showText;
OS_MUT mut1, mut2;					// declare Mutex

/* Forward declaration of tasks. */
__task void deposit5 (void);
__task void deposit2 (void);
__task void deposit1 (void);
__task void displayBalance (void);
__task void displayText (void);

/*---------------------------------------------------------------------------
  Tasks : Implement tasks
 *---------------------------------------------------------------------------*/
__task void initTask (void){
	/* Obtain own system task priority. */
	os_tsk_prio_self(5);

	/* Create tasks and obtain their task identification. */
	plus5 = os_tsk_create (deposit5, 2);
	plus2 = os_tsk_create (deposit2, 2);
	plus1 = os_tsk_create (deposit1, 2);
	showBal = os_tsk_create (displayBalance, 2);
	showText = os_tsk_create (displayText, 2);

	os_mut_init(&mut1);		// initialize Mutex
	os_mut_init(&mut2);		// initialize Mutex
	os_tsk_delete_self();
}
	
void Deposit (	unsigned int money,
					unsigned int process_time){
	os_mut_wait(&mut1,0xFFFF);	// wait for Mutex (lock resource)
	/* Read a/c balance, add money and delay. */
	os_dly_wait(process_time);
	bank_ac += money;
	
	os_mut_release(&mut1);		// release Mutex (unlock resource)
}

void Display (char *strDisp){
		os_mut_wait(&mut2,0xFFFF);	// wait for Mutex (lock resource)
		LCD_GLASS_DisplayString((unsigned char *) strDisp);
		os_mut_release(&mut2);		// release Mutex (unlock resource)
}

__task void deposit5(){
   for (;;) {
		 Deposit (5, 500);
		 os_evt_set(0x4,showBal);
	 }
}

__task void deposit2(){
   for (;;) {
		 Deposit (2, 200);
		 os_evt_set(0x4,showBal);
	 }
}

__task void deposit1(){
   for (;;) {
		 Deposit (1, 100);
		 os_evt_set(0x4,showBal);
	 }
}

__task void displayBalance(){
   for (;;) {
    os_evt_wait_or(0x4, 0xFFFF);
		sprintf(strDisp, "BAL %d", bank_ac);
		Display(strDisp);
	 }
}

__task void displayText(){
   for (;;) {
    os_dly_wait(100);
//		sprintf(strDisp, "---");
//		Display(strDisp);
	 }
}

/*----------------------------------------------------------------------------
  Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	SystemCoreClockSetHSI();		/* set HSI as SystemCoreClock */
	LCD_Configuration();				/* configure LCD */
	PB6_7_OUTPUT();							/* configure PB6 and PB7 */
	PA12_OUTPUT();							/* configure PA12 as OUTPUT */
	PC12_OUTPUT();							/* configure PC12 as OUTPUT */
	PD2_OUTPUT();								/* configure PD2 as OUTPUT */
	PA0_4_5_INPUT();						/* configure PA0, PA4 and PA5 as INPUT */
	PA11_INPUT();								/* configure PA11 as INPUT */
//	PA11_INPUT_INT();						/* configure PA11 for external interrupt */

	sprintf(strDisp, "BAL %d", bank_ac);
	LCD_GLASS_DisplayString((unsigned char *) strDisp);
	/* Start the RTX kernel, and then create and execute "initTask". */
	os_sys_init(initTask);
}
