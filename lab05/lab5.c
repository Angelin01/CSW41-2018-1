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

// Desabilita round robin de threads
#define OS_ROBIN 0

// Contexto da tela
tContext sContext;

// ID da thread main
osThreadId mainId;

// ===================
// ===== Threads =====
// ===================

void threadA() {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 0; i < 256; ++i) {
			total += i + (i + 2);
			threads[A].progress = (float) i/256;
		}
		
		osSignalSet(mainId, 1);
		osSignalWait(1, osWaitForever);
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
			threads[B].progress = (float) i/16;
		}
		osSignalSet(mainId, 1);
		osSignalWait(1, osWaitForever);
	}
}

void threadC() {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 1; i < 72; ++i) {
			total += (i + 1)/i;
			threads[C].progress = (float) i/72;
		}
		osSignalSet(mainId, 1);
		osSignalWait(1, osWaitForever);
	}
}

void threadD() {
	volatile double total;
	
	while(1) {
		total = 1 + 5/(3*2) + 5/(5*4*3*2) + 5/(7*6*5*4*3*2)	+ 5/(9*8*7*6*5*4*3*2);
		threads[D].progress = (float) 1;
		osSignalSet(mainId, 1);
		osSignalWait(1, osWaitForever);
	}
}

void threadE() {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 1; i < 100; ++i) {
			total += i*PI*PI;
			threads[E].progress = (float) i/100;
		}
		osSignalSet(mainId, 1);
		osSignalWait(1, osWaitForever);
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
			threads[F].progress = (float) i/128;
		}
		osSignalSet(mainId, 1);
		osSignalWait(1, osWaitForever);
	}
}

osThreadDef(threadA, osPriorityNormal, 1, 0);
osThreadDef(threadB, osPriorityNormal, 1, 0);
osThreadDef(threadC, osPriorityNormal, 1, 0);
osThreadDef(threadD, osPriorityNormal, 1, 0);
osThreadDef(threadE, osPriorityNormal, 1, 0);
osThreadDef(threadF, osPriorityNormal, 1, 0);


// =================
// ===== Inits =====
// =================

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

// =============================
// ===== Timers e handler ======
// =============================

void threadTimerHandler(void const* t) {
	threads[(threadNumber) t].state = ready;
	threads[(threadNumber) t].startTime = osKernelSysTick();
	osSignalSet(threads[(threadNumber) t].id, 1);
}

void mainTimerHandler() {
	osSignalSet(mainId, 1);
}

// Passa como argumento a thread no osTimerCreate
osTimerDef(timerA, threadTimerHandler);
osTimerDef(timerB, threadTimerHandler);
osTimerDef(timerC, threadTimerHandler);
osTimerDef(timerD, threadTimerHandler);
osTimerDef(timerE, threadTimerHandler);
osTimerDef(timerF, threadTimerHandler);

osTimerDef(timerMain, mainTimerHandler);


void masterFault(threadNumber i) {
	GrStringDraw(&sContext, "MASTER FAULT", -1, 5, i*10 + 5, 1);
	while(1);
}

