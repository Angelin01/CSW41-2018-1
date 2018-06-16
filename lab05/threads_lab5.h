#include <stdint.h>
#include "cmsis_os.h"

#ifndef __THREADS_LAB5_H__
#define __THREADS_LAB5_H__

#ifndef M_PI
#define M_PI 3.14159
#endif

/**
* Tempos tabelados de execucao, em ticks:
* A: 51528
* B: 10673
* C: 14781
* D: 293
* E: 49936
* F: 68307
*
* Tempo maximo de execucao sera o tempo tabelado acima vezes (1 + deadline)
* Os tempos, arrendodados para baixo, foram colocados nos defines abaixo:
**/

#define MAX_TICKS_A 87597
#define MAX_TICKS_B 16009
#define MAX_TICKS_C 19215
#define MAX_TICKS_D 439
#define MAX_TICKS_E 64916
#define MAX_TICKS_F 75137

// Estado da thread para o escalonador
typedef enum ThreadState {
	READY,
	RUNNING,
	WAITING,
	ENDED
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
	
	int16_t staticPrio;
	int16_t dynamPrio;
	uint32_t startTime;
	uint32_t endTime;
	uint32_t delay;
	uint32_t faultCount;
	volatile float progress; // volatile para eu enxergar no debugger (ele só mata a variável pq não usa)
	
	osThreadId id;
	ThreadState state;
} ThreadMetadata;

#endif
