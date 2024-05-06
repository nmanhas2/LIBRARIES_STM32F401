/**
 ******************************************************************************
 * @file           : i2c.h
 * @author         : Nubal Manhas
 * @brief          : Header file for I2C library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to declare functions, enums, and structs, that
 * will support the creation of a library for I2C functionality for the
 * STM32F01RE MCU
 *
 ******************************************************************************
 */

#ifndef I2C_H_
#define I2C_H_
#include "gpio.h"
#include "stm32f4xx.h"
#include "uart.h"

/*
 * Enumeration to keep track of all available
 * SDA pins
 *
 * Table. 9 in Datasheet for the mapping
 */
typedef enum
{
	I2C1_SDA_PB7 = 7,//AF04
	I2C1_SDA_PB9 = 9,//AF04
	I2C3_SDA_PC9 = 9,//AF04
	I2C3_SDA_PB4 = 4,//AF09
	I2C2_SDA_PB3 = 3,//AF04

}I2C_SDA_PIN;

/*
 * Struct for holding the SDA pin
 * port and number
 */
typedef struct
{
	I2C_SDA_PIN SDA_PIN;
	GPIO_TypeDef* GPIO_PORT;
} I2C_SDA_CONFIG;

/*
 * Enumeration to keep track of all available
 * SCL pins
 *
 * Table. 9 in Datasheet for the mapping
 */
typedef enum
{
	I2C1_SCL_PB6 = 6,//AF04
	I2C1_SCL_PB8 = 8,//AF04
	I2C2_SCL_PB10 = 10,//AF04
	I2C3_SCL_PA8 = 8//AF04

}I2C_SCL_PIN;

/*
 * Struct for holding the SCL pin
 * port and number
 */
typedef struct
{
	I2C_SCL_PIN SCL_PIN;
	GPIO_TypeDef* GPIO_PORT;
} I2C_SCL_CONFIG;

/*
 * Struct for configuring I2C. Holds the
 * SCL and SDA pin configurations, and chosen I2C
 * interface. The frequency is also set, from
 * 2 to 50 MHz.
 */
typedef struct
{
	I2C_SCL_CONFIG SCL_CONFIG;
	I2C_SDA_CONFIG SDA_CONFIG;
	I2C_TypeDef * I2C;
	int FREQ_IN_MHz;
}I2C_CONFIG;

//function to initialize I2C
void i2c_init(I2C_CONFIG i2c);

//function to generate a start condition for I2C
void i2c_start(I2C_CONFIG i2c);

//function to generate a stop condition
void i2c_stop(I2C_CONFIG i2c);

//function to transmit data to a slave
void i2c_write(I2C_CONFIG i2c, uint8_t data);

//function to send slave address to the master
void i2c_send_address(I2C_CONFIG i2c, uint8_t saddr);

//function to transmit multiple bytes of data to a slave
void i2c_burst_write(I2C_CONFIG i2c, uint8_t *data, uint8_t size);

#endif /* I2C_H_ */
