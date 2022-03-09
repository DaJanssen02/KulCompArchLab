#include <stdint.h>
#include <stm32l4xx.h>

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay--);
}

int main(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	//Led1
	GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk; //blok B -> alle bits van pin9 op 0 zetten
	GPIOB->MODER |= GPIO_MODER_MODE9_0; // bit 0 van pin9 hoog maken (=output)
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT9; // laag zetten voor push pull

	//Led2
	GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;
	GPIOC->MODER |= GPIO_MODER_MODE13_0;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;

	//KnopA
	GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk;

	//KnopB
	GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;

	//pull up weerstand hoog zetten
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;

	while (1) {
		if (!(GPIOB->IDR  & GPIO_IDR_ID13)) {
			GPIOC->ODR |= GPIO_ODR_OD13;
			GPIOB->ODR |= GPIO_ODR_OD9;
			delay(500000);
			GPIOC->ODR &= ~GPIO_ODR_OD13;
			GPIOB->ODR &= ~GPIO_ODR_OD9;
			delay(500000);
		}

		else if (!(GPIOB->IDR & GPIO_IDR_ID14)){
			GPIOC->ODR |= GPIO_ODR_OD13;
			GPIOB->ODR |= GPIO_ODR_OD9;
			delay(2500000);
			GPIOC->ODR &= ~GPIO_ODR_OD13;
			GPIOB->ODR &= ~GPIO_ODR_OD9;
			delay(2500000);
			}

		else {
			GPIOC->ODR &= ~GPIO_ODR_OD13;
			GPIOB->ODR &= ~GPIO_ODR_OD9;
		}
	}
}
