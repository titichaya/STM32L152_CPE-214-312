#include "stm32l1xx.h"
#include "stm32l_discovery_lcd.h"

/*---------------------------------------------------------------------------
  set HSI as SystemCoreClock (HSE is not populated on STM32L-Discovery board)
 *---------------------------------------------------------------------------*/
void SystemCoreClockSetHSI(void) {
  
  RCC->CR |= ((uint32_t)RCC_CR_HSION);                      /* Enable HSI                        */ 
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                   /* Wait for HSI Ready                */

  FLASH->ACR |= FLASH_ACR_ACC64;                            /* Enable 64-bit access              */
  FLASH->ACR |= FLASH_ACR_PRFTEN;                           /* Enable Prefetch Buffer            */
  FLASH->ACR |= FLASH_ACR_LATENCY;                          /* Flash 1 wait state                */

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;                        /* Enable the PWR APB1 Clock         */
  PWR->CR = PWR_CR_VOS_0;                                   /* Select the Voltage Range 1 (1.8V) */
  while((PWR->CSR & PWR_CSR_VOSF) != 0);                    /* Wait for Voltage Regulator Ready  */

  RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;                /* HCLK = SYSCLK                     */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;               /* PCLK2 = HCLK                      */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;               /* PCLK1 = HCLK                      */
    
  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI; /* HSI is system clock               */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);   /* Wait for HSI used as system clock */
}

void LCD_Configuration(void)
{  
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_LSICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_LCD | RCC_APB1Periph_PWR, ENABLE);
	/* Configure SysTick IRQ and SysTick Timer to generate interrupts */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 500);

  PWR_RTCAccessCmd(ENABLE);					//Allow access to the RTC
  RCC_RTCResetCmd(ENABLE);					//Reset Backup Domain
  RCC_RTCResetCmd(DISABLE);

  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET); //Wait till LSI is ready  
  RCC_RTCCLKCmd(ENABLE);
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);	//LCD Clock Source Selection

  LCD_GLASS_Init();
  LCD_GLASS_Configure_GPIO();
}


/*----------------------------------------------------------------------------
  Additional configuration: Initialize and configure additional peripherals
 *---------------------------------------------------------------------------*/
void PB6_7_OUTPUT(void){
	GPIO_InitTypeDef pb6_7;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	GPIO_StructInit (&pb6_7);
		pb6_7.GPIO_Mode = GPIO_Mode_OUT;
		pb6_7.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;
	GPIO_Init (GPIOB, &pb6_7);
}

void PA12_OUTPUT(void){
	GPIO_InitTypeDef pa12;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_StructInit (&pa12);
		pa12.GPIO_Mode = GPIO_Mode_OUT;
		pa12.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init (GPIOA, &pa12);
}

void PC12_OUTPUT(void){
	GPIO_InitTypeDef pc12;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	
	GPIO_StructInit (&pc12);
		pc12.GPIO_Mode = GPIO_Mode_OUT;
		pc12.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init (GPIOC, &pc12);
}

void PD2_OUTPUT(void){
	GPIO_InitTypeDef pd2;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD,ENABLE);
	
	GPIO_StructInit (&pd2);
		pd2.GPIO_Mode = GPIO_Mode_OUT;
		pd2.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init (GPIOD, &pd2);
}

void PA11_INPUT(void){
	GPIO_InitTypeDef pa11;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_StructInit (&pa11);
		pa11.GPIO_Mode = GPIO_Mode_IN;
		pa11.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init (GPIOA, &pa11);
}

void PA0_4_5_INPUT(void){
	GPIO_InitTypeDef pa0_4_5;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_StructInit (&pa0_4_5);
		pa0_4_5.GPIO_Mode = GPIO_Mode_IN;
		pa0_4_5.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init (GPIOA, &pa0_4_5);
}

void PA11_INPUT_INT(void){
	GPIO_InitTypeDef  ex_pa;
	EXTI_InitTypeDef  exin_11;
	NVIC_InitTypeDef  nvic_11;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	GPIO_StructInit(&ex_pa);
	ex_pa.GPIO_Mode = GPIO_Mode_IN;
	ex_pa.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA,&ex_pa);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11);
	
	EXTI_StructInit(&exin_11);
	exin_11.EXTI_Line = EXTI_Line11;
	exin_11.EXTI_LineCmd = ENABLE;
	exin_11.EXTI_Mode = EXTI_Mode_Interrupt;
	exin_11.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exin_11);
	
	nvic_11.NVIC_IRQChannel = EXTI15_10_IRQn;
	nvic_11.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(& nvic_11);
}
