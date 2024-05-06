/*/**
 ******************************************************************************
 * @file           : lcd.c
 * @author         : Nubal Manhas
 * @brief          : Main c file for I2C LCD library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to define functions that will support an I2C
 * LCD for the STM32F01RE MCU
 *
 * Datasheet: https://www.orientdisplay.com/wp-content/uploads/2019/10/AMC0802BR-B-Y6WFDY-I2C.pdf
 *
 ******************************************************************************
 */

#include "i2c.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"

/*
 * Initialize the I2C LCD
 *
 * slave address = 0x3C
 *
 * Following section 14 in LCD Datasheet
 */
void lcd_init(I2C_CONFIG i2c)
{
	//generate a start
	i2c_start(i2c);

	//send LCD slave address
	i2c_send_address(i2c, LCD_SLAVE_ADDR);

	/*section 12 in the LCD Datasheet for ALL instructions*/

	//send function set instruction
	//(8bit data, 2 line display, 5x11 dots)
	i2c_write(i2c, DEFAULT_FUNC_SET);

	//set display to on, with cursor blinking
	i2c_write(i2c, CURSOR_DISPLAY);

	//entry mode, assigning cursor moving direction
	i2c_write(i2c, CURSOR_INCREMENT);

	//clear display
	i2c_write(i2c, CLEAR_DISPLAY);

	//generate a stop
	i2c_stop(i2c);
}

/*
 * Function to write to the LCD
 *
 * This only works for up to 8 characters! I couldn't quite figure
 * out how to make the second line on the LCD work.
 *
 * Based on WriteData function on p.19 in LCD datasheet
 */
void lcd_write(I2C_CONFIG i2c, char* data)
{
	//store the length of the data
	int length = strlen(data);

	//while the length isn't fully decremented, keep sending
	//characters
	while(length)
	{
		//generate a start
		i2c_start(i2c);

		//send LCD slave address
		i2c_send_address(i2c, LCD_SLAVE_ADDR);
		systickDelayMS(1);

		//set CGRAM
		i2c_write(i2c, SET_CGRAM);
		systickDelayMS(1);

		//write char to LCD
		i2c_write(i2c, *data++);
		systickDelayMS(1);

		//generate a stop
		i2c_stop(i2c);

		length--;
	}
}



