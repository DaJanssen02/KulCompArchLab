#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>

int mux = 0;
int temperatuur = 0;
float weerstand;
float inputpot;
float inputweerstand;
float voltage;

const int A = 27273;
const int B = 24296;
const int C = 45863;
const int D = 40886;
const int E = 36402;
const int F = 34359;
const int G = 30621;
const int N = 1000;

array[202] = {A,N,C,N,D,N,D,N,D,E,N,F,N,F,
		F,G,N,N,E,N,E,N,D,C,N,C,N,D,
		A,N,C,N,D,N,D,N,D,E,N,F,N,F,
		F,G,N,N,E,N,E,N,D,C,N,D,
		A,N,C,N,D,N,D,N,N,D,F,N,G,N,G,
		G,A,N,N,B,B,N,A,G,N,A,N,D,
		D,E,N,F,N,F,N,G,N,N,A,D,
		D,F,N,E,N,E,N,F,D,N,E,
		A,C,N,D,N,D,N,N,D,E,N,N,F,N,N,F,
		F,N,G,N,N,E,N,N,E,N,N,D,N,C,N,N,C,N,N,D,
		A,C,N,D,N,D,N,N,D,E,N,N,F,N,N,F,
		F,N,G,N,N,E,N,N,E,N,N,D,N,C,N,N,D,
		A,C,N,N,N,D,N,N,D,N,N,N,D,F,N,G,N,G,
		G,A,N,B,B,N,A,G,N,A,D
		};

void delay(unsigned int n){
    volatile unsigned int delay = n;
    while (delay--);
}

void multiplexer(){
    if (mux == 1) {
        GPIOA->ODR &= ~GPIO_ODR_OD8;             //00
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7((temperatuur/1000)%10);
    } else if (mux == 2) {
        GPIOA->ODR |= GPIO_ODR_OD8;             //10
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        GPIOA->ODR |= GPIO_ODR_OD6;
        seg7((temperatuur/100)%10);
    } else if (mux == 3) {
        GPIOA->ODR &= ~GPIO_ODR_OD8;             //01
        GPIOA->ODR |= GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7((temperatuur/10)%10);
    } else if (mux == 4) {
        GPIOA->ODR |= GPIO_ODR_OD8;             //11
        GPIOA->ODR |= GPIO_ODR_OD15;
        GPIOA->ODR &= ~GPIO_ODR_OD6;
        seg7(temperatuur%10);
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

	//Timer
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

	//kanalen instellen
	ADC1->SMPR1 |= (ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2); //111 traagste sample frequentie
	ADC1->SMPR1 |= (ADC_SMPR1_SMP6_0 | ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_2); //111 traagste sample frequentie
	ADC1->SQR1 &= ~(ADC_SQR1_L_0 | ADC_SQR1_L_1 | ADC_SQR1_L_2 | ADC_SQR1_L_3);



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

    //NTC
	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1;

	GPIOB->MODER &= ~GPIO_MODER_MODE8_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE8_1;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT8;
	GPIOB->AFR[1] = (GPIOB->AFR[1] & (~GPIO_AFRH_AFSEL8_Msk)) | (0xE << GPIO_AFRH_AFSEL8_Pos);



	TIM16->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1FE;
	TIM16->CCER |= TIM_CCER_CC1E;
	TIM16->CCER &= ~TIM_CCER_CC1P;
	TIM16->CR1 |= TIM_CR1_CEN;



    while (1) {
    	// Start de ADC en wacht tot de sequentie klaar is
    	TIM16->PSC = 0;
    	for (int i=0; i<100; i++){
    		TIM16->ARR = array[i];
    		TIM16->CCR1 = array[i]/2;
    		delay(900000);
    	}

    	/*
    	TIM16->PSC = 0;		//A
    	TIM16->ARR = A;
    	TIM16->CCR1 = A/2;
    	delay(1000000);

    	TIM16->ARR = C;		//C
    	TIM16->CCR1 = C/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = G;		//G
    	TIM16->CCR1 = G/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = C;		//C
    	TIM16->CCR1 = C/2;
    	delay(1000000);

    	TIM16->ARR = C;		//C
    	TIM16->CCR1 = C/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = A;		//A
    	TIM16->CCR1 = A;
    	delay(1000000);

    	TIM16->ARR = C;		//C
    	TIM16->CCR1 = C/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = G;		//G
    	TIM16->CCR1 = G/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = C;		//C
    	TIM16->CCR1 = C/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = A;		//A
    	TIM16->CCR1 = A;
    	delay(1000000);

    	TIM16->ARR = C;		//C
    	TIM16->CCR1 = C/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = G;		//G
    	TIM16->CCR1 = G/2;
    	delay(1000000);

    	TIM16->ARR = G;		//G
    	TIM16->CCR1 = G/2;
    	delay(1000000);

    	TIM16->ARR = G;		//G
    	TIM16->CCR1 = G/2;
    	delay(1000000);

    	TIM16->ARR = A;		//A
    	TIM16->CCR1 = A;
    	delay(1000000);

    	TIM16->ARR = B;		//B
    	TIM16->CCR1 = B/2;
    	delay(1000000);

    	TIM16->ARR = B;		//B
    	TIM16->CCR1 = B/2;
    	delay(1000000);

    	TIM16->ARR = A;		//A
    	TIM16->CCR1 = A;
    	delay(1000000);

    	TIM16->ARR = G;		//G
    	TIM16->CCR1 = G/2;
    	delay(1000000);

    	TIM16->ARR = A;		//A
    	TIM16->CCR1 = A;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = G;		//G
    	TIM16->CCR1 = G/2;
    	delay(1000000);

    	TIM16->ARR = A;		//A
    	TIM16->CCR1 = A/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

    	TIM16->ARR = F;		//F
    	TIM16->CCR1 = F/2;
    	delay(1000000);

    	TIM16->ARR = D;		//D
    	TIM16->CCR1 = D/2;
    	delay(1000000);

    	TIM16->ARR = E;		//E
    	TIM16->CCR1 = E/2;
    	delay(1000000);

		*/













    	ADC1->SQR1 |= (ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_1); //00110
    	ADC1->SQR1 &= ~(ADC_SQR1_SQ1_0|ADC_SQR1_SQ1_3|ADC_SQR1_SQ1_4);
		ADC1->CR |= ADC_CR_ADSTART;
		while(!(ADC1->ISR & ADC_ISR_EOC));
		float inputpot = ADC1->DR;
		delay(1000);


		ADC1->SQR1 |= (ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_0); //00101
		ADC1->SQR1 &= ~(ADC_SQR1_SQ1_1|ADC_SQR1_SQ1_3|ADC_SQR1_SQ1_4);
    	ADC1->CR |= ADC_CR_ADSTART;
    	while(!(ADC1->ISR & ADC_ISR_EOC));
    	float inputweerstand = ADC1->DR;
    	float voltage = (inputweerstand*3.0f)/4096.0f;
    	float weerstand = (10000.0f*voltage)/(3.0f-voltage);
    	temperatuur = ((1.0f/((logf(weerstand/10000.0f)/3936.0f)+(1.0f/298.15f)))-273.15f)*100;
    	delay(1000);

    	if (inputpot > inputweerstand){  //ntc werkt geinverteerd -> als de weerstand verhoogt gaat de spanning afnemen
    		TIM16->BDTR |= TIM_BDTR_MOE;
    	}
    	else{
    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    	}
	}
}

void SysTick_Handler(void) {
	multiplexer();
    mux++;
}

