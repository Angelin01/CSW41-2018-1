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

// Threads (0 = A, 1 = B, etc.)
threadStuffs threads[6] = {
	{  10,    10, 0, 0, 0.0f, waiting, 0},
	{   0,     0, 0, 0, 0.0f, waiting, 0},
	{ -30,   -30, 0, 0, 0.0f, waiting, 0},
	{   0,     0, 0, 0, 0.0f, waiting, 0},
	{ -30,   -30, 0, 0, 0.0f, waiting, 0},
	{ -100, -100, 0, 0, 0.0f, waiting, 0}
};

typedef enum threadNumber {
	A = 0,
	B,
	C,
	D,
	E,
	F
} threadNumber;

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
	threadNumber n = (threadNumber) n;
	threads[n].state = ready;
	threads[n].progress = 0.0f;
	threads[n].startTime = osKernelSysTick();
	threads[n].delay = 0;
	osSignalSet(threads[n].id, 1);
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


int main(void) {
	osTimerId idMainTimer;
	osTimerId timerA;
	osTimerId timerB;
	osTimerId timerC;
	osTimerId timerD;
	osTimerId timerE;
	osTimerId timerF;
	
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
	timerA = osTimerCreate(osTimer(timerA), osTimerPeriodic, (void *)A);
	timerB = osTimerCreate(osTimer(timerB), osTimerPeriodic, (void *)B);
	timerC = osTimerCreate(osTimer(timerC), osTimerPeriodic, (void *)C);
	timerD = osTimerCreate(osTimer(timerD), osTimerPeriodic, (void *)D);
	timerE = osTimerCreate(osTimer(timerE), osTimerPeriodic, (void *)E);
	timerF = osTimerCreate(osTimer(timerF), osTimerPeriodic, (void *)F);
	idMainTimer = osTimerCreate(osTimer(timerMain), osTimerPeriodic, NULL);
	
	// Iniciando timers
	osTimerStart(timerA, 125);
	osTimerStart(timerB, 500);
	osTimerStart(timerC, 200);
	osTimerStart(timerD, 1000);
	osTimerStart(timerE, 167);
	osTimerStart(timerF, 100);
	osTimerStart(idMainTimer, 1);
	
	osKernelStart();
	
	while(1) {
		// Do scheduling stuff
		osSignalWait(1, osWaitForever);
	}
}
