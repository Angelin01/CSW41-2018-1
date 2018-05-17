#include "uart.h"

void uart_init() {
	accessReg(RCGCUART) |= (1<<0); // Habilita o UART0 no registrador de controle de modulos
	accessReg(RCGCGPIO) |= (1<<0); // Habilita clock para os modulos de UART
	accessReg(GPIOAFSELA) |= (1<<0) | (1<<1); // Troca os pinos 33 e 34 (PA0 e PA1) para controle de periferico
	// Nao eh necessario se preocupar com slew rate ou corrente nesse caso
	accessReg(GPIOPCTLA) |= (1<<0) | (1<<4); /* Pouco mais complexo. Ver tabela 26.5 na pagina 1808
	                                            Seleciona coluna 1 no PCM0 e PCM1 para PA0 e PA1 */
						 
/**** AVISO: O REGISTRADOR A SEGUIR TROCA PINOS DO ESTADO DE TRI-STATE PARA INPUT OUTPUT DIGITAL ****/
//	accessReg(GPIOADEN) |= (1<<0) | (1<<1); // Habilita sinais digitais nos pinos do GPIO		
	
	accessReg(UART0CTL) &= 0xfffe; // Desabilita UART (bit 0) antes de aplicar configuracoes
	
	// Proximos dois registradores recebem os valores de divisor de baud rate inteiro e fracinado
	/* BRD = clock / (divisor * baudRate)
	   BRD = 120000000 / (16 * 115200) = 65,10416666666667 = 65
	   FRAC = int(0.10416666666667 * 64 + 0.5) = int(7,16667) = 7 */
	accessReg(UART0IBRD) = 65;
	accessReg(UART0FBRD) = 7;
	
	accessReg(UART0LCRH) = 0x60; // = 0b01100000 Configura o serial para 8 data bits, 1 stop bit sem paridade
	accessReg(UART0CC) = 0x0; // Configura para usar o clock normal do sistema (igual a reset)
	
	accessReg(UART0CTL) |= (1<<0); // Reabilita o UART
}
