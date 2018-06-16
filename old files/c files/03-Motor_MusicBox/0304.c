#include "stm32l1xx.h" //include header file for stm32l1
#include "stm32l_discovery_lcd.h" //include header file for lcd
#include "discover_board.h" //include header file for board
#include <stdio.h> //include standard io header

char LCD_buff[6]; //LCD_buffer store text before display to LCD
uint16_t count = 0; //store count value

void TIM4_IRQHandler(void) //ISR for timer 4
{
	if(TIM_GetFlagStatus(TIM4,TIM_FLAG_Update)==SET)
	//make sure that is an interrupt from timer 4
	{
		Count++; //increase count by 1
		count%=31; //0<=count<=30
		sprintf(LCD_buff,"%d",count);
		//put string “<count’s value>” to LCD_buff
		LCD_GLASS_Clear(); //clear LCD Screen
		LCD_GLASS_DisplayString((unsigned char*)LCD_buff);
// display LCD_buff string on LCD
		TIM_ClearFlag(TIM4,TIM_FLAG_Update);
		//clear timer 4 interrupt flag
	}
}

int main() //main function goes here
{
	TIM_TimeBaseInitTypeDef tim4;
//store initialize value for timer
	NVIC_InitTypeDef tim4interrupt;
//store initialize value for interrupt
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	//open clock for timer 4
	
	TIM_TimeBaseStructInit(&tim4);
	//initialize tim value
	/*	1 sec->16,000,000 clock
		(ARR+1)*(PSC+1) = 16,000,000*1
		ARR = 4000-1
		PSC = 4000-1						*/
	tim4.TIM_Prescaler = 4000-1;
	tim4.TIM_Period = 4000-1;
	TIM_TimeBaseInit(TIM4,&tim4); //initialize tim4 value on timer 4
	
	tim4interrupt.NVIC_IRQChannel = TIM4_IRQn;
	//timer 4 interrupt
	tim4interrupt.NVIC_IRQChannelCmd = ENABLE;
	//enable interrupt for timer 4
	NVIC_Init(&tim4interrupt);
//initialize interrupt by tim4interrupt value
	
	RCC_Configuration(); //config RCC for LCD
	RTC_Configuration(); //config RTC for LCD
	LCD_GLASS_Configure_GPIO(); //config io pin for LCD
	LCD_GLASS_Init(); //initialize LCD
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
//config timer 4 interrupt
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
//clear timer 4 interrupt flag
	
	TIM_Cmd(TIM4,ENABLE); //enable timer 4
	
	while(1) //loop instruction goes here
	{
		
	}
}

