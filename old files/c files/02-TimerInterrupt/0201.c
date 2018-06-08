#include "stm32l1xx.h"

void EXTI0_IRQHandler(void){
	if(EXTI_GetFlagStatus(EXTI_Line0) == SET){
		GPIO_ToggleBits(GPIOB,GPIO_Pin_6);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

int main(){
	GPIO_InitTypeDef pa0,pb6;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB , ENABLE);
	
	GPIO_StructInit(&pa0);
	pa0.GPIO_Mode = GPIO_Mode_IN;
	pa0.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&pa0);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
	GPIO_StructInit(&pb6);
	pb6.GPIO_Mode = GPIO_Mode_OUT;
	pb6.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB,&pb6);
	
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line0;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&exti);
	
	nvic.NVIC_IRQChannel = EXTI0_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	while(1)
	{
		
	}
	
}
