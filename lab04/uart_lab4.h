/**
* Mini biblioteca para uso de UART
* UART em uso eh o UART0
* Documentacao e passos encontrados em http://www.ti.com/lit/ds/symlink/tm4c1294ncpdt.pdf
* Ver secao "16.4 Initialization and Configuration"
*/

// DELETAR DEPOIS
// VIDEO REFERENCIA
// https://www.youtube.com/watch?v=Vy-k7u5djM4&t=20s

#ifndef __UART_LAB4_H__
#define __UART_LAB4_H__

#include <stdint.h>
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

// Para facilidade de uso dos registradores
#define accessReg(x) \
        (*((volatile uint32_t *)(x)))

// Registradores de configuracao
#define RCGCUART   0x400FE618 // Registrador de controle dos modulos UART
#define RCGCGPIO   0x400FE608 // Registrador de controle de clock de modulos de I/O
#define GPIOAFSELA 0x40058420 // Registrador que controla se pinos são I/O generico ou usados por perifericos para porta A
#define GPIOPCTLA  0x4005852C // Registrador usado em conjunto com GPIOAFSELA para selecionar o tipo de sinal para cada pino
#define GPIOADEN   0x4005851C // Registrador que habilita e desabilita sinais digitais
#define UART0CTL   0x4000C030 // Registrador de controle da UART
#define UART0IBRD  0x4000C024 // Registrador que guarda o valor inteiro do divisor de baudrate
#define UART0FBRD  0x4000C028 // Registrador que guarda o valor de fracao do divisor de baudrate
#define UART0LCRH  0x4000C02C // Registrador de configuracao da serial (data bits, paridade, stop bit, etc)
#define UART0CC    0x4000CFC8 // Registrador de controle de clock source da UART
#define UART0IM    0x4000C038 // Registrador de mascaras de interrupcao

// Registradores de flag e transmissao
#define UART0FR    0x4000C018 // Registrador de flags
#define UART0DR    0x4000C000 // Registrador de dados

/* Funcao de inicializacao. Deve ser chamada antes de tentar enviar ou receber
   Notar os defines abaixo a serem usados com a funcao */
/* Usar os defines de como o seguinte exemplo:
	uart_init(BAUD_9600, DATA_8 | STOP_ONE | PARITY_NONE);
	O init acima configura baudrate em 9600, 8 databits, 1 stop bit e sem paridade */
void uart_init(uint32_t baudRate, uint32_t dataConfig);
/* A baixo, defines que devem ser usados com a funcao de init */
// Defines baudrate
#define BAUD_75 0
#define BAUD_110 1
#define BAUD_300 2
#define BAUD_1200 3
#define BAUD_2400 4
#define BAUD_4800 5
#define BAUD_9600 6
#define BAUD_19200 7
#define BAUD_38400 8
#define BAUD_57600 9
#define BAUD_115200 10

// Defines data bits
#define DATA_5 0x0
#define DATA_6 0x20
#define DATA_7 0x40
#define DATA_8 0x60

// Defines stop bits
#define STOP_ONE 0x0
#define STOP_TWO 0x8

// Defines paridade
#define PARITY_NONE 0x0
#define PARITY_ODD  0x2
#define PARITY_EVEN 0x6

/* Funcao para enviar caracteres BLOQUEANTE */
void uart_putChar(char c);

/* Funcao para enviar uma string BLOQUEANTE */
void uart_sendString(char* s);

/* Funcao para receber caracteres BLOQUEANTE */
char uart_getChar(void);

void uart_regIntHandler(void (*handler) (void));

#endif