int main(void) {
	int32_t lowestLaxityFactor;
	int32_t tmpLaxity;
	uint32_t tmpMaxTime;
	uint32_t schedulerRuns;
	int lowestLaxityThread;
	State previousStates[6] = {waiting, waiting, waiting, waiting, waiting, waiting};
	char toDisplay[10];
	int i;
	
	osTimerId idMainTimer;
	osTimerId idTimerA;
	osTimerId idTimerB;
	osTimerId idTimerC;
	osTimerId idTimerD;
	osTimerId idTimerE;
	osTimerId idTimerF;
	
	init_all();
	osKernelInitialize();
	
	mainId = osThreadGetId();
	
	// Criando threads
	threads[A].id = osThreadCreate(osThread(threadA), NULL);
	threads[B].id = osThreadCreate(osThread(threadB), NULL);
	threads[C].id = osThreadCreate(osThread(threadC), NULL);
	threads[D].id = osThreadCreate(osThread(threadD), NULL);
	threads[E].id = osThreadCreate(osThread(threadE), NULL);
	threads[F].id = osThreadCreate(osThread(threadF), NULL);
	
	
	// Criando timers
	idTimerA = osTimerCreate(osTimer(timerA), osTimerPeriodic, (void *)A);
	idTimerB = osTimerCreate(osTimer(timerB), osTimerPeriodic, (void *)B);
	idTimerC = osTimerCreate(osTimer(timerC), osTimerPeriodic, (void *)C);
	idTimerD = osTimerCreate(osTimer(timerD), osTimerPeriodic, (void *)D);
	idTimerE = osTimerCreate(osTimer(timerE), osTimerPeriodic, (void *)E);
	idTimerF = osTimerCreate(osTimer(timerF), osTimerPeriodic, (void *)F);
	idMainTimer = osTimerCreate(osTimer(timerMain), osTimerPeriodic, NULL);
	
	
	
	osKernelStart();
	
	// Iniciando timers
	osTimerStart(idTimerA, 125);
	osTimerStart(idTimerB, 500);
	osTimerStart(idTimerC, 200);
	osTimerStart(idTimerD, 1000);
	osTimerStart(idTimerE, 167);
	osTimerStart(idTimerF, 100);
	
	// =====================
	// ===== SCHEDULER =====
	// =====================
	
	//osThreadSetPriority(mainId, osPriorityHigh);
	
	for(schedulerRuns = 0; true; ++schedulerRuns) {
		osTimerStop(idMainTimer);
		
		lowestLaxityFactor = 1000; // Deve ser maior que 300
		for(i = 0; i < 6; ++i) {
			tmpMaxTime = threads[i].startTime + threads[i].maxTicks;
			if(threads[i].state != waiting) { // Se a thread nao esta dormindo, fazer coisas de scheduling
				if(osKernelSysTick() > tmpMaxTime) {
					if(threads[i].staticPrio <= -100) {
						++threads[i].faultCount;
						// MASTER FAULT
						// HCF
						// PICNIC
						// PRESS THE RED BUTTON
						//masterFault(i);
					}
					else {
						threads[i].delay = tmpMaxTime - osKernelSysTick();
					}
				}
				
				tmpLaxity = (double)((tmpMaxTime - osKernelSysTick())/threads[i].maxTicks)*200 + threads[i].staticPrio; // Somara ate +200 a prioridade de acordo com tempo restante
				sprintf(toDisplay, "%c %d", 'A' + i, tmpLaxity);
				GrStringDraw(&sContext, toDisplay, -1, 5,  5 + 10*i, 1);
				if(tmpLaxity < lowestLaxityFactor) {
					lowestLaxityFactor = tmpLaxity;
					lowestLaxityThread = i;
				}
				osThreadSetPriority(threads[i].id, osPriorityLow);
				previousStates[i] = threads[i].state;
				threads[i].state = ready;
				
			}
			else if (previousStates[i] != waiting) {
				if(threads[i].endTime > tmpMaxTime) {
					++threads[i].faultCount;
					if(threads[i].staticPrio <= -100) {
						// MASTER FAULT
						// HCF
						// PICNIC
						// PRESS THE RED BUTTON
						//masterFault(i);
					}
					else {
						threads[i].staticPrio -= 2;
					}
				}
				else if((threads[i].endTime - threads[i].startTime)/2 < threads[i].maxTicks) {
					++threads[i].faultCount;
					threads[i].staticPrio += 2;
				}
				previousStates[i] = waiting;
			}
			
		}
		
		if(schedulerRuns % 100 == 0) {
			// Printa umas coisas
		}
		
		osThreadSetPriority(threads[lowestLaxityThread].id, osPriorityBelowNormal);
		threads[lowestLaxityThread].state = running;
		
		osTimerStart(idMainTimer, 20);

		osSignalWait(1, osWaitForever);
	} 
}
