/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_tim.h"
#include "stm32l1xx_ll_lcd.h"
#include "stm32l1xx_ll_exti.h"

#include "stm32l152_glass_lcd.h"
#include "stdio.h"

void SystemClock_Config(void);

void TIMInputCapture_Config(void);

/*Not use*/
void TIMBase_Config(void);
void LED_Config(void);
void Button_Config(void);

volatile uint32_t uwICValue1, uwICValue2;
volatile uint32_t uwDiffCap = 0;
uint32_t counter;

int main(void)
{
  SystemClock_Config();
	TIMInputCapture_Config();
//	Button_Config();
  while (1)
	{
		counter = LL_TIM_GetCounter(TIM2);
	}
}


void TIMInputCapture_Config(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStructure;
	LL_TIM_IC_InitTypeDef TIM_IC_InitStructure;
	LL_TIM_InitTypeDef TIM_InitStructure;
  /* Enable the peripheral clock of GPIOs */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  /* GPIO TIM1_CH1 configuration */
	GPIO_InitStructure.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStructure.Alternate = LL_GPIO_AF_1;
	GPIO_InitStructure.Pin = LL_GPIO_PIN_0;
	GPIO_InitStructure.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOA, &	GPIO_InitStructure);
	
  /***************************************************************/
  /* Configure the NVIC to handle TIM3 capture/compare interrupt */
  /***************************************************************/
  NVIC_SetPriority(TIM2_IRQn, 0);
  NVIC_EnableIRQ(TIM2_IRQn);
  
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
 
	TIM_InitStructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_InitStructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStructure.Prescaler = 3200; //Prescale TIM2 clock to 10kHz
	TIM_InitStructure.Autoreload = 10000-1; //Create counting value for 1 ms
	LL_TIM_Init(TIM2,&TIM_InitStructure);

  /************************************/
  /* Input capture mode configuration */
  /************************************/
	TIM_IC_InitStructure.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
	TIM_IC_InitStructure.ICFilter = LL_TIM_IC_FILTER_FDIV1;
	TIM_IC_InitStructure.ICPrescaler = LL_TIM_ICPSC_DIV1;
	TIM_IC_InitStructure.ICPolarity = LL_TIM_IC_POLARITY_FALLING;
	LL_TIM_IC_Init(TIM2, LL_TIM_CHANNEL_CH1, &TIM_IC_InitStructure);
	 
  /* Enable the capture/compare interrupt for channel 1 */
  LL_TIM_EnableIT_CC1(TIM2);
 
  /* Enable output channel 1 */
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    
  /* Enable counter */
  LL_TIM_EnableCounter(TIM2);
}

void TIM2_IRQHandler(void)
{
	static uint8_t state = 0;
  /* Check whether update interrupt is pending */
  if(LL_TIM_IsActiveFlag_CC1(TIM2) == 1)
  {
		LL_TIM_ClearFlag_CC1(TIM2);
		if(state == 0)
		{
			uwICValue1 = LL_TIM_IC_GetCaptureCH1(TIM2);
			state = 1;
		}
		else
		{
			uwICValue2 = LL_TIM_IC_GetCaptureCH1(TIM2);
			/*Pulse computation*/
			if(uwICValue1 > uwICValue2)
			{
				uwDiffCap = uwICValue1 - uwICValue2;
			}
			else
			{
				uwDiffCap = ((LL_TIM_GetPrescaler(TIM2) - uwICValue1) + uwICValue2) + 1;
			}
			state = 0;
		}
		LL_TIM_SetCounter(TIM2, 0);
  }
}

void LED_Config(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStructure;
	
	/*Enable LED PB6 PB7 Clock*/
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	/*Config PB6 PB7 as OUTPUT mode*/
	GPIO_InitStructure.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStructure.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
	GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStructure.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*Initial Output state*/
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
}

//void TIMBase_Config(void)
//{
//	LL_TIM_InitTypeDef TIM_InitStructure;
//	
//	/* Enable the timer peripheral clock */
//  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
//	/* Config time base that overflow every 1 ms*/
//	TIM_InitStructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
//	TIM_InitStructure.CounterMode = LL_TIM_COUNTERMODE_UP;
//	TIM_InitStructure.Prescaler = 3200; //Prescale TIM2 clock to 10kHz
//	TIM_InitStructure.Autoreload = 10000-1; //Create counting value for 1 ms
//	LL_TIM_Init(TIM2,&TIM_InitStructure);
//	/*Config interrupt that trigger when 1 ms passed (ident by Update Event Flag (UEF))*/
//	LL_TIM_EnableIT_UPDATE(TIM2);
//  
//  /* Configure the NVIC to handle TIM2 update interrupt */
//  NVIC_SetPriority(TIM2_IRQn, 0);
//  NVIC_EnableIRQ(TIM2_IRQn);
//  
//  /* Enable counter */
//  LL_TIM_EnableCounter(TIM2);
//  
//  /* Force update generation */
//  //LL_TIM_GenerateEvent_UPDATE(TIM2);
//}

void Button_Config(void)
{
	LL_EXTI_InitTypeDef EXTI_InitStructure;

	/* Enable the BUTTON Clock */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
//  /* Configure GPIO for BUTTON */
//	GPIO_InitStructure.Mode = LL_GPIO_MODE_INPUT;
//	GPIO_InitStructure.Pin = LL_GPIO_PIN_0;
//	GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStructure.Pull = LL_GPIO_PULL_NO;
//	GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
//	LL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Connect External Line to the GPIO*/
    /* Enale EXTI Config Access */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);
    
  /* Enable a rising trigger EXTI line 13 Interrupt */
  EXTI_InitStructure.Line_0_31 = LL_EXTI_LINE_0;
	EXTI_InitStructure.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStructure.Trigger = LL_EXTI_TRIGGER_RISING;
	EXTI_InitStructure.LineCommand = ENABLE;
	LL_EXTI_Init(&EXTI_InitStructure);
    
  /* Configure NVIC for USER_BUTTON_EXTI_IRQn */
  NVIC_EnableIRQ(EXTI0_IRQn); 
  NVIC_SetPriority(EXTI0_IRQn,0x03);  
}

/* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 32000000
  *            HCLK(Hz)                       = 32000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLLMUL                         = 6
  *            PLLDIV                         = 3
  *            Flash Latency(WS)              = 1
  * @retval None
  */
void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}


//void TIM3_IRQHandler(void)
//{
//  /* Check whether CC1 interrupt is pending */
//  if(LL_TIM_IsActiveFlag_CC1(TIM3) == 1)
//  {
//    /* Clear the update interrupt flag*/
//    LL_TIM_ClearFlag_CC1(TIM3);
//		LL_TIM_SetCounter(TIM3, 0);
//  }
//}

void EXTI0_IRQHandler(void)
{
	static uint8_t state = 0;
	if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
		
		if(state == 0)
		{
			uwICValue1 = LL_TIM_IC_GetCaptureCH1(TIM2);
			state = 1;
		}
		else
		{
			uwICValue2 = LL_TIM_IC_GetCaptureCH1(TIM2);
			/*Pulse computation*/
			if(uwICValue1 > uwICValue2)
			{
				uwDiffCap = uwICValue1 - uwICValue2;
			}
			else
			{
				uwDiffCap = ((LL_TIM_GetPrescaler(TIM2) - uwICValue1) + uwICValue2) + 1;
			}
			state = 0;
		}

		
		
	}
}

/* ==============   BOARD SPECIFIC CONFIGURATION CODE END      ============== */

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
