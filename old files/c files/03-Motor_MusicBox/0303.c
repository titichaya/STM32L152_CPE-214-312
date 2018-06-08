#include "stm32l1xx.h" //include header file for stm32l1
#define C 15288 //define arr for note C

int main() //main function goes here
{
	GPIO_InitTypeDef pb7; //store initialize value for GPIO pin
	TIM_OCInitTypeDef tim4pb7;
//store initialize value for output compare timer
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
//open clock for GPIOB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//open clock for timer 4
	
	GPIO_StructInit(&pb7); //initialize pb7 value
	pb7.GPIO_Mode = GPIO_Mode_AF; //pb7->alternate function
	pb7.GPIO_Pin = GPIO_Pin_7; //pb7->pin7
	GPIO_Init(GPIOB,&pb7); //initialize pb7 value on GPIOB
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	//config GPIOB pin 7 to be an alternate function of timer 4
	
	TIM_OCStructInit(&tim4pb7); //initialize tim4pb7 value
	tim4pb7.TIM_OCMode = TIM_OCMode_PWM1; //tim4pb7->PWM
	tim4pb7.TIM_OutputState = TIM_OutputState_Enable; //start with 1
	TIM_OC2Init(TIM4,&tim4pb7); //initialize tim4pb7 value on TIM4
	
	TIM_SetAutoreload(TIM4,C);
//set autoreload value of timer 4 to have C frequency
	TIM_SetCompare2(TIM4,C*0.4);
//duty cycle = C*0.4/(C+1) ~= 40%
	
	TIM_ARRPreloadConfig(TIM4,ENABLE); //store arr for 1st pulse
	
	TIM_Cmd(TIM4,ENABLE); //enable timer 4
	while(1) //loop instruction goes here
	{
		
	}
}
