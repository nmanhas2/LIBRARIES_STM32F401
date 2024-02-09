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

//to configure the Timers, the following formula is used:
//Desired Frequency = SYSCLK_FREQ / (PRESCALER+PERIOD),
//and a 1.25uS (800KHz) frequency is required based on p. 4 in the
//WS2812B datasheet
const int TMR3_PRESCALER = 0;
const int TMR3_PERIOD = 20;

//UART with PA3 as RX, PA2 as TX for USART2
UART_CONFIG UART2 = {
					 USART2_RX_PA3,
					 USART2_TX_PA2,
					 USART2,
					 GPIOA
					};

/*
 * Configuring TIM3 to count to 1.25uS (16MHz system clk)
 *
 * This is because the data transfer time is 1.25uS for the
 * LED PWM (p. 4 in WS2812B datasheet)
 *
 * 16MHz/800KHz (1.25uS) = 20 = (prescaler + period)
 * so setting a prescaler for the clock isn't neeeded,
 * just a period of 20 at 16MHz is good enough.
 */
TIM2_5_CONFIG TMR3 = {
					  TIM3,
					  TIM2_5_UP,
					  TMR3_PRESCALER,
					  TMR3_PERIOD
					 };

//char buffer for uart transmitting
char str[30];
int main(void)
{
	#ifdef WS2812B_TEST

	//init uart2 at 115200 baud
	uart_init(UART2.USART, UART_BAUDRATE);

		while(1)
		{

		}
	#endif
}
