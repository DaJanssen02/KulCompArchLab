#include <stdint.h>
#include <stm32l4xx.h>

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay--);
}

//initialisatie van de temperatuur
float temperatuur = 0;

int __io_putchar(int ch){
		    while(!(USART1->ISR & USART_ISR_TXE));
		    USART1->TDR = ch;
		}

int main(void) {
	// Klok aanzetten
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// Klok selecteren, hier gebruiken we sysclk
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;

	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOA->MODER |=  GPIO_MODER_MODE9_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL9_Msk)) | (0x7 << GPIO_AFRH_AFSEL9_Pos);

	GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL10_Msk)) | (0x7 << GPIO_AFRH_AFSEL10_Pos);

	USART1->CR1 = 0;
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	USART1->BRR = 417;
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

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

	//kanalen instellen
	ADC1->SMPR1 |= (ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2); //111 traagste sample frequentie
	ADC1->SQR1 &= ~(ADC_SQR1_L_0 | ADC_SQR1_L_1 | ADC_SQR1_L_2 | ADC_SQR1_L_3);

    //NTC
	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1;

	ADC1->SMPR1 |= (ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2); //111 traagste sample frequentie
	ADC1->SQR1 &= ~(ADC_SQR1_L_0 | ADC_SQR1_L_1 | ADC_SQR1_L_2 | ADC_SQR1_L_3);

	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1;

	while (1) {
		printf("T=%2.1f",temperatuur);
		printf("°C\r\n");
		ADC1->SQR1 |= (ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_0); //00101
		ADC1->SQR1 &= ~(ADC_SQR1_SQ1_1|ADC_SQR1_SQ1_3|ADC_SQR1_SQ1_4);
		ADC1->CR |= ADC_CR_ADSTART;
		while(!(ADC1->ISR & ADC_ISR_EOS));
		float inputweerstand = ADC1->DR;
		float voltage = (inputweerstand*3.0f)/4096.0f;
		float weerstand = (10000.0f*voltage)/(3.0f-voltage);
		temperatuur = ((1.0f/((logf(weerstand/10000.0f)/3936.0f)+(1.0f/298.15f)))-273.15f);

		delay(7000000);
	}
}
