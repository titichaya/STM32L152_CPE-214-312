#include "NUC1xx.h"
#include "DrvSYS.h"
#include "DrvGPIO.h"
#include "LCD_Driver.h"
#include "ScanKey.h"
#include "Seven_Segment.h"
#include "stdio.h"
#include <stdlib.h>

int rand (void);         // function for random

int number=0,data=0,state=0,i=0,value=9;
char diplay[]="";

int main() {
  DrvSYS_Open(48000000);                  //set clock frequency 48 MHz
  OpenKeyPad();                           // Open port for Key pad 
  DrvGPIO_Open (E_GPB, 11, E_IO_OUTPUT);  //activate Buzzer
  DrvGPIO_Open (E_GPA, 13, E_IO_OUTPUT);	//activate RGB LED Green
  DrvGPIO_ClrBit(E_GPD, 14); 				      //activate LCD Background
  DrvGPIO_Open (E_GPC, 12, E_IO_OUTPUT);	//activate LED1
	DrvGPIO_Open (E_GPC, 13, E_IO_OUTPUT);	//activate LED2
	DrvGPIO_Open (E_GPC, 14, E_IO_OUTPUT);	//activate LED3
	DrvGPIO_Open (E_GPC, 15, E_IO_OUTPUT);	//activate LED4
	Initial_pannel();                       // initialize LCD panel
	clr_all_pannal();                       // clear LCD panel	 	
	print_lcd(0, "     Nuvoton    "); 
	print_lcd(1, "     NUC 140    "); 
	print_lcd(2, "                "); 
	print_lcd(3, "CPE-312-TNI-CE07"); 
  while(1) {
    number = Scankey();                   //Store value from Key pad
    if(number != 0 && state == 0) {       //State
      data = number;                      //Store value to data
      sprintf(diplay,"Number : %d      ",data);
      print_lcd(2, diplay);               //Display string & value in LCD line 1
      if(i>=4)
        i=0;
      value = rand()%10;            // random value 0 - 9
      close_seven_segment();        // Clear 7-Segment
      show_seven_segment(i,value);  // Display value (random) in i digit
      i++;                          // digit ++
      DrvGPIO_ClrBit(E_GPB, 11);    // Buzzer ON
      DrvSYS_Delay(1000000);        // Delay 1000000 us
      state=1;
    }
    else if(number == 0 && state == 1){
      DrvGPIO_SetBit(E_GPB, 11);    // Buzzer OFF
      state = 0;
    }
    if(value == data) {             // if keypad value = random value
      DrvGPIO_ClrBit(E_GPA, 13);    // RGB LED Green ON
      print_lcd(2, "  YOU  WIN !!!  ");
    }
    else
      DrvGPIO_SetBit(E_GPA, 13);
    
    if(data%2 == 0)               //if data is even
    {
      DrvGPIO_ClrBit(E_GPC, 12);  //LED1 ON
      DrvGPIO_ClrBit(E_GPC, 14);  //LED3 ON
      DrvGPIO_SetBit(E_GPC, 13);  //LED2 OFF
      DrvGPIO_SetBit(E_GPC, 15);  //LED4 OFF
    }
    else{
      DrvGPIO_SetBit(E_GPC, 12);  //LED1 OFF
      DrvGPIO_SetBit(E_GPC, 14);  //LED3 OFF
      DrvGPIO_ClrBit(E_GPC, 13);  //LED2 ON
      DrvGPIO_ClrBit(E_GPC, 15);  //LED4 ON
    }
  }
}
