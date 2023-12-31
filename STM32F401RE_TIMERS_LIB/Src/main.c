/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing Timer library for STM32F401RE, by using a delay function, checking,
 * count changes, and USART2
 *
 ******************************************************************************
*/
#include "stm32f4xx.h"
#include "uart.h"
#include "timer.h"
#include <stdio.h>
#include <stdint.h>

/* TESTS: */
//#define DOWN_TEST //un-comment this to test a 1Hz down counter
//#define UP_TEST //un-comment this to test a 1Hz up counter
//#define DELAY_TEST //un-comment this to test 1 second delay with an up counter

UART_CONFIG UART2;
TIM2_5_CONFIG TMR2;

int main(void)
{
	//UART for 9600 baudrate, PA3 as RX, PA2 as TX for USART2
	UART2.PORT = GPIOA;
	UART2.RX = USART2_RX_PA3;
	UART2.TX = USART2_TX_PA2;
	UART2.USART = USART2;

	//Initializing 1Hz timer
	TMR2.PERIOD = 10000;
	TMR2.PRESCALER = 1600;
	TMR2.TMR = TIM2;

	uart_init(UART2, 9600); //init uart at 9600 baud

	#ifdef DOWN_TEST
		TMR2.COUNTER_MODE = TIM2_5_DOWN;
		tim2_5_init(TMR2);
		while(1)
		{
			char s[50]; //buffer to send number of seconds over USART2
			//reset seconds count
			sprintf(s,"count: %i\n\r", (int)(TMR2.TMR->CNT));//put count into buffer
			uart_write_string(UART2.USART, s); //print to USART2
		}
	#endif

	#ifdef UP_TEST
		TMR2.COUNTER_MODE = TIM2_5_UP;
		tim2_5_init(TMR2);
		while(1)
		{
			char s[50]; //buffer to send number of seconds over USART2
			//reset seconds count
			sprintf(s,"count: %i\n\r", (int)(TMR2.TMR->CNT));//put count into buffer
			uart_write_string(UART2.USART, s); //print to USART2
		}
	#endif

	#ifdef DELAY_TEST
		TMR2.COUNTER_MODE = TIM2_5_UP;
		tim2_5_init(TMR2);
		while(1)
		{
			char s[50]; //buffer to send number of seconds over USART2
			//reset seconds count
			sprintf(s,"1 second past");//put message into buffer
			tim2_5_delay(TMR2); //1 second delay
			uart_write_string(UART2.USART, s); //print to USART2
		}
	#endif
}

