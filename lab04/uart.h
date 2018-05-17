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

// Registradores de flag e transmissao
#define UART0FR    0x4000C018 // Registrador de flags
#define UART0DR    0x4000C000 // Registrador de dados

// Funcao de inicializacao. Deve ser chamada antes de tentar enviar ou receber
void uart_init();

// Funcao para enviar caracteres BLOQUEANTE
void uart_putChar(char c);

// Funcao para receber caracteres BLOQUEANTE
char uart_getChar();

#endif
