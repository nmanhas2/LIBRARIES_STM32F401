/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing SYSTICK library for STM32F401RE, by using a delay function and USART2
 *
 ******************************************************************************
*/
#include "stm32f4xx.h"
#include "systick.h"
#include "uart.h"
#include <stdio.h>
#include <stdint.h>

/* TESTS: */
#define DELAY_TEST //un-comment this to test a 1000ms (1 second) delay over USART2

UART_CONFIG UART2;
int main(void)
{
	//UART for 9600 baudrate, PA3 as RX, PA2 as TX for USART2
	UART2.PORT = GPIOA;
	UART2.RX = USART2_RX_PA3;
	UART2.TX = USART2_TX_PA2;
	UART2.USART = USART2;

	uart_init(UART2, 9600); //init uart at 9600 baud

	#ifdef DELAY_TEST
		int i = 1; //variable to hold number of seconds
		while(1)
		{
			char s[50]; //buffer to send number of seconds over USART2
			//reset seconds count
			if(i > 10)
			{
				i = 1;
			}
			sprintf(s,"%i second(s) have past\n\r", i);//put number of seconds into buffer
			systickDelayMS(1000); //delay
			uart_write_string(UART2.USART, s); //print to USART2
			i++;//increment number of seconds
		}
	#endif
}

