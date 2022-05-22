#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>

int mux = 0;
double hoek = 0;

void delay(unsigned int n){
    volatile unsigned int delay = n;
    while (delay--);
}

void multiplexer(){
    if (mux == 1) {
        GPIOA->ODR &= ~GPIO_ODR_OD8;             //00
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7((int)hoek/10);
    } else if (mux == 2) {
        GPIOA->ODR |= GPIO_ODR_OD8;             //10
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        GPIOA->ODR |= GPIO_ODR_OD6;
        seg7(((int)hoek)%10);
    } else if (mux == 3) {
        GPIOA->ODR &= ~GPIO_ODR_OD8;             //01
        GPIOA->ODR |= GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7(((int)hoek*10)%10);
    } else if (mux == 4) {
        GPIOA->ODR |= GPIO_ODR_OD8;             //11
        GPIOA->ODR |= GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7(((int)hoek*100)%10);
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

void write_accel(int data, int reg){
	    I2C1->CR2 &= ~(1<<10);//enable write mode
		I2C1->CR2 |= I2C_CR2_NACK_Msk;
	    I2C1->CR2 |=  (1 << 13)|(2 << 16)|(0x53 << 1); //grote te verzende paket, conencted device,
	    while((I2C1->ISR & (1<<4)) == 0 && (I2C1->ISR & (1<<1)) == 0);
	     //NACKF = 0, TXIS = 0
	    if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
	        return;
	    }

	    I2C1->TXDR = reg;//register doorsturen

	    while(I2C1->ISR & (1<<4) == 0 && I2C1->ISR & (1<<1) == 0){};
	         //NACKF = 0, TXIS = 0
	    if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
	        return;
	    }
	    I2C1->TXDR = data;
		while((I2C1->ISR & I2C_ISR_STOPF) == 0);
}

int read_accel(int reg){
	while((I2C1->ISR & I2C_ISR_BUSY));
	I2C1->CR2 &= ~(1<<10);//enable write mode
	I2C1->CR2 &= ~I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= I2C_CR2_NACK_Msk;
	I2C1->CR2 |=  (1 << 13)|(1 << 16)|(0x53 << 1); //grote te verzende paket, conencted device,
	while(((I2C1->ISR & (1<<4)) == 0) && ((I2C1->ISR & (1<<1)) == 0)){};
	 //NACKF = 0, TXIS = 0
	if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
		return;
	}

	I2C1->TXDR = reg;//register doorsturen
	while((I2C1->ISR & (1<<6)) == 0);

	I2C1->CR2 |= I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 |= (1<<10);//enable read mode
	//read
	I2C1->CR2 |=  (1 << 16)|(0x53 << 1); //grote te verzende paket, conencted device,
	I2C1->CR2 |= (1<<13);
	while(!(I2C1->ISR & I2C_ISR_RXNE));

	return I2C1->RXDR;
}

int main(void) {
	//Klok aanzetten
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

	//systick configureren en interupt aanzetten
	SysTick_Config(48000);
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	//Klok selecteren, hier gebruiken we sysclk
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;

	//Deep powerdown modus uitzetten
	ADC1->CR &= ~ADC_CR_DEEPPWD;

	//ADC voltage regulator aanzetten
	ADC1->CR |= ADC_CR_ADVREGEN;

	delay(2000);

	//Kalibratie starten
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);

	//ADC aanzetten
	ADC1->CR |= ADC_CR_ADEN;

	//Kanalen instellen
	ADC1->SMPR1 |= (ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2); //111 traagste sample frequentie
	ADC1->SQR1 &= ~(ADC_SQR1_L_0 | ADC_SQR1_L_1 | ADC_SQR1_L_2 | ADC_SQR1_L_3);
	ADC1->SQR1 |= (ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_0); //00101

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

	// I2C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6_Msk)) | (0x4 << GPIO_AFRL_AFSEL6_Pos);

	GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7_Msk)) | (0x4 << GPIO_AFRL_AFSEL7_Pos);

	I2C1->TIMINGR = 0x20303E5D;
	I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	I2C1->CR1 |= I2C_CR1_PE;

	volatile int16_t array[3];
	write_accel(1<<3,0x2D);
	array[0] =  read_accel(0x2D);
    while (1) {
    	// Start de ADC en wacht tot de sequentie klaar is

    	//I2C_CR2.START

    	//I2C1->CR1 |= I2C_CR1_CRSTART;

    	//while(!(ADC1->ISR & ADC_ISR_EOS));
    	for (int i = 0; i<3; i++){
    		array[i] = read_accel(0x32+i*2)<<8+read_accel(0x32+i*2+1);
    	}

    	hoek = (acos(array[2]/(sqrt(array[0]*array[0]+array[1]*array[1]+array[2]*array[2]))))*(180/3.14);
    	delay(10000000);
	}
}

void SysTick_Handler(void) {
	multiplexer();
    mux++;
}



