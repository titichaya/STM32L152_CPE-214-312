#include "stm32l1xx.h"

unsigned int number[10] = {0xFC00,0x6000,0xDA00,0xF200,0x6600,0xB600,0xBE00,0xE000,0xFE00,0xF600};
void init_7segment(void);
void display(unsigned int num);
void delay (unsigned long int ncycle){ while(ncycle--);} 
int m=0,s=0,mode=0,time=0;

void TIM3_IRQHandler(void) {
	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update) == SET){
		s--;
		if(s<0){
			m--;
			s=59;
		} 
		if(m==0 && s == 0){
// 			mode=0;
			TIM_Cmd(TIM3,DISABLE);
			s=0;
			m=0;
		}
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	}
}

void EXTI1_IRQHandler(void){
	if(EXTI_GetFlagStatus(EXTI_Line1)==SET)
	{
// 		if(mode == 0)
			m++;
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void EXTI2_IRQHandler(void){
	if(EXTI_GetFlagStatus(EXTI_Line2)==SET)
	{
// 		if(mode == 0)
			s++;
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void EXTI3_IRQHandler(void){
	if(EXTI_GetFlagStatus(EXTI_Line3)==SET)
	{
// 		if(mode == 1){
			TIM_Cmd(TIM3,ENABLE);
// 			mode = 1;
// 		}
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

int main(){
	GPIO_InitTypeDef pa;
	TIM_TimeBaseInitTypeDef tim3;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic,nvicpa3;
	
	
	init_7segment();
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	GPIO_StructInit(&pa);
	pa.GPIO_Mode = GPIO_Mode_IN;
	pa.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Init(GPIOA,&pa);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource3);
	
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line1|EXTI_Line2|EXTI_Line3;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti);

	
	nvicpa3.NVIC_IRQChannel = EXTI1_IRQn;
	nvicpa3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicpa3);
	
	nvicpa3.NVIC_IRQChannel = EXTI2_IRQn;
	nvicpa3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicpa3);
	
	nvicpa3.NVIC_IRQChannel = EXTI3_IRQn;
	nvicpa3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicpa3);

  TIM_TimeBaseStructInit(&tim3);
  tim3.TIM_CounterMode = TIM_CounterMode_Down;
  tim3.TIM_Prescaler = 16000-1;
  tim3.TIM_Period = 1000-1;
  TIM_TimeBaseInit(TIM3,&tim3);
  
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
  
	nvic.NVIC_IRQChannel = TIM3_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	while(1)
	{
		if(s==60){
			m++;
			s=0;
		}

		time = (m*100) + s;
		display(time);
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
