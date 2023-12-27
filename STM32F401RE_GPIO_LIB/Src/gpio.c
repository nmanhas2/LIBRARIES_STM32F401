/**
 ******************************************************************************
 * @file           : gpio.c
 * @author         : Nubal Manhas
 * @brief          : Main c file for GPIO library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to define functions that will support GPIO
 * for the STM32F01RE MCU
 *
 ******************************************************************************
 */
#include "gpio.h"
#include "stm32f401xe.h"

//function to set pin mode in mode register
void gpio_set_moder(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin);

/*
 * Based on Fig. 3 in the datasheet, AHB1 Bus is where clock access
 * can be gained for all GPIO ports, each port has a corresponding
 * bit to enable it within the AHB1 bus
 *
 * The pin mode of the pin is also set in this function, which depends
 * on the bits set within the MODER register (8.4.1 in Ref Manual)
 *
 * gpioX = GPIO_TypeDef* (ie. pointer to GPIOA, GPIOB, etc, this is defined in stm32f401xe.h)
 * GPIOx_PIN_CONFIG = struct containing enums that have the bits needed for pin number and
 * 					  pin mode
 */
void gpio_init (GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin)
{
	//enable clock access to GPIOx w/ AHB1
	if(gpioX == GPIOA)
	{
		RCC->AHB1ENR |= AHB1ENR_GPIOA_EN;
	} else if(gpioX == GPIOB){
		RCC->AHB1ENR |= AHB1ENR_GPIOB_EN;
	}else if(gpioX == GPIOC){
		RCC->AHB1ENR |= AHB1ENR_GPIOC_EN;
	}else if(gpioX == GPIOD){
		RCC->AHB1ENR |= AHB1ENR_GPIOD_EN;
	}else if(gpioX == GPIOE){
		RCC->AHB1ENR |= AHB1ENR_GPIOE_EN;
	}else if(gpioX == GPIOH){
		RCC->AHB1ENR |= AHB1ENR_GPIOH_EN;
	} else{
		return;
	}

	//set pin mode
	gpio_set_moder(gpioX, pin);
}

/*
 * Configuring pin mode
 *
 * There are four modes: Output, Input, Alternate Function, and Analog Mode
 * 8.4.1 in Ref Manual
 */
void gpio_set_moder(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin)
{
	//clear pin
	gpioX->MODER &= ~(0x3 << (2*pin.PIN_NUM));
	//set pin mode, need to multiply by 2 because MODER is 32bit and each pin has 2 bits
	//that correspond to it's mode. PA15's mode for example will be set on bit 30 and 31
	gpioX->MODER |= (pin.PIN_MODE << (2*pin.PIN_NUM));
}

/*
 * Toggle an output pin
 *
 * Each of the 16 pins corresponds to 1 bit in the output register
 * 8.4.6 in Ref Manual
 */
void gpio_toggle_output(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin)
{
	gpioX->ODR ^= (GPIOx_SET_OUTPUT<<pin.PIN_NUM);
}

/*
 * Write to an output pin
 *
 * 1 = high
 * not 1 = low
 *
 * 8.4.6 in Ref Manual
 */
void gpio_write_output(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin, uint8_t val)
{
	if(val == GPIOx_SET_OUTPUT )
	{
		gpioX->ODR |= (GPIOx_SET_OUTPUT<<pin.PIN_NUM);
	} else{
		gpioX->ODR &= ~(GPIOx_SET_OUTPUT<<pin.PIN_NUM);
	}
}

/*
 * Read a value from an input pin
 *
 * return 1 or 0 depending on what is read
 *
 * 8.4.5 in Ref Manual
 *
 */
uint8_t gpio_input_read(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin)
{
	//read the value at the pin number
	//if we want to read when PA5 is high (0000 0000 0010 0000) for example:
	//0000 0000 0010 0000 >> 5 results in 1, then we just need to & 1 to confirm
	//if it's high or not
	return ((gpioX->IDR >> pin.PIN_NUM) & 1);
}

/*
 * Atomic bit set/reset for the ODR bits using BSRR register
 *
 * 8.4.7 in Ref Manual
 */
void gpio_output_bit_setreset(GPIO_TypeDef* gpioX, GPIOx_PIN_CONFIG pin, uint8_t val)
{
	if(val == GPIOx_BSRR_SET)
	{
		gpioX ->BSRR = (GPIOx_SET_OUTPUT<<pin.PIN_NUM);
	}

	if(val == GPIOx_BSRR_RESET)
	{
		gpioX ->BSRR = (GPIOx_SET_OUTPUT<<(pin.PIN_NUM + 16 ));
	}
}
