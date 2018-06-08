#include "stm32l1xx.h"

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

void PA0_INPUT_INT(void){
	GPIO_InitTypeDef  ex_pa;
	EXTI_InitTypeDef  exin_0;
	NVIC_InitTypeDef  nvic_0;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	GPIO_StructInit(&ex_pa);
	ex_pa.GPIO_Mode = GPIO_Mode_IN;
	ex_pa.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC,&ex_pa);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	EXTI_StructInit(&exin_0);
	exin_0.EXTI_Line = EXTI_Line0;
	exin_0.EXTI_LineCmd = ENABLE;
	exin_0.EXTI_Mode = EXTI_Mode_Interrupt;
	exin_0.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exin_0);
	
	nvic_0.NVIC_IRQChannel = EXTI0_IRQn;
	nvic_0.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(& nvic_0);
}
