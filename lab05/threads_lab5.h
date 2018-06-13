#include <stdint.h>
#include "cmsis_os.h"

#ifndef __THREADS_LAB5_H__
#define __THREADS_LAB5_H__

#ifndef PI
#define PI 3.14159
#endif

/**
* Tempos tabelados de execucao, em ticks, com -O2:
* A: 39637
* B: 8210
* C: 11370
* D: 226
* E: 38413
* F: 56390
*
* Tempo maximo de execucao sera o tempo tabelado acima vezes (1 + deadline)
* Os tempos, arrendodados para baixo, foram colocados nos defines abaixo:
**/

#define maxTicksA 67382
#define maxTicksB 12315
#define maxTicksC 14781
#define maxTicksD 293
#define maxTicksE 49936
#define maxTicksF 62029

typedef enum State {
	ready,
	running,
	waiting
} State;

// =============================================
// ===== Estruturas com coisas das threads =====
// =============================================

typedef enum threadNumber {
	A = 0,
	B,
	C,
	D,
	E,
	F
} threadNumber;

typedef struct threadStuffs {
	int16_t staticPrio;
    uint32_t startTime;
	uint32_t endTime;
	const uint32_t maxTicks;
	uint32_t delay;
    float progress;
    State state;
	osThreadId id;
	uint32_t faultCount;
} threadStuffs;

// Threads (0 = A, 1 = B, etc.)
threadStuffs threads[6] = {
	{  10, 0, 0, maxTicksA, 0, 0.0f, waiting, 0, 0},
	{   0, 0, 0, maxTicksB, 0, 0.0f, waiting, 0, 0},
	{ -30, 0, 0, maxTicksC, 0, 0.0f, waiting, 0, 0},
	{   0, 0, 0, maxTicksD, 0, 0.0f, waiting, 0, 0},
	{ -30, 0, 0, maxTicksE, 0, 0.0f, waiting, 0, 0},
	{-100, 0, 0, maxTicksF, 0, 0.0f, waiting, 0, 0}
};

/**
* Ha de calcular os tempos dos timers
* Considerando que o tempo do timer se da em ticks
* e que o clock tem frequencia fixa de 120 MHz
* pode-se simplesmente converter a peridiocidade (x)
* o para um tempo em ticks (t):
* t = 120M / x
**/

#define ticksTimerA 15000000
#define ticksTimerB 60000000
#define ticksTimerC 24000000
#define ticksTimerD 120000000
#define ticksTimerE 20000000
#define ticksTimerF 12000000

// ===== 

#endif
