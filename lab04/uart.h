/**
* Mini biblioteca para uso de UART
* UART em uso eh o UART0
* Documentacao e passos encontrados em http://www.ti.com/lit/ds/symlink/tm4c1294ncpdt.pdf
* Ver secao "16.4 Initialization and Configuration"
*/

#ifndef __UART__
#define __UART__

#define RCGCUART 0x400FE618 // Registrador de controle dos modulos UART
#define RCGCGPIO 0x400FE608 // Registrador de controle de clock de modulos de I/O
#define GPIOAFSELA 0x40058420 // Registrador que controla se pinos são I/O generico ou usados por perifericos para porta A
#define GPIOPCTLA 0x4005852C // Registrador usado em conjunto com GPIOAFSELA para selecionar o tipo de sinal para cada pino
#define GPIOADEN 0x4005851C // Registrador que habilita e desabilita sinais digitais

bool uart_init();

#endif