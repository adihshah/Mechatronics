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
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include "serial.h"
#include <util/delay.h>
#include <avr/interrupt.h>

int period=0;

//Can't ever have B0,B1(Motor1) or B2,B3(Motor2) both high at the same time
//Originally:
//Right wheel is B0 and B1
//Left wheel is B2 and B3

//Right wheel is B0 and B1
//Change wiring of right wheel such that B0 -> B1 and B1 -> B2
//Start forward - B1 -> 1 B0 -> 0 becomes B2 -> 1 and B1 -> 0
//Start back - B1 -> 0 and B0 -> 1 become B2 -> 0 and B1 -> 1
//Use timer1 with pins B1 and B2
// pwm on 9 and 10 (OC1A and OC1B)

//Left wheel is B2 and B3
//Change wiring of left wheel such that B2 -> D3 and B3 -> B3
//Start forward - B2 -> 1 B3 -> 0 becomes D3 -> 1 and B3 -> 0
//Start back - B2 -> 0 and B3 -> 1 become D3 -> 0 and B3 -> 1
//Use timer2 with pins B3 and D3
// pwm on 3 and 11 (OC2B and OC2A)

void startforward(){
	PORTB |= 0b00000100;
	PORTD |= 0b00001000;
}

void stopforward(){
	PORTB &= 0b11111011;
	PORTD &= 0b11110111;
}

void startback(){
	PORTB |= 0b00001010;
}

void stopback(){
	PORTB &= 0b11110101;
}


//Acc to old wiring where //Right wheel is B0 and B1
//Left wheel is B2 and B3
/*
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
*/

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
	DDRD = 0b00101000; 



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

		startforward();
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
				startforward();
				_delay_ms(900);
				stopforward();
				PORTD |= 0b00100000; //Start the motor
				_delay_ms(480); //continue motor for 0.4 seconds
				PORTD &= 0b11011111; //Stop the motor
				startback();
				_delay_ms(1800);
				stopback();

				//Start PWM now
				
				//So set OCR1A and OCR1B, both to be out of phase
				//Use timer1 with pins B1 and B2
				// pwm on 9 and 10 (OC1A and OC1B)
				DDRB |= 0b00000110;

				
				// Set the timer1 prescaler to 1
				// CS12,CS11,CS10 = 001 (table 20-7)
				// select waveform generation mode 8 (phase and frequency correct PWM)
				// WGM13,WGM12,WGM11,WGM10 = 1000 (table 20-7)
				// set COM bits. clear OC1A on up-counting, set on down-counting; set OC1B on up-counting, clear on down-counting (this makes them out of phase)
				// COM1A1,COM1A0 = 10 (Table 20-5)// COM1B1,COM1B0 = 11 (Table 20-5)
				TCCR1A = 0b10110000;
				TCCR1B = 0b00010001;

				// in Table 20-6, TOP is the maximum value the timer will count up // to. That value is stored in this register.
				ICR1 = 0xFFFF;

				deadtime = 10;
				
				// our PWM set point (between 10 and 65,525 to prevent overflow
				// 10 will be full speed one direction, 65,525 will be full speed
				// in the other direction, halfway will be motor stopped
				setpoint = 32767;
				OCR1A = setPoint–deadTime; // set output compare registers such that OCR1B>OCR1A
				OCR1B = setPoint+ deadTime; // this ensures dead time as shown on prev. slide
				//Sets its in PWM for the right wheel 

				DDRB |= 0b00001000;
				DDRD |= 0b00001000;

				// Set the timer1 prescaler to 1
				// CS22,CS21,CS20 = 001 (page 206)
				// select waveform generation mode 5 (phase correct PWM)
				// WGM22,WGM21,WGM20 = 101 (page 205 )
				// set COM bits. clear OC1A on up-counting, set on down-counting; set OC1B on up-counting, clear on down-counting (this makes them out of phase)
				// COM2A1,COM2A0 = 10 (Table 20-5)// COM2B1,COM2B0 = 11 (Table 20-5)
				TCCR1A = 0b10110001;
				TCCR1B = 0b00001001;
				
				OCRA = 0xFF;
				deadtime = 10;
				setpoint = 127;
				OCR1A = setPoint–deadTime; // set output compare registers such that OCR1B>OCR1A
				OCR1B = setPoint+ deadTime; // this ensures dead time as shown on prev. slide
				//Sets its in PWM for the left wheel 
			
				while (1){
					
				}
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