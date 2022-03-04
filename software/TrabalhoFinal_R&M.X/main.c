/*******************************************************************************
    UFSC- Universidade Federal de Santa Catarina
    *Project Name:
 Trabalho Final - Irrigaï¿½ï¿½o
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

#define SENSOR_UMIDADE      PORTAbits.RA0

#define BTN_IRRIGAR         PORTBbits.RB0
#define VALVULA             PORTBbits.RB1
#define BTN_ENTER           PORTBbits.RB2
#define BTN_INC             PORTBbits.RB3
#define BTN_DEC             PORTBbits.RB4
#define LED_SETUP           PORTBbits.RB5
#define LED_IRRIGACAO       PORTBbits.RB6
#define LED_UMIDADE         PORTBbits.RB7

#define E                   PORTDbits.RD0
#define RS                  PORTDbits.RD1
#define RW                  PORTDbits.RD2
#define D4                  PORTDbits.RD4
#define D5                  PORTDbits.RD5
#define D6                  PORTDbits.RD6
#define D7                  PORTDbits.RD7
#define EN                  PORTDbits.RD3


// Adiciona a lib do LCD
#include "lcd.h"

// mili_s ï¿½ o tempo desejado pra esperar
// ï¿½ necessï¿½rio essa funï¿½ï¿½o pois o tempo serï¿½ variado, dependendo do setup
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
    setupTimer(500); // sï¿½ p ter um valor default
    Lcd_Init();

    while (1)
    {
        verifySensor();
        verifyMenu();
    }
    return;
}
