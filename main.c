//	INCLUDES
	#include "stm32f0xx.h"
	#include <stdint.h>
    #include "lcd_stm32f0.h"

//	FUNCTION PROTOTYPES
	void init_Timer();
	void init_adc();
	//void init_LCD();

//GLOBAL VARIABLES
	//unsigned char a, buffer[16];

/*
Abstract
--------
The code was written for the STM32F0. The function of the code is to produce two PWM signals - with one being inverted.
The PWM signal are produces using timer 2 and 3 at PB4, PB10 and PB11. The resulting signals have a 3.3V peak to peak voltage at a
frequency of 105kHz and 50Hz
*/

int main(void)
{
	init_Timer();
	init_adc();
	init_LCD();

	/* Infinite loop */
	while (1)
	{
        // ADC used to change the duty cycle.
		TIM3->CCR1 =  (((ADC1-> DR)*50))*0.8; // PB4
		TIM2->CCR3 =  (((ADC1-> DR)*0.350))*0.8; // PB10 -- (((ADC1-> DR)*_%DUTY CYCLE_0))*0.8
		TIM2->CCR4 =  (((ADC1-> DR)*0.350))*0.8; // PB11

	}
}

//	FUNCTIONS

	void init_Timer()
	{
		RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;  // enable clock PORT B.
		RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN; // enable clock for TIM2.
		RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN; // enable clock for TIM3.

		GPIOB -> MODER |= GPIO_MODER_MODER4_1; // set PB4 to AF mode.
		GPIOB -> MODER |= GPIO_MODER_MODER10_1; // set PB10 to AF mode.
		GPIOB -> MODER |= GPIO_MODER_MODER11_1; // set PB11 to AF mode.

        //The following step maps the Port Pin to the relevant channel of Timer 2 (see Gaffar's Notes)
		GPIOB -> AFR[0] |= 0b00000000000000010000000000000000; // set PB4 to AF1 - 0001. (see Gaffar's Notes) - set bit 16
		GPIOB -> AFR[1] |= 0x200;   // set PB10 to AF2.
		GPIOB -> AFR[1] |= 0x2000;  // set PB11 to AF2.

		TIM3->PSC = 14;
		TIM3 -> ARR = 63999; // f = 50Hz
		TIM2-> ARR = 455;    // f = 105kHz

		//specify PWM mode: OCxM bits in CCMRx.
		TIM3->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1); // select PWM Mode 1 for PB4.
		TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // select PWM Mode 1 for PB10.
		TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // select PWM Mode 1 for PB11.

		TIM3->CCER |= TIM_CCER_CC1P;  // set channel 1 as active low
		TIM2->CCER |= TIM_CCER_CC4P;  // set channel 4 as active low

		//enable the OC channels.
		TIM3->CCER |= TIM_CCER_CC1E; // PB4.
		TIM2->CCER |= TIM_CCER_CC3E; // PB10.
		TIM2->CCER |= TIM_CCER_CC4E; // PB11.

		TIM3 -> CR1 |= TIM_CR1_CEN; // enable counter for TIM3.
		TIM2 -> CR1 |= TIM_CR1_CEN; // enable counter for TIM2.
	}

	//I will not need this piece of code at all, I am not using the potentiometer for the servo
	void init_adc()
	{
		RCC->APB2ENR |= RCC_APB2ENR_ADCEN; // clock enable ADC
		ADC1->CFGR1 |= ADC_CFGR1_CONT;	// continuous mode
		ADC1->CFGR1 |= ADC_CFGR1_RES_0;	// 10 bit resolution
		ADC1->CHSELR |= ADC_CHSELR_CHSEL5; // select channel 5 for pot in PA5.
		ADC1->CR |=ADC_CR_ADEN;
		while ((ADC1->ISR &= 1<<0)==0){}
		ADC1->CR |= ADC_CR_ADSTART;
		while ((ADC1->ISR &=1<<2)==0){}
	}
