#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"
#include "inc/hw_ints.h"

#include "rgb.h"
#include "cfaf128x128x16.h"
#include "uart_lab4.h"
#include "colors.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

#define PI 3.14159

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


void threadA() {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 0; i < 256; ++i) {
			total += i + (i + 2);
		}
	}
}

void threadB() {
	volatile double total;
	double pot, fact;
	int i;
	
	while(1) {
		total = 0;
		pot = 1;
		fact = 1;
		
		for(i = 1; i < 16; ++i) {
			pot *= 2;
			fact *= i;
			total += pot/fact;
		}
	}
}

void threadC() {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 1; i < 72; ++i) {
			total += (i + 1)/i;
		}
	}
}

void threadD() {
	volatile double total;
	const float fact3 = 3*2;
	const float fact5 = 5*4*fact3;
	const float fact7 = 7*6*fact5;
	const float fact9 = 9*8*fact7;
	
	while(1) {
		total = 1 + 5/fact3 + 5/fact5 + 5/fact7	+ 5/fact9;	
	}
}

void threadE() {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 1; i < 100; ++i) {
			total += i*PI*PI;
		}
	}
}

void threadF() {
	volatile double total;
	double pot;
	int i;
	
	while(1) {
		total = 0;
		pot = 1;
		
		for(i = 1; i < 128; ++i) {
			pot *= 2;
			total += (i*i*i)/pot;
		}
	}
}

osThreadDef(threadA, osPriorityNormal, 1, 0);
osThreadDef(threadB, osPriorityNormal, 1, 0);
osThreadDef(threadC, osPriorityNormal, 1, 0);
osThreadDef(threadD, osPriorityNormal, 1, 0);
osThreadDef(threadE, osPriorityNormal, 1, 0);
osThreadDef(threadF, osPriorityNormal, 1, 0);

int main(void) {
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
