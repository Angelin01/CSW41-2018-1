#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "cmsis_os.h"
#include "TM4C129.h"

#include "grlib/grlib.h"
#include "cfaf128x128x16.h"
#include "uart_lab4.h"
#include "threads_lab5.h"

// Suprimir erros de funcoes declaradas mas nao referenciadas das libs
#pragma diag_suppress 177

// Desabilita round robin de threads
#define OS_ROBIN 0

#define SEC_TIMER_A 125
#define SEC_TIMER_B 500
#define SEC_TIMER_C 200
#define SEC_TIMER_D 1000
#define SEC_TIMER_E 167
#define SEC_TIMER_F 100

#define MSEC_TIMER_MAIN 3

// Comente para que acontecam master faults
#define DISABLE_MASTER_FAULT

// Formula de prioridade
// Basicamente: PorcentagemTicksRestantes * 200 + PrioridadeNormal
#define laxity(x) \
        ((double)threadMeta[x].startTime + (double)threadMeta[x].maxTicks - (double)osKernelSysTick())
#define dynamicPrio(x) \
        ((laxity(x)/threadMeta[x].maxTicks) * 200 + threadMeta[x].staticPrio)
		
#define runningTime(x) \
		(threadMeta[x].endTime - threadMeta[x].startTime)

/*===========================================================================*/

/* Variáveis globais */

// Contexto da tela
tContext sContext;

// IDs das threads do sistema
osThreadId mainId;
osThreadId idleId;

// IDs dos timers
osTimerId threadTimerId[6];
osTimerId mainTimerId;

// Número da thread que obteve o processador na última execução do escalonador
ThreadNumber execThread = THR_IDLE;

// Metadados iniciais de cada thread
ThreadMetadata threadMeta[6] = {
	{MAX_TICKS_A,   10,   10, 0, 0, 0, 0, 0, 0.0f, 0, WAITING},
	{MAX_TICKS_B,    0,    0, 0, 0, 0, 0, 0, 0.0f, 0, WAITING},
	{MAX_TICKS_C,  -30,  -30, 0, 0, 0, 0, 0, 0.0f, 0, WAITING},
	{MAX_TICKS_D,    0,    0, 0, 0, 0, 0, 0, 0.0f, 0, WAITING},
	{MAX_TICKS_E,  -30,  -30, 0, 0, 0, 0, 0, 0.0f, 0, WAITING},
	{MAX_TICKS_F, -100, -100, 0, 0, 0, 0, 0, 0.0f, 0, WAITING}
};

/*===========================================================================*/

/* Funções */

// Função que faz a thread atual dormir, e força o escalonador a executar antes
// do seu timer.
// Deve ser chamada em cada thread após esta fazer o devido cálculo.
void lab5Yield() {
	threadMeta[execThread].state = ENDED;
	threadMeta[execThread].endTime = osKernelSysTick();
	osSignalSet(mainId, 0x1);
}

/*===========================================================================*/

/* Threads */

void threadA(void const* args) {
	volatile double total;
	int i;
	
	while (true) {
		total = 0;
		threadMeta[THR_A].progress = 0.0f;
		
		for (i = 0; i < 256; ++i) {
			total += i + (i + 2);
			threadMeta[THR_A].progress = (float) (i+1)/256;
		}
		
		lab5Yield();
	}
}

void threadB(void const* args) {
	volatile double total;
	double pot;
	double fact;
	int i;
	
	while (true) {
		total = 0;
		pot = 1;
		fact = 1;
		threadMeta[THR_B].progress = 0.0f;
		
		for (i = 1; i < 16; ++i) {
			pot *= 2;
			fact *= i;
			total += (double) pot/fact;
			threadMeta[THR_B].progress = (float) (i+1)/16;
		}
		
		lab5Yield();
	}
}

void threadC(void const* args) {
	volatile double total;
	int i;
	
	while (true) {
		total = 0;
		threadMeta[THR_C].progress = 0.0f;
		
		for (i = 1; i < 72; ++i) {
			total += (double) (i + 1)/i;
			threadMeta[THR_C].progress = (float) (i+1)/72;
		}
		
		lab5Yield();
	}
}

