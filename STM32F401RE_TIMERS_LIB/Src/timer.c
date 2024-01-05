/**
 ******************************************************************************
 * @file           : timer.c
 * @author         : Nubal Manhas
 * @brief          : Main c file for Timer library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to define functions that will support Timer
 * for the STM32F01RE MCU
 *
 ******************************************************************************
 */
#include "timer.h"
#include "gpio.h"
#include "stm32f4xx.h"

void tim2_5_init_output_compare(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare);
void tim2_5_init_input_capture(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare);
void pin_init(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare);

/*
 * Function to initialize a given GPIO compare/capture pin as an alternate function
 * for the given timer
 */
void pin_init(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare)
{
	//setup GPIO pin
	GPIOx_PIN_CONFIG pin;
	pin.PIN_NUM = compare.PIN_NUM;
	pin.PIN_MODE = GPIOx_PIN_ALTERNATE;
	pin.PUPDR_MODE = GPIOx_PUPDR_NONE;
	pin.OTYPER_MODE = GPIOx_OTYPER_PUSH_PULL;

	//determine which alternate function mode to set
	//the GPIO pin as.
	//
	//Table 9. in Datasheet for mapping
	if(timer.TMR == TIM2)
	{
		pin.ALT_FUNC = GPIOx_ALT_AF1;
	}
	else if(timer.TMR == TIM3 || timer.TMR == TIM4 || timer.TMR == TIM5)
	{
		pin.ALT_FUNC = GPIOx_ALT_AF2;
	}
	else
	{
		return;
	}

	//init the GPIO given pin for the timer channel
	gpio_init(compare.PORT, pin);
}

/*
 * Function to initialize output compare for the given timer and pin
 *
 * 13.4.7 in Ref Manual
 */
void tim2_5_init_output_compare(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare)
{
	//check for the channel, since the register and bit position will change
	//based on this, and set the bits required for the desired output
	//compare mode
	if(compare.CHANNEL == TIM2_5_CH1)
	{
		timer.TMR->CCMR1 |= (compare.OUTPUT_MODE << TIM_CCMR1_OC1M_Pos);
	}
	else if(compare.CHANNEL == TIM2_5_CH2)
	{
		timer.TMR->CCMR1 |= (compare.OUTPUT_MODE << TIM_CCMR1_OC2M_Pos);
	}
	else if(compare.CHANNEL == TIM2_5_CH3)
	{
		timer.TMR->CCMR2 |= (compare.OUTPUT_MODE << TIM_CCMR2_OC3M_Pos);
	}
	else if(compare.CHANNEL == TIM2_5_CH4)
	{
		timer.TMR->CCMR2 |= (compare.OUTPUT_MODE << TIM_CCMR2_OC4M_Pos);
	}
	else
	{
		return;
	}

	//enable capture/compare output
	//13.4.9 in Ref Manual
	timer.TMR->CCER |= (1U<<(compare.CHANNEL * 4));
}

/*
 * Function to enable input capture on the given timer and
 * channel
 *
 * 13.4.7 in Ref Manual
 */
void tim2_5_init_input_capture(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare)
{
	//set the capture/compare selection to be input on TI2 (IC1)
	//for the given channel
	if(compare.CHANNEL == TIM2_5_CH1)
	{
		timer.TMR->CCMR1 |= (1U << TIM_CCMR1_CC1S_Pos);
	}
	else if(compare.CHANNEL == TIM2_5_CH2)
	{
		timer.TMR->CCMR1 |= (1U << TIM_CCMR1_CC2S_Pos);
	}
	else if(compare.CHANNEL == TIM2_5_CH3)
	{
		timer.TMR->CCMR2 |= (1U << TIM_CCMR2_CC3S_Pos);
	}
	else if(compare.CHANNEL == TIM2_5_CH4)
	{
		timer.TMR->CCMR2 |= (1U << TIM_CCMR2_CC4S_Pos);
	}
	else
	{
		return;
	}

	//enable capture/compare output
	//13.4.9 in Ref Manual
	timer.TMR->CCER |= (1U<<(compare.CHANNEL * 4));
}


/*
 * Function to initialize a given timer and it's
 * configuration
 *
 * 13.4 in Ref Manual
 */
