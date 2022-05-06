# Lab 4: De buzzer

## Opzet

Het doel van dit labo is om de PWM mogelijkheden van de TIMER blokken in de STM32 te leren gebruiken.

## TIMER

📌 **RM887**

Meeste microcontrollers hebben timer/counter blokken om nauwkeurige tijdsmetingen of tijdssturingen te maken. Een voorbeeld van een aantal toepassingen zijn:

* Generen van een periodisch interrupt
* Meten van lengte van een puls
* Meten van de frequentie van signaal
* Het vertragen van een pulse
* Het generen van PWM signaal
* Inlezen van encoder
* Tellen van een aantal pulsen 

Een timer/counter blok is opgebouw een uit digitale teller. De klok van deze teller kan afgeleid worden van een intern klok signaal of deze kan komen van uit een extern pin. 

Voor dat deze klok gebruikt wordt in de teller, gaat deze eerst door een prescaler. Deze prescaler deelt de frequentie van de klok door een instelbare waarde. Op de STM32L4 is heeft deze prescaler een maximaal deeltal van 65536. 

Deze afgedeelde klok gaat naar de teller. Deze kan je op verschillende manieren configureren. Zo kan deze teller optellen of aftellen. Ook kan je de waarde instellen waarop deze zou overflowen of stoppen. Dit noemt men meestal de period van de teller. Afhankelijk van de instelling gaat deze teller als die deze waarde bereikt, zal deze terug vanaf 0 starten of stoppen met tellen. Die laatste instellen wordt meestal een one-shot timer genoemt.

Nu kan telkens afhankelijk van de instelling als de timer de waarde 0 bereikt een interrupt worden afgevuurt. Deze modus wordt gebruikt om accurate vertragingen of events doormiddel van interrupt in de software te implementeren.

Een tweede mogelijkheid is om de capture en compare blokken te gebruiken. Deze blok kan twee dingen doen. De waarde van de teller bewaren als er een event binnenkomt. Dit event kan bijvoorbeeld zijn wanneer een pin van de timer van waarde verandert. In de refence manual van de microcontroller staan de verschillen mogelijkheden van waar deze events kunnen komen.

Een tweede mogelijkheid is de compare functionaliteit te gebruiken. Dit laat de timer events genereren die andere hardware blokken kunnen gebruiken. Ook kunnen deze events als interrupts worden gebruikt. Een veel voorkomende gebruik van deze events is om PWM signalen te maken. Deze events kunnen ook gebruikt worden om andere timers te triggeren, of om een ADC meting starten.


### Implementatie

1. Aanzetten van de klok voor de Timer blok

		// TIMER
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
		RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

2. Configureren GPIO

		GPIOB->MODER &= ~GPIO_MODER_MODE8_Msk;
		GPIOB->MODER |=  GPIO_MODER_MODE8_1;
		GPIOB->OTYPER &= ~GPIO_OTYPER_OT8;
		GPIOB->AFR[1] = (GPIOB->AFR[1] & (~GPIO_AFRH_AFSEL8_Msk)) | (0xE << GPIO_AFRH_AFSEL8_Pos);

3. Instellen Timer, hoofdteller

		TIM16->PSC = 0;
		TIM16->ARR = 24000;
		TIM16->CCR1 = 12000;

4. Instellen Timer, capture & compare 

		TIM16->CCMR1 &= ~TIM_CCMR1_CC1S;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1FE;
		TIM16->CCER |= TIM_CCER_CC1E;
		TIM16->CCER &= ~TIM_CCER_CC1P;
		TIM16->BDTR |= TIM_BDTR_MOE;
		TIM16->CR1 |= TIM_CR1_CEN;


## Opdracht

1. Breid het programma van lab3 uit zodat de buzzer een signaal geeft als de temperatuur boven een bepaalde drempelwaarde komt. Deze drempelwaarde kan men instellen via de potmeter.
	* Verander de code zo dat de buzzer een "Tuut ta Tuut ta" melodie maakt.
	* Leg uit hoe de configuratie van de timer in elkaar zit voor deze toepassing. Bespreek de hoe je die configureert. 
	* Wat bedoelen ze met de alternate mode van de GPIOls
	 

