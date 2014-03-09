#include <stdlib.h>
#include <io.h>
#include <avr/io.h> 
#include <avr/interrupt.h> 


#define set_bit(reg,bit) ((reg)|=(1<<(bit)))
#define clear_bit(reg,bit) ((reg)&=~(1<<(bit)))

#define CLOCK 1
#define DISPLAY 2
#define CALCULATOR 3


void DisplayHH(int value);
void DisplayHL(int value);
void DisplayMH(int value);
void DisplayML(int value);
void DisplaySH(int value);
void DisplaySL(int value);
void DisplayClock(void);
void GenericDisplay(void);
void Tick(int showDisplay);


volatile int Hours = 0;
volatile int Minutes = 0;
volatile int Seconds = 0;
volatile int DekatronSpeed = 0;


int HH = 0;
int HL = 0;
int MH = 0;
int ML = 0;
int SH = 0;
int SL = 0;


void DisplayClock(void)
{
	if (Hours >= 20)
	{
		HL = Hours-20;
		HH = 2;
	}
	else if (Hours >=10)
	{
		HL = Hours-10;
		HH = 1;
	}
	else
	{
		HL = Hours;
		HH = 0;
	}

	if (Minutes >= 50)
	{
		ML = Minutes-50;
		MH = 5;
	}
	else if (Minutes >=40)
	{
		ML = Minutes-40;
		MH = 4;
	}
	else if (Minutes >=30)
	{
		ML = Minutes-30;
		MH = 3;
	}
	else if (Minutes >=20)
	{
		ML = Minutes-20;
		MH = 2;
	}
	else if (Minutes >=10)
	{
		ML = Minutes-10;
		MH = 1;
	}
	else
	{
		ML = Minutes;
		MH = 0;
	}

	if (Seconds >= 50)
	{
		SL = Seconds-50;
		SH = 5;
	}
	else if (Seconds >=40)
	{
		SL = Seconds-40;
		SH = 4;
	}
	else if (Seconds >=30)
	{
		SL = Seconds-30;
		SH = 3;
	}
	else if (Seconds >=20)
	{
		SL = Seconds-20;
		SH = 2;
	}
	else if (Seconds >=10)
	{
		SL = Seconds-10;
		SH = 1;
	}
	else
	{
		SL = Seconds;
		SH = 0;
	}



	DisplayHH(HH);
	DisplayHL(HL);
	DisplayMH(MH);
	DisplayML(ML);
	DisplaySH(SH);
	DisplaySL(SL);

}


void GenericDisplay()
{
	HH = Hours/10;
	HL = Hours-HH*10;
	MH = Minutes/10;
	ML = Minutes-MH*10;
	SH = Seconds/10;
	SL = Seconds-SH*10;

	DisplayHH(HH);
	DisplayHL(HL);
	DisplayMH(MH);
	DisplayML(ML);
	DisplaySH(SH);
	DisplaySL(SL);
}



void InitializePorts()
{
	DDRA = 0xFF;
	DDRB = 0xFF; 
	DDRC = 0xFF;
	DDRD = 0xFD;
}





void Wait(long delay)
{
	long j;
	for (j=0; j<delay;j++);
}


void GlowWait(void)
{
	long a;
	for (a=0; a <8000;a++);
}


int Dekatron(int n)
{

	switch(n)
	{
		case 0:	set_bit(PORTD, 1);//001
				set_bit(PORTD, 2);	
				clear_bit(PORTD,3);	
				GlowWait();
				set_bit(PORTD, 1);	//011
				clear_bit(PORTD, 2);	
				clear_bit(PORTD, 3);	
				GlowWait();
				break;
		case 1: set_bit(PORTD, 1);	//010
				clear_bit(PORTD, 2);	
				set_bit(PORTD, 3);	
				GlowWait();
			   	clear_bit(PORTD, 1);	//110
			  	clear_bit(PORTD, 2);	
				set_bit(PORTD, 3);	
				GlowWait();
				break;
		case 2: clear_bit(PORTD, 1);	//100
			 	set_bit(PORTD, 2);	
				set_bit(PORTD, 3);	
				GlowWait();
				clear_bit(PORTD, 1);	//101
				set_bit(PORTD, 2);	
				clear_bit(PORTD, 3);	
				GlowWait();
				break;
	}
	n++;
	if (n>2)
		n = 0;

	return n;
}


void TestCounting(void)
{
	int value = 0;
	while(1)
	{
		for (value = 0; value < 10; value++)
		{
			DisplayHH(value);
			DisplayHL(value);
			DisplayMH(value);
			DisplayML(value);
			DisplaySH(value);
			DisplaySL(value);
			Wait(10000);
		}
	}
}



void InitTimer(void)
{
	TCCR1B |= (1 << CS10) | (1 << CS12); // clk/1024 prescaler

	TIMSK |= (1 << OCIE1A); // Timer/Counter0 Output Compare Match Interrupt Enable
	TCCR1B |= (1 << WGM12);
	OCR1A = 10800; // 11059200 Mhz, prescale 1024 = >10800 => ticks/sec
}


void InitUART (void) 
{ 
   UCSRB |= (1 << RXEN);   // Turn on the reception circuitry 

   UBRRL = 63; 
   UBRRH = 2; 

   UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt (USART_RXC) 
   UCSRA |= (1 << U2X); // Asynch double speed
} 


