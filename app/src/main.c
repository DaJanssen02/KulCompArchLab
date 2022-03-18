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
        GPIOA->ODR &= ~GPIO_ODR_OD8;             // 00 - 10 - 01 - 11
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        seg7(uren/10);
    } else if (mux == 2) {
        GPIOA->ODR |= GPIO_ODR_OD8;             // 00 - 10 - 01 - 11
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        GPIOA->ODR |= GPIO_ODR_OD6;
        seg7(uren%10);
    } else if (mux == 3) {
        GPIOA->ODR &= ~GPIO_ODR_OD8;             // 00 - 10 - 01 - 11
        GPIOA->ODR |= GPIO_ODR_OD15;
        seg7(minuten/10);
    } else if (mux == 4) {
        GPIOA->ODR |= GPIO_ODR_OD8;             // 00 - 10 - 01 - 11
        GPIOA->ODR |= GPIO_ODR_OD15;
        seg7(minuten%10);
        mux = 0;
    }
}



void seg7(int n){
    //reset all segments
    GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5 | GPIO_ODR_OD6);
    GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);

    //segment  E
    if (n == 0 || n == 2 || n == 6 || n == 8){
        GPIOB->ODR |= GPIO_ODR_OD15;
    }

    //segment F
    if (n != 1 && n != 3 && n != 7){
        GPIOB->ODR |= GPIO_ODR_OD1;
    }

    //segment A
    if (n != 1 && n != 4){
        GPIOB->ODR |= GPIO_ODR_OD0;
    }

    //segment B
    if (n != 5 && n != 6){
        GPIOA->ODR |= GPIO_ODR_OD7;
    }

    //segment C
    if (n != 2){
        GPIOA->ODR |= GPIO_ODR_OD5;
    }

    //segment D
    if (n != 1 && n != 4 && n != 7){
        GPIOB->ODR |= GPIO_ODR_OD12;
    }

    //segment G
    if (n != 1 && n != 7 && n != 0){
        GPIOB->ODR |= GPIO_ODR_OD2;
    }
}


/*void convertNumber(char* display, int number) {
    char thousand = 0;
    char hunderd = 0;
    char ten = 0;
    char one = 0;

    thousand = number / 1000;
    	number -= thousand * 1000;
    hunderd = number / 100;
    	number -= hunderd * 100;
    ten = number / 10;
    	number -= ten * 10;
    one = number;

    display[0] = thousand;
    display[1] = hunderd;
    display[2] = ten;
    display[3] = one;
}

void convertTime(char* display, int hours, int minutes) {
    convertNumber(display, hours * 100 + minutes);
}*/


int main(void) {
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


    //systick
    SysTick_Config(48000);

    //enable interupt
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
    	if(minuten>59){
    		minuten = 0;
    		uren++;
    		if(uren > 23){
    			uren=0;
    		}
    	}
    }
}
