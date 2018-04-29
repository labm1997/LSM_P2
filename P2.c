#include <msp430.h>
#include <stdint.h>
#include <math.h>
//#include <stdio.h>

/*
 * main.c
 */

#define TA0_CCR0_INT 53
#define VELOCIDADE_SOM 34000 // cm/s
#define FREQUENCIA 1000000   // Hz
#define TRIGGER_TEMPO 10     // us

void debounce(){
    volatile uint16_t i=0xffff;
    while(i--);
}

uint16_t distancia = 0; // Em centímetros
uint16_t inicio = 0; // Em us

/* Interrupção executada quando echo sobe/desce */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_CCR0_ISR(){
    if(TA0CTL & SCCI){ // Valor da última captura
    
        /* 58 está em uma documentação */
        distancia = (TA0CCR0-inicio)/58;
        
        /* Muda estado dos LEDs */
        if(distancia < 20) {
            P1OUT |= BIT0;
            P4OUT &= ~BIT7;
        }
        else if(distancia >= 20 && distancia <= 40) {
            P1OUT &= ~BIT0;
            P4OUT |= BIT7;
        }
        else {
            P1OUT |= BIT0;
            P4OUT |= BIT7;
        }
        
    }
    else {
        inicio = TA0CCR0;
        distancia = 0;
    }
    TA0CCTL0 &= ~CCIFG;
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer
    
    /* Configura o botão S1 (P2.1) */
    P2REN |= BIT1;  // Resistor de 
    P2OUT |= BIT1;  // Pull-up
    P2DIR &= ~BIT1; // Entrada
    
    /* Configura o LED vermelho */
    P1OUT &= ~BIT0; // Desligado
    P1DIR |= BIT0;  // Saída
    
    /* Configura o LED verde */
    P4OUT &= ~BIT7; // Desligado
    P4DIR |= BIT7;  // Saída
                
    /* Deve-se ligar o sinal de echo na porta P1.1 */
    TA0CCTL0 = (CAP | CM_3 | CCIS_0 | CCIE);
    
    
    __enable_interrupt();
    
    while(1){
        // Amostragem para S1
        if(!(P2IN & BIT1)){
            
            /* Zera e configura timer A */
            TA0CTL |= (TACLR | TASSEL__SMCLK | MC__CONTINUOUS);
            
            /* Deve-se ligar o sinal de trigger na porta P1.2 */
            TA0CCTL1 = OUTMOD_5; // Reset
            TA0CCR1 = TRIGGER_TEMPO-1; // Conta 10us
            
            debounce();
            while(!(P2IN & BIT1)); // Aguarda soltar
            debounce();
            
        }
    }
    
}

