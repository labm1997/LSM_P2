#include <msp430.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

/*
 * main.c
 */

#define TA0_CCR0_INT 53
#define VELOCIDADE_SOM 32500 // cm/s
#define FREQUENCIA 1000000 // Hz
#define TRIGGER_TEMPO 10 // us

void debounce(){
	volatile int i=1000000;
	while(i--);
}

uint16_t distancia = 0; // Em centímetros

/* Interrupção executada quando echo sobe/desce */
#pragma vector=TA0_CCR0_INT
__interrupt void TA0_CCR0_ISR(){
	if(TA0CCTL & SCCI){ // Valor da última captura
		/* !TODO Como calcula distância com TA0CCR0? */
	}
	else {
		distancia = 0;
		TA0CCTL |= (TACLR | MC__CONT); // Começa a contar no TA0
	}
	TA0CCTL0 &= ~CCIFG;
}

void geraSinalTrigger(){
	
	/* Faz trigger ser 1 por 10us */
	TA1CCTL1 = OUTMOD_5; // Reset
	TA1CCR1 = TRIGGER_TEMPO-1; // Ideal, conta de 0 a 9 (10us)
	
	/* Timer para trigger, saída */
	TA1CCTL = (TACLR | TASSEL__SMCLK | MC__CONT);
	
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    
	/* Configura o botão S1 (P2.1) */
	P2REN |= BIT1;  // Resistor de 
	P2OUT |= BIT1;  // Pull-up
	P2DIR &= ~BIT1; // Entrada
	
	/* Timer para echo, entrada
	   Se for SMCLK, conta a cada 1us = 0,001ms: de 0ms a ~60ms 
	 */
	TA0CCTL = (TACLR | TASSEL__SMCLK | MC__HOLD);
	
	__enable_interrupt();
	
	while(1){
		// Amostragem para S1
		if(!(P2IN & BIT1)){
			
			geraSinalTrigger();
			
			TA1CCTL |= TACLR;
			
			/* Deve-se ligar o sinal de echo na porta P1.1 */
			TA0CCTL0 = (CAP | CM_3 | CCIS_A | CCIE);
					
			debounce();
			while(!(P2IN & BIT1)); // Aguarda soltar
			debounce();
			
		}
	
	}
	
}

