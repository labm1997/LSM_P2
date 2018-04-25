#include <msp430.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

/*
 * main.c
 */

#define TA1_CCRN_INT 52
#define VELOCIDADE_SOM 32500 // m/s
#define FREQUENCIA 1000000 // Hz

void debounce(){
	volatile int i=1000000;
	while(i--);
}

uint16_t distancia = 0; // Em centímetros

#pragma vector=TA1_CCRN_INT
__interrupt void TA1_CCRN_ISR(){
	switch(TA1IV){
		case 0x04: // CCR2
		/*  Distância é VELOCIDADE_SOM*(tempo até o eco)/2 */
		distancia = (VELOCIDADE_SOM*TA0CCR2/FREQUENCIA) << 1;
	}
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    
	/* Configura o botão S1 (P2.1) */
	P2REN |= BIT1; // Resistor de 
	P2OUT |= BIT1; // Pull-up
	P2DIR &= ~BIT1; // Entrada
	
	/* Se for SMCLK, conta a cada 1us = 0,001ms: de 0ms a ~60ms, para
	   20cm e 40cm e usando 325m/s a velocidade do som deve-se contar até 
	   0,60ms e 1,20ms, dá para usar SMCLK */
	TA0CCTL = (TACLR | TASSEL__SMCLK | MC__HOLD);
	
	while(1){
		// Amostragem para S1
		if(!(P2IN & BIT1)){
		
			TA0CCTL |= TACLR;
			distancia = 0;
			
			/* Faz trigger ser 1 por 10us, podemos usar o timer A */
			TA0CCTL1 = OUTMOD_5; // Reset
			TA0CCR1 = 9; // Ideal, conta de 0 a 9 (10us)
			
			/* Deve-se ligar a porta P1.2 no trigger */
			TA0CCTL |= MC__CONT;
			
			/* Deve-se ligar o sinal de echo na porta P1.3 */
			TA0CCTL2 = (CAP | CM_1 | CCIS_A | CCIE);
					
			debounce();
			while(!(P2IN & BIT1)); // Aguarda soltar
			debounce();
			
		}
	
	}
	
}

