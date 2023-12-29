/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing ADC library for STM32F401RE, using Channel 1 (PA1)
 *
 ******************************************************************************
*/
#include "stm32f4xx.h"
#include "gpio.h"
#include "uart.h"
#include "adc.h"
#include <stdio.h>
#include <stdint.h>

/* TESTS: */
//#define SINGLE_TEST //un-comment this to test single conversion for ADC
//#define CONTINUOUS_TEST //un-comment this to test continuous conversion for ADC

UART_CONFIG UART2;
ADC_CONFIG adc;
int main(void)
{
	//UART for 115200 baudrate, PA3 as RX, PA2 as TX for USART2
	UART2.PORT = GPIOA;
	UART2.RX = USART2_RX_PA3;
	UART2.TX = USART2_TX_PA2;
	UART2.USART = USART2;

	//ADC1 on channel 1 (PA1), SQ1, 1 conversion (0 length)
	adc.CHANNEL = ADC_CH1;
	adc.SEQUENCE = ADC_SQ1;
	adc.SEQ_LENGTH = 0;

	uart_init(UART2, 115200); //init uart at 115200 baud

	adc_init(adc); //init adc

	uint32_t val; //storing ADC value

	#ifdef SINGLE_TEST

		adc_start_single(); //start single conversion

		while(1)
		{
			char str[100]; //buffer for USART

			//start new conversion
			adc_start_single();

			val = adc_read(); //read ADC value

			sprintf(str, "ADC = %d \n\r",(int) val); //sprintf to buffer with the ADC value read

			uart_write_string(UART2.USART, str); //print to USART2
		}
	#endif

	#ifdef CONTINUOUS_TEST

		adc_start_continuous(); //start continuous conversion

		while(1)
		{
			char str[100];

			val = adc_read();

			sprintf(str, "ADC = %d \n\r",(int) val);

			uart_write_string(UART2.USART, str);
		}
	#endif
}

