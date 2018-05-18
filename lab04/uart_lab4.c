#include "uart_lab4.h"

void uart_init(uint32_t baudRate, uint32_t dataConfig) {
	accessReg(RCGCUART) |= (1<<0); // Habilita o UART0 no registrador de controle de modulos
	accessReg(RCGCGPIO) |= (1<<0); // Habilita clock para os modulos de UART
	accessReg(GPIOAFSELA) |= (1<<0) | (1<<1); // Troca os pinos 33 e 34 (PA0 e PA1) para controle de periferico
	// Nao eh necessario se preocupar com slew rate ou corrente nesse caso
	accessReg(GPIOPCTLA) |= (1<<0) | (1<<4); /* Pouco mais complexo. Ver tabela 26.5 na pagina 1808
	                                            Seleciona coluna 1 no PCM0 e PCM1 para PA0 e PA1 */
						 
/**** AVISO: O REGISTRADOR A SEGUIR TROCA PINOS DO ESTADO DE TRI-STATE PARA INPUT OUTPUT DIGITAL ****/
	accessReg(GPIOADEN) |= (1<<0) | (1<<1); // Habilita sinais digitais nos pinos do GPIO		
	
	accessReg(UART0CTL) &= 0xfffe; // Desabilita UART (bit 0) antes de aplicar configuracoes
	
	// Proximos dois registradores recebem os valores de divisor de baud rate inteiro e fracinado
	/*
	Valores calculados sao tabelados de acordo com as seguintes formulas:
		IBRD = INT(CLOCK / (DIVISORdeCLOCK * BAUDRATE))
		FBRD = INT(DECIMAL(IBRD) * 64 + 0.5)
	*/
	switch(baudRate) {
		case BAUD_75:
			accessReg(UART0IBRD) = 100000;
			accessReg(UART0FBRD) = 0;
			break;
		
		case BAUD_110:
			accessReg(UART0IBRD) = 68181;
			accessReg(UART0FBRD) = 52;
			break;
		
		case BAUD_300:
			accessReg(UART0IBRD) = 25000;
			accessReg(UART0FBRD) = 0;
			break;
		
		case BAUD_1200:
			accessReg(UART0IBRD) = 6250;
			accessReg(UART0FBRD) = 0;
			break;
		
		case BAUD_2400:
			accessReg(UART0IBRD) = 3125;
			accessReg(UART0FBRD) = 0;
			break;
		
		case BAUD_4800:
			accessReg(UART0IBRD) = 5;
			accessReg(UART0FBRD) = 1562;
			break;
		case BAUD_9600:
			accessReg(UART0IBRD) = 781;
			accessReg(UART0FBRD) = 16;
			break;
		
		case BAUD_19200:
			accessReg(UART0IBRD) = 390;
			accessReg(UART0FBRD) = 40;
			break;
		
		case BAUD_38400:
			accessReg(UART0IBRD) = 195;
			accessReg(UART0FBRD) = 20;
			break;
		
		case BAUD_57600:
			accessReg(UART0IBRD) = 130;
			accessReg(UART0FBRD) = 13;
			break;
		
		case BAUD_115200:
			accessReg(UART0IBRD) = 65;
			accessReg(UART0FBRD) = 7;
			break;
		default: 
			accessReg(UART0IBRD) = 65;
			accessReg(UART0FBRD) = 7;
			break;
	}
	
	accessReg(UART0LCRH) = dataConfig; // Configura os parametros da serial
	accessReg(UART0CC) = 0x0; // Configura para usar o clock normal do sistema (igual a reset)
	
	accessReg(UART0CTL) |= (1<<0) | (1<<8) | (1<<9); // Reabilita o UART
}

void uart_putChar(char c) {
	while(accessReg(UART0FR) & (1<<5)); // Espera a FIFO de saida estar vazia
	accessReg(UART0DR) = c; // Coloca valor no reg de dados
}

void uart_sendString(char* s) {
	while(*s) {
		uart_putChar(*s++);
	}
}

char uart_getChar(void) {
	char c;
	while(accessReg(UART0FR) & (1<<4)); // Espera a FIFO de entrada estar vazia
	c = accessReg(UART0DR); // Pega o valor do reg de dados. Variavel intermediaria por seguranca
	return(c);
}
