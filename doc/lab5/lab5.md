# Lab 5: Logging

## Opzet

Het doel van dit labo is om de USART te leren kennen.

## USART

📌 **RM1194**

Universal Asynchronous Receiver/Transmitter of kortweg UART is de module die wordt gebruikt voor seriële communicatie tussen twee of meer deelnemers. Er zijn eigenlijk twee vormen van UART-hardware als volgt:

* UART - Universal Asynchronous Receiver/Transmitter
* USART - Universele Synchrone/Asynchrone Ontvanger/Transmitter

Bij de synchrone variant genereert de zender een bitklok en stuurt deze ook door naar de ontvanger. De ontvanger gebruikt deze klok om de inkomende data te synchronizeren. Anderzijds genereert het asynchrone variant ook de bitklok, maar word deze niet doorgestuurd naar de ontvanger. De ontvanger moet zelf een bitklok generen. Het is belangrijk dat de twee deelnemers dezelfde baudsnelheid gebruiken.
### Kernmerken

De USART biedt een flexibele manier voor full-duplex datacommunicatie met externe componenten die NRZ (Non-Return-To-Zero) asynchrone seriële communicatie nodig heeft. De USART biedt een zeer breed bereik van baudrates door gebruik te maken van een fractionele baudrategenerator.
De module ondersteunt synchrone eenrichtingscommunicatie en half-duplex eendraadscommunicatie. Ook ondersteunt deze tevens de LIN (local interconnection network), Smartcard Protocol en IrDA (infrared data association) SIR ENDEC specificaties, en flowcontrol (CTS/RTS).


* Full duplex, asynchrone communicatie
* Fractionele baudrate-generatorsystemen - Een gemeenschappelijk programmeerbare zend- en ontvangstbaudrate tot 4,5 MBits/s
* Pariteitscontrole
* Programmeerbare data woordlengte (8 of 9 bits)
* Configureerbare stopbits - ondersteuning voor 1 of 2 stopbits
* LIN Master Synchrone Break zendmogelijkheid en LIN slave break detectiemogelijkheid
* Zenderklok uitgang voor synchrone transmissie
* IrDA SIR Encoder-Decoder - ondersteuning voor 3/16 bit duur voor normale modus
* Smartcard-emulatievermogen
* Single-wire half-duplex communicatie
* Afzonderlijke enable bits voor zender en ontvanger

  3. STM32 USART Hardware Functionaliteiten   
 
Elke USART bidirectionele communicatie vereist een minimum van twee pinnen: Receive Data In (RX) en Transmit Data Out (TX). Via deze pennen worden seriële gegevens verzonden en ontvangen in de normale USART-modus. De CK-pen is vereist voor een interface in synchrone modus. De CTS- en RTS-pinnen zijn nodig in de hardware flow control-modus.

3.1 USART-blokschema

Het digitale blokschema voor deze UART-hardwaremodule laat zien dat er twee afzonderlijke schuifregisters en dubbel gebufferde in/uit-gegevens voor een full-duplex gegevensoverdracht en -ontvangst. Beide schuifregisters die de gegevens in- of uitschakelen tijdens ontvangst/transmissie worden geklokt op de snelheid van het BRG (baud rate generator) circuit onderin het diagram. 

![ADC blokdiagramma](img/block.PNG)

### Zender

De USART zender kan datawoorden van 8 of 9 bits verzenden, afhankelijk van de M bit status. Wanneer de transmit enable bit (TE) is ingesteld, wordt de data in het transmit shift register naar de TX pen verschoven, en worden de corresponderende klokpulsen op de CK pen uitgegeven (voor synchrone modus).

Tijdens een USART-transmissie schuift de data eerst met de minst significante bit naar buiten op de TX-pin. Elk karakter wordt voorafgegaan door een startbit, dat een logisch niveau laag is voor een periode van één bit. Het karakter wordt afgesloten met een configureerbaar aantal stopbits. De volgende stopbits worden door USART ondersteund: 0,5, 1, 1,5, en 2 stopbits.


### Ontvanger

Tijdens een USART-ontvangst schuift de data eerst in het minst significante bit via de RX-pin. In deze modus bestaat het USART_DR register uit een buffer (RDR) tussen de interne bus en het ontvangen schuifregister.

## Gebruik

Klok aanzetten van GPIO en UART blok

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

GPIO configureren en alternative mode aanzetten

	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOA->MODER |=  GPIO_MODER_MODE9_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL9_Msk)) | (0x7 << GPIO_AFRH_AFSEL9_Pos);

	GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL10_Msk)) | (0x7 << GPIO_AFRH_AFSEL10_Pos);

UART configureren

	USART1->CR1 = 0;
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	USART1->BRR = 417;
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    
## C library

Door de **__io_putchar** functie te implemeteren laten we weten aan de C-library hoe deze charakters kan laten weergeven op een seriële console.

    int __io_putchar(int ch){
        while(!(USART1->ISR & USART_ISR_TXE));
        USART1->TDR = ch;
    }

Nu wordt de output van de **printf** functie afgeleid naar deze functie.

## Opdracht

1. Stel de UART in als transmitter met de volgende configuratie, een baudrate van 115200, 8 databits, geen paritieit en 1 stop bit.
2. Schrijf een functie dat een ascii teken verstuurt via de seriële poort.
3. Configureer de C library dat de "printf" functie de seriële poort gebruikt.
3. Stuur 1 keer per seconde de temperatuur uit via de seriele poort naar de computer. Gebruik volgende formaat "T=24.1°C" en meeting start op een nieuwe lijn.

Tip: PUTTY kan ook seriele poorten openen.

