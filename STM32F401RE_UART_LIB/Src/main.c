/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing USART library for STM32F401RE, using USART2 PA2 for TX, PA3 for RX
 *
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "uart.h"
#include <stdio.h>

/* TESTS: */
//#define WRITE_TEST //un-comment this to test writing over USART2
//#define READ_TEST //un-comment this to test reading over USART2, PA5 should go high in response to '1'

UART_CONFIG UART2;
int main(void)
{
	//initializing UART for 115200 baudrate, PA3 as RX, PA2 as TX for USART2
	UART2.PORT = GPIOA;
	UART2.RX = USART2_RX_PA3;
	UART2.TX = USART2_TX_PA2;
	UART2.USART = USART2;

	uart_init(UART2, 115200);

	#ifdef WRITE_TEST

		while(1)
		{
			uart_write_string(UART2.USART, "HELLO\n\r");
		}
	#endif

	#ifdef READ_TEST
		//configuring PA5 as output (LED2 on the DEV board)
		GPIOx_PIN_CONFIG LED2;
		LED2.PIN_MODE = GPIOx_PIN_OUTPUT;
		LED2.PIN_NUM = GPIOx_PIN_5;

		gpio_init(GPIOA,LED2);

		while(1)
		{
			//if '1' is read, set the pin to high, otherwise low
			if(uart_read(UART2.USART) == '1')
			{
				gpio_write_output(GPIOA, LED2,1);
			} else
			{
				gpio_write_output(GPIOA, LED2,0);
			}
		}
	#endif
}

