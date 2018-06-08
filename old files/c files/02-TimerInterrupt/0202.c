#include "stm32l1xx.h"

unsigned int number[10] = {0xFC00,0x6000,0xDA00,0xF200,0x6600,0xB600,0xBE00,0xE000,0xFE00,0xF600};
void init_7segment(void);
void display(unsigned int num);
void delay (unsigned long int ncycle){ while(ncycle--);} 
unsigned int counter=0;

void EXTI0_IRQHandler(void){
	if(EXTI_GetFlagStatus(EXTI_Line0) == SET){
		counter++;
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

int main(){
	GPIO_InitTypeDef pa0;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);
	
	init_7segment();
	
	GPIO_StructInit(&pa0);
	pa0.GPIO_Mode = GPIO_Mode_IN;
	pa0.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&pa0);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
	
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line0;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti);
	
	nvic.NVIC_IRQChannel = EXTI0_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	while(1)
	{
		if(counter==40)
			counter=0;
		display(counter);
	}
	
}

void init_7segment(void)
{
	GPIO_InitTypeDef pb,pc;
	RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC , ENABLE);
	GPIO_StructInit(&pb);
	pb.GPIO_Mode = GPIO_Mode_OUT;
	pb.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_10|GPIO_Pin_9;
	GPIO_Init(GPIOB,&pb);
	
	GPIO_StructInit(&pc);
	pc.GPIO_Mode = GPIO_Mode_OUT;
	pc.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Init(GPIOC,&pc);
}

void display(unsigned int num){
	GPIO_SetBits(GPIOB,number[num/1000]);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,number[num/1000]);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	
	GPIO_SetBits(GPIOB,number[(num%1000)/100]);
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,number[(num%1000)/100]);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
	
	GPIO_SetBits(GPIOB,number[(num%100)/10]);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
	GPIO_ResetBits(GPIOB,number[(num%100)/10]);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	
	GPIO_SetBits(GPIOB,number[num%10]);
	GPIO_SetBits(GPIOC,GPIO_Pin_3);
	GPIO_ResetBits(GPIOB,number[num%10]);
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);
}
