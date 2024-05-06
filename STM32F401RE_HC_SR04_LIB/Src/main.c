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
 * Datasheet for buzzer: https://product.tdk.com/en/system/files?file=dam/doc/product/sw_piezo/sw_piezo/piezo-buzzer/catalog/piezoelectronic_buzzer_ps_en.pdf
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

//callback for TIM2 input capture interrupt
static void tim2_callback(void);

/* TESTS: */
#define HCSR04_TEST

//prescaler value for the timer
//sysclock = 16MHz by default,
//16MHz clk/100KHz = 160, 100KHz = 10us
const int PRESCALER = 160;

//period will be as high as possible for the timer
const int PERIOD = 0xFFFF;

//baudrate for UART
const int UART_BAUDRATE = 115200;

//divisor needed to calculate distance in CM, in accordance to the ultrasonic datsheet
const int CM_DIVISOR = 58;

//a 10uS timer is being used and the trigger pin is required to be high for at least 10uS to activate the echo pin.
//a check of the timer count being equal to 1 would mean that 10uS has past, therefore the trigger pin can be set to low,
//since enough time has past for the echo pin to begin its signal
const int TEN_MICROSECONDS_COUNT = 1;

//the ultrasonic datasheet recommends a 60ms delay between measurements, this delay will occur using the SYSTICK timer and
//will happen before the trigger pin goes high
const int TRIGGER_DELAY_MILLISECONDS = 60;

//distance measurement in CM that is required to activate the buzzer, in this case it's < 10cm by default
const int BUZZER_MEASUREMENT = 10;

/*
 * Enumeration to keep track of ultrasonic state
 *
 * There will be three phases, based on the ultrasonic datasheet, with an additional fourth for measurement computation:
 *
 * TRIGGER_HIGH: set trigger pin high and wait at least 10uS, then set the trigger pin back to low
 * ECHO_RISING: capture timer count during the rising edge of the echo input capture, then change polarity to falling edge
 * ECHO_FALLING: capture timer count during the falling edge of the echo input capture, then disable interrupts
 * MEASUREMENT: compute the distance in CM by using the formula in the ultrasonic datasheet, display over UART, and reset
 * 				the state machine back to TRIGGER_HIGH
 */
typedef enum
{
	TRIGGER_HIGH,
	ECHO_RISING,
	ECHO_FALLING,
	MEASUREMENT
}ULTRASONIC_STATE;

//UART with PA3 as RX, PA2 as TX for USART2
UART_CONFIG UART2 = {
					 USART2_RX_PA3,
					 USART2_TX_PA2,
					 USART2,
					 GPIOA
					};

//configuration for about 10us timer
TIM2_5_CONFIG TMR2 = {
					  TIM2,
					  TIM2_5_UP,
					  PRESCALER,
					  PERIOD
					 };

//configuration for the PWM timer, the numbers are fairly arbitrary, just picked based on what sounded good based
//on the noise made from the buzzer
TIM2_5_CONFIG TMR3 = {
					  TIM3,
					  TIM2_5_UP,
					  PRESCALER * 100000,
					  PERIOD / 500
					 };

//trigger pin, this needs to go high for 10us to start the distancing
//this is why it is configured as an output
GPIOx_PIN_CONFIG TRIGGER_PIN = {
								GPIOx_PIN_0,
								GPIOx_PIN_OUTPUT,
								0,
								GPIOx_PUPDR_NONE,
								GPIOx_OTYPER_PUSH_PULL
					   	   	   };

//echo pin configuration as an input capture on TIM2 using channel 2 and PA1.
//8 40KHz ultrasound signals will be sent out from this pin once the TRIGGER_PIN is set to high for 10uS.
//the pulse width of the signal that follows the 8 bursts will determine the range of the object detected.
//the input capture rising edge and falling edge polarities will be used to capture the counts, and essentially
//determine the width based on this.
TIM2_5_CAPTURE_COMPARE_CONFIG ECHO_PIN = {
										  TIM2_CH2_PA1,
										  GPIOA,
										  TIM2_5_INPUT,
										  TIM2_5_CH2,
										  TIM2_5_NONE
										 };

//buzzer pin configuration to set it up as a PWM on PC7 using Channel 2 for TIM3
//if the measurement distance is < 10CM, it will activate the PWM timer, otherwise it will be disabled
TIM2_5_CAPTURE_COMPARE_CONFIG BUZZER_PIN = {
											TIM3_CH2_PC7,
											GPIOC,
											TIM2_5_OUTPUT,
											TIM2_5_CH2,
											TIM2_5_PWM_MODE1,
										    };

//integers to store counter values during the rising edge/falling edge of the echo pin input capture.
volatile int risingCount = 0;
volatile int fallingCount = 0;

