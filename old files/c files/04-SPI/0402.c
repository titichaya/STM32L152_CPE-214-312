#include "stm32l1xx.h"
unsigned int status,readData;
void delay(unsigned int x){ while(x--); }
void init_SPI(void);
void transmit_data(SPI_TypeDef* SPIx ,unsigned int data);
unsigned int receive_data(SPI_TypeDef* SPIx);
unsigned int read_statue(void);
void write_data (SPI_TypeDef* SPIx ,unsigned int data ,unsigned int address);
unsigned int read_data (SPI_TypeDef* SPIx, unsigned int address );

int main()
{
	init_SPI();
	status = read_statue();
	write_data(SPI2,0x35,0x214);
	readData = read_data(SPI2,0x214);
}

void init_SPI(void)
{
	SPI_InitTypeDef SPI_2;
	GPIO_InitTypeDef PA,PB;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	
	GPIO_StructInit (&PA);
	PA.GPIO_Mode = GPIO_Mode_OUT;
	PA.GPIO_Pin = GPIO_Pin_0;
	PA.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&PA);
	
	GPIO_StructInit(&PB);
  PB.GPIO_Mode = GPIO_Mode_AF;
	PB.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	PB.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&PB);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	
	SPI_SSOutputCmd(SPI2,ENABLE);

	SPI_StructInit (&SPI_2);
	SPI_2.SPI_Mode = SPI_Mode_Master;
	SPI_2.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_2.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_2.SPI_CPOL = SPI_CPOL_Low;
	SPI_2.SPI_DataSize = SPI_DataSize_8b ;
	SPI_2.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_2.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI2,&SPI_2);
	SPI_Cmd(SPI2,ENABLE);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	delay(10000);
}

void transmit_data(SPI_TypeDef* SPIx ,unsigned int data)
{
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY)==1);
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_TXE)==0);
	SPI_I2S_SendData(SPIx,data);
}
unsigned int receive_data(SPI_TypeDef* SPIx)
{
	unsigned int receive_data;
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_RXNE)==0);
	receive_data = SPI_I2S_ReceiveData(SPIx);
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY)==1);
	return receive_data;
}
unsigned int read_statue(void)
{
	unsigned int data;
	delay(10000);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	delay(10000);
	
	transmit_data(SPI2,0x5);
	
	//by reading data, RXNE is cleared for prior data
	data = receive_data(SPI2);
	transmit_data(SPI2,0xFF);
	//read the right data in sequence
	data = receive_data(SPI2);
	
	delay(10000);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	delay(10000);
	
	
	return data;
}
void write_data (SPI_TypeDef* SPIx ,unsigned int data ,unsigned int address)
{
	delay(10000);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	delay(10000);
	
	transmit_data(SPIx,0x2);
	transmit_data(SPIx,address>>8);
	transmit_data(SPIx,address & 0xFF);
	transmit_data(SPIx,data);
	
	delay(10000);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	delay(10000);
	
}
unsigned int read_data (SPI_TypeDef* SPIx, unsigned int address )
{
	int a;
	delay(10000);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	delay(10000);
	
	transmit_data(SPIx,0x3);
	transmit_data(SPIx,address>>8);
	transmit_data(SPIx,address & 0xFF);
	
	//by reading data, RXNE is cleared for prior data
	a = receive_data(SPI2);
	transmit_data(SPI2,0xFF);
	//read the right data in sequence
	a = receive_data(SPI2);
	
	
	delay(10000);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	delay(10000);
	
	return a;
}