void threadD(void const* args) {
	volatile double total;
	
	while (true) {
		total = 0;
		threadMeta[THR_D].progress = 0.0f;
		
		total = 1 + 5/(3*2) + 5/(5*4*3*2) + 5/(7*6*5*4*3*2) + 5/(9*8*7*6*5*4*3*2);
		threadMeta[THR_D].progress = 1.0f;
		
		lab5Yield();
	}
}

void threadE(void const* args) {
	volatile double total;
	int i;
	
	while (true) {
		total = 0;
		threadMeta[THR_E].progress = 0.0f;
		
		for (i = 1; i < 100; ++i) {
			total += (double) i * M_PI * M_PI;
			threadMeta[THR_E].progress =  (float) (i+1)/100;
		}
		
		lab5Yield();
	}
}

void threadF(void const* args) {
	volatile double total;
	double pot;
	int i;
	
	while (true) {
		total = 0;
		pot = 1;
		threadMeta[THR_F].progress = 0.0f;
		
		for (i = 1; i < 128; ++i) {
			pot *= 2;
			total += (double) i*i*i/pot;
			threadMeta[THR_F].progress = (float) (i+1)/128;
		}
		
		lab5Yield();
	}
}

void threadIdle(void const* args) {
	while (true) {
		// Talvez colocar display aqui
	}
}

osThreadDef(threadA, osPriorityIdle, 1, 0);
osThreadDef(threadB, osPriorityIdle, 1, 0);
osThreadDef(threadC, osPriorityIdle, 1, 0);
osThreadDef(threadD, osPriorityIdle, 1, 0);
osThreadDef(threadE, osPriorityIdle, 1, 0);
osThreadDef(threadF, osPriorityIdle, 1, 0);
osThreadDef(threadIdle, osPriorityBelowNormal, 1, 0);

/*===========================================================================*/

/* Timers */

void threadTimerHandler(void const* t) {
	if (threadMeta[(ThreadNumber)t].state == WAITING) {
		threadMeta[(ThreadNumber)t].state = READY;
		threadMeta[(ThreadNumber)t].startTime = osKernelSysTick();
	}
	
	// Caso a thread em execução atual seja THR_IDLE, seta o signal do main
	if (execThread == THR_IDLE) {
		osSignalSet(mainId, 0x1);
	}
}

void mainTimerHandler(void const* args) {
	// Ativa o signal do main.
	osSignalSet(mainId, 0x1);
}

void masterFault() {
	GrContextForegroundSet(&sContext, ClrRed);
	GrStringDraw(&sContext, "MASTER FAULT", -1, 10, 60, 1);
	while(1);
}

osTimerDef(timerA, threadTimerHandler);
osTimerDef(timerB, threadTimerHandler);
osTimerDef(timerC, threadTimerHandler);
osTimerDef(timerD, threadTimerHandler);
osTimerDef(timerE, threadTimerHandler);
osTimerDef(timerF, threadTimerHandler);
osTimerDef(timerMain, mainTimerHandler);

/*===========================================================================*/

/* Main */

