#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"

#include "rgb.h"
#include "cfaf128x128x16.h"
#include "uart_lab4.h"
#include "threads_lab5.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

// Suprimir erros de funcoes declaradas mas nao referenciadas das libs
#pragma diag_suppress 177

// Contexto da tela
tContext sContext;

// IDs das threads
osThreadId idA, idB, idC, idD, idE, idF;

void init_tela() {
	GrContextInit(&sContext, &g_sCfaf128x128x16);
	GrFlush(&sContext);
	GrContextFontSet(&sContext, g_psFontFixed6x8);
	
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextBackgroundSet(&sContext, ClrBlack);
}

void init_all() {
	cfaf128x128x16Init();
	rgb_init();
	uart_init(BAUD_115200, DATA_8 | STOP_ONE | PARITY_NONE);
	init_tela();
}

osThreadDef(threadA, osPriorityNormal, 1, 0);
osThreadDef(threadB, osPriorityNormal, 1, 0);
osThreadDef(threadC, osPriorityNormal, 1, 0);
osThreadDef(threadD, osPriorityNormal, 1, 0);
osThreadDef(threadE, osPriorityNormal, 1, 0);
osThreadDef(threadF, osPriorityNormal, 1, 0);

int main(void) {
	uint32_t startTime, stopTime;
	volatile double total;
	double pot, fact;
	int i;
	char yay[10];
	const float fact3 = 3*2;
	const float fact5 = 5*4*fact3;
	const float fact7 = 7*6*fact5;
	const float fact9 = 9*8*fact7;
	
	init_all();
	
	osKernelInitialize();
	
	// Criando threads
	idA = osThreadCreate(osThread(threadA), NULL);
	idB = osThreadCreate(osThread(threadB), NULL);
	idC = osThreadCreate(osThread(threadC), NULL);
	idD = osThreadCreate(osThread(threadD), NULL);
	idE = osThreadCreate(osThread(threadE), NULL);
	idF = osThreadCreate(osThread(threadF), NULL);
	
	osKernelStart();
	osDelay(osWaitForever);
}
