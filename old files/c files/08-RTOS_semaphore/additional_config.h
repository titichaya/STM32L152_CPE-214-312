/* Includes ------------------------------------------------------------------*/
#ifndef ADDITIONAL_CONFIG
#define ADDITIONAL_CONFIG

#include "stm32l1xx.h"   
#include "discover_board.h"

void SystemCoreClockSetHSI(void);		/* set HSI as SystemCoreClock */
void LCD_Configuration(void);		/* configure LCD */
void PB6_7_OUTPUT(void);				/* configure PB6 and PB7 as OUTPUT */
void PA12_OUTPUT(void);					/* configure PA12 as OUTPUT */
void PC12_OUTPUT(void);					/* configure PC12 as OUTPUT */
void PD2_OUTPUT(void);					/* configure PD2 as OUTPUT */
void PA11_INPUT(void);					/* configure PA11 as INPUT */
void PA0_4_5_INPUT(void);				/* configure PA0, PA4 and PA5 as INPUT */
void PA11_INPUT_INT(void);			/* configure PA11 for external interrupt */
void PA0_INPUT_INT(void);				/* configure PA0 for external interrupt */

#endif
