#include <inttypes.h>
#include <tm4c123gh6pm.h>

// Worm jumper = PB0 - Action 1
// Yellow jumper = PB1 - Action 2
// Green jumper = PB2 - Action 3
// Grey jumper = PB3 - Action 4
// Orange jumper = PB4 - START
// White jumper = PB5 - SELECT
// Blue jumper = PA2 - Direction 1 <
// Brown jumper = PA3 - Direction 2 >
// Black jumper = PA4 - Direction 3 ^
// Red jumper = PA5 - Direction 4 v
// Red jumper = PA6 - Action 5 (ANALOG1)
// White jumper = PD1 - X axis
// Brown jumper = PD0 - Y axis

uint32_t leituraEixoX;
uint32_t leituraEixoY;
uint32_t i = 0;
uint32_t tempo = 200000; // 4.000.000 = 1000ms = 1s || 200000 = 50ms

unsigned char moves[20];
unsigned char data;


void SYSTICK_Init(void){

    NVIC_ST_RELOAD_R = tempo; // Set reload rate
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE; // Enables interruption and SYSTICK
}

void ANALOG_Init(void){


    SYSCTL_RCGCGPIO_R |= 0x08; // Enables PORTD
    SYSCTL_RCGCADC_R = 0x03; // Activates ADC0 & ADC1
    GPIO_PORTD_AFSEL_R |= 0x03; // Selected pins will use alternate function
    GPIO_PORTD_AMSEL_R |= 0x03; // Enables analog function

    ADC0_ACTSS_R = 0x00; // Disables sample sequencer for configuration
    ADC0_EMUX_R = 0x0F; // Select trigger source for SS
    ADC0_SSMUX0_R = 0x07; // Which AIN for which SS position
    ADC0_SSCTL0_R = 0x02; // Defines the last sample for SS
    ADC0_ACTSS_R = 0x01; // Activates sample sequencer
    ADC0_PSSI_R = 0x01; // Starts sampling


    ADC1_ACTSS_R = 0x00; // Disables sample sequencer for configuration
    ADC1_EMUX_R = 0x0F; // Select trigger source for SS
    ADC1_SSMUX0_R = 0x06; // Which AIN for which SS position
    ADC1_SSCTL0_R = 0x02; // Defines the last sample for SS
    ADC1_ACTSS_R = 0x01; // Activates sample sequencer
    ADC1_PSSI_R = 0x01; // Starts sampling
}


void BOT_Init(void){

    SYSCTL_RCGCGPIO_R |= 0x03; // Enables PORTA & PORTB
    GPIO_PORTA_DATA_R = 0x00; // Clears PORTA
    GPIO_PORTB_DATA_R = 0x00; // Clears PORTB
    GPIO_PORTA_PUR_R |= 0x40; // Pull up resistor for Analog button

    GPIO_PORTA_IS_R = 0x00; // Defines it as edge-sensitive
    GPIO_PORTB_IS_R = 0x00; // Defines it as edge-sensitive
    GPIO_PORTA_IEV_R = 0x00; // Defines it as falling-edge
    GPIO_PORTB_IEV_R = 0x00; // Defines it as falling-edge
    GPIO_PORTA_IBE_R = 0x00; // Interruption is controlled by IEV
    GPIO_PORTB_IBE_R = 0x00; // Interruption is controlled by IEV

    GPIO_PORTA_IM_R = 0x7C; // Enables pin interruption
    GPIO_PORTB_IM_R = 0x3F; // Enables pin interruption

    NVIC_EN0_R |= 1 << 0; // Enables interruption on ports
    NVIC_EN0_R |= 1 << 1; // Enables interruption on ports

    GPIO_PORTA_DEN_R |= 0x7C; // Enables PINS
    GPIO_PORTB_DEN_R |= 0x3F; // Enables PINS
}

void UART0_Init(void){

    SYSCTL_RCGCUART_R |= 0x01; // Initializes UART0
    SYSCTL_RCGCGPIO_R |= 0x01; // Enables PORTA
    GPIO_PORTA_AFSEL_R |= 0x03; // Selected pins will use alternate function
    GPIO_PORTA_PCTL_R |= 0x11; // Select the alternate function
    UART0_CTL_R = 0x00; // Disables UART for configuration
    UART0_IBRD_R = 8; //  integer (16 000 000 / (16 * 115 200)) = integer (8.68055)
    UART0_FBRD_R = 44; // integer (0.68055 * 64 + 0.5) = integer (44.0552)
    UART0_LCRH_R = 0x70; // Configures UART
    UART0_CC_R = 0x00; // Set baud clock = System clock
    UART0_CTL_R = 0x301; // Enables RXE, TXE & UART itself
    GPIO_PORTA_DEN_R |= 0x03; // Enables PIN PA0 & PA1
}

