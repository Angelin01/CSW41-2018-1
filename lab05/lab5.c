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

// Threads (0 = A, 1 = B, etc.)
threadStuffs threads[6] = {
	{  10,    10, 0, 0, maxTicksA, 0, 0.0f, waiting, 0},
	{   0,     0, 0, 0, maxTicksB, 0, 0.0f, waiting, 0},
	{ -30,   -30, 0, 0, maxTicksC, 0, 0.0f, waiting, 0},
	{   0,     0, 0, 0, maxTicksD, 0, 0.0f, waiting, 0},
	{ -30,   -30, 0, 0, maxTicksE, 0, 0.0f, waiting, 0},
	{ -100, -100, 0, 0, maxTicksF, 0, 0.0f, waiting, 0}
};

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
	init_all();
	
	osKernelInitialize();
	
	// Criando threads
	threads[0].id = osThreadCreate(osThread(threadA), NULL);
	threads[1].id = osThreadCreate(osThread(threadB), NULL);
	threads[2].id = osThreadCreate(osThread(threadC), NULL);
	threads[3].id = osThreadCreate(osThread(threadD), NULL);
	threads[4].id = osThreadCreate(osThread(threadE), NULL);
	threads[5].id = osThreadCreate(osThread(threadF), NULL);
	
	osKernelStart();
	osDelay(osWaitForever);
}
