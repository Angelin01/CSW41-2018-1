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

#define RCGCUART (volatile uint32_t*) 0x400FE618 // Registrador de controle dos modulos UART
#define RCGCGPIO (volatile uint32_t*) 0x400FE608 // Registrador de controle de clock de modulos de I/O
#define GPIOAFSELA (volatile uint32_t*) 0x40058420 // Registrador que controla se pinos são I/O generico ou usados por perifericos para porta A
#define GPIOPCTLA (volatile uint32_t*) 0x4005852C // Registrador usado em conjunto com GPIOAFSELA para selecionar o tipo de sinal para cada pino
#define GPIOADEN (volatile uint32_t*) 0x4005851C // Registrador que habilita e desabilita sinais digitais

bool uart_init();

#endif