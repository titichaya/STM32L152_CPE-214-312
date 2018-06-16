#include "stm32l1xx.h" //include header file for stm32l1

int main() //main function goes here
{
	GPIO_InitTypeDef pb6,pb7,pd2;
	//store initialize value for GPIO pins
	TIM_OCInitTypeDef tim4pb6,tim4pb7;
	//store initialize value for output compare timers
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOD,ENABLE);
	//open clock for GPIOB and GPIOD
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//open clock for timer 4
	

GPIO_StructInit(&pb6); //initialize pb6 value
	pb6.GPIO_Mode = GPIO_Mode_AF; //pb6->alternate function
	pb6.GPIO_Pin = GPIO_Pin_6; //pb6->pin6
	GPIO_Init(GPIOB,&pb6); //initialize pb6 value on GPIOB	
	
	GPIO_StructInit(&pb7); //initialize pb7 value
	pb7.GPIO_Mode = GPIO_Mode_AF; //pb7->alternate function
	pb7.GPIO_Pin = GPIO_Pin_7; //pb7->pin7
	GPIO_Init(GPIOB,&pb7); //initialize pb7 value on GPIOB
	
	GPIO_StructInit(&pd2); //initialize pd2 value
	pd2.GPIO_Mode = GPIO_Mode_OUT; //pd2->output
	pd2.GPIO_Pin = GPIO_Pin_2; //pd2->pin2
	GPIO_Init(GPIOD,&pd2); //initialize pd2 value on GPIOD
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
//config GPIOB pin 6 to be an alternate function of timer 4
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
//config GPIOB pin 7 to be an alternate function of timer 4
	
	TIM_SetAutoreload(TIM4,99);
//set autoreload value of timer 4 to have 100 level : 0-99
	
TIM_OCStructInit(&tim4pb6); //initialize tim4pb6 value
	tim4pb6.TIM_OCMode = TIM_OCMode_PWM1; //tim4pb6->PWM
	tim4pb6.TIM_OutputState = TIM_OutputState_Enable; //start with 1
	tim4pb6.TIM_Pulse = 50; //duty cycle = 50/(99+1) = 50%
	TIM_OC1Init(TIM4,&tim4pb6); //initialize tim4pb6 value on TIM4

	TIM_OCStructInit(&tim4pb7); //initialize tim4pb7 value
	tim4pb7.TIM_OCMode = TIM_OCMode_PWM1; //tim4pb7->PWM
	tim4pb7.TIM_OutputState = TIM_OutputState_Enable; //start with 1
	tim4pb7.TIM_Pulse = 0; //duty cycle = 0/(99+1) = 0%
	TIM_OC2Init(TIM4,&tim4pb7); //initialize tim4pb7 value on TIM4
	
	TIM_ARRPreloadConfig(TIM4,ENABLE); //store arr for 1st pulse

GPIO_SetBits(GPIOD,GPIO_Pin_2); //enable L293D

	TIM_Cmd(TIM4,ENABLE); //enable timer 4
	while(1) //loop instruction goes here
	{
		
	}
}
