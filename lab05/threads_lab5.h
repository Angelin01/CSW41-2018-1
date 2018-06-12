#include <stdint.h>

#ifndef __THREADS_LAB5_H__
#define __THREADS_LAB5_H__

#ifndef PI
#define PI 3.14159
#endif

typedef enum State {
	ready,
	running,
	waiting
} State;

typedef struct threadStuffs {
	int8_t staticPrio;
	int16_t currentPrio;
	uint32_t maxTime;
    uint32_t startTime;
	uint32_t delay;
    float progress;
    State state;
} threadStuffs;

void threadA(void);

void threadB(void);

void threadC(void);

void threadD(void);

void threadE(void);

void threadF(void);

#endif
