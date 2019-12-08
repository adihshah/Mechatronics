/*
 * GccApplication3.c
 *
 * Created: 11/22/2019 4:49:12 PM
 * Author : MAE Lab
 * 
 * Resources for PWM
 * https://sites.google.com/site/qeewiki/books/avr-guide/pwm-on-the-atmega328
 * Lecture Slide 
 * Arduino Pinout
 * Set Pins B1 and B2 (Bot moves forward) to 25% PWM using Timer1
 * Doubts: 
 * 1.Can we use timer1? (Since its used for color sensor)
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include "serial.h"
#include <util/delay.h>
#include <avr/interrupt.h>

int period=0;

//Can't ever have B0,B1 or B2,B3 both high at the same time

void startback(){
	PORTB |= 0b00001001; 
}

void stopback(){
	PORTB &= 0b11110110;
}

void startright(){
	PORTB |= 0b00001010;
}

void stopright(){
	PORTB &= 0b11110101;
}

void startleft(){
	PORTB |= 0b00000101;
}

void stopleft(){
	PORTB &= 0b11111010;
}

void startforward(){
	PORTB |= 0b00000110;
}

void stopforward(){
	PORTB &= 0b11111001;
}

void moveinsquare(){
	
	startforward();

	_delay_ms(1400);

   	stopforward();

   	startright();

   	_delay_ms(630);

   	stopright();

   	startforward();

	_delay_ms(1400);

   	stopforward();

   	startleft(); //turn left

   	_delay_ms(630);

   	stopleft();

   	startforward(); //go forward for half the time

	_delay_ms(700);

   	stopforward();


   	startback(); //go backwards

   	_delay_ms(2800);

   	stopback();


   	startleft(); //turn left

   	_delay_ms(630);

   	stopleft();


   	startforward();

	_delay_ms(1400);

   	stopforward();

	
   	startright();

   	_delay_ms(630);

   	stopright(); //at this point, it is back to original position
}
	

ISR(PCINT0_vect){
	if((PINB&0b00010000)==0b00010000){ // Check rising or falling edge
	TCNT1 = 0b00000000; // Reset the timer
	}
	else{
	period = TCNT1; // Set period to current timer value
	}
}

//Left connected 6(PD6,PCINT 22), Right connected to 7(PD7,PCINT23)
ISR(PCINT2_vect){
	printf("Comes to QTI Interrupt");
	printf("\n");
	printf("%d",PIND);
	printf("\n");
	if (((PIND & 0b01000000) == 0b01000000) && ((PIND & 0b10000000) == 0b10000000)){ // if left and right detect black, then ???
			printf("Both detect black");
			startright(); //turn 135 degrees
			_delay_ms(900);
			stopright();
		}

		//else if left only, turn 45 to the right
	else if ((PIND & 0b01000000) == 0b01000000){
			printf("Left detects black");
			/*startright(); //turn 45 degrees
			_delay_ms(300);
			stopright();*/
			
		}
		
		//else if right only, turn 45 to the left
	else if ((PIND & 0b10000000) == 0b10000000){
			printf("Right detects black");
			startleft(); //turn left 45 degrees
			_delay_ms(300);
			stopleft();
		}
}

/*ISR(TIMER2_COMPA_vect){
	//if it enters interrupt, means that you have reach the middle
	//of second half
	stopforward();
	stopback();
	stopleft();
	stopright();
	PORTD |= 0b00100000; //Start the motor
	_delay_ms(1000); //continue motor for 1 second
	PORTD &= 0b11011111; //Stop the motor
}*/

void initializeColor() {
sei(); // Enable interrupts globally
PCICR = 0b00000101; // Initialize Register B (color sensor) and D (QTI) for PC interrupts
DDRB &= 0b11101111; // Pin PB4 is input pin
DDRB |= 0b00100000; // Pin PB5 is output LED
TCCR1B |=0b00000001; // Set the timer1 prescaler to 1
TCCR1B &=0b11111001;
}

/*void initimer(){
TCCR2B |=0b00000001; // Set the timer2 prescaler to 1 - Step 1
TCCR2B &=0b11111001; // Set the timer2 prescaler to 1 - Step 2
OCR2A = 16000; //Number of clock ticks
//TIMSK2 |= 0b00000010 if enabled and 0 otherwise
}*/

/*void initqti(){
	PCICR = 0b00000101; // Initialize Register B (color sensor) and D (QTI) for PC interrupts
	PCMSK2 |= 0b11000000; // Enable PD6,D7 as a PC interrupts
	DDRD = 0;
}*/

//returns 0 if yellow and 1 if blue
int getColor() {
PCMSK0 |= 0b00010000; // Enable PB4 as a PC interrupt
_delay_ms(10); // Give the interrupt a sec
PCMSK0 &= 0b11101111; // Prevent further interrupts
int time = period*.0625*2; // Change to units of microsecs
/*printf("Period is %d",time);
printf("\n");*/
if (time<200) return 0;
else return 1;
}

int main(void) {
	init_uart(); // Initialize serial
	initializeColor(); // Initialize color sensor
	//initqti();
	//For QTI Pin Change interupt, set appropriate pcvector, set app registers as output, use PCMSK to enable interrupts on those pins,
	//initialize PCICR for app register
	PCMSK2 |= 0b11000000; // Enable PD6,D7 as a PC interrupts
	//initimer();
	int numcolorchanges = 0;
	
	PORTB |= 0b00100000; // Set B5 as output
	int initcolor = 0; // O if yellow, 1 if blue
	//******************CHANGED*******************//
	//int numcolorchanges = 0;
	//******************CHANGED*******************//
	if (getColor() == 1) initcolor = 1;

	DDRB = 0b00001111;
	DDRD = 0b00100000; 



	//Timer 2 used to calculate time since timer1 is used for color change
	//We don't set TIMSK - because this isn't an interrupt, 



    while(1){
		
		//******************CHANGED*******************//

		//if timer value exceed amount determined experimentally, stop, and break
		//replace 'value' with an actual integer
		/*if (TCNT2 > value){
			stopforward();
			break;
		}*/
		//******************CHANGED*******************//

		startback();
		_delay_ms(75); //can vary

		int currcolor = getColor();
		/*printf("Color is: %i",currcolor);
		printf("\n");*/

		//Turn around 180 degrees if you're not on your original color
		if (currcolor != initcolor){
			if (numcolorchanges == 0){
				/*TCNT2 = 0;
				TIMSK2 |= 0b00000010; //if enabled and 0 otherwise*/
				numcolorchanges ++;
				//rest of the logic
		startback();
				_delay_ms(900);
		stopback();
				/*PORTD |= 0b00100000; //Start the motor
				_delay_ms(505); //continue motor for 0.4 seconds
				PORTD &= 0b11011111; //Stop the motor*/
				
   				startleft(); //turn 180 degrees

   				_delay_ms(1000);

   				stopleft();

		startback();
				_delay_ms(1800);
						stopback();

				
				_delay_ms(100000);

			}

			

			/*PCMSK2 &= 0b00000000; // Disable PD6,D7 as a PC interrupts
			stopforward();
			startright(); //turn 180 degrees
			_delay_ms(1440);//adjust to make it 180 degreees
			stopright();
			startforward(); //go ahead so you're back in the original color
			_delay_ms(400);
			stopforward();
			PCMSK2 |= 0b11000000; // Enable PD6,D7 as a PC interrupts*/

		}

	}

}