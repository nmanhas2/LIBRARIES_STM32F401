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
 * Struct containing the necessary parameters
 * for configuring TIM2-5 capture input/output
 */
typedef struct
{
	TIM_TypeDef * TMR;
	TIM2_5_COUNTER_MODE COUNTER_MODE;
	TIM2_5_PIN PIN_NUM;
	GPIO_TypeDef* PORT;
	int prescaler;
	int period;
}TIM2_5_CAPTURE_CONFIG;

typedef struct
{
	TIM_TypeDef * TMR;
	TIM2_5_COUNTER_MODE COUNTER_MODE;
	int PRESCALER;
	int PERIOD;
}TIM2_5_CONFIG;

void tim2_5_init_capture(TIM2_5_CAPTURE_CONFIG timer);

void tim2_5_init(TIM2_5_CONFIG timer);

void tim2_5_delay(TIM2_5_CONFIG timer);
#endif /* TIMER_H_ */
