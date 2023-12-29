/**
 ******************************************************************************
 * @file           : systick.c
 * @author         : Nubal Manhas
 * @brief          : Main c file for SYSTICK Timer library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to define functions that will support SYSTICK
 * for the STM32F01RE MCU
 *
 ******************************************************************************
 */
#include "systick.h"
#include "stm32f4xx.h"

#define SYSCLK_SPEED		16000000 //default sysclk speed

//1ms = 0.001 seconds, 16MHz clk speed = 16M cycles/second
//Number of clock cycles = 16MHz * 0.001 seconds = 16000 cycles/millisecond
#define SYSTICK_RELOAD_VAL	(SYSCLK_SPEED*0.001)

/*
 * Function to create a delay in milliseconds
 *
 * Based on the System Timer (SysTick) in the
 * Cortex-M4 Core peripherals
 *
 * 4.4 in CortexM4 Generic User Guide
 */
void systickDelayMS(int delay)
{
	//4.4.5 in CortexM4 Generic User Guide
	//says to program reload value, clear current
	//value, then program the control and status
	//register
	SysTick->LOAD = SYSTICK_RELOAD_VAL; //load number of clock pulses for 1ms

	SysTick->VAL &= ~SysTick_VAL_CURRENT_Msk ; //clear current value register

	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //set to processor (internal) clock
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //enable counter

	//start delay
	for(int i = 0; i < delay; i++)
	{
		//since SysTick is a 24-bit down-counter, it should go to 0 once it's done counting,
		//which causes the flag to be raised. wait until that happens.
		while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	}

	SysTick->CTRL = 0; //clear control register once the delay is done to stop the counter
}
