#include <stdint.h>
#include "cmsis_os.h"

#ifndef __THREADS_LAB5_H__
#define __THREADS_LAB5_H__

#ifndef M_PI
#define M_PI 3.14159
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

#define MAX_TICKS_A 67382
#define MAX_TICKS_B 12315
#define MAX_TICKS_C 14781
#define MAX_TICKS_D 293
#define MAX_TICKS_E 49936
#define MAX_TICKS_F 62029

// Estado da thread para o escalonador
typedef enum ThreadState {
	READY,
	RUNNING,
	WAITING
} ThreadState;

// Número da thread (índice do vetor)
typedef enum ThreadNumber {
	THR_A = 0,
	THR_B,
	THR_C,
	THR_D,
	THR_E,
	THR_F,
	THR_IDLE
} ThreadNumber;

/*===========================================================================*/

/* Estruturas */

// Metadados das threads
typedef struct ThreadMetadata {
	const uint32_t maxTicks;
	const int16_t staticPrio;
	
	uint32_t startTime;
	uint32_t endTime;
	uint32_t delay;
	uint32_t faultCount;
	volatile float progress; // volatile para eu enxergar no debugger (ele só mata a variável pq não usa)
	
	osThreadId id;
	ThreadState state;
} ThreadMetadata;

#endif
