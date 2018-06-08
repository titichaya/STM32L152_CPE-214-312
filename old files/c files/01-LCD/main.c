#include "stm32l1xx.h"
#include "stm32l_discovery_lcd.h"
#include "discover_board.h"
#include "stdio.h"

int data = 9;
char strDisp[20] = "ABCD";
void  RCC_RTC_Configuration(void);

int main() {
  
  RCC_RTC_Configuration();
  LCD_GLASS_Init();
  LCD_GLASS_Configure_GPIO();
    
    /*Blink*/
    LCD_GLASS_DisplayString((unsigned char *) strDisp);
    LCD_BlinkConfig(LCD_BlinkMode_AllSEG_AllCOM,LCD_BlinkFrequency_Div512);
    Delay(150);
    Delay(150);
    Delay(150);
    Delay(150);
    Delay(150);	
    LCD_BlinkConfig(LCD_BlinkMode_Off,LCD_BlinkFrequency_Div512);
  
  while(1)
  {
    /*display*/
    sprintf(strDisp, "test %d", data );
    LCD_GLASS_DisplayString((unsigned char *) strDisp);
    

    /*scroll*/
    //LCD_GLASS_ScrollSentence("Hello world",1,150);
    
  }
}

void RCC_RTC_Configuration(void)
{  
  RCC_ClocksTypeDef RCC_Clocks;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_LCD | RCC_APB1Periph_PWR, ENABLE);
  
  PWR_RTCAccessCmd(ENABLE);					//Allow access to the RTC
  RCC_RTCResetCmd(ENABLE);					//Reset Backup Domain
  RCC_RTCResetCmd(DISABLE);
  RCC_RTCCLKCmd(ENABLE);
  
  RCC_LSICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET); //Wait till LSI is ready  
  
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);	//LCD Clock Source Selection
  
  
  	/* Configure SysTick IRQ and SysTick Timer to generate interrupts */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 500);
  
}