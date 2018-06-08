#include "stm32l1xx.h"

unsigned int number[10] = {0xFC00,0x6000,0xDA00,0xF200,0x6600,0xB600,0xBE00,0xE000,0xFE00,0xF600};
void init_7segment(void);
void display(unsigned int num);
void delay (unsigned long int ncycle){ while(ncycle--);} 
unsigned int counter=0;

void TIM3_IRQHandler(void) {
//	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update) == SET){
//		counter++;
//		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
//	}
		if(TIM3->SR & TIM_SR_UIF){
		TIM3->SR &= ~TIM_SR_UIF;

		counter++;
	}

}

int main(){
	TIM_TimeBaseInitTypeDef tim3;
	NVIC_InitTypeDef nvic;
	init_7segment();
	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC->APB1ENR |= (1<<1);  // RCC enble for timer 3
  
//  TIM_TimeBaseStructInit(&tim3);
//  tim3.TIM_CounterMode = TIM_CounterMode_Down;
//  tim3.TIM_Prescaler = 16000-1;
//  tim3.TIM_Period = 2000-1;
//  TIM_TimeBaseInit(TIM3,&tim3);
	TIM3->PSC = 16000-1;
	TIM3->ARR = 2000-1;
  
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
//	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM3->DIER |= (1<<0);		// timer3 Update interrupt enable
	
//  TIM_Cmd(TIM3,ENABLE);
	TIM3->CR1 |= (1<<0);		// timer3 enable
	
//	nvic.NVIC_IRQChannel = TIM3_IRQn;
//	nvic.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&nvic);
	NVIC_EnableIRQ(TIM3_IRQn);			// Enable NVIC for Pin 0 
	
	while(1)
	{
		if(counter==31)
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
