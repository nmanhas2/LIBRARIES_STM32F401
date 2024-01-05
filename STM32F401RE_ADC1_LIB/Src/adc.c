/**
 ******************************************************************************
 * @file           : adc.c
 * @author         : Nubal Manhas
 * @brief          : Main c file for ADC library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to define functions that will support ADC
 * for the STM32F01RE MCU
 *
 ******************************************************************************
 */
#include "adc.h"

#define MAX_SQR_BITS	30 //number of configurable SQRx Register bits
#define MAX_SEQ_LENGTH  15 //max number of ADC conversions per sequence

//function to help configure what sequences to set the conversion to
void sequence_config(ADC_CONFIG adc);

/*
 * Function for initializing adc based on the configurable ADC structure
 *
 * Involves initializing the given channel to the correct port and pin for ADC,
 * enabling APB2 bus for ADC, configuring the conversion sequence, and finally enabling
 * the ADC in the control register
 */
void adc_init(ADC_CONFIG adc)
{
	//setup GPIO for analog mode
	GPIOx_PIN_CONFIG GPIO;
	GPIO.PIN_MODE = GPIOx_PIN_ANALOG;
	GPIO.PUPDR_MODE = GPIOx_PUPDR_NONE;
	GPIO.OTYPER_MODE = GPIOx_OTYPER_PUSH_PULL;

	//init gpio based on channel number
	//Table 8. in datasheet to see mapping
	if(adc.CHANNEL <= 7)
	{
		//PA0-PA7 = ADC_IN0-ADC_IN7
		GPIO.PIN_NUM = adc.CHANNEL;
		gpio_init(GPIOA, GPIO);

	}
	else if(adc.CHANNEL > 7 && adc.CHANNEL < 10)
	{
		//PB0-PB1 = ADC8-AD9
		GPIO.PIN_NUM = adc.CHANNEL - 8;
		gpio_init(GPIOB, GPIO);
	}
	else
	{
		//PC0-PC5 = ADC10-ADC15
		GPIO.PIN_NUM = adc.CHANNEL - 10;
		gpio_init(GPIOC, GPIO);
	}

	//ADC1 clock access is from APB2 bus
	//Figure 3. in datasheet
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	//setup for conversion sequence
	sequence_config(adc);

	ADC1->CR2 |= ADC_CR2_ADON;
}

/*
 * Function to configure the conversion sequence for ADC
 *
 * This involves assigning the given channel number into the right sequence register
 * based on the bit position calculated
 *
 *11.12.9-11.12.11 in Ref Manual
 */
void sequence_config(ADC_CONFIG adc)
{
	//each sequence takes 5 bits for and can be configured for channels 0-18.
	//take the sequence number and multiply by 5 to get its position in the
	//SQR1-SQR3 register (32 bit registers, 30 bits configurable in each). then substract
	//by the previous registers configurable bits (30), since it will now go into the next
	//register
	//
	//ex: ADC_SQ7 = 6. 6 * 5 = bit 30, 30-30 = bit 0 in SQ2
	//ADC_SQ13 = 12. 12 * 5 = bit 60, 60 - (30 * 2)(times SQ3 + SQ2 = 30 bits each = 60 bits total)
	//                      = bit 0 in SQ1
	int seq = adc.SEQUENCE * 5;
	if(seq < MAX_SQR_BITS)
	{
		ADC1->SQR3 |= (adc.CHANNEL << seq);
	}
	else if(seq >= MAX_SQR_BITS && seq < (MAX_SQR_BITS * 2))
	{
		ADC1->SQR2 |= (adc.CHANNEL << (seq - MAX_SQR_BITS));
	} else{
		ADC1->SQR1 |= (adc.CHANNEL << (seq - (MAX_SQR_BITS * 2)));
	}

	//set number of conversions (min = 1 (0000), max = 16 (1111) conversions)
	//11.12.9 in Ref Manual
	if(adc.SEQ_LENGTH > MAX_SEQ_LENGTH)
	{
		adc.SEQ_LENGTH = MAX_SEQ_LENGTH;
	}

	if(adc.SEQ_LENGTH < 0)
	{
		adc.SEQ_LENGTH = 0;
	}

	//set number of conversions
	ADC1->SQR1 |= (adc.SEQ_LENGTH << ADC_SQR1_L_Pos);


}

/*
 * Function to start the single conversion of the channels using software
 *
 * Simply involves setting the start bit in CR2, and waiting
 * for the EOC flag (which is done by adc_read() already)
 *
 * 11.3.4 in Ref Manual
 */
void adc_start_single(void)
{
	ADC1->CR2 |= ADC_CR2_SWSTART; //start adc conversion
}

/*
 * Function to start the continuous conversion of the channels using software
 *
 * Need to set the CONT bit to 1 within CR2, then start bit.
 * After this, wait for EOC flag (adc_read())
 *
 * 11.3.5 in Ref Manual
 */
void adc_start_continuous(void)
{
	ADC1->CR2 |= ADC_CR2_CONT; //enable continuous conversion, based on Section 11.12.3 in Reference Manual
	ADC1->CR2 |= ADC_CR2_SWSTART; //start adc conversion
}

/*
 * Function to read ADC value once conversion is complete
 *
 * Wait for the conversion to be complete by waiting for
 * EOC flag in the status register, then return what was
 * read in the data register
 */
uint32_t adc_read(void)
{
	while(!(ADC1->SR & ADC_SR_EOC)); //wait for completion, based on Section 11.12.1 in Reference Manual
	return (ADC1->DR); //return data read from data register, based on Section 11.12.14 in Reference Manual
}
