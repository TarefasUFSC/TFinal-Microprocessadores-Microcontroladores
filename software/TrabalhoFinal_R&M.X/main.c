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
#include <math.h>

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

#define EN                  PORTDbits.RD0
#define RS                  PORTDbits.RD1
#define RW                  PORTDbits.RD2
#define D4                  PORTDbits.RD4
#define D5                  PORTDbits.RD5
#define D6                  PORTDbits.RD6
#define D7                  PORTDbits.RD7


            

// Adiciona a lib do LCD
#include "lcd.h"

/* ******************* Variáveis Globais ************************/
int timer_counter = 0;
int timer_counter_max = 10;
int irrigacao_ativa = 0;
int umidade_minima = 10; // em %


// 5s = 200ml
// 1s = 40ml
// 1ml = 0.025s = 25ms
int MLxMS = 25;


/****************************************************************/


// passa como parametro a qnt de ms que a valvula tem que ficar aberta
void changeTimerMaxConter(int mili_s){
    
    timer_counter_max = (mili_s/500);
    return;
    
}


// passa o parametro da nova qtd de ml
void setupNewVolumeFlow(int new_ml)
{
    // converter ml em ms
    int new_ms = new_ml*MLxMS;
    // chama o setup timer com o novo ms
    changeTimerMaxConter(new_ms);
    return;
}

void setupTimer()
{
    // Configs de interrupção
    INTCONbits.GIE      = 1;
    INTCONbits.PEIE     = 1;
    PIE1bits.TMR1IE     = 1;
    
    /* Configuração do Timer1 como temporazidaor*/
    T1CONbits.TMR1CS    = 0;
    
    // Define o pre-scaler em 1:8
    T1CONbits.T1CKPS0   = 1;
    T1CONbits.T1CKPS1   = 1;
    
    /* Calculos para o contador
     * clock = 4Mhz -> clock/4 = 1Mhz
     * 1Mhz/8 = 125Khz -> periodo = 0.000008s ou 8ms
     * Para uma interrupção a cada 500ms são necessárias 62500 ciclos de máquina
     * 65536 - 62500 = 3036     
     */
    TMR1H               = 0x0B;
    TMR1L               = 0xDC;
    
    T1CONbits.TMR1ON    = 0;
    
    
    // inicia o contador com um valor padrão de ML
    setupNewVolumeFlow(200);
    return;
}




void handleTimerInterruption()
{
    if(TMR1IF){
        if(irrigacao_ativa){
            
            timer_counter++;
            if(timer_counter_max <= timer_counter){
                VALVULA = 0;
                irrigacao_ativa = 0;
            }
        }
        else{
            timer_counter = 0;
            VALVULA = 0;
        }
        PIR1bits.TMR1IF = 0;
        TMR1H = 0x0B;
        TMR1L = 0xDC;
    }
    return;
}


void irrigar(){
    irrigacao_ativa = 1;
    timer_counter = 0;
    VALVULA = 1;
    
    // ativa o timer 1
    T1CONbits.TMR1ON = 1;
    
    // não pode usar o menu enquanto tiver aqui
    while(irrigacao_ativa);
    
    // desliga o timer 1
    T1CONbits.TMR1ON = 0;
    
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

// Retorna o valor em %
int getADConverterValue(){
    ADCON0bits.GO       = 1;
    __delay_us(10);
    float leitura = 100*ADRESH/256;
    return leitura;
}
void verifySensor()
{
    PORTD = getADConverterValue();
    if(getADConverterValue()<umidade_minima){
        irrigar();
    }
    return;
}

void setupWatchdogTimer()
{
    return;
}


void setupADC(){
    
    
    // define o A0 como entrada analogica
    // e o A3 e A2 como VREFs
    ADCON1bits.PCFG0    = 1;
    ADCON1bits.PCFG1    = 1;
    ADCON1bits.PCFG2    = 1;
    ADCON1bits.PCFG3    = 1;
    
    // clock de conversão
    ADCON1bits.ADCS2    = 1;
    ADCON0bits.ADCS1    = 1;
    ADCON0bits.ADCS0    = 0;
    
    // Configura a conversão em 8 bits
    ADCON1bits.ADFM     = 0;
    
    // inicialização do conversor
    ADRESL              = 0;
    ADRESH              = 0;
    
    
    // Liga o AD
    ADCON0bits.ADON     = 1;
    
    // Seleciona a POrta A0 p ler
    ADCON0bits.CHS0     = 0;
    ADCON0bits.CHS1     = 0;
    ADCON0bits.CHS2     = 0;
    
       
    return;
}


void main(void)
{
    // INICIALIZAÇÕES
    TRISA = 0b00000001;             //configura as portas usadas
    TRISB = 0b00011101;
    TRISD = 0b00000000;
    OPTION_REGbits.nRBPU = 0;       //habilita os resistores de pull-up
    PORTB = 0;
    
    setupExternalInterruption();
    setupWatchdogTimer();
    setupTimer(); 
    setupADC();
    Lcd_Init();
    int a = 0;
    while (1)
    {
        verifySensor();
        verifyMenu();
        
       
    }
    return;
}
