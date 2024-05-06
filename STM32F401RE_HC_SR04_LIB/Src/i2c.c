/**
 ******************************************************************************
 * @file           : i2c.c
 * @author         : Nubal Manhas
 * @brief          : Main c file for I2C library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to define functions that will support I2C
 * for the STM32F01RE MCU
 *
 ******************************************************************************
 */
#include "gpio.h"
#include "i2c.h"

//clock control value for the master mode clock configuration
//see i2c_init() to see the math behind it
#define CRR_VAL 			80

//max rise time for SCL
//see i2c_init() to see the math behind it
#define MAX_TRISE			17

//maximum allowed peripheral clock frequency
const int MAX_PERIPH_FREQ = 50;

//minimum allowed peripheral clock frequency
const int MIN_PERIPH_FREQ = 2;

//function to initialize pins for SCL and SDA
void i2c_gpio_init(I2C_CONFIG i2c);

/*
 * Function to initialize I2C on the given
 * I2C interface, SCL pin, and SDA pin.
 * This will initialize in Master Mode.
 *
 * Following 18.3.3 in Ref Manual
 */
void i2c_init(I2C_CONFIG i2c)
{
	i2c_gpio_init(i2c);

	//enable clock access for the given I2C (on APB1 bus)
	//Figure 3. in Datasheet for the block diagram
	if(i2c.I2C == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_Msk;
	}

	if(i2c.I2C == I2C2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN_Msk;
	}

	if(i2c.I2C == I2C3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN_Msk;
	}

	//reset I2C, bit 15 in the CR1 register does this
	//18.6.1 in Ref Manual
	i2c.I2C->CR1 |= I2C_CR1_SWRST_Msk;
	i2c.I2C->CR1 &= ~I2C_CR1_SWRST_Msk; //come out of reset after

	//set peripheral clock frequency
	//the APB1 is limited to 50MHz, a
	//the minimum allowed frequenecy
	//is 2MHz
	//18.6.2 in Ref Manual
	if(i2c.FREQ_IN_MHz > MAX_PERIPH_FREQ)
	{
		i2c.FREQ_IN_MHz = MAX_PERIPH_FREQ;
	}

	if(i2c.FREQ_IN_MHz < MIN_PERIPH_FREQ)
	{
		i2c.FREQ_IN_MHz = MIN_PERIPH_FREQ;
	}

	i2c.I2C->CR2 |= (i2c.FREQ_IN_MHz << I2C_CR2_FREQ_Pos);

	//CCR is the clock control, and bits 0 to 11 setup the
	//SCL clock in master mode. This library will stick to
	//standard mode, and uses T(high) = tr(SCL) + tw(SCLH).
	//The 't(SCL)' values can be found in Table 59. in the datasheet.
	//tw(SCLH) = 4us = 4000ns, tr(SCL) = 1000ns, default PCLK1 = 16MHz
	//based on 18.6.8 in Ref Manual, T(high)/T(PCLK1) = CCR
	//							     5000ns/(1/16MHz) = 80
	i2c.I2C->CCR |= CRR_VAL;

	//TRISE is the max duration of the SCL feedback loop in master
	// mode. this boils down to (T/T(PCLK1)) + 1 = TRISE, so
	//(1000ns/(1/16MHz)) + 1 = 17 = TRISE
	//18.5.9 in Ref Manual
	i2c.I2C->TRISE = MAX_TRISE;

	//enable I2C peripheral in CR1
	//18.6.1 in Ref Manual
	i2c.I2C->CR1 |= I2C_CR1_PE_Msk;
}

/*
 * Function to generate start condition
 *
 * Following 18.3.3 in Ref Manual
 */
void i2c_start(I2C_CONFIG i2c)
{
	//start generation
	i2c.I2C->CR1 |= I2C_CR1_START_Msk;

	//wait for start condition, when it is generated SB = 1
	//18.6.6 in Ref Manual
	while(!(i2c.I2C->SR1 & I2C_SR1_SB_Msk));
}

/*
 * Function for master transmitter
 *
 * Following Figure 164. in Ref Manual
 */