void tim2_5_init(TIM2_5_CONFIG timer)
{
	//determine which bit to enable in APB1 bus for clock
	//access to the given timer
	if(timer.TMR == TIM2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk;
	}
	else if(timer.TMR == TIM3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN_Msk;
	}
	else if(timer.TMR == TIM4)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN_Msk;
	}
	else if(timer.TMR == TIM5)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN_Msk;
	}
	else
	{
		return;
	}

	//set the prescaler and period
	//clock speed (16MHz)/(prescaler * period) = desired delay
	if(timer.PRESCALER >= 0)
	{
		timer.TMR->PSC = timer.PRESCALER - 1;
	}
	else
	{
		return;
	}

	if(timer.PERIOD >= 0)
	{
		timer.TMR->ARR = timer.PERIOD - 1;
	}
	else
	{
		return;
	}


	//clear the counter
	timer.TMR->CNT = 0;

	//set counter mode (up/down)
	if(timer.COUNTER_MODE == TIM2_5_UP)
	{
		timer.TMR->CR1 &= ~TIM_CR1_DIR_Msk;
	}
	else if (timer.COUNTER_MODE == TIM2_5_DOWN)
	{
		timer.TMR->CR1 |= TIM_CR1_DIR_Msk;
	}
	else
	{
		return;
	}
}

/*
 * Function to initialize + enable the timer immediately
 */
void tim2_5_init_enable(TIM2_5_CONFIG timer)
{
	tim2_5_init(timer);
	tim2_5_enable(timer);
}

/*
 * Function to initialize capture/compare mode for a given timer
 * pin with the channel specified
 */
void tim2_5_init_capture_compare(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare)
{
	pin_init(timer, compare);

	//init the timer
	tim2_5_init(timer);

	//check which compare mode is needed and init that mode
	if(compare.CAPTURE_COMPARE_MODE == TIM2_5_OUTPUT)
	{
		tim2_5_init_output_compare(timer,compare);
	}
	else if(compare.CAPTURE_COMPARE_MODE == TIM2_5_INPUT)
	{
		tim2_5_init_input_capture(timer, compare);
	}
	else
	{
		return;
	}

	//enable timer
	tim2_5_enable(timer);
}

/*
 * Function to enable timer using CR1
 *
 * 13.4.1 in Ref Manual
 */
void tim2_5_enable(TIM2_5_CONFIG timer)
{
	//enable counter
	timer.TMR->CR1 |= TIM_CR1_CEN_Msk;
}

/*
 * Simple delay function
 *
 * Whenever an update event occurs, the update flag
 * gets set. An update event is considered to be
 * when an overflow/underflow happens in the CR1
 * register, assuming UDIS=0. In down counting
 * mode, an underflow event happens when the
 * ARR value is 0. An overflow happens once
 * the ARR value is reached in up count mode.
 *
 * Waiting for the under/overflow essentially
 * waits for the counter to complete, then the
 * flag needs to be reset to start it again.
 *
 * 13.3.2/13.4.5 in Ref Manual
 */
void tim2_5_delay(TIM2_5_CONFIG timer)
{
	while(!(timer.TMR->SR & TIM_SR_UIF_Msk));
	timer.TMR->SR &= ~TIM_SR_UIF_Msk;
}

/*
 * Function to wait for an input to be captured.
 *
 * This happens when the a counter value has been captured
 * in the TIMx_CCR1 register, which causes the CCxIF flag
 * to be set. The counter value is captured when an edge has
 * been detected on IC1, which matches the selected polarity.
 * Since the CCER register has the polarity set to rising edge,
 * by default, every rising edge will cause the flag to be set.
 *
 * 13.4.5/13.4.9 in Ref Manual
 */
void tim2_5_capture_wait(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG capture)
{
	while(!(timer.TMR->SR & (1U << (capture.CHANNEL + 1)))); //flags start on bit 1

}

/*
 * Function to read an input capture value once the CCxIF flag
 * has been set
 *
 * 13.4.5/13.4.13 in Ref Manual
 */
int tim2_5_capture_read(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG capture)
{

	tim2_5_capture_wait(timer, capture);

	//determine channel, and return the value read in the
	//corresponding capture/compare register (CCRx)
	switch (capture.CHANNEL)
	{
		case (TIM2_5_CH1):
			return timer.TMR->CCR1;
			break;

		case (TIM2_5_CH2):
			return timer.TMR->CCR2;
			break;

		case (TIM2_5_CH3):
			return timer.TMR->CCR3;
			break;

		case (TIM2_5_CH4):
			return timer.TMR->CCR4;
			break;
		default:
			return -1;
	}
}
