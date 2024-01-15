/**
 ******************************************************************************
 * @file           : uart.h
 * @author         : Nubal Manhas
 * @brief          : Header file for UART library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to declare functions, enums, and structs, that
 * will support the creation of a library for UART functionality for the
 * STM32F01RE MCU
 *
 ******************************************************************************
 */
#ifndef UART_H_
#define UART_H_
#include "stm32f401xe.h"

//bits to enable USARTx on APB1/APB2 bus
//6.3.11/6.3.12 in Ref Manual
#define USART2_EN	     (1U<<17) //APB1
#define USART1_EN	     (1U<<4) //APB2
#define USART6_EN	     (1U<<5) //APB2

//default clock speed for APB1 and APB2 in Hz
#define APB1_FREQ	     16000000
#define APB2_FREQ	     16000000 //not sure about this one?

//bits for enabling USART/TX/RX in CR1
//19.6.4 in Ref Manual
#define USART_CR1_TXEN    (1U<<3)
#define USART_CR1_RXEN    (1U<<2)

/*
 * USARTx RX can be configured to different
 * pins for each USARTx
 *
 * Table.9 in Datasheet for mapping
 */
typedef enum
{
	USART2_RX_PA3 = 3,
	USART1_RX_PA10 = 10,
	USART6_RX_PA12 = 12,
	USART1_RX_PB7 = 7,
	USART6_RX_PC7 = 7,
	USART2_RX_PD6 = 6,
	USARTX_RX_NONE = -1
}UART_RX_PIN;

/*
 * USARTx TX can be configured to different
 * pins for each USARTx
 *
 * Table.9 in Datasheet for mapping
 */
typedef enum
{
	USART2_TX_PA2 = 2,
	USART1_TX_PA9 = 9,
	USART6_TX_PA11 = 11,
	USART1_TX_PB6 = 6,
	USART6_TX_PC6 = 6,
	USART2_TX_PD5 = 5,
	USARTX_TX_NONE = -1
}UART_TX_PIN;

/*
 * Struct to help configure USART on the
 * specified RX/TX pin, PORT, and USART number
 */
typedef struct
{
	UART_TX_PIN TX;
	UART_RX_PIN RX;
	USART_TypeDef* USART;
	GPIO_TypeDef* PORT;
}UART_CONFIG;

//USART init function
void uart_init(UART_CONFIG UART, uint32_t baudrate);

//function to write to transmit data over USART
void uart_write(USART_TypeDef* USART, int ch);

//function to write a full string over USART
void uart_write_string(USART_TypeDef* USART, char* str);

//function to read data register, when it's status is not empty
char uart_read(USART_TypeDef* USART);
#endif /* UART_H_ */
