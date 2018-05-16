#include "uart.h"

void uart_init() {
	*RCGCUART |= 0x1; // 1 - Habilita o UART0 no registrador de controle de modulos
	*RCGCGPIO |= 0x1; // 2 - Habilita clock para os modulos de UART
	*GPIOAFSELA |= 0x3; // 3 - Troca os pinos 33 e 34 (PA0 e PA1) para controle de periferico
	// 4 - Nao eh necessario se preocupar com slew rate ou corrente nesse caso
	*GPIOPCTLA |= 0x11; /* Pouco mais complexo. Ver tabela 26.5 na pagina 1808
	                       Seleciona o PMC1, de acordo com a tabela. Este eh o bit LSB.
	                       Entao habilita o PA com outro bit */
						 
/**** AVISO: O REGISTRADOR A SEGUIR TROCA PINOS DO ESTADO DE TRI-STATE PARA INPUT OUTPUT DIGITAL ****/
/**** POR SEGURANCA, ISSO FICARA DESABILITADO ATE QUE SE PROVE NECESSARIO ****/
//	*GPIOADEN | = 0x3; // Habilita sinais digitais nos pinos do GPIO		
}