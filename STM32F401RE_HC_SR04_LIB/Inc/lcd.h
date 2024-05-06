/**
 ******************************************************************************
 * @file           : lcd.h
 * @author         : Nubal Manhas
 * @brief          : Header file for I2C LCD library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to declare functions, enums, and structs, that
 * will support the creation of a library for I2C LCD functionality for the
 * STM32F01RE MCU
 *
 * Datasheet: https://www.orientdisplay.com/wp-content/uploads/2019/10/AMC0802BR-B-Y6WFDY-I2C.pdf
 *
 ******************************************************************************
 */

#ifndef LCD_H_
#define LCD_H_

//LCD Slave address, check p. 17 in
//the LCD datasheet to verify
#define LCD_SLAVE_ADDR 		0x3C

/*Section 12 in the LCD Datasheet for ALL instructions*/

//Instruction to turn cursor + display on
#define CURSOR_DISPLAY		0x0F

//Instruction to clear LCD display
#define CLEAR_DISPLAY		0x01

//Instruction to confgiure the function set
//(8bit data, 2 line display, 5x8 dots)
#define DEFAULT_FUNC_SET	0x38

//Instruction to set the cursor to increment by one when moving
//and no shift
#define CURSOR_INCREMENT    0x06

//Instruction to set CGRAM address, to begin writing data to the
//LCD
#define SET_CGRAM			0x40

//function to initialize the LCD
void lcd_init(I2C_CONFIG i2c);

//function to write characters to the LCD
void lcd_write(I2C_CONFIG i2c, char* data);

#endif /* LCD_H_ */
