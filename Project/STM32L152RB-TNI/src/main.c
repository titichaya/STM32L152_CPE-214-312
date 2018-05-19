/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_lcd.h"

#include "stm32l152_glass_lcd.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void GPIO_Config(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  SystemClock_Config();
	LCD_GLASS_Init();
	LCD_GLASS_DisplayString((uint8_t*) "1+1=2");

  while (1)
  {
		//LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
		//GPIOB->BSRR |= (1<<6);
		//*(uint32_t*)(0x40020418) |= (1<<6); //Enable Output at PB8 via BSSR

  }
}

void GPIO_Config(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	/** Raw Style **/
	*(uint32_t*)0x4002381C |= (1<<1); //Enable GPIOB Clock via AHBENR
	*(uint32_t*)0x40020400 |= (1<<12); //Config Output mode via MODER
	*(uint32_t*)0x40020404 |= ~(1<<6); // Config Ouput Type via OTYPE
	*(uint32_t*)0x40020408 |= (1<<12); //Config Speed via OSPEEDR
	*(uint32_t*)0x4002040C |= ~(3<<12); //Config Push pull type via PUPD
	
	/**Bit Shifting Style **/
//	RCC->AHBENR |= (1<<1); //Enable GPIOB clock source
//	
//	GPIOB->MODER |= (1<<12); //GPIOB pin 6 to output mode
//	GPIOB->OTYPER |= ~(1<<6); //GPIOB pin 6 to push pull output type
//	GPIOB->OSPEEDR |= (2<<13); //GPIOB pin 6 to speed high
//	GPIOB->PUPDR |= ~(3<<13); //GPIOB pin 6 to no pull
	
	/** TNI Traditional Way **/
	/*Enable Clock at AHB bus*/
//	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
//	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
//	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
//	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/** LL Style Pin Config **/
//	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
//  LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
//  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_LOW);
//  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);
	
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
