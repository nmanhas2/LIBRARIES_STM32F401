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
//#define OUTPUT_TEST //un-comment this to test output compare on PA5 (LED2 should toggle every second)
//#define INPUT_TEST //un-comment this to test input capture, wire PA5 (output compare) to PA6 (input capture)

UART_CONFIG UART2; //struct to configure UART2
TIM2_5_CONFIG TMR2; //struct to configure TIM2 (this will be used for output compare as well)
TIM2_5_CAPTURE_COMPARE_CONFIG CAPTURE_COMPARE; //struct to configure output compare
TIM2_5_CAPTURE_COMPARE_CONFIG COMPARE_CAPTURE; //struct to configure input capture

int timestamp = 0; //used to store input capture counter, global allows one to use live expressions in the debugger to view

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

	//init uart at 9600 baud
	uart_init(UART2, 115200);

	#ifdef DOWN_TEST
		//down counter
		TMR2.COUNTER_MODE = TIM2_5_DOWN;

		//init + enable timer
		tim2_5_init_enable(TMR2);
		while(1)
		{
			char s[50]; //buffer to send count over USART2

			sprintf(s,"count: %i\n\r", (int)(TMR2.TMR->CNT));//put count into buffer

			uart_write_string(UART2.USART, s); //print to USART2
		}
	#endif

	#ifdef UP_TEST
		//up counter
		TMR2.COUNTER_MODE = TIM2_5_UP;

		//init + enable timer
		tim2_5_init_enable(TMR2);
		while(1)
		{
			char s[50];

			sprintf(s,"count: %i\n\r", (int)(TMR2.TMR->CNT));//put count into buffer

			uart_write_string(UART2.USART, s); //print to USART2
		}
	#endif

	#ifdef DELAY_TEST
		TMR2.COUNTER_MODE = TIM2_5_UP;

		tim2_5_init_enable(TMR2);
		while(1)
		{
			char s[50]; //buffer to send number of seconds over USART2

			sprintf(s,"1 second past\n\r");//put message into buffer

			tim2_5_delay(TMR2); //1 second delay

			uart_write_string(UART2.USART, s); //print to USART2
		}
	#endif

	#ifdef OUTPUT_TEST
		//up counter
		TMR2.COUNTER_MODE = TIM2_5_UP;

		//Initializing compare mode pin
		CAPTURE_COMPARE.CAPTURE_COMPARE_MODE = TIM2_5_OUTPUT;
		CAPTURE_COMPARE.CHANNEL = TIM2_5_CH1;
		CAPTURE_COMPARE.OUTPUT_MODE = TIM2_5_TOGGLE;
		CAPTURE_COMPARE.PIN_NUM = TIM2_CH1_PA5;
		CAPTURE_COMPARE.PORT = GPIOA;

		//init + enable output compare
		tim2_5_init_capture_compare(TMR2,CAPTURE_COMPARE);

		while(1)
		{
		}
	#endif

	#ifdef INPUT_TEST
		//timer for input capture
		TIM2_5_CONFIG TMR3;
		TMR3.TMR = TIM3;
		TMR3.PERIOD = 65535; //max value
		TMR3.PRESCALER = 16000;
		TMR3.COUNTER_MODE = TIM2_5_UP;

		//up counter for output compares
		TMR2.COUNTER_MODE = TIM2_5_UP;

		//Initializing output compare mode pin
		CAPTURE_COMPARE.CAPTURE_COMPARE_MODE = TIM2_5_OUTPUT;
		CAPTURE_COMPARE.CHANNEL = TIM2_5_CH1;
		CAPTURE_COMPARE.OUTPUT_MODE = TIM2_5_TOGGLE;
		CAPTURE_COMPARE.PIN_NUM = TIM2_CH1_PA5;
		CAPTURE_COMPARE.PORT = GPIOA;

		//Initializing input capture mode pin
		COMPARE_CAPTURE.CAPTURE_COMPARE_MODE = TIM2_5_INPUT;
		COMPARE_CAPTURE.CHANNEL = TIM2_5_CH1;
		COMPARE_CAPTURE.OUTPUT_MODE = TIM2_5_NONE;
		COMPARE_CAPTURE.PIN_NUM = TIM3_CH1_PA6;
		COMPARE_CAPTURE.PORT = GPIOA;

		//init + enable output compare
		tim2_5_init_capture_compare(TMR2,CAPTURE_COMPARE);

		//init + enable input capture
		tim2_5_init_capture_compare(TMR3,COMPARE_CAPTURE);
		while(1)
		{
			timestamp = tim2_5_capture_read(TMR3, COMPARE_CAPTURE); //store count, should go up by 2000 every time PA5 goes high

			char s[50]; //buffer to send counter value over USART2

			sprintf(s,"counter value: %i\n\r", timestamp);//put count into buffer

			uart_write_string(UART2.USART, s); //print to USART2
		}
	#endif
}

