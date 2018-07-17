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
#include "stm32l1xx_ll_usart.h"

#include "stm32l152_glass_lcd.h"
#include "stdio.h"

#define C_NOTE					15288.
#define Db_NOTE					13550
#define D_NOTE					13619
#define Eb_NOTE					12855.
#define E_NOTE					12134
#define F_NOTE					11456
#define Gb_NOTE					10809
#define G_NOTE					10203
#define Ab_NOTE					9630
#define A_NOTE					9089
#define Bb_NOTE					8579
#define B_NOTE					8098
#define C_HIGH_NOTE			7643
#define MUTE_NOTE				0

const uint32_t musicSheet[] = {E_NOTE, D_NOTE, C_NOTE, D_NOTE, E_NOTE, E_NOTE, E_NOTE, D_NOTE, D_NOTE, D_NOTE, D_NOTE, G_NOTE, G_NOTE,
															 E_NOTE, D_NOTE, C_NOTE, D_NOTE, E_NOTE, E_NOTE, E_NOTE, D_NOTE, D_NOTE, D_NOTE, E_NOTE, D_NOTE, C_NOTE};
const uint8_t tempo[] = {1,1,1,1,1,1,2,1,1,1,2,1,4,1,1,1,1,1,1,2,1,1,1,1,4};
uint8_t idx_conductor = 0;

uint32_t song[sizeof(musicSheet)/sizeof(uint32_t) * 2];
uint8_t  rythm[sizeof(tempo)/sizeof(uint8_t) * 2]; 

void SystemClock_Config(void);

	
void CreateSong()
{
	uint8_t i;
	
	for(i = 0; i < sizeof(song)/sizeof(uint32_t); ++i)
	{
		song[i] = musicSheet[i];
		rythm[i] = tempo[i] * 4;
		song[i + 1] = MUTE_NOTE;
		rythm[i + 1] = 1;
	}
}	
uint32_t ARR_TempoCal(uint8_t beat)
{
	/*return arr value according to desire tempo (beat per min or BPM thus 60/beat = BPS or beat per sec)*/
	return (SystemCoreClock/(32000*(beat/60)));
}
void TIMBase_Config(uint8_t tempo)
{
	LL_TIM_InitTypeDef tim_initstruct;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	
	tim_initstruct.Autoreload = ARR_TempoCal(tempo);
	tim_initstruct.Prescaler = 32000;
	tim_initstruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim_initstruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM2, &tim_initstruct);
	
//	LL_TIM_EnableIT_UPDATE(TIM2);
//	
//	NVIC_SetPriority(TIM2_IRQn, 0);
//	NVIC_EnableIRQ(TIM2_IRQn);
//  /* Enable counter */
  LL_TIM_EnableCounter(TIM2);
//  
//  /* Force update generation */
//  LL_TIM_GenerateEvent_UPDATE(TIM2);
}

void LED_Config(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);
	
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
}

void TIMOC_GPIO_Config()
{
	LL_GPIO_InitTypeDef gpio_initstruct;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_initstruct.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_initstruct.Alternate = LL_GPIO_AF_2;
	gpio_initstruct.Pin = LL_GPIO_PIN_7;
	gpio_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_initstruct.Pull = LL_GPIO_PULL_DOWN;
	gpio_initstruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &gpio_initstruct);
	
}
void TIMOC_Config()
{
	LL_TIM_InitTypeDef tim_initstruct;
	LL_TIM_OC_InitTypeDef tim_oc_initstruct;
	
	TIMOC_GPIO_Config();
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	/*Config time base for timer 4*/
	tim_initstruct.Autoreload = E_NOTE;
	tim_initstruct.Prescaler = 2;
	tim_initstruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim_initstruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM4, &tim_initstruct);
	
	NVIC_SetPriority(TIM4_IRQn, 1);
	NVIC_EnableIRQ(TIM4_IRQn);
	
	tim_oc_initstruct.OCState = LL_TIM_OCSTATE_DISABLE;
	tim_oc_initstruct.OCMode = LL_TIM_OCMODE_PWM1;
	tim_oc_initstruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	tim_oc_initstruct.CompareValue = tim_initstruct.Autoreload/4;
	LL_TIM_OC_Init(TIM4, LL_TIM_CHANNEL_CH2, &tim_oc_initstruct);
	
	LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH2);
	
  LL_TIM_EnableCounter(TIM4);
	
  LL_TIM_EnableIT_CC2(TIM4);
	
	LL_TIM_OC_SetCompareCH1(TIM4, 0);
	
  LL_TIM_GenerateEvent_UPDATE(TIM4);
}

void Button_Config()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);
}

void DutyCycle_Adjust(uint32_t d)
{
	LL_TIM_SetAutoReload(TIM4, d);
	LL_TIM_OC_SetCompareCH2(TIM4, LL_TIM_GetAutoReload(TIM4)/2);
}
/*Not use*/

	uint8_t idx = 0;
int main(void)
{

  SystemClock_Config();
	TIMOC_Config();
	/*Config time base that beat 60 BPM (Beat per Mins)*/
	TIMBase_Config(210);
	LED_Config();
	CreateSong();
  while (1)
	{
		if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == 1)
		{
			LL_TIM_ClearFlag_UPDATE(TIM2);
			idx_conductor++;
			if(idx_conductor >= rythm[idx])
			{
				idx_conductor = 0;
				DutyCycle_Adjust(song[idx]);
				++idx;
			}
		}
	}
}

void TIM4_IRQHandler(void)
{
  /* Check whether CC1 interrupt is pending */
  if(LL_TIM_IsActiveFlag_CC2(TIM4) == 1)
  {
    /* Clear the update interrupt flag*/
    LL_TIM_ClearFlag_CC2(TIM4);
  }
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
