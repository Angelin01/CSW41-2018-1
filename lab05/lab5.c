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

#define currentTick osKernelSysTick()

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
		osSignalWait(1, osWaitForever);
		total = 0;
		
		for(i = 0; i < 256; ++i) {
			total += i + (i + 2);
			threads[A].progress = (float) (i+1)/256;
		}
		
		osSignalSet(mainId, 1);
		threads[A].state = waiting;
		threads[A].endTime = currentTick;
	}
}

void threadB() {
	volatile double total;
	double pot, fact;
	int i;
	
	while(1) {
		osSignalWait(1, osWaitForever);
		total = 0;
		pot = 1;
		fact = 1;
		
		for(i = 1; i < 16; ++i) {
			pot *= 2;
			fact *= i;
			total += pot/fact;
			threads[B].progress = (float) (i+1)/16;
		}
		osSignalSet(mainId, 1);
		threads[B].state = waiting;
		threads[B].endTime = currentTick;
	}
}

void threadC() {
	volatile double total;
	int i;
	
	while(1) {
		osSignalWait(1, osWaitForever);
		total = 0;
		
		for(i = 1; i < 72; ++i) {
			total += (i + 1)/i;
			threads[C].progress = (float) (i+1)/72;
		}
		osSignalSet(mainId, 1);
		threads[C].state = waiting;
		threads[C].endTime = currentTick;
	}
}

void threadD() {
	volatile double total;
	
	while(1) {
		osSignalWait(1, osWaitForever);
		total = 1 + 5/(3*2) + 5/(5*4*3*2) + 5/(7*6*5*4*3*2)	+ 5/(9*8*7*6*5*4*3*2);
		threads[D].progress = 1.0f;
		osSignalSet(mainId, 1);
		threads[D].state = waiting;
		threads[D].endTime = currentTick;
	}
}

void threadE() {
	volatile double total;
	int i;
	
	while(1) {
		osSignalWait(1, osWaitForever);
		total = 0;
		
		for(i = 1; i < 100; ++i) {
			total += i*PI*PI;
			threads[E].progress = (float) (i+1)/100;
		}
		osSignalSet(mainId, 1);
		threads[E].state = waiting;
		threads[E].endTime = currentTick;
	}
}

void threadF() {
	volatile double total;
	double pot;
	int i;
	
	while(1) {
		osSignalWait(1, osWaitForever);
		total = 0;
		pot = 1;
		
		for(i = 1; i < 128; ++i) {
			pot *= 2;
			total += (i*i*i)/pot;
			threads[F].progress = (float) (i+1)/128;
		}
		osSignalSet(mainId, 1);
		threads[F].state = waiting;
		threads[F].endTime = currentTick;
	}
}

osThreadDef(threadA, osPriorityLow, 1, 0);
osThreadDef(threadB, osPriorityLow, 1, 0);
osThreadDef(threadC, osPriorityLow, 1, 0);
osThreadDef(threadD, osPriorityLow, 1, 0);
osThreadDef(threadE, osPriorityLow, 1, 0);
osThreadDef(threadF, osPriorityLow, 1, 0);


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
	//threads[(threadNumber) t].progress = 0.0f;
	threads[(threadNumber) t].startTime = currentTick;
	threads[(threadNumber) t].delay = 0;
	osSignalSet(threads[(threadNumber) t].id, 1);
}

void mainTimerHandler() {
	osSignalSet(mainId, 1);
	osThreadYield();
}

// Passa como argumento a thread no osTimerCreate
osTimerDef(timerA, threadTimerHandler);
osTimerDef(timerB, threadTimerHandler);
osTimerDef(timerC, threadTimerHandler);
osTimerDef(timerD, threadTimerHandler);
osTimerDef(timerE, threadTimerHandler);
osTimerDef(timerF, threadTimerHandler);
osTimerDef(timerMain, mainTimerHandler);


int main(void) {
	char toDisplay[10];
	int i;
	uint32_t tmpMaxTime;
	int32_t tmpLaxity;
	int32_t lowestLaxityFactor;
	uint32_t schedulerRuns;
	int lowestLaxityThread;
	State previousStates[6] = {waiting, waiting, waiting, waiting, waiting, waiting};
	
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
	/*osTimerStart(idTimerA, 125);
	osTimerStart(idTimerB, 500);
	osTimerStart(idTimerC, 200);
	osTimerStart(idTimerD, 1000);
	osTimerStart(idTimerE, 167);
	osTimerStart(idTimerF, 100);*/
	
	// =====================
	// ===== SCHEDULER =====
	// =====================
	
	osThreadSetPriority(mainId, osPriorityAboveNormal);
	
	for(schedulerRuns = 0; true; ++schedulerRuns) {
		osTimerStop(idMainTimer);
		for(i = 0; i < 6; ++i) {
			osThreadSetPriority(threads[i].id, osPriorityLow);
		}
		osThreadSetPriority(threads[schedulerRuns%6].id, osPriorityBelowNormal);
		osSignalSet(threads[schedulerRuns%6].id, 1);
		osTimerStart(idMainTimer, 1);
		osSignalWait(1, osWaitForever);
	} 
	
}
