#include "stm32l1xx.h" //include header file for stm32l1
#include "stm32l_discovery_lcd.h" //include header file for lcd
#include "discover_board.h" //include header file for board
#include <stdio.h> //include standard io header

char LCD_buff[6]; //LCD_buffer store text before display to LCD
uint16_t count = 0; //store count value

void pa0interrupt_Cmd(bool ena)
//function to enable/disable GPIOA pin 0 external interrupt
{
	NVIC_InitTypeDef ipa0; //store initialize value for interrupt
	ipa0.NVIC_IRQChannel = EXTI0_IRQn; //external interrupt line 0
	ipa0.NVIC_IRQChannelCmd = ena?ENABLE:DISABLE;
	//enable when ena is TRUE
	NVIC_Init(&ipa0); //initialize interrupt by ipa0 value
}

void tim4interrupt_Cmd(bool ena)
//function to enable/disable timer 4 interrupt
{
	NVIC_InitTypeDef tim4interrupt;
//store initialize value for interrupt
	tim4interrupt.NVIC_IRQChannel = TIM4_IRQn; //timer 4 interrupt
	tim4interrupt.NVIC_IRQChannelCmd = ena?ENABLE:DISABLE;
	//enable when ena is TRUE
	NVIC_Init(&tim4interrupt); 
//initialize interrupt by tim4interrupt value
}

void EXTI0_IRQHandler (void) //ISR for pa0
{
	if(EXTI_GetITStatus(EXTI_Line0)==SET)
	//make sure that is an interrupt from pa0
	{
		count = 49; //set count = 49

		/* DISPLAY FOR THE FIRST TIME AFTER pa0 interrupt  */
		sprintf(LCD_buff,"%d",count);
		//put string “<count’s value>” to LCD_buff
		LCD_GLASS_Clear(); //clear LCD Screen
		LCD_GLASS_DisplayString((unsigned char*)LCD_buff);
		// display LCD_buff string on LCD
		pa0interrupt_Cmd(FALSE); //disable pa0 external interrupt
		tim4interrupt_Cmd(TRUE); //enable timer 4 interrupt
		TIM_ClearFlag(TIM4,TIM_FLAG_Update);
//clear timer 4 interrupt flag
		TIM_Cmd(TIM4,ENABLE); //enable timer 4
		EXTI_ClearFlag(EXTI_Line0); //clear pa0 interrupt flag
	}
}

void TIM4_IRQHandler(void) //ISR for timer 4
{
	if(TIM_GetFlagStatus(TIM4,TIM_FLAG_Update)==SET)
	//make sure that is an interrupt from timer 4
	{
		count--; //decrease count by 1
		sprintf(LCD_buff,"%d",count);
		//put string “<count’s value>” to LCD_buff
		LCD_GLASS_Clear(); //clear LCD Screen
		LCD_GLASS_DisplayString((unsigned char*)LCD_buff);
		// display LCD_buff string on LCD
		if(count==0) //do if count to 0
		{
			EXTI_ClearITPendingBit(EXTI_Line0);
			//clear pa0 external interrupt pending bit
			pa0interrupt_Cmd(TRUE);
//enable pa0 external interrupt
			tim4interrupt_Cmd(FALSE); //disable timer 4 interrupt
			TIM_Cmd(TIM4,DISABLE); //disable timer 4
		}
		TIM_ClearFlag(TIM4,TIM_FLAG_Update);
		//clear timer 4 interrupt flag
	}
}

int main() //main function goes here
{
	GPIO_InitTypeDef pa0; //store initialize value for GPIO ports
	EXTI_InitTypeDef epa0;
//store initialize value for external interrupt
	TIM_TimeBaseInitTypeDef tim4;
	//store initialize value for timer
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
//open clock for GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	//open clock for timer 4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
//open clock for system config	

	GPIO_StructInit(&pa0); //initialize pa0 value
	pa0.GPIO_Mode = GPIO_Mode_IN; //pa0->input
	pa0.GPIO_Pin = GPIO_Pin_0; //pa0->pin0
	GPIO_Init(GPIOA,&pa0); //initialize pa0 value on GPIOA
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	//set pa0 to be an external interrupt
	
	EXTI_StructInit(&epa0); //initialize epa0 value
	epa0.EXTI_Line = EXTI_Line0; //pa0->line0
	epa0.EXTI_LineCmd = ENABLE; //enable external interrupt for pa0
	EXTI_Init(&epa0); //initialize external interrupt by epa0 value
	
	TIM_TimeBaseStructInit(&tim4);
	tim4.TIM_Prescaler = 8000-1;
	tim4.TIM_Period = 4000-1;
	TIM_TimeBaseInit(TIM4,&tim4);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	RCC_Configuration(); //config RCC for LCD
	RTC_Configuration(); //config RTC for LCD
	LCD_GLASS_Configure_GPIO(); //config io pin for LCD
	LCD_GLASS_Init(); //initialize LCD
	
	/* DISPLAY FOR THE FIRST TIME */
	sprintf(LCD_buff,"%d",count);
	//put string “C <count’s value>” to LCD_buff
	LCD_GLASS_Clear(); //clear LCD Screen
	LCD_GLASS_DisplayString((unsigned char*)LCD_buff);
	// display LCD_buff string on LCD

	pa0interrupt_Cmd(TRUE); //enable pa0 external interrupt
	while(1) //loop instruction goes here
	{
		
	}
}



Define note :

Note		Frequency		Period(T)		 ARR
   C		   1046.5		  9.56e-4		15288
   D		   1174.7		  8.51e-4		13620
   E		   1318.5		  7.58e-4		12134
   F		   1396.9		  7.16e-4		11453
   G		   1568.0		  6.38e-4		10203
   A		   1760.0		  5.68e-4		 9090
   B		   1975.5		  5.06e-4		 8098
  Ch		   2093.0		  4.78e-4		 7644
  Dh		   2349.3		  4.26e-4		 6810
  Eh		   2637.0		  3.79e-4		 6067


sheetMusic :

sheetNote[] = {Eh, Dh, Eh, Dh, Ch, A, Ch, A, G, E, G, A, Ch, Eh, Dh, Eh, Dh, Ch, A, Ch, Ch, Ch, Ch, G, E, G, Ch, B, A, Eh, Ch, B, A, A, B, A, G, E, Eh, Eh, Eh, Eh, Dh, Eh, Dh, Ch, A, G, A, B, Ch, Dh}
