#include "stm32l1xx.h"

#define C 30534 
#define CS 28881 
#define D 27211 
#define DS 25723
#define E 24242 
#define F 22923 
#define FS 21622
#define G 20408 
#define GS 19277
#define A 18182 
#define AS 17167
#define B 16194
#define CC 15296 
#define P 1
#define S 0
#define DC 54422
#define EC 48484
#define FC 45846
#define GC 40816

int sheetMusic[ ] = {G,P,G,P,A,P,P,P,G,P,P,P,CC,P,B,P,P,P,G,P,G,P,A,P,
                    P,P,G,P,DC,P,CC,P,P,P,G,P,G,P,GC,P,P,P,EC,P,P,P,CC,P, 
											P,P,B,P,P,P,A,P,P,P,FC,P,FC,P,EC,P,P,P,CC,P,EC,P,DC,P,
                      P,P,G,P,G,P,A,P,P,P,G,P,P,P,CC,P,B,P,P,P,G,P,G,P,A,P,P,P,
											G,P,DC,P,CC,P,P,P,G,P,G,P,GC,P,P,P,EC,P,P,P,CC,P,P,P,B,P,P,P, 
											A,P,P,P,FC,P,FC,P,EC,P,P,P,CC,P,DC,P,CC,S};

unsigned int songPosition=0;

int main(){
	GPIO_InitTypeDef pc9;
	TIM_OCInitTypeDef tim_pc9;
  TIM_TimeBaseInitTypeDef tim9;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
  
  GPIO_StructInit(&pc9);
  pc9.GPIO_Mode = GPIO_Mode_AF;
  pc9.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOC,&pc9);
  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);
  
  TIM_OCStructInit(&tim_pc9);
  tim_pc9.TIM_OCMode = TIM_OCMode_PWM1;
  tim_pc9.TIM_OCPolarity = TIM_OCPolarity_High;
  tim_pc9.TIM_OutputState = TIM_OutputState_Enable;
  tim_pc9.TIM_Pulse = 15767;
  TIM_OC4Init(TIM3,&tim_pc9);
  TIM_Cmd(TIM3,ENABLE);
  
  TIM_TimeBaseStructInit(&tim9);
	tim9.TIM_ClockDivision = 0;
	tim9.TIM_CounterMode = TIM_CounterMode_Up;
	tim9.TIM_Period = 199;
	tim9.TIM_Prescaler = 7999;
	TIM_TimeBaseInit(TIM9,&tim9);
  
  TIM_ClearFlag(TIM9,TIM_FLAG_Update);
	
	TIM_Cmd(TIM9,ENABLE);
  
  
	while(1)
	{
    if(TIM_GetFlagStatus(TIM9,TIM_FLAG_Update) == SET)
		{
      TIM_SetAutoreload(TIM3,sheetMusic[songPosition]);
			TIM_SetCompare1(TIM3,((sheetMusic[songPosition])>>1));
      if(sheetMusic[songPosition] == S)
      {
        TIM_Cmd(TIM9,DISABLE);
        TIM_Cmd(TIM3,DISABLE);
      }
			songPosition++;
      TIM_ClearFlag(TIM9,TIM_FLAG_Update);
    }
	}
	
}