int main(void) {
	uint8_t i;
	int32_t lowestPrioValueFound;
	
	ThreadNumber nextThread;
	
	// Inicializações
	osKernelInitialize();
	
	threadMeta[THR_A].id = osThreadCreate(osThread(threadA), NULL);
	threadMeta[THR_B].id = osThreadCreate(osThread(threadB), NULL);
	threadMeta[THR_C].id = osThreadCreate(osThread(threadC), NULL);
	threadMeta[THR_D].id = osThreadCreate(osThread(threadD), NULL);
	threadMeta[THR_E].id = osThreadCreate(osThread(threadE), NULL);
	threadMeta[THR_F].id = osThreadCreate(osThread(threadF), NULL);
	mainId = osThreadGetId();
	idleId = osThreadCreate(osThread(threadIdle), NULL);
	
	threadTimerId[THR_A] = osTimerCreate(osTimer(timerA), osTimerPeriodic, (void*) THR_A);
	threadTimerId[THR_B] = osTimerCreate(osTimer(timerB), osTimerPeriodic, (void*) THR_B);
	threadTimerId[THR_C] = osTimerCreate(osTimer(timerC), osTimerPeriodic, (void*) THR_C);
	threadTimerId[THR_D] = osTimerCreate(osTimer(timerD), osTimerPeriodic, (void*) THR_D);
	threadTimerId[THR_E] = osTimerCreate(osTimer(timerE), osTimerPeriodic, (void*) THR_E);
	threadTimerId[THR_F] = osTimerCreate(osTimer(timerF), osTimerPeriodic, (void*) THR_F);
	mainTimerId = osTimerCreate(osTimer(timerMain), osTimerPeriodic, NULL);
	
	osTimerStart(threadTimerId[THR_A], SEC_TIMER_A);
	osTimerStart(threadTimerId[THR_B], SEC_TIMER_B);
	osTimerStart(threadTimerId[THR_C], SEC_TIMER_C);
	osTimerStart(threadTimerId[THR_D], SEC_TIMER_D);
	osTimerStart(threadTimerId[THR_E], SEC_TIMER_E);
	osTimerStart(threadTimerId[THR_F], SEC_TIMER_F);
	
	osThreadSetPriority(mainId, osPriorityAboveNormal);
	
	osKernelStart();
	
	/* Corpo do scheduler */
	
	while (true) {
		// Desliga o relógio
		osTimerStop(mainTimerId);
		
		// Procura a thread de maior prioridade
		lowestPrioValueFound = 0x7FFF;
		nextThread = THR_IDLE;
		for (i = 0; i < THR_IDLE; ++i) {
			// Checa por faults
			if(threadMeta[i].state == ENDED) {
				if(runningTime(i) < threadMeta[i].maxTicks/2) {
					++threadMeta[i].faultCount;
					// Executou muito rapido! Diminuir prioridade
					threadMeta[i].staticPrio += 5;
				}
				else if(runningTime(i) > threadMeta[i].maxTicks) {
					++threadMeta[i].faultCount;
					// Atrasou! Aumentar prioridade
					
					if(threadMeta[i].staticPrio > -100) {
						threadMeta[i].staticPrio -= 5;
					}
					#ifndef DISABLE_MASTER_FAULT
					else {
						masterFault();
					}
					#endif
				}
				threadMeta[i].state = WAITING;
			}
			
			if (threadMeta[i].state != WAITING) {
				threadMeta[i].remainingTicks = laxity(i);
				threadMeta[i].dynamPrio = dynamicPrio(i);
				if (threadMeta[i].dynamPrio < lowestPrioValueFound) {
					lowestPrioValueFound = threadMeta[i].dynamPrio;
					nextThread = i;
				}
			}
		}
		
		// A thread idle tem prioridade osPriorityBelowNormal.
		// O escalonador tem prioridade osPriorityAboveNormal.
		// A thread de timers do RTOS tem prioridade osPriorityHigh.
		// Ou seja, a thread que estava sendo executada deve ter sua
		// prioridade alterada para osPriorityIdle,
		// e a próxima thread a executar deve ter sua prioridade
		// alterada para osPriorityNormal.
		
		// Altera as prioridades da última thread a executar e da próxima,
		// EXCETO se for a thread idle (cuja prioridade não deve ser alterada).
		if (execThread != THR_IDLE) {
			osThreadSetPriority(threadMeta[execThread].id, osPriorityIdle);
			// Se a tarefa sofreu preempção, deixa ela READY, para ela poder
			// ser escolhida em alguma próxima execução do escalonador.
			if (threadMeta[execThread].state == RUNNING) {
				threadMeta[execThread].state = READY;
			}
		}
		if (nextThread != THR_IDLE) {
			osThreadSetPriority(threadMeta[nextThread].id, osPriorityNormal);
			threadMeta[nextThread].state = RUNNING;
		}
		
		// Prepara as variáveis e timer para a próxima execução do escalonador
		execThread = nextThread;
		osTimerStart(mainTimerId, MSEC_TIMER_MAIN);
		osSignalClear(mainId, 0x1);
		osSignalWait(0x1, osWaitForever);
	}
	
	return 0;
}