//integer to store the distance measurement
volatile int measurement = 0;

//enum to keep track of current state of ultrasonic sensor
ULTRASONIC_STATE CURRENT_STATE = TRIGGER_HIGH;

//char buffer for uart transmitting
char str[30];
int main(void)
{

	//init uart at 115200 baud
	uart_init(UART2, UART_BAUDRATE);

	//configure trigger pin for GPIOA
	gpio_init(GPIOA, TRIGGER_PIN);
	tim2_5_init_pwm(TMR3, BUZZER_PIN, 50, TIM2_5_RISING_EDGE);
	#ifdef HCSR04_TEST

		//initialize + enable timer immediately with input capture on PA1
		tim2_5_init_capture_compare(TMR2, ECHO_PIN);
		tim2_5_enable(TMR2);
		while(1)
		{
			//gpio_toggle_output(GPIOA, BUZZER_PIN);
			//switch
			switch (CURRENT_STATE)
			{
				case TRIGGER_HIGH:
					//wait 60ms before next trigger
					systickDelayMS(TRIGGER_DELAY_MILLISECONDS);

					//generate a timer event, to reset the counter register
					tim2_5_generate_event(TMR2);

					//set the trigger output to high
					gpio_output_bit_setreset(GPIOA, TRIGGER_PIN, GPIOx_BSRR_SET);

					//wait 10us while trigger is high, as datasheet states
					while(!(TMR2.TMR->CNT == TEN_MICROSECONDS_COUNT));

					//set trigger pin back to low
					gpio_output_bit_setreset(GPIOA, TRIGGER_PIN, GPIOx_BSRR_RESET);

					//increment to the echo rise state
					CURRENT_STATE++;

					//enable interrupts for capture/compare channel 2, in line with the ECHO_PIN configuration
					tim2_5_interrupt_enable(TMR2, TIM2_5_CC2_INTERRUPT);

					break;

				//both echo states should do nothing outside the interrupt, since the CURRENT_STATE and the input capture
				//count will be out of sync
				case ECHO_RISING:
					break;

				case ECHO_FALLING:
					break;

				case MEASUREMENT:
					//sprintf the count in CM. the timer is set up for 10uS from the prescaler, but the formula for
					//distance measurement from the ultrasonic datasheet is: distance (CM) = time (uS) / 58, so the
					//count time must be mulitiplied by 10 to convert to uS since the timer is in 10uS increments
					measurement = ((fallingCount - risingCount)*10)/CM_DIVISOR;
					sprintf(str, "%i CM\n\r", measurement);

					//enable/disable the PWM timer based on the measurement
					if(measurement < BUZZER_MEASUREMENT)
					{
						tim2_5_enable(TMR3);
					}
					else
					{
						tim2_5_disable(TMR3);
					}

					//write the distance to uart using the str buffer
					uart_write_string(USART2, str);

					//set the state back to trigger high to begin another measurement
					CURRENT_STATE = TRIGGER_HIGH;
					break;
			}
		}
	#endif
}

/*
 * Function to be used as a callback for input capture interrupts
 *
 * Handles the rising edge and falling edge count capture + polarity change.
 * The interrupts will be disabled once a measurement is ready, after the counts
 * are captured
 */
static void tim2_callback(void)
{
	switch (CURRENT_STATE)
	{
		//the trigger high state should do nothing, since only the echo input capture is what caused the interrupt to occur
		case TRIGGER_HIGH:
			break;

		case ECHO_RISING:
			//read the capture/compare register value for the capture count
			risingCount = tim2_5_capture_read(TMR2, ECHO_PIN);

			//set polarity for input capture to falling edge for the next state
			tim2_5_cc_set_polarity(TMR2, ECHO_PIN, TIM2_5_FALLING_EDGE);

			//increment state to capture the input capture count on falling edge
			CURRENT_STATE++;
			break;

		case ECHO_FALLING:
			//read the capture/compare register value for the capture count
			fallingCount = tim2_5_capture_read(TMR2, ECHO_PIN);

			//set the polarity back to rising edge to prepare for the next measurement
			tim2_5_cc_set_polarity(TMR2, ECHO_PIN, TIM2_5_RISING_EDGE);

			//increment to measurement state
			CURRENT_STATE++;

			//disable interrupt since the falling and rising edge counts have been captured
			tim2_5_interrupt_disable(TMR2, TIM2_5_CC2_INTERRUPT);
			break;

		case MEASUREMENT:
			break;
	}
}

//interrupt request handler, check Startup Folder -> startup_stm32f401retx.s for implementation
void TIM2_IRQHandler(void)
{
	tim2_callback();
}
