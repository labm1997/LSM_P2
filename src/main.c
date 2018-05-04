#include <msp430.h>
#include <stdint.h>
#include "clock.h"
#include "pmm.h"

#define S1_DELAY 0xffff
#define TRIGGER_TIME 10

uint16_t valorInicial;

void delay(uint16_t time){
    /* TA0 */
    TA0CTL = (TACLR | MC__CONTINUOUS | TASSEL__SMCLK);
    while(TA0R != time);
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void TA2_CCR0_ISR(){
    if(TA2CCTL0 & CCI) valorInicial = TA2CCR0; // Echo subiu
    else {        
        uint16_t diferenca = TA2CCR0-valorInicial;
        
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
    TA2CCTL0 &= ~TAIFG;
}


void main()
{
    // Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	// Increase core voltage so CPU can run at 16Mhz
	// Step 1 allows clocks up to 12MHz, step 2 allows rising MCLK to 16MHz
	pmmVCore(1);                             
	pmmVCore(2);

	// Configure clock
    // This should make MCLK @16MHz, SMCLK @1MHz and ACLK @32768Hz
	clockInit();

	// Your program here...
	
	
    /* S1 */
    P2REN |=  BIT1;
    P2OUT |=  BIT1; // Pull-up
    P2DIR &= ~BIT1; // Entrada
    
    /* LED1 */
    P1OUT &= ~BIT0;
    P1DIR |=  BIT0; // Saída
    
    /* LED2 */
    P4OUT &= ~BIT7;
    P4DIR |=  BIT7; // Saída
    
    /* P2.3 (TA2.0)*/
    P2SEL |=  BIT3; // Funcionalidade dedicada
    P2DIR &= ~BIT3; // Entrada
    
    /* P2.4 (TA2.1) */
    //P2SEL |= BIT4; 
    P2DIR |=  BIT4; // Saída
    P2OUT &= ~BIT4;
    
    __enable_interrupt();
    
    while(1){
        if(!(P2IN & BIT1)){
            
            /* Zera os LEDs */
            P1OUT &= ~BIT0;
            P4OUT &= ~BIT7;
            
            /* TA2.0 (echo) */
            TA2CCTL0 = (CAP | CM_3 | CCIS_0 | CCIE);
            
            /* TA2 */
            TA2CTL = (TACLR | MC__CONTINUOUS | TASSEL__SMCLK);
            
            /* Manda o trigger */
            P2OUT |= BIT4;
            delay(TRIGGER_TIME-1);
            P2OUT &= ~BIT4;
            
            /* Manda o trigger */
/*            TA2CCR1 = TRIGGER_TIME-1;*/
/*            TA2CCTL1 |= OUTMOD_5;*/
            
            /* Debounce */
            delay(S1_DELAY);
            while(!(P2IN & BIT1));
            delay(S1_DELAY);
        }
    }


}
