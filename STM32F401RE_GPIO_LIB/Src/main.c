/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing GPIO library for STM32F401RE, PA5 is configured to LED2 on this NUCLEO
 * DEV board, with PC13 configured to the on-board B1 button
 *
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "gpio.h"

/* TESTS: */
//#define TOGGLE_TEST  //un-comment this to test for output toggle on PA5
//#define OUTPUT_WRITE //un-comment this to test for output write to PA5, should be a similar result to TOGGLE_TEST
//#define INPUT_TEST //un-comment this to test input for PC13, PA5 should go high when PC13 is high
//#define OUTPUT_SETRESET //un-comment this to test output bit set/reset, should be similar to INPUT_TEST

GPIOx_PIN_CONFIG PIN5; //PA5
GPIOx_PIN_CONFIG PIN13; //PC13

int main(void)
{
	//we want PA5 as an output for all the tests
	PIN5.PIN_MODE = GPIOx_PIN_OUTPUT;
	PIN5.PIN_NUM = GPIOx_PIN_5;

	#ifdef TOGGLE_TEST
		gpio_init(GPIOA, PIN5); //init PA5 as an output
		while(1)
		{
			gpio_toggle_output(GPIOA, PIN5); //toggle PA5
			for(int i = 0; i < 100000; i++){} //wait to confirm
		}
	#endif

	#ifdef OUTPUT_WRITE
		gpio_init(GPIOA, pin); //init PA5 as output
		while(1)
		{
			gpio_write_output(GPIOA, PIN5, GPIOx_SET_OUTPUT); //write output to high
			for(int i = 0; i < 100000; i++){} //wait to confirm
			gpio_write_output(GPIOA, PIN5, 0); //write output to low
			for(int i = 0; i < 100000; i++){} //wait to confirm
		}
	#endif

	#ifdef INPUT_TEST
		//we want PC13 as an input for this test
		PIN13.PIN_MODE = GPIOx_PIN_INPUT;
		PIN13.PIN_NUM = GPIOx_PIN_13;

		gpio_init(GPIOA, PIN5); //init PA5 as output
		gpio_init(GPIOC, PIN13); //init PC13 as input
		while(1)
		{
			//check if PC13 is high, if it is then toggle make PA5 high as well, otherwise low
			if(gpio_input_read(GPIOC,PIN13)){
				gpio_write_output(GPIOA, PIN5, GPIOx_SET_OUTPUT); //write output to high
			} else{
				gpio_write_output(GPIOA, PIN5, 0); //write output to low
			}
		}
	#endif

	#ifdef OUTPUT_SETRESET
		//we want PC13 as an input for this test
		PIN13.PIN_MODE = GPIOx_PIN_INPUT;
		PIN13.PIN_NUM = GPIOx_PIN_13;

		gpio_init(GPIOA, PIN5); //init PA5 as output
		gpio_init(GPIOC, PIN13); //init PC13 as input
		while(1)
		{
			//check if PC13 is high, if it is then toggle make PA5 high as well, otherwise low
			if(gpio_input_read(GPIOC,PIN13)){
				gpio_output_bit_setreset(GPIOA, PIN5, GPIOx_BSRR_SET); //set the set output bit
			} else{
				gpio_output_bit_setreset(GPIOA, PIN5, GPIOx_BSRR_RESET); //set the reset output bits
			}
		}
	#endif
}
