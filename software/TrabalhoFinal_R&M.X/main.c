/*******************************************************************************
    UFSC- Universidade Federal de Santa Catarina
    *Project Name:
 Trabalho Final - Irrigação
    *Copyright:;
            Rodrigo Ferraz Souza
            Manoella Rockembach
    *Test Configuration:
         MCU:              P16F877A
         Oscillator:       HS, 4.0 MHz
         Ext. Modules:     -
    *NOTES:

*******************************************************************************/

#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>
#include <xc.h>


#define _XTAL_FREQ 4000000

//configurando o watchdog time
//#pragma config WDTE = ON //habilita o uso do WDT
//#pragma config FOSC = HS //define uso do clock externo em 4 ou 20Mhz
//#pragma config PWRTE = ON //habilita reset ao ligar (pode ser usado no lugar do capacitor)
//#pragma config BOREN = ON //Habilita o reset por Brown-out (vales na tensão)

#define SENSOR_UMIDADE      PORTAbits.RA0

#define BTN_IRRIGAR         PORTBbits.RB0
#define VALVULA             PORTBbits.RB1
#define BTN_ENTER           PORTBbits.RB2
#define BTN_INC             PORTBbits.RB3
#define BTN_DEC             PORTBbits.RB4
#define LED_SETUP           PORTBbits.RB5
#define LED_IRRIGACAO       PORTBbits.RB6
#define LED_UMIDADE         PORTBbits.RB7

//#define EN                  PORTDbits.RD0
//#define RS                  PORTDbits.RD1
//#define RW                  PORTDbits.RD2
//#define D4                  PORTDbits.RD4
//#define D5                  PORTDbits.RD5
//#define D6                  PORTDbits.RD6
//#define D7                  PORTDbits.RD7

//*** define pinos referentes a interface com LCD
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

// Adiciona a lib do LCD
#include "lcd.h"

// mili_s e o tempo desejado pra esperar
// e necessario essa funcao pois o tempo sera variado, dependendo do setup
void setupTimer(int mili_s)
{
    return;
}

void setupNewVolumeFlow(int new_ml)
{
    // converter ml em ms
    int new_ms;
    // chama o setup timer com o novo ms
    setupTimer(new_ms);
    return;
}

void handleTimerInterruption()
{
    return;
}

void handleExternalInterruption()
{
    if(INTF){
        //if(!irrigacao_ativa)
        //irrigar
        LED_IRRIGACAO = 1;
    }
    return;
}

void __interrupt() interrupcao(void)
{
    handleTimerInterruption();
    handleExternalInterruption();
    return;
}

void setupExternalInterruption()
{
    OPTION_REGbits.INTEDG = 0;
	INTCONbits.GIE = 1;
	INTCONbits.INTE = 1;
    return;
}

void handleMenu()
{
    return;
}
void verifyMenu()
{
    if (BTN_ENTER)
    {
        handleMenu();
    }
    return;
}
void verifySensor()
{
    return;
}

void setupWatchdogTimer()
{
    //OPTION_REGbits.PSA = 1; //define que o prescaler esta associado ao WTD
    //OPTION_REGbits.PS0 = 0; // define o prescaler do WTD para 1:64 (1152ms)
    //OPTION_REGbits.PS1 = 1;
    //OPTION_REGbits.PS2 = 1;
    //CLRWDT();
    return;
}

void main(void)
{
    // INICIALIZAÇÕES
    TRISA = 0b00000001;             //configura as portas usadas
    TRISB = 0b00011101;
    TRISD = 0b00000000;
    OPTION_REGbits.nRBPU = 0;       //habilita os resistores de pull-up
    
    setupExternalInterruption();
    setupWatchdogTimer();
    setupTimer(500); // so p ter um valor default
    Lcd_Init();
    
    PORTB = 0;

    while (1)
    {
        verifySensor();
        verifyMenu();
        Lcd_Clear();                    //limpa LCD
        Lcd_Set_Cursor(1,1);            //Poe cursor linha 1 coluna 1
    
        Lcd_Write_String("OLA MUNDO");  //escreve string
        __delay_ms(1000);
    
        Lcd_Set_Cursor(2,1);             //linha 2 coluna 1
        Lcd_Write_String("MUNDO DOIDO"); //escreve string]
        __delay_ms(2000);
    }
    return;
}
