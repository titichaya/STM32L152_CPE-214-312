#include "stm32l1xx.h"
#include "stm32l_discovery_lcd.h"
#include "discover_board.h"
#include "stdio.h"
#include "string.h"

#define USARTx USART1
#define RX_BUFFER_SIZE 512

int data=1,RX_BUFFER_INDEX=0,status=0,length=0;
char RX_BUFFER[RX_BUFFER_SIZE]={0};
char RESPONSE_BUFFER[10]={0};
char *response_, *response_nocharge;
char DataSendToServer[RX_BUFFER_SIZE]={0};
char nbr_DataSendToServer[10]={0};
int state = 0,count=0;

void init_USART(void);
void send_data(const char *str_send);
void wait_data(const char *str_response);
void display(const char *str_display);
char *strnstr(const char *haystack, const char *needle, size_t len);

void  RCC_RTC_Configuration(void);
/*---------------------------------------------------------------------------
  Main program start here
 *---------------------------------------------------------------------------*/
int main() 
{
  GPIO_InitTypeDef pa0;
  
  RCC_RTC_Configuration();
  LCD_GLASS_Init();
  LCD_GLASS_Configure_GPIO();
  
  init_USART();
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
  
  GPIO_StructInit(&pa0);
  pa0.GPIO_Mode = GPIO_Mode_IN;
  pa0.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA,&pa0);

    while(1) {
    if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 1 && state == 0)
    {
      count++;
      sprintf(DataSendToServer,"GET /cpe312/index.php?Name=TESTING_%d HTTP/1.1\r\nHost: markdang.lnw.mn\r\n\r\n",count);
      
      // Test AT startup
      send_data("AT\r\n");
      wait_data("OK");
      
      // Restart module
      send_data("AT+RST\r\n");
      wait_data("ready");

      display("OK RST");
   
      // Set Station & softAP Mode
      send_data("AT+CWMODE_CUR=3\r\n");
      wait_data("OK");
      display("STA+AP");
      
      // Set Station & softAP Mode
      send_data("AT+CWJAP_CUR=\"CPE312\",\"25033333\"\r\n");
      wait_data("OK");
      display("SET AP");
      
      // Set TCP , Address & Port : Check data http://markdang.lnw.mn/cpe312/show_data.php
      send_data("AT+CIPSTART=\"TCP\",\"markdang.lnw.mn\",80\r\n");
      wait_data("CONNECT");
      display("SETTCP");
      
      length = strlen(DataSendToServer);  // find length of data
      sprintf(nbr_DataSendToServer, "AT+CIPSEND=%d\r\n", length); // Set data size
      
      // Send length of data to server
      send_data(nbr_DataSendToServer);
      wait_data(">");
      display("SetLEN");
      
      // Send data to server
      send_data(DataSendToServer);
      wait_data("SEND OK");
      display("SENDOK");
      
      // Close AP
      send_data("AT+CWQAP\r\n");
      wait_data("OK");
      display("Close");
      
      state = 1;
    }
    else if ( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0 && state == 1)
    {
      state = 0;
    }
    display("ready ");
    }
}

void USART1_IRQHandler(void){
	if(USART_GetITStatus(USARTx,USART_IT_RXNE) == 1)
	{
    data = USART_ReceiveData(USARTx);
      RX_BUFFER[RX_BUFFER_INDEX++] = data;
    if(RX_BUFFER_INDEX >= RX_BUFFER_SIZE)
      RX_BUFFER_INDEX = 0;
    USART_ClearITPendingBit(USARTx,USART_IT_RXNE);
	}
}

void init_USART(void){
  USART_InitTypeDef ua;
	GPIO_InitTypeDef pb6_7;
  NVIC_InitTypeDef nv;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
  
  GPIO_StructInit (&pb6_7);
		pb6_7.GPIO_Mode = GPIO_Mode_AF;
		pb6_7.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;
	GPIO_Init (GPIOB, &pb6_7);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
	
	USART_StructInit(&ua);
	ua.USART_BaudRate=115200;
	ua.USART_WordLength=USART_WordLength_8b;
	ua.USART_StopBits=USART_StopBits_1;
	ua.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USARTx,&ua);
	USART_Cmd(USARTx,ENABLE);
  
  USART_ITConfig(USARTx,USART_IT_RXNE,ENABLE);
 	USART_Cmd(USARTx,ENABLE);
	
	nv.NVIC_IRQChannel = USART1_IRQn;
	nv.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nv);
}

void send_data(const char *str_send)
{
  int i=0,len = strlen(str_send);
  while(len--) {
    USART_SendData(USARTx, str_send[i]);
    i++;
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
  }
}

void wait_data(const char *str_response)
{
    response_ = strnstr(RX_BUFFER, str_response,RX_BUFFER_SIZE);
    response_nocharge = strnstr(RX_BUFFER, "no change",RX_BUFFER_SIZE);
    while( response_ == NULL && response_nocharge == NULL) {
      response_nocharge = strnstr(RX_BUFFER, "no change",RX_BUFFER_SIZE);
      response_ = strnstr(RX_BUFFER, str_response,RX_BUFFER_SIZE);
    }
    RX_BUFFER_INDEX = 0;
    memset(RX_BUFFER,RX_BUFFER_INDEX,RX_BUFFER_SIZE);
}

char *strnstr(const char *haystack, const char *needle, size_t len)
{
        int i;
        size_t needle_len;

        /* segfault here if needle is not NULL terminated */
        if (0 == (needle_len = strlen(needle)))
                return (char *)haystack;

        for (i=0; i<=(int)(len-needle_len); i++)
        {
                if ((haystack[0] == needle[0]) &&
                        (0 == strncmp(haystack, needle, needle_len)))
                        return (char *)haystack;

                haystack++;
        }
        return NULL;
}
void display(const char *str_display)
{
  LCD_GLASS_Clear();
  LCD_GLASS_DisplayString((unsigned char*)str_display);
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
