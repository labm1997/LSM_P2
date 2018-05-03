#include <msp430.h>
#include <stdint.h>
#define S1_DELAY 0xffff
#define TRIGGER_TIME 10

uint16_t valorInicial;

void delay(uint16_t time){
    /* TA0 */
    TA0CTL = (TACLR | MC__CONTINUOUS | TASSEL__SMCLK);
    while(TA0R != time);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1_CCR0_ISR(){
    if(TA1CCTL0 & CCI) valorInicial = TA1CCR0; // Echo subiu
    else {
        uint16_t diferenca = TA1CCR0-valorInicial;
        
        if(diferenca < 1160) {
            P1OUT |= BIT0;
            P4OUT &= ~BIT7;
        }
        else if(diferenca >= 1160 && diferenca <= 2320) {
            P1OUT &= ~BIT0;
            P4OUT |= BIT7;
        }
        else {
            P1OUT |= BIT0;
            P4OUT |= BIT7;
        }
    }
    TA1CCTL0 &= ~TAIFG;
}

void main(){

    /* S1 */
    P2REN |=  BIT0;
    P2OUT |=  BIT0; // Pull-up
    P2DIR &= ~BIT0; // Entrada
    
    /* LED1 */
    P1OUT &= ~BIT0;
    P1DIR |=  BIT0; // Saída
    
    /* LED2 */
    P4OUT &= ~BIT7;
    P4DIR |=  BIT7; // Saída
    
    /* P1.7 (TA1.0)*/
    P1SEL |=  BIT7; // Funcionalidade dedicada
    P1DIR &= ~BIT7; // Entrada
    
    /* P2.0 (TA1.1) */
    P2SEL |=  BIT0; // Funcionalidade dedicada
    P2DIR |=  BIT0; // Saída
    
    __enable_interrupt();
    
    while(1){
        if(!(P2IN & BIT0)){
        
            /* TA1 */
            TA1CTL = (TACLR | MC__STOP | TASSEL__SMCLK);
            
            /* TA1.0 (echo) */
            TA1CCTL0 = (CAP | CM_3 | CCIS_0 | CCIE);
            
            /* Inicia contagem */
            TA1CTL |= MC__CONTINUOUS;
            
            /* TA1.1 (trigger) */
            TA1CCTL1 = OUTMOD_5; // Modo reset
            TA1CCR1 = TRIGGER_TIME-1;
            
            /* Debounce */
            delay(S1_DELAY);
            while(!(P2IN & BIT0));
            delay(S1_DELAY);
        }
    }
    
}
