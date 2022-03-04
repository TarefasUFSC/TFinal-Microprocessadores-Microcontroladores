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
#include "lcd.h"

#define _XTAL_FREQ 4000000


#define SENSOR_UMIDADE  PORTAbits.RA0

#define BTN_IRRIGAR     PORTBbits.RB0
#define VALVULA         PORTBbits.RB1
#define BTN_ENTER       PORTBbits.RB2
#define BTN_+           PORTBbits.RB3
#define BTN_-           PORTBbits.RB4
#define LED_SETUP       PORTBbits.RB5
#define LED_IRRIGACAO   PORTBbits.RB6
#define LED_UMIDADE     PORTBbits.RB7

#define LCD_E           PORTDbits.RD0
#define LCD_RS          PORTDbits.RD1
#define LCD_RW          PORTDbits.RD2
#define LCD_D4          PORTDbits.RD4
#define LCD_D5          PORTDbits.RD5
#define LCD_D6          PORTDbits.RD6
#define LCD_D7          PORTDbits.RD7




// mili_s é o tempo desejado pra esperar
// é necessário essa função pois o tempo será variado, dependendo do setup
void setupTimer(int mili_s){
    return;
}


void setupNewVolumeFlow(int new_ml){
    //converter ml em ms
    int new_ms;
    // chama o setup timer com o novo ms
    setupTimer(new_ms);
}

void handleTimerInterruption(){
    return;
}

void handleExternalInterruption(){
    
    return;
}

void __interrupt() interrupcao(void){
    handleTimerInterruption();
    handleExternalInterruption();
    
}


void setupExternalInterruption(){
    return;
    
}

void handleMenu(){
    return;
    
}
void verifyMenu(){
    if(BTN_ENTER){
        handleMenu();
    }
}
void verifySensor(){
    return;
}

void setupWatchdogTimer(){
    return;
}

void main(void) {
    
    setupExternalInterruption();
    setupWatchdogTimer();
    setupTimer(500); //só p ter um valor default 
    Lcd_Init();
    
    while(1){
        verifySensor();
        verifyMenu();
    }
    return;
}
