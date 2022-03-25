#include <stdint.h>
#include <stm32l4xx.h>

int mux = 0;
int minuten = 0;
int uren = 0;
int ms = 0;

void delay(unsigned int n){
    volatile unsigned int delay = n;
    while (delay--);
}

void multiplexer(){
    if (mux == 1) {
        GPIOA->ODR &= ~GPIO_ODR_OD8;             //00
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7(uren/10);
    } else if (mux == 2) {
        GPIOA->ODR |= GPIO_ODR_OD8;             //10
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        GPIOA->ODR |= GPIO_ODR_OD6;
        seg7(uren%10);
    } else if (mux == 3) {
        GPIOA->ODR &= ~GPIO_ODR_OD8;             //01
        GPIOA->ODR |= GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7(minuten/10);
    } else if (mux == 4) {
        GPIOA->ODR |= GPIO_ODR_OD8;             //11
        GPIOA->ODR |= GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7(minuten%10);
        mux = 0;
    }
}

void seg7(int n){
    //alle segmenten resetten
    GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5);
    GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);


    if (n != 1 && n != 4){							//Segment A
        GPIOB->ODR |= GPIO_ODR_OD0;
    }

    if (n != 5 && n != 6){							//Segment B
        GPIOA->ODR |= GPIO_ODR_OD7;
    }

    if (n != 2){									//Segment C
        GPIOA->ODR |= GPIO_ODR_OD5;
    }

    if (n != 1 && n != 4 && n != 7){ 				//Segment D
        GPIOB->ODR |= GPIO_ODR_OD12;
    }

    if (n == 0 || n == 2 || n == 6 || n == 8){ 		//Segment E
        GPIOB->ODR |= GPIO_ODR_OD15;
    }

    if (n != 1 && n != 2 && n != 3 && n != 7){ 		//Segment F
        GPIOB->ODR |= GPIO_ODR_OD1;
    }

    if (n != 1 && n != 7 && n != 0){ 				//Segment G
        GPIOB->ODR |= GPIO_ODR_OD2;
    }
}

int main(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    // 7 segmenten aanzetten (als output declareren) en laag maken
    GPIOA->MODER &= ~(GPIO_MODER_MODE7_Msk | GPIO_MODER_MODE5_Msk);
    GPIOA->MODER |= (GPIO_MODER_MODE7_0 | GPIO_MODER_MODE5_0);
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT5);

    GPIOB->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE12_Msk | GPIO_MODER_MODE15_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE2_Msk);
    GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE12_0 | GPIO_MODER_MODE15_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0);
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT12 | GPIO_OTYPER_OT15 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2);

    //mux en punt aansturen
    GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE15_Msk | GPIO_MODER_MODE6_Msk);
    GPIOA->MODER |= (GPIO_MODER_MODE8_0 | GPIO_MODER_MODE15_0 | GPIO_MODER_MODE6_0);
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT15 | GPIO_OTYPER_OT6);

    //knoppen: pin 13 (knop B) en pin 14 (knop A) declareren als input
    GPIOB->MODER &= ~(GPIO_MODER_MODE13_Msk | GPIO_MODER_MODE14_Msk);
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk | GPIO_PUPDR_PUPD14_Msk);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD14_0);

    //systick configereren en interupt aanzetten
    SysTick_Config(48000);
    NVIC_SetPriority(SysTick_IRQn, 128);
    NVIC_EnableIRQ(SysTick_IRQn);

    while (1) {
		if (!(GPIOB->IDR  & GPIO_IDR_ID13)) {
			minuten++;
			if(minuten > 59){
				minuten = 0;
			}
			delay(1000000);
		}

		if (!(GPIOB->IDR & GPIO_IDR_ID14)){
			uren++;
			if(uren > 23){
				uren = 0;
			}
			delay(1000000);
		}
	}
}

void SysTick_Handler(void) {
	multiplexer();
    mux++;
    ms++;

    if(ms == 60000){
    	ms = 0;
    	minuten++;
    } if(minuten>59){
   		minuten = 0;
   		uren++;
    } if(uren > 23){
		uren=0;
    }
}
