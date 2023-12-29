/**
 ******************************************************************************
 * @file           : adc.h
 * @author         : Nubal Manhas
 * @brief          : Header file for ADC library
 ******************************************************************************
 * @purpose
 *
 * The purpose of this file is to declare functions, enums, and structs, that
 * will support the creation of a library for ADC functionality for the
 * STM32F01RE MCU
 *
 ******************************************************************************
 */

#ifndef ADC_H_
#define ADC_H_
#include "gpio.h"
/*
 * Enumeration for differentiating between ADC channels
 *
 * There are 19 channels, ADC_IN16 and ADC_IN18 are allocated
 * to the on-board temperature sensor with ADC_IN17 being the
 * VREF (11.9 in Ref Manual). The rest can be used for their
 * own purpose.
 *
 * PA0-PA7 = ADC_IN0-ADC_IN7
 * PB0-PB1 = ADC8-AD9
 * PC0-PC5 = ADC10-ADC15
 * Table 8 in Datasheet
 */
typedef enum
{
	ADC_CH0,
	ADC_CH1,
	ADC_CH2,
	ADC_CH3,
	ADC_CH4,
	ADC_CH5,
	ADC_CH6,
	ADC_CH7,
	ADC_CH8,
	ADC_CH9,
	ADC_CH10,
	ADC_CH11,
	ADC_CH12,
	ADC_CH13,
	ADC_CH14,
	ADC_CH15,
	ADC_CH16,//temperature sensor
	ADC_CH17,//internal reference voltage
	ADC_CH18//temperature sensor
}ADC_CH;

/*
 * Enumeration to account for all 16 conversion sequences
 *
 * 11.12.10 in Ref Manual
 */
typedef enum
{
	ADC_SQ1,
	ADC_SQ2,
	ADC_SQ3,
	ADC_SQ4,
	ADC_SQ5,
	ADC_SQ6,
	ADC_SQ7,
	ADC_SQ8,
	ADC_SQ9,
	ADC_SQ10,
	ADC_SQ11,
	ADC_SQ12,
	ADC_SQ13,
	ADC_SQ14,
	ADC_SQ15,
	ADC_SQ16
}ADC_SQ;

/*
 * Struct to configure mode, sequence number
 * and channel number for ADC
 */
typedef struct
{
	ADC_SQ SEQUENCE;
	ADC_CH CHANNEL;
	int SEQ_LENGTH;
}ADC_CONFIG;

void adc_init(ADC_CONFIG adc);//function to configure adc based on given sequence number, channel number, mode and length
void adc_start_single(void);//function to start the single conversion of the channels using software
void adc_start_continuous(void);//function to start the continuous conversion of the channels using software
uint32_t adc_read(void);//function to wait until the conversion is complete, and return value contained in data register if not
#endif /* ADC_H_ */
