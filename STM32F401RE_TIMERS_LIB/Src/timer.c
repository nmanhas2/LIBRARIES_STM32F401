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
void tim2_5_nvic_enable(TIM2_5_CONFIG timer);

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

	//enable compare output
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
 * Function to initialize PWM on a given timer.
 *
 * Provide the polarity and duty cycle, as well as the compare configuration
 * since the channel is required
 */
void tim2_5_init_pwm(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare, uint16_t duty, TIM2_5_CC_POLARITY polarity)
{

	//enable output compare
	tim2_5_init_capture_compare(timer, compare);

	//check for the channel, then enable preload bit
	//within the CCMRx register
	switch (compare.CHANNEL )
	{
		case TIM2_5_CH1:
			timer.TMR->CCMR1 |= TIM_CCMR1_OC1PE_Msk;
			timer.TMR->CCR1 |= duty;
			break;
		case TIM2_5_CH2:
			timer.TMR->CCMR1 |= TIM_CCMR1_OC2PE_Msk;
			timer.TMR->CCR2 |= duty;
			break;
		case TIM2_5_CH3:
			timer.TMR->CCMR2 |= TIM_CCMR2_OC3PE_Msk;
			timer.TMR->CCR3 |= duty;
			break;
		case TIM2_5_CH4:
			timer.TMR->CCMR2 |= TIM_CCMR2_OC4PE_Msk;
			timer.TMR->CCR4 |= duty;
			break;
	}

	tim2_5_cc_set_polarity(timer, compare, polarity);

	//enable auto-reload preload
	timer.TMR->CR1 |= TIM_CR1_ARPE_Msk;
}


void tim2_5_cc_set_polarity(TIM2_5_CONFIG timer, TIM2_5_CAPTURE_COMPARE_CONFIG compare, TIM2_5_CC_POLARITY polarity)
{
	//polarity is determined by the CCxP and
	//CCxNP bits within the CCER register, these
	//are bit masks that will work for all 4 channels
	//in a "math way"
	int ccxp, ccxnp;
	ccxp = (1U << ((compare.CHANNEL * 4) + 1));
	ccxnp = (1U << ((compare.CHANNEL * 4) + 3));

	//check polarity and configure the polarity bits
	//in the CCER register
	switch(polarity)
	{

		case TIM2_5_RISING_EDGE:
			timer.TMR->CCER &= ~(ccxp);
			timer.TMR->CCER &= ~(ccxnp);
			break;
		case TIM2_5_FALLING_EDGE:
			timer.TMR->CCER |= ccxp;
			timer.TMR->CCER &= ~ccxnp;
			break;
		case TIM2_5_BOTH_EDGE:
			timer.TMR->CCER |= (ccxp | ccxnp);
			break;
	}
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
	//tim2_5_enable(timer);
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

void tim2_5_disable(TIM2_5_CONFIG timer)
{
	//enable counter
	timer.TMR->CR1 &= ~TIM_CR1_CEN_Msk;
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

	//tim2_5_capture_wait(timer, capture);

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

/*
 * Function to read and return the count register value for a given timer
 *
 * 13.4.10 in Ref Manual
 */
volatile uint32_t tim2_5_count_read(TIM2_5_CONFIG timer)
{
	return (timer.TMR->CNT);
}

/*
 * Function to generate an update event, essentially clearing the count
 * register
 *
 * 13.4.6 in Ref Manual
 */
void tim2_5_generate_event(TIM2_5_CONFIG timer)
{
	timer.TMR->EGR |= TIM_EGR_UG;
}

/*
 * Function for enabling the given interrupt on
 * a specified timer using the DMA/Interrupt ENR
 *
 * The Timer must be enabled + initialized before this
 *
 * 13.4.4 in Ref Manual
 */
void tim2_5_interrupt_enable(TIM2_5_CONFIG timer, TIM2_5_INTERRUPT_EN interrupt)
{
	//clear the interrupt bit, then enable
	timer.TMR->DIER &= ~(1U << interrupt);
	timer.TMR->DIER |= (1U << interrupt);

	tim2_5_nvic_enable(timer);
}

/*
 * Function for clearing a given interrupt flag
 * in the timer status register
 *
 * 13.4.5 in Ref Manual
 */
void tim2_5_clear_interrupt_flag(TIM2_5_CONFIG timer, TIM2_5_INTERRUPT_EN interrupt)
{
	timer.TMR->SR &= ~(1U << interrupt);
}

/*
 * Function for enabling global interrupts for
 * the timers, this device uses the a nested vectored
 * interrupt controller for this.
 *
 * There are a few 32bit registers that cover each function,
 * the mapping can be seen in Table 38 in the Ref
 * Manual. The priority of interrupts is shown there as well.
 *
 * The NVIC interrupt enable register can be
 * seen in 4.2.1 in the Cortex-M4 User Guide.
 */
void tim2_5_nvic_enable(TIM2_5_CONFIG timer)
{
	//check the which timer it is, then enable that global
	//interrupt. the bit positions with ISER can be seen in Table 38
	//in the Ref Manual, but bits 28 to 30 are TIM2 to TIM4, and
	//TIM5 = 50
	if(timer.TMR == TIM2)
	{
		NVIC->ISER[0] |= (1U << TIM2_IRQn);
	}
	else if(timer.TMR == TIM3)
	{
		NVIC->ISER[0] |= (1U << TIM3_IRQn);
	}
	else if(timer.TMR == TIM4)
	{
		NVIC->ISER[0] |= (1U << TIM4_IRQn);
	}
	else if(timer.TMR == TIM5)
	{
		NVIC->ISER[1] |= (1U << (TIM5_IRQn-32));
	}
	else
	{
		return;
	}
}