void UART0_OutChar(unsigned char data){

    while((UART0_FR_R & UART_FR_TXFF) != 0){} // Verifies if transmitter is full
    UART0_DR_R = data; // Data register
}


void TrataSystick(void)
{
    leituraEixoY = ADC0_SSFIFO0_R;
    leituraEixoX = ADC1_SSFIFO0_R;

    if(leituraEixoX > 2400){
        UART0_OutChar('R');
    }
    if(leituraEixoX < 1700){
        UART0_OutChar('L');
    }
    if(leituraEixoY > 2400){
        UART0_OutChar('U');
    }
    if(leituraEixoY < 1700){
        UART0_OutChar('N');
    }


    for(i = 0; i < 20; i++)
    {
        if(moves[i] != '0')
        {
            UART0_OutChar(moves[i]);
        }
    }
    for(i = 0; i < 20; i++)
    {
        moves[i] = '0';
    }
    i = 0;
}
void TrataPortA(void)
{
    if ((GPIO_PORTA_RIS_R & 0x04) == 0x04) // Verifies which pin generated interruption (PA2)
                    {
                        GPIO_PORTA_ICR_R = 0x04; // Clears interruption
                        moves[i] = '<';
                        i++;
                    }
    if ((GPIO_PORTA_RIS_R & 0x08) == 0x08) // Verifies which pin generated interruption (PA3)
                    {
                        GPIO_PORTA_ICR_R = 0x08; // Clears interruption
                        moves[i] = '>';
                        i++;
                    }
    if ((GPIO_PORTA_RIS_R & 0x10) == 0x10) // Verifies which pin generated interruption (PA4)
                    {
                        GPIO_PORTA_ICR_R = 0x10; // Clears interruption
                        moves[i] = '^';
                        i++;
                    }
    if ((GPIO_PORTA_RIS_R & 0x20) == 0x20) // Verifies which pin generated interruption (PA5)
                    {
                        GPIO_PORTA_ICR_R = 0x20; // Clears interruption
                        moves[i] = 'v';
                        i++;
                    }
    if ((GPIO_PORTA_RIS_R & 0x40) == 0x40) // Verifies which pin generated interruption (PA6)
                    {
                        GPIO_PORTA_ICR_R = 0x40; // Clears interruption
                        moves[i] = '*';
                        i++;
                    }
}

void TrataPortB(void)
{
    if ((GPIO_PORTB_RIS_R & 0x01) == 0x01) // Verifies which pin generated interruption (PB0)
                {
                    GPIO_PORTB_ICR_R = 0x01; // Clears interruption
                    moves[i] = 'A';
                    i++;
                }
    if ((GPIO_PORTB_RIS_R & 0x02) == 0x02) // Verifies which pin generated interruption (PB1)
                {
                    GPIO_PORTB_ICR_R = 0x02; // Clears interruption
                    moves[i] = 'B';
                    i++;
                }
    if ((GPIO_PORTB_RIS_R & 0x04) == 0x04) // Verifies which pin generated interruption (PB2)
                {
                    GPIO_PORTB_ICR_R = 0x04; // Clears interruption
                    moves[i] = 'C';
                    i++;
                }
    if ((GPIO_PORTB_RIS_R & 0x08) == 0x08) // Verifies which pin generated interruption (PB3)
                {
                    GPIO_PORTB_ICR_R = 0x08; // Clears interruption
                    moves[i] = 'D';
                    i++;
                }
    if ((GPIO_PORTB_RIS_R & 0x10) == 0x10) // Verifies which pin generated interruption (PB4)
                {
                    GPIO_PORTB_ICR_R = 0x10; // Clears interruption
                    moves[i] = 'E';
                    i++;
                }
    if ((GPIO_PORTB_RIS_R & 0x20) == 0x20) // Verifies which pin generated interruption (PB5)
                {
                    GPIO_PORTB_ICR_R = 0x20; // Clears interruption
                    UART0_OutChar(35);
                }
}

int main(void) {

    SYSTICK_Init();
    BOT_Init();
    UART0_Init();
    ANALOG_Init();

    while(1)
    {
    }
}
