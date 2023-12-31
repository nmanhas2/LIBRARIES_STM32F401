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

void tim2_5_init_capture(TIM2_5_CAPTURE_CONFIG timer)
{
	GPIOx_PIN_CONFIG pin;
	pin.PIN_NUM = timer.PIN_NUM;
	pin.PIN_MODE = GPIOx_PIN_ALTERNATE;

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

	gpio_init(timer.PORT, pin);
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
	timer.TMR->PSC = timer.PRESCALER - 1;
	timer.TMR->ARR = timer.PERIOD - 1;

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

	//enable counter
	timer.TMR->CR1 |= TIM_CR1_CEN_Msk;
}

/*
 * Simple delay function
 *
 * Whenever an update event occurs, the update flag
 * gets set. An update event is considered to be
 * when an overflow/underflow happens in the CR1
 * register, assuming UDIS=0. In downcounting
 * mode, an underflow event happens when the
 * ARR value is 0. An overflow happens once
 * the ARR value is reached in upcount mode.
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
