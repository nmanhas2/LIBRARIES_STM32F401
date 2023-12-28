/**
 ******************************************************************************
 * @file           : uart.c
 * @author         : Nubal Manhas
 * @brief          : Main c file for UART library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to define functions that will support UART
 * for the STM32F01RE MCU
 *
 ******************************************************************************
 */
#include "gpio.h"
#include "uart.h"
#include <math.h>
#include <stdint.h>

void uart_enable_clk(UART_CONFIG UART);
void uart_baudrate(UART_CONFIG UART,uint32_t bd);
void uart_cr1_enable(UART_CONFIG UART);

/*
 * Function for initializing UART
 *
 * Takes a UART struct containing port, pin numbers for RX/TX
 * and USART number. Also takes baudrate.
 */
void uart_init(UART_CONFIG UART, uint32_t baudrate)
{
	/*
	 * Configure GPIO registers
	 *
	 * Check if TX/RX need to be configured on a pin
	 * then set alternate function mode and init the
	 * GPIO pin that was specified
	 *
	 * Table 9. in the datasheet shows that AF08 is
	 * for USART6, and the other two are AF07
	*/
	if(UART.TX != USARTX_TX_NONE)
	{
		GPIOx_PIN_CONFIG TX_Pin;

		if(UART.USART == USART6)
		{
			TX_Pin.ALT_FUNC = GPIOx_ALT_AF8;
		} else
		{
			TX_Pin.ALT_FUNC = GPIOx_ALT_AF7;
		}

		TX_Pin.PIN_MODE = GPIOx_PIN_ALTERNATE;

		TX_Pin.PIN_NUM = UART.TX;

		gpio_init(UART.PORT, TX_Pin);
	}

	if(UART.RX != USARTX_RX_NONE)
	{
		GPIOx_PIN_CONFIG RX_Pin;

		if(UART.USART == USART6)
		{
			RX_Pin.ALT_FUNC = GPIOx_ALT_AF8;
		} else
		{
			RX_Pin.ALT_FUNC = GPIOx_ALT_AF7;
		}

		RX_Pin.PIN_MODE = GPIOx_PIN_ALTERNATE;

		RX_Pin.PIN_NUM = UART.RX;

		gpio_init(UART.PORT, RX_Pin);
	}

	//enable USART on APB1/APB2 clock bus
	uart_enable_clk(UART);

	//set baudrate in BRR register
	uart_baudrate(UART, baudrate);

	//enable uart/tx/rx in CR1 register
	uart_cr1_enable(UART);
}

/*
 * Function for enabling clock access depending on the USART
 *
 * Based on Fig. 3 in Datasheet, APB1 = USART2, APB2 = USART1/USART6
 */
void uart_enable_clk(UART_CONFIG UART)
{
	//Set USART enable bit in peripheral clock enable
	//register for clock access (6.3.11/6.3.12 in Ref Manual)
	//APB1 = 42MHz
	//APB2 = 84MHz
	if(UART.USART == USART2)
	{
		RCC->APB1ENR |= USART2_EN;
	}
	if(UART.USART == USART1)
	{
		RCC->APB2ENR |= USART1_EN;
	}
	if(UART.USART == USART6)
	{
		RCC->APB2ENR |= USART6_EN;
	}

}

/*
 * Function to configure USART baudrate
 *
 * Based on the formula in 19.3.4 in Ref Manual
 */
void uart_baudrate(UART_CONFIG UART,uint32_t bd)
{
	double USARTDIV = 0; //stores number to be coded to USART_BRR register
	int i_frac = 0;
	int mantissa = 0;
	double frac = 0;
	double fracDiff = 0;
	if(UART.USART == USART2)
	{
		//16MHz = SY
		USARTDIV = ((double)APB1_FREQ)/((double)(16*bd));
	} else{
		//APB2 = 84MHz
		USARTDIV = APB2_FREQ/(16*bd);
	}
	mantissa = (int)USARTDIV; //getting the DIV_MANTISSA part
	frac = (USARTDIV - mantissa) * 16; //getting DIV_Fraction part

	//DIV_Fraction is 4 bits hex, so anything higher than 15d (0xF) is an overflow
	if(frac > 15)
	{
		//carry over the overflow to the mantissa
		mantissa += 1;
		frac = 0;
	}

	//need to check decimal for rounding purposes, since round() doesn't seem to work
	fracDiff = frac - ((int) frac);

	if(fracDiff >= 0.5)
	{
		frac++;
	}

	i_frac = (int)(frac); //want to round down to int

	//set baudrate in USART_BRR
	//19.6.3 in Ref Manual
	UART.USART->BRR |= (i_frac<<0);
	UART.USART->BRR |= (mantissa<<4);
}

/*
 * Function to enable RX/TX/USART in Control Register 1
 *
 * 19.6.4 in Ref Manual
 */
void uart_cr1_enable(UART_CONFIG UART)
{
	if(UART.RX != USARTX_RX_NONE)
	{
		UART.USART->CR1 |= USART_CR1_RXEN;
	}

	if(UART.TX != USARTX_TX_NONE)
	{
		UART.USART->CR1 |= USART_CR1_TXEN;
	}

	UART.USART->CR1 |= USART_CR1_UE;
}

/*
 * Function to write to transmit data over USART
 *
 * 19.6.1/19.6.2
 */
void uart_write(USART_TypeDef* USART, int ch)
{
	while(!(USART->SR & USART_SR_TXE)); //wait until transmit data register is empty with the status register
	USART->DR = (ch & 0xFF); //writing data value to the data register (8 bits)
}
/*
 * Function to write strings over USART
 *
 * 19.6.1 in Ref Manual
 */
void uart_write_string(USART_TypeDef* USART, char* str)
{
	while(*str)
	{
		uart_write(USART, (*str++));
	}
}

/*
 * Function to read data register, when it's status is not empty
 *
 * 19.6.1/19.6.2 in Ref Manual
 */
char uart_read(USART_TypeDef* USART)
{
	while(!(USART->SR & USART_SR_RXNE)){}
	return USART->DR;
}
