#include <msp430.h> 

#define SMCLKRATE 16000000
#define BAUDRATE 9600 //115200 would be ideal
#define BRDIV16 ((16*SMCLKRATE)/BAUDRATE) //26666
#define BRDIV (BRDIV16/16) //1666
#define BRMOD ((BRDIV16-(16*BRDIV)+1)/16) //24999
#define BRDIVHI (BRDIV/256) //6
#define BRDIVLO (BRDIV-BRDIVHI*256) //130
#define UART_TX 0x04
#define UART_RX 0x02

#define SDA 0x01
#define SCL 0x02

volatile char last_clock;
volatile char last_data;
int bit_count = 0;
char tx_data;

int main(void) {
	//calibration for clock
	BCSCTL1 = CALBC1_16MHZ;
    DCOCTL  = CALDCO_16MHZ;
	//Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;
	
	 //Initialize UART
	 //reset and hold UART state machine
	 UCA0CTL1 = UCSWRST;
	 //SMCLK is clock source
	 UCA0CTL1 |= UCSSEL_2;
	 //set baud parameters, prescaler hi
	 UCA0BR1 = BRDIVHI;
	 //prescaler lo
	 UCA0BR0 = BRDIVLO;
	 //modulation
	 UCA0MCTL = 2*BRMOD;
	 //Connect transmitter pin
	 P1SEL |= UART_TX;
	 P1SEL2 |= UART_TX;
	 //loop mode enabled
	 //UCA0STAT |=  UCLISTEN;
	 //allow the UART state machine to operate
	 UCA0CTL1 &= ~UCSWRST;

	 //Initialze timer
	 TA1CTL |= TACLR; // reset clock
	 //SMCLK is source, divider is zero, up mode, interrupt enabled
	 TA1CTL = TASSEL_2 + ID_0 + MC_1;
	 //capture mode, interrupt on rising and falling edge
	 TA1CCTL0 = CM_3 + SCS + CAP;
	 //interrupt enabled
	 TA1CCTL1 = CCIE;
	 //connect pins
	 P2SEL |= SDA + SCL;
	 P2DIR &= ~(SDA + SCL);
	 //Lines high by default
	 P2REN |= SDA + SCL;

	 _bis_SR_register(GIE + LPM0_bits);
}

void interrupt timer_handler() {

}
ISR_VECTOR(timer_handler, ".int13")

