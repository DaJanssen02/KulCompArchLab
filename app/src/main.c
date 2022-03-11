#include <stdint.h>
#include <stm32l4xx.h>

int mux = 0;
int array[4] = {0};

void convert(int getal, char * array){
	int duizend = getal/1000;
	 	 getal -= duizend * 1000;
	int honderd = getal/100;
	 	 getal -= honderd * 100;
	int tien = getal/10;
	  	 getal -= tien * 10;
	int een = getal/1;
	 	 getal -= een * 1;

}


void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay--);

void multiplexer(int uren, int minuten){

	GPIOA->ODR |= GPIO_ODR_OD8; 			// 00 - 10 - 01 - 11
	GPIOA->ODR &= ~GPIO_ODR_OD15;
	seg7(5);
	(mux == 3)?mux = 0: mux++;
}


void seg7(int n){
	GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5);
	GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);

	if (n == 0 || n == 2 || n == 6 || n == 8){  //segment  E
		GPIOB->ODR |= GPIO_ODR_OD15;
	}

	if (n != 1 && n != 3 && n != 7){  //segment F
		GPIOB->ODR |= GPIO_ODR_OD1;
	}

	if (n != 1 && n != 4){  //segment A
		GPIOB->ODR |= GPIO_ODR_OD0;
	}

	if (n != 5 && n != 6){  //segment B
		GPIOA->ODR |= GPIO_ODR_OD7;
	}

	if (n != 2){  //segment C
		GPIOA->ODR |= GPIO_ODR_OD5;
	}

	if (n != 1 && n != 4 && n != 7){  //segment D
		GPIOB->ODR |= GPIO_ODR_OD12;
	}

	if (n != 1 && n != 7 && n != 0){  //segment G
		GPIOB->ODR |= GPIO_ODR_OD2;
	}

}



int main(void) {
	int uren = 0;
	int minuten = 0 ;
	int getal = (uren * 100) + minuten;
	convert(getal,array);

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	//7segmentjes
	GPIOA->MODER &= ~(GPIO_MODER_MODE7_Msk | GPIO_MODER_MODE5_Msk);
	GPIOA->MODER |= (GPIO_MODER_MODE7_0 | GPIO_MODER_MODE5_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT5);

	GPIOB->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE12_Msk | GPIO_MODER_MODE15_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE2_Msk);
	GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE12_0 | GPIO_MODER_MODE15_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT12 | GPIO_OTYPER_OT15 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2);

	//mux + punt
	GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE15_Msk | GPIO_MODER_MODE6_Msk);
	GPIOA->MODER |= (GPIO_MODER_MODE8_0 | GPIO_MODER_MODE15_0 | GPIO_MODER_MODE6_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT15 | GPIO_OTYPER_OT6);

	//knoppen
	// GPIOB Pin 14 configureren als input
	GPIOB->MODER &= ~(GPIO_MODER_MODE13_Msk | GPIO_MODER_MODE14_Msk);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk | GPIO_PUPDR_PUPD14_Msk);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD14_0);



	while (1) {

		multiplexer(0,0);


	}

}

