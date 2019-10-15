#include <stdint.h>
#include "stm32f0xx.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "lcd_stm32f0.h"
#include <stdint.h>
#include <stm32f0xx_tim.h>

/*===================================
 * Function declarations
 *===================================*/
void main(void);
void init_sensors(void);
void align_coils(void);
void drone_lands(void);
void init_pushbuttons(void);
//void init_servo(void);     //need to write this method

/*===================================
 * Global constants
 *===================================*/
#define SW0 GPIO_IDR_0

/*===================================
 * Main loop
 *===================================*/

void main(void)
{

	init_LCD();
	lcd_command(CLEAR);
	lcd_putstring("Power. No");
	lcd_command(LINE_TWO);
    lcd_putstring("strings attached");

	init_pushbuttons(); //initialise SW0

	//waits here until the drone lands on pad
	drone_lands();

	init_sensors();//initialise rx sensors

	//init_servo();//initialise servo motor

	for(;;)  //loop forever
	{
		align_coils();  ////checks if Tx and Rxs are aligned, if not, aligns them
	}
}
/*===================================
 * Initialization Functions
 *===================================*/
void init_pushbuttons(void)
{//initialise PA0 so that SW0 can be configured

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable clock for port A

	GPIOA->MODER &= ~(GPIO_MODER_MODER0); //set PA0 as input, GPIOA_MODER0 = 00

	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR0_0); //enable pull-up resistor for PA0, GPIOA_PUPDR0 = 01
}

void init_sensors(void)
{//initialise PA9 and PA10 to take in the signals from the sensors

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable clock for port A

	GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10); //set PA9 and PA10 as GPIO inputs, GPIOA_MODER9 = GPIOA_MODER10 = 00

	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0); //enable pull-up resistors for PA9 and PA10, GPIOA_PUPDR9 = GPIOA_PUPD10 = 01

}
/*
void init_servo(void)
{

}*/

/*===================================
 * Functions
 *===================================*/
void align_coils(void)    //checks if Tx and Rxs are aligned, if not, aligns them
{   //this is done by checking the status of the sensors and then commanding the servo accordingly
	//using PA9 for left sensor and PA10 for right sensor

			//if both PA9 and PA10 are high/both sensors not detecting coils - swirl
			if( ( (GPIOA->IDR & GPIO_IDR_9) != 0x00000000 ) && ( (GPIOA->IDR & GPIO_IDR_10) != 0x00000000 ) )
			{
				lcd_command(CLEAR);
			    lcd_putstring("swirl");
			    delay(5000);
			}

			//if PA9 is low (detecting) and PA10 is high (not detecting) - rotate left
			else if( ( (GPIOA->IDR & GPIO_IDR_9) == 0x00000000 )  &&  ( (GPIOA->IDR & GPIO_IDR_10) != 0x00000000 ) )
		    {
			    lcd_command(CLEAR);
				lcd_putstring("rotate left");
				delay(5000);
			}

			//if PA9 is high (not detecting) and PA10 is low (detecting) - rotate right
			else if( ( (GPIOA->IDR & GPIO_IDR_9) != 0x00000000 )  &&  ( (GPIOA->IDR & GPIO_IDR_10) == 0x00000000 ) )
			{
				lcd_command(CLEAR);
				lcd_putstring("rotate right");
				delay(5000);
		    }

			//if both PA9 and PA10 are low/both sensors detecting coils - stay put
			else if( ( (GPIOA->IDR & GPIO_IDR_9) == 0x00000000 )  &&  ( (GPIOA->IDR & GPIO_IDR_10) == 0x00000000 ) )
			{
				lcd_command(CLEAR);
				lcd_putstring("aligned!");
			    delay(5000);
			}

			else //I don't have a clue why this would ever run
			{
				lcd_command(CLEAR);
			    lcd_putstring("confused!");
			    delay(1000);
			}

}

void drone_lands(void)
{//landing of the drone on the landing pad is simulated by the press of SW0

	while((GPIOA->IDR & SW0)!=0); //wait (do nothing) until SW0 is pressed

}
