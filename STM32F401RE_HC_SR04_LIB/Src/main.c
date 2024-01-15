/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Nubal Manhas
 * @brief          : Main program body
 ******************************************************************************
 * @purpose
 *
 * Testing HC-SR04 ultrasonic sensor
 *
 * Datasheet for HC-SR04: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
 *
 ******************************************************************************
*/
#include "stm32f4xx.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "systick.h"
#include <stdio.h>
#include <stdint.h>

/* TESTS: */
#define HCSR04_TEST

UART_CONFIG UART2; //struct to configure UART2
TIM2_5_CONFIG TMR2; //struct to configure TIM2

uint32_t timestamp = 0; //used to store counter values
char str[60];
int main(void)
{
	//UART with PA3 as RX, PA2 as TX for USART2
	UART2.PORT = GPIOA;
	UART2.RX = USART2_RX_PA3;
	UART2.TX = USART2_TX_PA2;
	UART2.USART = USART2;

	//Initializing about 10us timer
	TMR2.PERIOD = 0xFFFF; //period will be as high as possible
	TMR2.PRESCALER = 160; //16MHz clk/100KHz = 160, 100KHz = 10us
	TMR2.TMR = TIM2;
	TMR2.COUNTER_MODE = TIM2_5_UP;

	//init uart at 115200 baud
	uart_init(UART2, 115200);

	//trigger pin, this needs to go high for 10us to start the distancing
	//this is why it is configured as an output
	GPIOx_PIN_CONFIG pa0 = {GPIOx_PIN_0, GPIOx_PIN_OUTPUT, 0, GPIOx_PUPDR_NONE, GPIOx_OTYPER_PUSH_PULL};

	//echo pin, this will send out 8 cycle burst of ultrasound after the trigger pin is high for 10us,
	//then it will send out a TTL signal, which has it's proportion = range, hence why it is an input.
	GPIOx_PIN_CONFIG pa1 = {GPIOx_PIN_1, GPIOx_PIN_INPUT, 0, GPIOx_PUPDR_NONE, GPIOx_OTYPER_PUSH_PULL};

	//configure both pins, GPIOA
	gpio_init(GPIOA, pa0);
	gpio_init(GPIOA, pa1);
	#ifdef HCSR04_TEST

		//initialize + enable timer immediately
		tim2_5_init_enable(TMR2);
		while(1)
		{
			//set the trigger output to high
			gpio_output_bit_setreset(GPIOA, pa0, GPIOx_BSRR_SET);
			//wait 10us
			while(!(TMR2.TMR->CNT == 1));
			//set trigger pin to low
			gpio_output_bit_setreset(GPIOA, pa0, GPIOx_BSRR_RESET);

			//the 8 burst ultrasound should come from the echo pin now,
			//wait for that to be over
			while(!gpio_input_read(GPIOA,pa1));
			//capture the start of the echo signal, now that the burst is over,
			//this is where the TTL signal for the range comes in
			timestamp = tim2_5_count_read(TMR2);
			//wait for the "range" signal to end
			while(gpio_input_read(GPIOA,pa1));

			//capture the end value and subtract the start value from it,
			//multiply by 10 since the formula according
			//to the hc-sr04 datasheet says distance(cm) = time(us) /  58
			//and TMR2 is set to be 10us for it's speed
			timestamp = (((tim2_5_count_read(TMR2)) - timestamp)*10)/58;
			//sprintf to a char array buffer, showing the distance in cm
			sprintf(str, "%i CM\n\r", timestamp);

			//transmit the distance over usart2
			uart_write_string(USART2, str);

			//60ms delay is recommended between intervals, according to
			//the hc-sr04 datsheet
			systickDelayMS(60);

			//generate a timer event, to reset the count
			tim2_5_generate_event(TMR2);
		}
	#endif
}


