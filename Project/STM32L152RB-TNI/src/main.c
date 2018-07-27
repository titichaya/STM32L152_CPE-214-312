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
#include "stm32l1xx_ll_spi.h"
#include "stm32l152_glass_lcd.h"
#include "stdio.h"

/* 24K640 Related Definition */
#define DUMMY_BYTE							0xFF

#define WRITE_BYTE_CMD					0x02
#define READ_BYTE_CMD						0x03
#define WRITE_STATUS_CMD				0x01
#define READ_STATUS_CMD					0x05

/* SPI Deifinition */
#define SPIx										SPI1
#define SPIx_CLK_ENABLE()				LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

#define	SPI_MOSI_PIN						LL_GPIO_PIN_5
#define	SPI_MOSI_PORT						GPIOB
#define SPI_MOSI_AF_CONFIG() 		LL_GPIO_SetAFPin_0_7(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_AF_5);

#define	SPI_MISO_PIN						LL_GPIO_PIN_4
#define	SPI_MISO_PORT						GPIOB
#define SPI_MISO_AF_CONFIG()		LL_GPIO_SetAFPin_0_7(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_AF_5);

#define	SPI_SCLK_PIN						LL_GPIO_PIN_3
#define	SPI_SCLK_PORT						GPIOB
#define SPI_SLCK_AF_CONFIG()		LL_GPIO_SetAFPin_0_7(SPI_SCLK_PORT, SPI_SCLK_PIN, LL_GPIO_AF_5);

#define SPI_CS_PIN							LL_GPIO_PIN_15
#define SPI_CS_PORT							GPIOB

#define SPI_START_COMM()				LL_GPIO_ResetOutputPin(SPI_CS_PORT, SPI_CS_PIN)
#define SPI_END_COMM()					LL_GPIO_SetOutputPin(SPI_CS_PORT, SPI_CS_PIN)

/* Private function */
void SystemClock_Config(void);
void SPI_GPIO_Config(void);
void SPI_Config(void);
void SPI_SendByte(uint8_t);
uint8_t SPI_RecvByte(void);
void SRAM_MemWriteByte(uint16_t addr, uint8_t data);
uint8_t SRAM_MemReadByte(uint16_t addr);
uint8_t SRAM_ReadStatus(void);

/* Private Variable */
uint8_t cmd_seq[4];
uint8_t data;



int main(void)
{
  SystemClock_Config();
	SPI_Config();
	
	SRAM_MemWriteByte(0xF0F0, 0x45);
	data = SRAM_MemReadByte(0xF0F0);
	
  while (1);
}


void SPI_GPIO_Config(void)
{
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /* Configure SCK Pin connected to pin 31 of CN10 connector */
  LL_GPIO_SetPinMode(SPI_SCLK_PORT, SPI_SCLK_PIN, LL_GPIO_MODE_ALTERNATE);
  SPI_SLCK_AF_CONFIG();
  LL_GPIO_SetPinSpeed(SPI_SCLK_PORT, SPI_SCLK_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinPull(SPI_SCLK_PORT, SPI_SCLK_PIN, LL_GPIO_PULL_DOWN);

  /* Configure MISO Pin connected to pin 27 of CN10 connector */
  LL_GPIO_SetPinMode(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_MODE_ALTERNATE);
  SPI_MISO_AF_CONFIG();
  LL_GPIO_SetPinSpeed(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinPull(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_PULL_DOWN);

  /* Configure MOSI Pin connected to pin 29 of CN10 connector */
  LL_GPIO_SetPinMode(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_MODE_ALTERNATE);
	SPI_MOSI_AF_CONFIG();
  LL_GPIO_SetPinSpeed(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinPull(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_PULL_DOWN);
	
	  /* Configure CS Pin  */
  LL_GPIO_SetPinMode(SPI_CS_PORT, SPI_CS_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinSpeed(SPI_CS_PORT, SPI_CS_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinPull(SPI_CS_PORT, SPI_CS_PIN, LL_GPIO_PULL_NO);
	
	LL_GPIO_SetOutputPin(SPI_CS_PORT, SPI_CS_PIN);
}

void SPI_Config(void)
{

  SPI_GPIO_Config();
  /* (3) Configure SPI functional parameters ********************************/
  SPIx_CLK_ENABLE();

  /* Configure SPI communication */
  LL_SPI_SetBaudRatePrescaler(SPIx, LL_SPI_BAUDRATEPRESCALER_DIV256);
  LL_SPI_SetTransferDirection(SPIx,LL_SPI_FULL_DUPLEX);
  LL_SPI_SetClockPhase(SPIx, LL_SPI_PHASE_1EDGE);
  LL_SPI_SetClockPolarity(SPIx, LL_SPI_POLARITY_LOW);
  /* Reset value is LL_SPI_MSB_FIRST */
  LL_SPI_SetTransferBitOrder(SPIx, LL_SPI_MSB_FIRST);
  LL_SPI_SetDataWidth(SPIx, LL_SPI_DATAWIDTH_8BIT);
  LL_SPI_SetNSSMode(SPIx, LL_SPI_NSS_SOFT);
  LL_SPI_SetMode(SPIx, LL_SPI_MODE_MASTER);
	
	LL_SPI_Enable(SPIx);
}

void SPI_SendByte(uint8_t data)
{
	while(LL_SPI_IsActiveFlag_TXE(SPIx) == RESET);
	LL_SPI_TransmitData8(SPIx, data);
}

uint8_t SPI_RecvByte()
{
	uint8_t temp;
	/* Wait for RX buffer empty flag */
	while(LL_SPI_IsActiveFlag_RXNE(SPIx) == RESET);
	temp = LL_SPI_ReceiveData8(SPIx);
	return temp;
}

void SRAM_MemWriteByte(uint16_t addr, uint8_t data)
{
	uint8_t i;
	
	cmd_seq[0] = WRITE_BYTE_CMD;
	cmd_seq[1] = (addr>>8);
	cmd_seq[2] = (addr&0xFF);
	cmd_seq[3] = data;

	
	/* PULL CS LOW */
	SPI_START_COMM();

	/* START SENDING SEQ */
	for(i = 0l; i < 4; ++i)
	{
		SPI_SendByte(cmd_seq[i]);
		SPI_RecvByte();
	}
	
	/* PUT CS HIGH */
	SPI_END_COMM();
}

uint8_t SRAM_MemReadByte(uint16_t addr)
{
	uint8_t i;
	uint8_t tmp;
	
	cmd_seq[0] = READ_BYTE_CMD;
	cmd_seq[1] = (addr>>8);
	cmd_seq[2] = (addr&0xFF);
	cmd_seq[3] = DUMMY_BYTE;
	
	/* PULL CS LOW */
	SPI_START_COMM();
	
	for(i = 0; i < 4; ++i)
	{
		SPI_SendByte(cmd_seq[i]);
		tmp = SPI_RecvByte();
	}
	
	/* PUT CS HIGH */
	SPI_END_COMM();
	
	return tmp;
	
}

uint8_t SRAM_ReadStatus(void)
{
	uint8_t tmp;
		/* PULL CS LOW */
	SPI_START_COMM();
	LL_mDelay(1);
	
	/* START SENDING SEQ */
	SPI_SendByte(READ_STATUS_CMD);
	SPI_RecvByte();
	SPI_SendByte(DUMMY_BYTE);
	tmp = SPI_RecvByte();
	
	/* PUT CS HIGH */
	SPI_END_COMM();
	return tmp;
	
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
