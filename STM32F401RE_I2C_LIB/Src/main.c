/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing I2C library for STM32F401RE, by using an I2C LCD
 *
 ******************************************************************************
*/
#include "stm32f4xx.h"
#include "uart.h"
#include "i2c.h"
#include "lcd.h"
#include <stdio.h>
#include <stdint.h>

int main(void)
{

	/*configure i2c, SCL = PB8, SDA = PB9, at 16MHz*/

	//struct configure i2c
	I2C_CONFIG i2c;
	i2c.FREQ_IN_MHz = 16;
	i2c.I2C = I2C1;

	//configure SCL for PB8
	I2C_SCL_CONFIG scl = {I2C1_SCL_PB8, GPIOB};
	i2c.SCL_CONFIG = scl;

	//configure SDA for PB9
	I2C_SDA_CONFIG sda = {I2C1_SDA_PB9, GPIOB};
	i2c.SDA_CONFIG = sda;

	//init i2c
	i2c_init(i2c);

	//init lcd
	lcd_init(i2c);

	//write to the lcd, up to 8 characters
	lcd_write(i2c, "HELLO");
}

