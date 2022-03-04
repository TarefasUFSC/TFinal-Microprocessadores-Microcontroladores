/*******************************************************************************
    UFSC- Universidade Federal de Santa Catarina
    *Project Name:
 Trabalho Final - Irriga��o
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

//configurando o watchdog time
//#pragma config WDTE = ON //habilita o uso do WDT
//#pragma config FOSC = HS //define uso do clock externo em 4 ou 20Mhz
//#pragma config PWRTE = ON //habilita reset ao ligar (pode ser usado no lugar do capacitor)
//#pragma config BOREN = ON //Habilita o reset por Brown-out (vales na tens�o)

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

/* ******************* Vari�veis Globais ************************/
int timer_counter = 0;
int timer_counter_max = 10;
int irrigacao_ativa = 0;
int umidade_minima = 10; // em %
int water_volume = 200;
int setup_menu = 0;


// 5s = 200ml
// 1s = 40ml
// 1ml = 0.025s = 25ms
int MLxMS = 25;


int ext_int_ativa = 0;

/****************************************************************/


// passa como parametro a qnt de ms que a valvula tem que ficar aberta
void changeTimerMaxConter(int mili_s){
    
    timer_counter_max = (mili_s/500);
    PORTC = timer_counter_max;
    return;
    
}


// passa o parametro da nova qtd de ml
void setupNewVolumeFlow()
{
    // converter ml em ms
    int new_ms = water_volume*MLxMS;
    // chama o setup timer com o novo ms
    changeTimerMaxConter(new_ms);
    return;
}

void setupTimer()
{
    // Configs de interrup��o
    INTCONbits.GIE      = 1;
    INTCONbits.PEIE     = 1;
    PIE1bits.TMR1IE     = 1;
    
    /* Configura��o do Timer1 como temporazidaor*/
    T1CONbits.TMR1CS    = 0;
    
    // Define o pre-scaler em 1:8
    T1CONbits.T1CKPS0   = 1;
    T1CONbits.T1CKPS1   = 1;
    
    /* Calculos para o contador
     * clock = 4Mhz -> clock/4 = 1Mhz
     * 1Mhz/8 = 125Khz -> periodo = 0.000008s ou 8ms
     * Para uma interrup��o a cada 500ms s�o necess�rias 62500 ciclos de m�quina
     * 65536 - 62500 = 3036     
     */
    TMR1H               = 0x0B;
    TMR1L               = 0xDC;
    
    T1CONbits.TMR1ON    = 0;
    
    
    // inicia o contador com um valor padr�o de ML
    setupNewVolumeFlow();
    return;
}




void handleTimerInterruption()
{
    if(TMR1IF){
        if(irrigacao_ativa){
            timer_counter++;
            VALVULA = 1;
            LED_IRRIGACAO = 1;
            if(timer_counter_max <= timer_counter){
                VALVULA = 0;
                irrigacao_ativa = 0;
                LED_IRRIGACAO = 0;
                
                // desliga o timer 1
                T1CONbits.TMR1ON = 0;
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
    // ativa o timer 1
    T1CONbits.TMR1ON = 1;
    
    
    // n�o pode usar o menu enquanto tiver aqui
    //while(irrigacao_ativa);
    
    
}

void handleExternalInterruption()
{
    if(INTF){
        if(!irrigacao_ativa && ext_int_ativa){
            
            
            irrigar();
        }
        INTCONbits.INTF = 0;
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
    OPTION_REGbits.INTEDG   = 0;
	INTCONbits.GIE          = 1;
	INTCONbits.INTE         = 1;
    INTCONbits.INTF = 0;
    return;
}

void writeValor(int valor)
{
    char centena = valor/100 + 48;
    char dezena = (valor%100)/10 + 48;
    char unidade = valor%10 + 48;
    Lcd_Set_Cursor(2,1);
    Lcd_Write_Char(centena);
    Lcd_Write_Char(dezena);
    Lcd_Write_Char(unidade);
    return;
}

void handleMenu()
{
    int volume = water_volume;
    int umidade = umidade_minima;
    
    while(setup_menu == 1){
        CLRWDT();
        __delay_ms(100);
        Lcd_Set_Cursor(1,1);            //Poe cursor linha 1 coluna 1
        Lcd_Write_String("NEW THRESHOLD:");  //escreve string
        writeValor(umidade);
        if(!BTN_ENTER){
            umidade_minima = umidade;
            setup_menu = 2;
            __delay_ms(500);
        }
        if(!BTN_INC){
            umidade++;
            if(umidade > 999) umidade = 0;
            writeValor(umidade);
            __delay_ms(100);
        }
        if(!BTN_DEC){
            umidade--;
            if(umidade < 0) umidade = 0;
            writeValor(umidade);
            __delay_ms(100);
        }
    }
    Lcd_Clear();
    while(setup_menu == 2){
        CLRWDT();
        __delay_ms(100);
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("NEW VOLUME:");
        writeValor(volume);
        if(!BTN_ENTER){
            water_volume = volume;
            setupNewVolumeFlow();
            setup_menu = 3;
            __delay_ms(500);
        }
        if(!BTN_INC){
            volume++;
            if(volume > 999) volume = 0;
            writeValor(volume);
            __delay_ms(100);
        }
        if(!BTN_DEC){
            volume--;
            if(volume < 0) volume = 0;
            writeValor(volume);
            __delay_ms(100);
        }
    }
    Lcd_Clear();
    while(setup_menu == 3){
        CLRWDT();
        LED_SETUP = 0;
        break;
    }
    Lcd_Clear();
    return;
}
void verifyMenu()
{
    if (!BTN_ENTER)
    {
        setup_menu = 1;
        LED_SETUP = 1;
        __delay_ms(500);
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

    if(getADConverterValue()<umidade_minima){
        LED_UMIDADE = 1;
        irrigar();
    }
    else{
        LED_UMIDADE = 0;
    }
    return;
}

void setupWatchdogTimer()
{
    OPTION_REGbits.PSA = 1; //define que o prescaler esta associado ao WTD
    OPTION_REGbits.PS0 = 0; // define o prescaler do WTD para 1:64 (1152ms)
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;
    CLRWDT();
    return;
}


void setupADC(){
    
    
    // define o A0 como entrada analogica
    // e o A3 e A2 como VREFs
    ADCON1bits.PCFG0    = 1;
    ADCON1bits.PCFG1    = 1;
    ADCON1bits.PCFG2    = 1;
    ADCON1bits.PCFG3    = 1;
    
    // clock de convers�o
    ADCON1bits.ADCS2    = 1;
    ADCON0bits.ADCS1    = 1;
    ADCON0bits.ADCS0    = 0;
    
    // Configura a convers�o em 8 bits
    ADCON1bits.ADFM     = 0;
    
    // inicializa��o do conversor
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
    // INICIALIZA��ES
    TRISA = 0b00000001;             //configura as portas usadas
    TRISB = 0b00011101;
    TRISC = 0;
    PORTC = 0;
    TRISD = 0b00000000;
    OPTION_REGbits.nRBPU = 0;       //habilita os resistores de pull-up
    PORTB = 0;
    
    setupExternalInterruption();
    setupWatchdogTimer();
    setupTimer(); 
    setupADC();
    Lcd_Init();
   
    __delay_ms(100);
    ext_int_ativa = 1;
    
    while (1)
    {
        verifySensor();
        verifyMenu();
        CLRWDT();
    }
    return;
}
