/*
 * AVRGCC1.c
 *
 * Created: 2012-01-19 21:27:11
 *  Author: Simon
 *	LCDDR19:0 - LCD Segments
 *	-------------------------------------------
 *	For device initialization it is recommended 
 *	that the following values are used: 
 *	drive time 300 microseconds, 
 *	contrast control voltage 3.35 V, 
 *	external asynchronous clock source, 
 *	1/3 bias, 
 *	1/4 duty cycle, 
 *	25 segments enabled, 
 *	prescaler setting N=16, 
 *	clock divider setting D=8, 
 *	LCD enabled, 
 *	low power waveform, no frame interrupt, no blanking.
 *	----------------------------------------------------
 */ 
#include <avr/io.h>
#include <avr/portpins.h>
#include <stdbool.h>
#define true	1
#define false	0

long prime = 2;
int buttonPrevious = 0;

int ASCII_TABLE[10] = 
{
		0x1551,		// 0 = 1551, A = 0F51
		0x0110,		// 1
		0x1e11,		// 2
		0x1B11,		// 3
		0x0B50,		// 4
		0x1B41,		// 5
		0x1F41,		// 6
		0x0111,		// 7
		0x1F51,		// 8
		0x0B51		// 9
};
void Boss(){
	/*	Istället för funktionsanrop sätter vi varje funktions kod under en och samma while loop. 
	*	Detta på grund att funktionsanrop inte är möjligt vid busy-wait, där funktionen tar all
	*	kraft.
	*/
	int buttonPrevious = 0;
	int y;
	int i = 0;
	TCCR1B = 0x04;
	TCNT1 = 0x0000;
	long prime = 2;
	
	// Master while loop!
	while(1){
		
	//Button
	if((PINB >> 7) == 1 & buttonPrevious == 0){
		buttonPrevious = 1;                                                                                                                                          
	}
	if((PINB >> 7) == 0 & buttonPrevious == 1){
		if(LCDDR13 == 0x0 & LCDDR18 == 0x0 & buttonPrevious == 1){
			LCDDR13 = 0x1;
			LCDDR18 = 0x1;
			buttonPrevious = 0;
		}
		if(LCDDR13 == 0x1 & LCDDR18 == 0x1 & buttonPrevious == 1){
			LCDDR13 = 0x0;
			LCDDR18 = 0x0;
			buttonPrevious = 0;
		}
	}	

	//Prime	 
		if(is_prime(prime) == true){
			writeLong(prime);
			prime++;
			i++;
		}
		else{
			prime++;
			i++;
		}
		
	// Blink
		if(TCNT1 >= 0x3D09){
			if(LCDDR3 != 0){
				LCDDR3 = 0x0;
				TCNT1 = 0x0000;
			}
			else{
				LCDDR3 = 0x1;
				TCNT1 = 0x0000;
			}
		}		
	}	
}
void writeChar(char ch, int pos){
	// ch = character we want to write, pos = position on the LCD: 0->5 digits.
	char mask;
	int karaktar = 0x0000;
	char nibbles = 0x00;
	int x;
	char lcddr = 0xec;

	if(pos > 5 | pos < 0){
		return;
	}
	if(pos & 0x01){
        mask = 0x0F;                // Position 1, 3, 5
	}		
    else{
        mask = 0xF0;                // Position 0, 2, 4
	}
	if(ch <= 9 && ch >= 0){
	karaktar = ASCII_TABLE[(int)ch];
	}	
	else{
	karaktar = 0x0000;
	}	
	lcddr += (pos>>1); // (0,0,1,1,2,2) (LCDDR0, LCDDR0) (LCDDR1, LCDDR1) (LCDDR2, LCDDR2)

	for(x=0; x<4; x++){
		nibbles = karaktar & 0x000f;
		karaktar = karaktar >> 4;
		if(pos & 0x01){					// True om 1,3,5
			nibbles = nibbles << 4;
		}
		_SFR_MEM8(lcddr) = ((_SFR_MEM8(lcddr) & mask) | nibbles);
		lcddr += 5;
	}
}
int writeLong(long i){	
	/*	Converts the long integer i to a string of characters.
	*	Calls writeChar, with position.
	*	If more than 6 characters, show atleast 6 digits.
	*	(sizeof(array)/sizeof(int))
	*/	
	char ch;		
	int steg;
	int position = 5;
	for(steg = 0; steg < 6 ;steg++){
		ch = (i % 10);
		writeChar(ch, position);
		i = (i / 10);
		position--;
		}			
}
int primes(long value){
	/*	Creates prime numbers to be printed.	
	 *	A long variable is incremented for each turn in a loop.
	 */
	int i = 0;
	while(i < 27000){			 
		if(is_prime(value) == true){
			writeLong(value);
			value++;
			i++;
		}
		else{
			value++;
			i++;
		}
	}				
}
int is_prime(long i){
	/*	Checks if it's a prime number.
	*	Done by computing i % n, 
	*	(i.e., the remainder from division i/n) for all 2 <= n < i, 
	*	and returning false (0) if any such expression is 0, 
	*	true (1) otherwise.
	*/
	int start;
	for(start = 2; start < i; start++){
		if ((i % start) == 0) {
			return false;
		}		
	}
	return true;
}
void blink(){
	// Read Timer/counter1 for it to reach MAX.
	TCCR1B = 0x04;						// prescaling factor = 256.
	TCNT1 = 0x0000;						// Reset the register.
	//unsigned int i;						// Current value for TCNT1.
	//unsigned int timer_value = 0xFFFF;	// The MAX value that we want TCNT1 to have.
	while(1){
		if(TCNT1 >= 0x3D09){
			if(LCDDR0 != 0){
				LCDDR0 = 0x00;
				TCNT1 = 0x0000;
			}
			else{
				LCDDR0 = 0x04;
				TCNT1 = 0x0000;
			}
		}
	}	
}
void button(){
	while(1){
		if((PINB >> 7) == 0 & buttonPrevious == 0){
			buttonPrevious = 1;
		}
		if((PINB >> 7) == 1 & buttonPrevious == 1){
			if(LCDDR13 == 0x0 & LCDDR18 == 0x0 & buttonPrevious == 1){
				LCDDR13 = 0x1;
				LCDDR18 = 0x1;
				buttonPrevious = 0;
			}
			if(LCDDR13 == 0x1 & LCDDR18 == 0x1 & buttonPrevious == 1){
				LCDDR13 = 0x0;
				LCDDR18 = 0x0;
				buttonPrevious = 0;
			}
		}
	}
}

int main(void){
	// Clock prescaler
    CLKPR = 0x80;	//(1<<CLKPCE);
	CLKPR =	0x00;	//(0<<CLKPS3);
	
	// Device Initialization values:
	LCDCRA = 0xC0;	//(1<<LCDEN)  | (1<<LCDAB);						//0xC0;
	LCDCRB = 0xB7;	//(1<<LCDCS)  | (3<<LCDMUX0) | (7<<LCDPM0);		//0xB7;
	LCDFRR = 0x07;	//(0<<LCDPS0) | (7<<LCDCD0);					//0x07;
	LCDCCR = 0x0F;	//(15<<LCDCC0);								//0x0F;
	PORTB = (1 << PORTB7);			//Button Setting.
	
	//Boss();
	//writeChar(5,3);
	//writeLong(42);
	//writeLong(42236);
	//writeLong(99991234);
	//writeLong(42);
	//primes(2);
	//primes(2000);
	//primes(10000);
	//primes(25000);
	//blink();
	button();
	
}
