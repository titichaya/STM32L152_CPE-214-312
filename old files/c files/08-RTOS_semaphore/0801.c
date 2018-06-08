/*---------------------------------------------------------------------------
 *      RL-ARM - RTX  --  example code for "RTOS" lab, CPE-312
 *      lab#4 : counter service (use time delay)
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include "stm32l1xx.h"
#include "stdio.h"
#include "discover_board.h"
#include "stm32l_discovery_lcd.h"
#include "additional_config.h"

int data = 3;
char strDisp[6] ;
unsigned int CounterNo = 1;
unsigned int QList[3] = {0,0,0};
int Qrun = 1, Qrun0 = 0, Qcall = 0, Qcall0 = 0;
int writePos=0, readPos=0;

/* id1, id2 will contain task identifications at run-time. */
OS_TID Qlist, C1, C2, C3, showLCD;
OS_MUT LCDmutex;											// declare Mutex
OS_SEM emptySemaphore,fillSemaphore;	// declare semaphore

/* Forward declaration of tasks. */
__task void RunningQ (void);
__task void Counter1 (void);
__task void Counter2 (void);
__task void Counter3 (void);
//__task void DisplayLCD (void);

/*---------------------------------------------------------------------------
  Tasks : Implement tasks
 *---------------------------------------------------------------------------*/
__task void initTask (void){
	/* Obtain own system task priority. */
	os_tsk_prio_self(2);

	/* Create tasks and obtain their task identification. */
	Qlist = os_tsk_create (RunningQ, 1);
	C1 = os_tsk_create (Counter1, 1);
	C2 = os_tsk_create (Counter2, 1);
	C3 = os_tsk_create (Counter3, 1);
//	showLCD = os_tsk_create (DisplayLCD, 1);

	os_mut_init(&LCDmutex);						// initialize Mutex
	os_sem_init(& emptySemaphore,3);	// initialize semaphore with 2 tokens
	os_sem_init(& fillSemaphore,0);		// initialize semaphore with 0 token
	os_tsk_delete_self();
}
	
void writeQ (void){
	os_sem_wait(& emptySemaphore, 0xffff);	// decrement 'emptySemaphore'
	/* Write data to buffer. */
	if(writePos==3)  writePos=0;
	Qrun0 = Qrun;
	QList[writePos] = Qrun ;
	writePos++;
	Qrun ++;
	os_sem_send(& fillSemaphore);						// increment 'fillSemaphore'
}

void readQ (void){
	os_sem_wait(& fillSemaphore, 0xffff);		// decrement 'fillSemaphore'
	/* Read data from buffer. */
	if(readPos==3) readPos=0;
	Qcall0 = Qcall;
	Qcall = QList[readPos];
	readPos++;   
	os_sem_send(& emptySemaphore);					// increment 'emptySemaphore'
}

void DisplayQ (void){
	os_mut_wait(&LCDmutex,0xFFFF);	// wait for Mutex (lock resource)
	LCD_GLASS_Clear();
	sprintf(strDisp, " Q %d", Qrun0);
	LCD_GLASS_DisplayString((unsigned char *) strDisp);
	os_dly_wait(100);
	os_mut_release(&LCDmutex);		// release Mutex (unlock resource)
}

void DisplayC (int C_no){
	os_mut_wait(&LCDmutex,0xFFFF);	// wait for Mutex (lock resource)
	LCD_GLASS_Clear();
	sprintf(strDisp, "C%d-%d", C_no,Qcall);
	LCD_GLASS_DisplayString((unsigned char *) strDisp);
	os_dly_wait(100);
	os_mut_release(&LCDmutex);		// release Mutex (unlock resource)
}

__task void RunningQ(){
		 DisplayQ();
   for (;;) {
//		 sprintf(strDisp, " Q %d", Qrun);
		 writeQ ();
		 DisplayQ();
		 os_dly_wait(100);
	 }
}

__task void Counter1(){
   for (;;) {
		 readQ ();
//		 sprintf(strDisp, "C%d-%d", CounterNo,Qcall);
//		 CounterNo=1;
		 DisplayC(1);
		 os_dly_wait(300);
	 }
}

__task void Counter2(){
   for (;;) {
		 readQ ();
//		 sprintf(strDisp, "C%d-%d", CounterNo,Qcall);
//		 CounterNo=2;
		 DisplayC(2);
		 os_dly_wait(600);
	 }
}

__task void Counter3(){
   for (;;) {
		 readQ ();
//		 sprintf(strDisp, "C%d-%d", CounterNo,Qcall);
//		 CounterNo=3;
		 DisplayC(3);
		 os_dly_wait(200);
	 }
}

//__task void DisplayLCD(){
//   for (;;) {
//		 if (Qcall != Qcall0){
//			sprintf(strDisp, " Q %d", Qrun);
//			Display(strDisp);
//		 }
//		 if (Qrun != Qrun0){
//			 sprintf(strDisp, "C%d-%d", CounterNo,Qcall);
//			 Display(strDisp);
//		 }
//	 }
//}

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
//	PA11_INPUT();								/* configure PA11 as INPUT */
	PA11_INPUT_INT();						/* configure PA11 for external interrupt */

	sprintf(strDisp, " Q 0");
	LCD_GLASS_DisplayString((unsigned char *) strDisp);
	/* Start the RTX kernel, and then create and execute "initTask". */
	os_sys_init(initTask);
}