void InitInterrupts()
{
	InitTimer();
	InitUART();
	sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed 
}


volatile char commandBuffer[] = {0,0,0,0,0,0,0,0,0,0};
volatile int cbIndex = 0;
volatile int  currentCommand = 0;
volatile int commandLength = 0;
volatile int dekatronTick = 0;
volatile int sixteenSecondCost = 0;
volatile long totalCost = 0;
volatile int sixteenSecondCounter = 0;



SIGNAL(SIG_OUTPUT_COMPARE1A)
{
	OCR1A = 10800; 
	switch(currentCommand)
	{
		case CLOCK: 		Tick(1); 
						dekatronTick = Dekatron(dekatronTick);

						break;
		case CALCULATOR: break;
		case DISPLAY :  if (DekatronSpeed != 0)
						{
							dekatronTick = Dekatron(dekatronTick);
						}
	    default: break;

	}
}


//ISR(USART_RXC_vect) 
SIGNAL(SIG_UART_RECV)
{ 

	while ( !( UCSRA & (1<<UDRE)) );

	commandBuffer[cbIndex] = UDR;


	if (0 == cbIndex)
	{
		currentCommand = commandBuffer[cbIndex];
		switch(currentCommand)
		{
			case CLOCK: commandLength = 5; break;
			case DISPLAY: commandLength = 6; break;
			case CALCULATOR: commandLength = 4; break;
		}
	}
	cbIndex++;
	
	if (cbIndex > commandLength)
	{
		// execute command


		switch(currentCommand)
		{
			case CLOCK: Hours = commandBuffer[1];
					Minutes = commandBuffer[2];
					Seconds = commandBuffer[3];
					DisplayClock();

					break;
			case CALCULATOR: 
			
					break;
			case DISPLAY:
					Hours = commandBuffer[1];
					Minutes = commandBuffer[2];
					Seconds = commandBuffer[3];
					DekatronSpeed = commandBuffer[4];
					GenericDisplay();
					break;
		}


		cbIndex = 0;
		commandLength = 0;
		// TODO: CRC checksum
	}		

} 







int main( void )
{


	InitializePorts();


/*	DisplayHH(1);
	DisplayHL(2);
	DisplayMH(3);
	DisplayML(4);
	DisplaySH(5);
	DisplaySL(6);
*/


	InitInterrupts();		

	for (;;);
}


void Tick(int showDisplay)
{
	Seconds++;
	if (Seconds >= 60)
	{
		Seconds = 0;
		Minutes++;
		if (Minutes >= 60)
		{
			Minutes = 0;

			Hours++;
			if (Hours >= 24)
			{
				Hours = 0;
			}

		}
	}

	if (showDisplay)
		DisplayClock();
}





void DisplayHH(int value)
{
	clear_bit(PORTA, 0);
	clear_bit(PORTA, 1);
	clear_bit(PORTA, 2);
	clear_bit(PORTA, 3);

	if (value & 1)
		set_bit(PORTA, 0);
	if (value & 2)
		set_bit(PORTA, 1);
	if (value & 4)
		set_bit(PORTA, 2);
	if (value & 8)
		set_bit(PORTA, 3);
}


void DisplayHL(int value)
{
	clear_bit(PORTB, 0);
	clear_bit(PORTB, 1);
	clear_bit(PORTB, 2);
	clear_bit(PORTB, 3);

	if (value & 1)
		set_bit(PORTB, 0);
	if (value & 2)
		set_bit(PORTB, 1);
	if (value & 4)
		set_bit(PORTB, 2);
	if (value & 8)
		set_bit(PORTB, 3);
}


void DisplayMH(int value)
{
	clear_bit(PORTA, 4);
	clear_bit(PORTA, 5);
	clear_bit(PORTA, 6);
	clear_bit(PORTA, 7);

	if (value & 1)
		set_bit(PORTA, 4);
	if (value & 2)
		set_bit(PORTA, 5);
	if (value & 4)
		set_bit(PORTA, 6);
	if (value & 8)
		set_bit(PORTA, 7);
}


void DisplayML(int value)
{

	clear_bit(PORTD, 4);
	clear_bit(PORTD, 5);
	clear_bit(PORTD, 6);
	clear_bit(PORTD, 7);

	if (value & 1)
		set_bit(PORTD, 4);
	if (value & 2)
		set_bit(PORTD, 5);
	if (value & 4)
		set_bit(PORTD, 6);
	if (value & 8)
		set_bit(PORTD, 7);
}


void DisplaySH(int value)
{
	clear_bit(PORTC, 0);
	clear_bit(PORTC, 1);
	clear_bit(PORTC, 2);
	clear_bit(PORTC, 3);

	if (value & 1)
		set_bit(PORTC, 0);
	if (value & 2)
		set_bit(PORTC, 1);
	if (value & 4)
		set_bit(PORTC, 2);
	if (value & 8)
		set_bit(PORTC, 3);
}


void DisplaySL(int value)
{
	clear_bit(PORTC, 4);
	clear_bit(PORTC, 5);
	clear_bit(PORTC, 6);
	clear_bit(PORTC, 7);

	if (value & 1)
		set_bit(PORTC, 4);
	if (value & 2)
		set_bit(PORTC, 5);
	if (value & 4)
		set_bit(PORTC, 6);
	if (value & 8)
		set_bit(PORTC, 7);
}






