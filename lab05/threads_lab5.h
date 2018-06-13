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

typedef struct threadStuffs {
	int8_t staticPrio;
	int16_t currentPrio;
	uint32_t endTime;
    uint32_t startTime;
	const uint32_t maxTicks;
	uint32_t delay;
    float progress;
    State state;
	osThreadId id;
} threadStuffs;

void threadA(void);
void threadB(void);
void threadC(void);
void threadD(void);
void threadE(void);
void threadF(void);

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
