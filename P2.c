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
uint16_t inicio = 0; // Em us

/* Interrupção executada quando echo sobe/desce */
#pragma vector=TA0_CCR0_INT
__interrupt void TA0_CCR0_ISR(){
	if(TA0CCTL & SCCI){ // Valor da última captura
		distancia = (TA0CCR0-inicio)/58; // 58 está em uma documentação
	}
	else {
		inicio = TA0CCR0;
		distancia = 0;
	}
	TA0CCTL0 &= ~CCIFG;
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    
	/* Configura o botão S1 (P2.1) */
	P2REN |= BIT1;  // Resistor de 
	P2OUT |= BIT1;  // Pull-up
	P2DIR &= ~BIT1; // Entrada
				
	/* Deve-se ligar o sinal de echo na porta P1.1 */
	TA0CCTL0 = (CAP | CM_3 | CCIS_A | CCIE);
	
	/* Deve-se ligar o sinal de trigger na porta P1.2 */
	TA0CCTL1 = OUTMOD_5; // Reset
	TA0CCR1 = TRIGGER_TEMPO-1; // Conta 10us
	
	__enable_interrupt();
	
	while(1){
		// Amostragem para S1
		if(!(P2IN & BIT1)){
			
			TA0CCTL |= (TACLR | TASSEL__SMCLK | MC__CONT); // Zera e configura timer A
					
			debounce();
			while(!(P2IN & BIT1)); // Aguarda soltar
			debounce();
			
		}
	
	}
	
}