void i2c_write(I2C_CONFIG i2c, uint8_t data)
{
	//wait for TXE (transmitter data register) to be empty (TXE = 1)
	//18.6.6 in Ref Manual
	while(!(i2c.I2C->SR1 & I2C_SR1_TXE_Msk));

	//transmit byte to the data register
	//18.6.5 in Ref Manual
	i2c.I2C->DR = data;

	//wait for byte transfer to finish. the byte transfer flag (BTF)
	//will go high when this happens
	//18.6.6 in Ref Manual
	while(!(i2c.I2C->SR1 & I2C_SR1_BTF_Msk));
}

/*
 * Function to send the 7-bit slave address to the master
 *
 * Following 18.3.3 in Ref Manual
 */
void i2c_send_address(I2C_CONFIG i2c, uint8_t saddr)
{
	//send slave address to the data register (7 bit address)
	i2c.I2C->DR = saddr << 1;
	//wait for address (ADDR) to be matched/received
	//18.6.6 in Ref Manual
	while(!(i2c.I2C->SR1 & I2C_SR1_ADDR_Msk));
	//according to Figure 164. in Ref Manual, ADDR can be reset
	//by reading the SR1 register, followed by the SR2 register
	uint8_t tmp = i2c.I2C->SR1 | i2c.I2C->SR2;
}

/*
 * Function to generate a stop
 */
void i2c_stop(I2C_CONFIG i2c)
{
	//generate a stop
	//18.6.2 in Ref Manual
	i2c.I2C->CR1 |= I2C_CR1_STOP_Msk;
}

/*
 * Function to transmit multiple bytes of data, with the given size
 */
void i2c_burst_write(I2C_CONFIG i2c, uint8_t *data, uint8_t size)
{
	//wait for TXE (transmitter data register) to be empty (TXE = 1)
	//18.6.6 in Ref Manual
	while(!(i2c.I2C->SR1 & I2C_SR1_TXE_Msk));

	//following Figure 164. in Ref Manual. data is transmitted, then
	//wait for TXE to be empty. continue this until all data is sent,
	//then wait for the byte transfer flag
	while(size)
	{
		while(!(i2c.I2C->SR1 & I2C_SR1_TXE_Msk));

		i2c.I2C->DR = (volatile uint32_t)*data++;

		size--;
	}

	while(!(i2c.I2C->SR1 & I2C_SR1_BTF_Msk));
}

/*
 * Function to initialize both SCL and SDA
 * pins in alternate function mode with the
 * appropriate AF bits set, open drain output,
 * and with pull-ups enabled
 */
void i2c_gpio_init(I2C_CONFIG i2c)
{
	//configure SCL pin, I2C is open drain by
	//definition, need pull-ups, and requires
	//alternate function mode with bit AF04 for
	//all SCL's. Table.9 in Datasheet for the mapping
	GPIOx_PIN_CONFIG sclPin;
	sclPin.OTYPER_MODE = GPIOx_OTYPER_OPEN_DRAIN;
	sclPin.PIN_MODE = GPIOx_PIN_ALTERNATE;
	sclPin.PUPDR_MODE = GPIOx_PUPDR_PULL_UP;
	sclPin.ALT_FUNC = GPIOx_ALT_AF4;
	sclPin.PIN_NUM = i2c.SCL_CONFIG.SCL_PIN;
	gpio_init(i2c.SCL_CONFIG.GPIO_PORT, sclPin);

	//do the same for the SDA pin, but some of the
	//pins require AF09 instead of AF04, so check
	//for those pins
	GPIOx_PIN_CONFIG sdaPin;
	sdaPin.OTYPER_MODE = GPIOx_OTYPER_OPEN_DRAIN;
	sdaPin.PIN_MODE = GPIOx_PIN_ALTERNATE;
	sdaPin.PUPDR_MODE = GPIOx_PUPDR_PULL_UP;
	sdaPin.PIN_NUM = i2c.SDA_CONFIG.SDA_PIN;

	if(i2c.SDA_CONFIG.SDA_PIN == I2C3_SDA_PB4 && i2c.SDA_CONFIG.GPIO_PORT == GPIOB)
	{
		sdaPin.ALT_FUNC = GPIOx_ALT_AF9;
	}
	else
	{
		sdaPin.ALT_FUNC = GPIOx_ALT_AF4;
	}

	gpio_init(i2c.SDA_CONFIG.GPIO_PORT, sdaPin);
}

