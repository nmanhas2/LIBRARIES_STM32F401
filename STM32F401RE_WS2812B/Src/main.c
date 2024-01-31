/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing WS2812B Addressable LED strip
 *
 * Datasheet for WS2812B: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
 *
 ******************************************************************************
*/
#include "stm32f4xx.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "systick.h"
#include <stdio.h>
#include <stdint.h>

#define WS2812B_TEST

//baudrate for UART
const int UART_BAUDRATE = 115200;

//UART with PA3 as RX, PA2 as TX for USART2
UART_CONFIG UART2 = {
					 USART2_RX_PA3,
					 USART2_TX_PA2,
					 USART2,
					 GPIOA
					};


//char buffer for uart transmitting
char str[30];
int main(void)
{
	#ifdef WS2812B_TEST

	//init uart at 115200 baud
	uart_init(UART2.USART, UART_BAUDRATE);

		while(1)
		{

		}
	#endif
}
