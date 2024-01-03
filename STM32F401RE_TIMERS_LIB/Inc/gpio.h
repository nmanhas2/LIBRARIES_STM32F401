/**
 ******************************************************************************
 * @file           : gpio.h
 * @author         : Nubal Manhas
 * @brief          : Header file for GPIO library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to declare functions, enums, and structs, that
 * will support the creation of a library for GPIO functionality for the
 * STM32F01RE MCU
 *
 ******************************************************************************
 */

#ifndef GPIO_H_
#define GPIO_H_
#include "stm32f401xe.h"

/*
 * Enumeration representation of bits needed
 * for enabling GPIOx on AHB1 bus to gain
 * clock access
 *
 * The pin masks for these are defined in the
 * stm32f401xe.h file already (CMSIS)
 *
 * 6.3.9 in Ref Manual
*/
#define AHB1ENR_GPIOA_EN  	(RCC_AHB1ENR_GPIOAEN_Msk)
#define	AHB1ENR_GPIOB_EN  	(RCC_AHB1ENR_GPIOBEN_Msk)
#define	AHB1ENR_GPIOC_EN  	(RCC_AHB1ENR_GPIOCEN_Msk)
#define	AHB1ENR_GPIOD_EN  	(RCC_AHB1ENR_GPIODEN_Msk)
#define	AHB1ENR_GPIOE_EN  	(RCC_AHB1ENR_GPIOEEN_Msk)
#define	AHB1ENR_GPIOH_EN  	(RCC_AHB1ENR_GPIOHEN_Msk)

//used to set a GPIO pins output to high
#define GPIOx_SET_OUTPUT    1

/* Used to distinguish between setting/resetting within
 * the BSRR register
 *
 *  8.4.6 in Ref Manual
*/
#define GPIOx_BSRR_RESET	0
#define GPIOx_BSRR_SET	    1

/*
 * 2 different output types are available:
 *
 * Output push-pull = 0
 * Output open-drain = 1
 *
 * 8.4.2 in Ref Manual
 */
typedef enum
{
	GPIOx_OTYPER_PUSH_PULL,
	GPIOx_OTYPER_OPEN_DRAIN,
	GPIOx_OTYPER_NONE
}GPIOx_OTYPER_MODE;

/*
 * 3 different internal resistor modes available:
 *
 * No pull-up or pull-down = 00
 * Pull-up = 01
 * Pull-down = 10
 *
 * 8.4.4 in Ref Manual
 */
typedef enum
{
	GPIOx_PUPDR_NONE,
	GPIOx_PUPDR_PULL_UP,
	GPIOx_PUPDR_PULL_DOWN
}GPIOx_PUPDR_MODE;


/*
 * Each port has 16 configurable I/O's
 *
 * 8.2 in Ref Manual
 */
typedef enum
{
	GPIOx_PIN_0,
	GPIOx_PIN_1,
	GPIOx_PIN_2,
	GPIOx_PIN_3,
	GPIOx_PIN_4,
	GPIOx_PIN_5,
	GPIOx_PIN_6,
	GPIOx_PIN_7,
	GPIOx_PIN_8,
	GPIOx_PIN_9,
	GPIOx_PIN_10,
	GPIOx_PIN_11,
	GPIOx_PIN_12,
	GPIOx_PIN_13,
	GPIOx_PIN_14,
	GPIOx_PIN_15
}GPIOx_PIN_NUM;

/*
 * Four different pin modes are available:
 *
 * Input = 00
 * Output = 01
 * Alternate Function = 10
 * Analog: 11
 *
 * 8.4.1 in Ref Manual
 */
typedef enum
{
	GPIOx_PIN_INPUT,
	GPIOx_PIN_OUTPUT,
	GPIOx_PIN_ALTERNATE,
	GPIOx_PIN_ANALOG,
}GPIOx_PIN_MODE;

/*
 * There are 16 alternate functions to select from for
 * each GPIO, some are mapped to a function and some are not.
 * Each constant within this enum represent one of the 16 bits
 * that correspond to one of the 16 alternate functions that
 * are available. These are set within the AFRH/AFRL registers
 *
 * Check Table 9 in the datasheet to see the mapping of each
 * function, and 8.4.9/8.4.10 in Ref Manual to see the registers
 */
typedef enum
{
	GPIOx_ALT_AF0,
	GPIOx_ALT_AF1,
	GPIOx_ALT_AF2,
	GPIOx_ALT_AF3,
	GPIOx_ALT_AF4,
	GPIOx_ALT_AF5,
	GPIOx_ALT_AF6,
	GPIOx_ALT_AF7,
	GPIOx_ALT_AF8,
	GPIOx_ALT_AF9,
	GPIOx_ALT_AF10,
	GPIOx_ALT_AF11,
	GPIOx_ALT_AF12,
	GPIOx_ALT_AF13,
	GPIOx_ALT_AF14,
	GPIOx_ALT_AF15
}GPIOx_ALT_FUNC;

/*
 * Struct to configure a GPIO pin and
 * it's mode
 */
typedef struct{
	GPIOx_PIN_NUM PIN_NUM;
	GPIOx_PIN_MODE PIN_MODE;
	GPIOx_ALT_FUNC ALT_FUNC;
	GPIOx_PUPDR_MODE PUPDR_MODE;
	GPIOx_OTYPER_MODE OTYPER_MODE;
}GPIOx_PIN_CONFIG;

//toggling output
void gpio_toggle_output(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin);

//init function
void gpio_init (GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin);

//writing to an output pin
void gpio_write_output(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin, uint8_t val);

//reading value from an input pin
uint8_t gpio_input_read(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin);

//function to utilize bit set/reset register for outputs
void gpio_output_bit_setreset(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin, uint8_t val);
#endif /* GPIO_H_ */
