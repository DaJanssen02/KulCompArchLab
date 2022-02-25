/**
 * @file main.c
 * @version 0.1
 *
 */

#include <stdint.h>
#include <stm32l4xx.h>


void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay--);
}

int main(void) {

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE9_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT9;

	GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;
	GPIOC->MODER |= GPIO_MODER_MODE13_0;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;

	while (1) {
		if ((GPIOB->IDR &= ~GPIO_MODER_MODE13_Msk) = 1) {
			GPIOC->ODR |= GPIO_ODR_OD13;
			GPIOB->ODR |= GPIO_ODR_OD9;
			delay(5000);
			GPIOC->ODR &= ~GPIO_ODR_OD13;
			GPIOB->ODR &= ~GPIO_ODR_OD9;
			delay(5000);
		}

		else if ((GPIOB->IDR &= ~GPIO_MODER_MODE14_Msk) = 1){
			GPIOC->ODR |= GPIO_ODR_OD13;
			GPIOB->ODR |= GPIO_ODR_OD9;
			delay(25000);
			GPIOC->ODR &= ~GPIO_ODR_OD13;
			GPIOB->ODR &= ~GPIO_ODR_OD9;
			delay(25000);
			}

		else {
			GPIOC->ODR |= GPIO_ODR_OD13;
			GPIOB->ODR |= GPIO_ODR_OD9;
		}
	}


}
