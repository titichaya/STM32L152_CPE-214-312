#include "stm32l1xx.h" //include header file for stm32l1
int main() //main function goes here
{
	GPIO_InitTypeDef pb6; //store initialize value for GPIO pin
	TIM_OCInitTypeDef tim4pb6;
//store initialize value for output compare timer
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	//open clock for GPIOB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	//open clock for timer 4

	GPIO_StructInit(&pb6); //initialize pb6 value
	pb6.GPIO_Mode = GPIO_Mode_AF; //pb6->alternate function
	pb6.GPIO_Pin = GPIO_Pin_6; //pb6->pin6
	GPIO_Init(GPIOB,&pb6); //initialize pb6 value on GPIOB
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
	//config GPIOB pin 6 to be an alternate function of timer 4
	
	TIM_SetAutoreload(TIM4,99);
//set autoreload value of timer 4 to have 100 level : 0-99
	
	TIM_OCStructInit(&tim4pb6); //initialize tim4pb6 value
	tim4pb6.TIM_OCMode = TIM_OCMode_PWM1; //tim4pb6->PWM
	tim4pb6.TIM_OutputState = TIM_OutputState_Enable; //start with 1
	tim4pb6.TIM_Pulse = 12; //duty cycle = 12/(99+1) = 12%
	TIM_OC1Init(TIM4,&tim4pb6); //initialize tim4pb6 value on TIM4
	
TIM_ARRPreloadConfig(TIM4,ENABLE); //store arr for 1st pulse

	TIM_Cmd(TIM4,ENABLE); //enable timer 4
	while(1) //loop instruction goes here
	{
		
	}
}
