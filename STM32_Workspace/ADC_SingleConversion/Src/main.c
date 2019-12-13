/**
 ******************************************************************************
 * @file    main.c
 * @author  Auto-generated by STM32CubeIDE
 * @version V1.0
 * @brief   Default main function.
 ******************************************************************************
 */
#include <stdint.h>
#include "STM32F303RE.h"

#define RCC_CR				*((uint32_t *)0x40021000UL)
#define RCC_CFGR2			*((uint32_t *)0x4002102CUL)
#define RCC_AHBENR_ADDR		*((uint32_t *)0x40021014UL)
#define ADC12EN				28

#define ADC1_CR_ADDR		*((uint32_t *)0x50000008UL)
#define ADVREGEN 			28
#define ADEN				0
#define ADSTART				2
#define ADC1_ISR_ADDR		*((uint32_t *)0x50000000UL)
#define ADRDY				0
#define EOC					2
#define ADC1_CFGR_ADDR		*((uint32_t *)0x5000000CUL)
#define CONT				13
#define ADC1_SQR1_ADDR		*((uint32_t *)0x50000030UL)
#define L3					0
#define SQ1					6
#define ADC1_2_CCR_ADDR		*((uint32_t *)0x50000308UL)
#define TSEN				23
#define ADC1_DR_ADDR		*((uint32_t *)0x50000040UL)



int main(void)
{
	uint32_t value = 0;

	//Enable ADC12_CLK and set prescaler to 1
	RCC->CFGR2 |= BIT8;

	//PLL Enable
	RCC->CR |= BIT24;

	//Wait PLL clock ready Flag
	while( !(RCC->CR & BIT25) );

	//Enable ADC1 clock
	RCC->AHBENR	|= BIT28;

	//ADC1 Voltage Regulator enable sequence
	ADC1->CR &= ~BIT29;	//Clear bit 29
	ADC1->CR |=  BIT28; //Set bit 28

	//ADC1 enable
	ADC1->CR |= BIT1;


	//Set asynchronous CLKMODE -->ADC1 and ADC2 Clock input from PLL
	ADC1_2->CCR &= (BIT17 | BIT16);

	//Enable ADC1
	ADC1_CR_ADDR |= (1 << ADEN);

	//Wait until ADC1 is ready for operation
	while( !(ADC1_ISR_ADDR & (1 << ADRDY)) );

	//Set Single Conversion Mode
	ADC1_CFGR_ADDR &= ~(1 << CONT);

	//Set length of group acquisition equal to 1
	ADC1_SQR1_ADDR |= (1 << L3);

	//Select Temperature Sensor as input
	ADC1_SQR1_ADDR |= (0xF << SQ1);

	//Enable Temperature Sensor
	ADC1_2_CCR_ADDR |= (1 << TSEN);

	while(1){
		//Start ADC1
		ADC1_CR_ADDR |= (1 << ADSTART);

		//Wait for End of Conversion Flag
		while( !(ADC1_ISR_ADDR & (1 << EOC)) );

		//Read Result from Data Register
		value = ADC1_DR_ADDR;
	}

	for(;;);
}