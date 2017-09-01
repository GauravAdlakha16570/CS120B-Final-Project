#include <stdlib.h>
#include "io.c"
#include <avr/interrupt.h>
#include <stdio.h>

volatile unsigned char TimerFlag = 0; 

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	
	TCCR1B = 0x0B;
	OCR1A = 125;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int A0, A1, A2, A3 = 0;



int main (void)
{
	TimerOn();
	TimerSet(100);
	
	DDRA = 0x00; PORTA = 0xFF; // Buttons
	DDRB = 0xFF; PORTB = 0x00; // LEDS
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	LCD_init();
	LCD_DisplayString(6, "SIMON      PRESS TO START");

	unsigned char r = rand() % 4;
	unsigned char i = 0;
	unsigned char gameStarted = 0;
	unsigned char loopIteration = 0;
	unsigned char scoreNum = 0;
	unsigned char lostFlag = 0;
	unsigned char lvlArr[6] = {4, 5, 6, 7, 8, 9};
	unsigned char incArraySize = 0;
	
	while(1){
		
		A0 = ~PINA & 0x01;
		A1 = ~PINA & 0x02;
		A2 = ~PINA & 0x04;
		A3 = ~PINA & 0x08;

		if ( (A0 == 0x01 || A1 == 0x02 || A2 == 0x04 || A3 == 0x08) && (gameStarted == 0) ) {
			gameStarted = 1; //flag for beginning of game
			LCD_ClearScreen();
			LCD_DisplayString(1, "Score: ");
			LCD_WriteData('0');
			
		}
		
		else if ( gameStarted == 1 ) {
			
			if ( lvlArr[i] == 4 ) {
				loopIteration = 0x04;
			}
			
			else if ( lvlArr[i] == 5 ) {
				loopIteration = 0x05;
			}
			
			else if ( lvlArr[i] == 6 ) {
				loopIteration = 0x06;
			}
			
			else if ( lvlArr[i] == 7 ) {
				loopIteration = 0x07;
			}
			
			else if ( lvlArr[i] == 8 ) {
				loopIteration = 0x08;
			}
			
			else if ( lvlArr[i] == 9 ) {
				loopIteration = 0x09;
			}
			
			unsigned char inputs[9]; //9 is max amount of iterations
			unsigned char j = 0;

			for ( j = 0; j < loopIteration; j++) {
				r = rand() % 4;
				while(!TimerFlag);
				
				TimerFlag = 0;
				PORTB = 0x00;
				
				while(!TimerFlag);
				TimerFlag = 0;
				
				if ( r == 0 ) {
					PORTB = 0x01;
					inputs[j] = 0x01;
					incArraySize++;
				}
				
				else if  ( r == 1 )  {
					PORTB = 0x02;
					inputs[j] = 0x02;
					incArraySize++;
				}
				
				else if ( r == 2 ) {
					PORTB = 0x04;
					inputs[j] = 0x04;
					incArraySize++;
				}
				
				else if ( r == 3 ) {
					PORTB = 0x08;
					inputs[j] = 0x08;
					incArraySize++;
				}
				
				while(!TimerFlag);
				TimerFlag = 0;
				PORTB = 0x00;
				
				while(!TimerFlag); //slow loop so game is playable
				TimerFlag = 0;
			}
			
			unsigned char userIn = 0;
			unsigned char k = 0;
			while( userIn == 0) {
				
				A0 = ~PINA & 0x01;
				A1 = ~PINA & 0x02;
				A2 = ~PINA & 0x04;
				A3 = ~PINA & 0x08;

				PORTB = 0;
				
				if ( k < incArraySize ) {
					
					if ( A0 != 0x01 && A1 != 0x02 && A2 != 0x04 && A3 != 0x08) {
						continue;
					}
					
					else if ( inputs[k] == 0x01 && A0 == 0x01 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x01;
					
						k++;
					}
					
					else if ( inputs[k] == 0x02 && A1 == 0x02 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x02;
						
						
						k++;
					}
					
					else if ( inputs[k] == 0x04 && A2 == 0x04 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x04;
						
						
						k++;
					}
					
					else if ( inputs[k] == 0x08 && A3 == 0x08 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x08;
						
						k++;
					}
					
					else {
						LCD_DisplayString(1, "Wrong! You lose!!!”);
						lostFlag = 1;
						
						break;
					}
					
				}
				
				if (k == incArraySize) {
					inputs[9] = 0;
					incArraySize = 0;
					userIn = 1;
					k = 0;
					i++;
					scoreNum++;
				}
			}
			
			if (lostFlag == 1) {
				userIn = 1;
				if (i > 0) {
					i—;
				}
				else {
					i = 0;
				}
				
				k = 0;
				lostFlag = 0;
				incArraySize = 0;
				inputs[9] = 0;
				
				LCD_DisplayString(1, "Your Score: ");
				LCD_WriteData('0');
				
				while(!TimerFlag);
				TimerFlag = 0;
				PORTB = 0x00;
				
				while(!TimerFlag);
				TimerFlag = 0;
			}
			else {
				PORTB = 0x00;
				LCD_DisplayString(1, "Your Score: ");
				LCD_WriteData('0' + scoreNum);
				
				while(!TimerFlag);
				TimerFlag = 0;
				
				if ( scoreNum == 6 ) {
					LCD_DisplayString(1, "You Win!!!");
					break;
				}
			}
			
		}
		
	}
}