# Lab 6: De hellingsmeter

## Opzet

Het doel van dit labo is om de I2C module en de ADXL343 accelerometer te leren kennen.

## I2C

📌 **RM1127**

De I2C-bus is een synchrone seriële bus waarmee verschillende devices met elkaar kunnen communiceren door middel van twee lijnen. Een korte introductie over I2C kan je vinden op https://nl.wikipedia.org/wiki/I%C2%B2C-bus en uitgebreidere is hier te vinden https://en.wikipedia.org/wiki/I%C2%B2C.

Om de I2C module van de STM32 te initialiseren en als master mode te gebruiken staat in de reference manual 📌 **RM1147** de nodige informatie.


De module wordt op volgende manier geïnitialiseerd. Eerste de klok aanzetten van de GPIO block met de I2C pinnen, en ook voor de I2C module.

	// I2C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;


De GPIO correct configureren voor de alternative mode

	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6_Msk)) | (0x4 << GPIO_AFRL_AFSEL6_Pos);

	GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7_Msk)) | (0x4 << GPIO_AFRL_AFSEL7_Pos);

De I2C module instellen, voor 100kHz klok. En deze dan ook aanzetten.

	I2C1->TIMINGR = 0x20303E5D;
	I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	I2C1->CR1 |= I2C_CR1_PE;

## Accelerometer

Een korte Quick-Start voor de Accelerometer om deze te kunnen gebruiken:

1. In het 0x2D—POWER_CTL register moet de "Measure" bit hoog staan om de Accelerometer te laten werken
2. Register 0x32 tot Register 0x37 (DATAX0, DATAX1, DATAY0, DATAY1, DATAZ0, DATAZ1) kan dan de versnellingswaardes worden ingelezen.

Een goede library waar ideeën kunnen worden uitgehaald is https://github.com/adafruit/Adafruit_ADXL343

## Opdracht

1. Schrijf functies om een register van de ADXL343 te lezen en te schrijven
2. Configureer de ADXL343 correct om de aardversnelling te meten
3. Schrijf een programma dat de hellingshoek berekend van het bordje ten opzicht van de aarde en dit weergeeft op het LED display.

