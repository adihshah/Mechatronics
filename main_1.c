/*
 * GccApplication3.c
 *
 * Created: 11/22/2019 4:49:12 PM
 * Author : MAE Lab
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include "serial.h"
#include <util/delay.h>
#include <avr/interrupt.h>

int period=0;

//Can't ever have B0,B1 or B2,B3 both high at the same time

void startback(){
	PORTB |= 0b00001001; //go forward
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
	if ((PIND & 0b01000000 == 0b010000000) && (PIND & 0b10000000 == 0b100000000)){ // if left and right detect black, then ???
			printf("Both detect black");
			startright(); //turn 135 degrees
			_delay_ms(900);
			stopright();
		}

		//else if left only, turn 45 to the right
	else if (PIND & 0b01000000 == 0b010000000){
			printf("Left detects black");
			startright(); //turn 45 degrees
			_delay_ms(300);
			stopright();
		}
		
		//else if right only, turn 45 to the left
	else if (PIND & 0b10000000 == 0b100000000){
			printf("Right detects black");
			startleft(); //turn left 45 degrees
			_delay_ms(300);
			stopleft();
		}
}

void initializeColor() {
sei(); // Enable interrupts globally
PCICR = 0b00000101; // Initialize Register B and D for PC interrupts
DDRB &= 0b11101111; // Pin PB4 is input pin
DDRB |= 0b00100000; // Pin PB5 is output LED
TCCR1B |=0b00000001; // Set the timer prescaler to 1
TCCR1B &=0b11111001;
}

//returns 0 if yellow and 1 if blue
int getColor() {
PCMSK0 |= 0b00010000; // Enable PB4 as a PC interrupt
_delay_ms(10); // Give the interrupt a sec
PCMSK0 &= 0b11101111; // Prevent further interrupts
int time = period*.0625*2; // Change to units of microsecs
printf("Period is %d",time);
printf("\n");
if (time<200) return 0;
else return 1
}


/*int main(void){
	while(1){
	PORTB |= 0b00001001; //go forward

	_delay_ms(1400);

   	 PORTB &= 0b11110110;

   	 _delay_ms(1400);
	}
}*/

int main(void) {
	init_uart(); // Initialize serial
	initializeColor(); // Initialize the stuff
	PORTB |= 0b00100000; // Set B5 as output
	int initcolor = 0; // O if yellow, 1 if blue
	if (getColor() == 1) initcolor = 1;

	DDRB = 0b00001111; 

	//For QTI Pin Change interupt, set appropriate pcvector, set app registers as output, use PCMSK to enable interrupts on those pins,
	//initialize PCICR for app register
	PCMSK2 |= 0b11000000; // Enable PD6,D7 as a PC interrupts


    while(1){
	
		startforward()
		_delay_ms(75); //can vary

		int currcolor = getColor();
		printf("Color is: %i",currcolor);
		printf("\n");

		//Turn around 180 degrees if you're not on your original color
		if (currcolor != initcolor){
			stopforward();
			startright(); //turn 180 degrees
			_delay_ms(1200);//adjust to make it 180 degreees
			stopright();
			startforward() //go ahead so you're back in the original color
			_delay_ms(400);
			stopforward();
		}

	}

