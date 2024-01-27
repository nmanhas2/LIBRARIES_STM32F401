 /******************************************************************************
 * @file           : timer.h
 * @author         : Nubal Manhas
 * @brief          : Header file for Timer library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to declare functions, enums, and structs, that
 * will support the creation of a library for Timer functionality for the
 * STM32F01RE MCU
 *
 ******************************************************************************
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "stm32f4xx.h"

/*
 * Enumeration to differentiate between polarities
 * for the Compare/Capture Timer mode
 *
 * 13.4.9 in Ref Manual
 */
typedef enum
{
	TIM2_5_RISING_EDGE,
	TIM2_5_FALLING_EDGE,
	TIM2_5_BOTH_EDGE = 3
}TIM2_5_CC_POLARITY;

/*
 * Four channels are possible for each timers
 * capture/compare mode
 */
typedef enum
{
	TIM2_5_CH1,
	TIM2_5_CH2,
	TIM2_5_CH3,
	TIM2_5_CH4
}TIM2_5_CH;

/*
 * Enumeration to keep track of the different
 * modes available for output compare
 *
 * 13.4.7 in Ref Manual (OC1M bits)
 */
typedef enum
{
	TIM2_5_FROZEN,
	TIM2_5_ACTIVE,
	TIM2_5_INACTIVE,
	TIM2_5_TOGGLE,
	TIM2_5_FORCE_INACTIVE,
	TIM2_5_FORCE_ACTIVE,
	TIM2_5_PWM_MODE1,
	TIM2_5_PWM_MODE2,
	TIM2_5_NONE = -1

}TIM2_5_OUTPUT_MODE;

/*
 * Enumeration to store all possible GPIO bit positions that
 * contain channels for TIM2-5
 *
 * Table.9 in Datasheet for the mapping
 */
typedef enum
{
	TIM2_CH1_PA0,
	TIM2_CH2_PA1,
	TIM2_CH3_PA2,
	TIM2_CH4_PA3,

	TIM2_CH1_PA5 = 5,

	TIM2_CH1_PA15 = 15,
	TIM2_CH2_PB3 = 3,

	TIM3_CH1_PA6 = 6,
	TIM3_CH2_PA7,

	TIM3_CH3_PB0 = 0,
	TIM3_CH4_PB1,

	TIM3_CH1_PB4 = 4,
	TIM3_CH2_PB5,

	TIM3_CH1_PC6,
	TIM3_CH2_PC7,
	TIM3_CH3_PC8,
	TIM3_CH4_PC9,

	TIM4_CH1_PB6 = 6,
	TIM4_CH2_PB7,
	TIM4_CH3_PB8,
	TIM4_CH4_PB9,

	TIM5_CH1_PA0 = 0,
	TIM5_CH2_PA1,
	TIM5_CH3_PA2,
	TIM5_CH4_PA3,
}TIM2_5_PIN;

/*
 * The counter can be an up counter (0-count) or
 * down counter (count-0)
 *
 * 13.4.1 in Ref Manual (DIR bit)
 */
typedef enum
{
	TIM2_5_UP,
	TIM2_5_DOWN,
}TIM2_5_COUNTER_MODE;

/*
 * Enumeration to differentiate between
 * Input capture and Output compare mode
 */
typedef enum
{
	TIM2_5_INPUT,
	TIM2_5_OUTPUT
}TIM2_5_CAPTURE_COMPARE_MODE;

/*
 * Enumeration for holding bit position values
 * of all possible interrupts
 *
 * 13.4.4 in Ref Manual
 */
typedef enum
{
	TIM2_5_UPDATE_INTERRUPT,
	TIM2_5_CC1_INTERRUPT, //CCx = Capture/Compare channel
	TIM2_5_CC2_INTERRUPT,
	TIM2_5_CC3_INTERRUPT,
	TIM2_5_CC4_INTERRUPT,
	TIM2_5_TRIGGER_INTERRUPT = 6,
}TIM2_5_INTERRUPT_EN;

/*
 * Struct containing the necessary parameters
 * for configuring TIM2-5 capture input/output
 */
typedef struct
{
	TIM2_5_PIN PIN_NUM;
	GPIO_TypeDef* PORT;
	TIM2_5_CAPTURE_COMPARE_MODE CAPTURE_COMPARE_MODE;
	TIM2_5_CH CHANNEL;
	TIM2_5_OUTPUT_MODE OUTPUT_MODE;
	TIM2_5_CC_POLARITY CC_POLARITY;
}TIM2_5_CAPTURE_COMPARE_CONFIG;

/*
 * Struct containing basic parameters required
 * to configure a timer
 */
typedef struct
{
	TIM_TypeDef * TMR;
	TIM2_5_COUNTER_MODE COUNTER_MODE;
	int PRESCALER;
	int PERIOD;
}TIM2_5_CONFIG;

//function to initialize capture/compare mode depending on what is given
void tim2_5_init_capture_compare(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare);

//function to initialize a given timer
void tim2_5_init(TIM2_5_CONFIG timer);

//function to enable a given timer
void tim2_5_init_enable(TIM2_5_CONFIG timer);

//function for a simple delay with a given timer
void tim2_5_delay(TIM2_5_CONFIG timer);

//function to enable a given timer
void tim2_5_enable(TIM2_5_CONFIG timer);

void tim2_5_disable(TIM2_5_CONFIG timer);

//function to create a blocking delay until an input is captured
void tim2_5_capture_wait(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG capture);

//function to read an input capture
int tim2_5_capture_read(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG capture);

//function to configure PWM
void tim2_5_init_pwm(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare, uint16_t duty, TIM2_5_CC_POLARITY polarity);

//function to read and return the count register value for a given timer
uint32_t tim2_5_count_read(TIM2_5_CONFIG timer);

//function to generate a timer update event
void tim2_5_generate_event(TIM2_5_CONFIG timer);

//function for enabling timer interrupt
void tim2_5_interrupt_enable(TIM2_5_CONFIG timer, TIM2_5_INTERRUPT_EN interrupt);

//function for clearing a given timer interrupt flag
void tim2_5_clear_interrupt_flag(TIM2_5_CONFIG timer, TIM2_5_INTERRUPT_EN interrupt);

//function for setting the polarity of a capture/compare mode timer
void tim2_5_cc_set_polarity(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare, TIM2_5_CC_POLARITY polarity);

//function for disabling timer interrupt
void tim2_5_interrupt_disable(TIM2_5_CONFIG timer, TIM2_5_INTERRUPT_EN interrupt);
#endif /* TIMER_H_ */
