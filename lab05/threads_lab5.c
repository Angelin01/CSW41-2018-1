#include "threads_lab5.h"

void threadA(void) {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 0; i < 256; ++i) {
			total += i + (i + 2);
		}
	}
}

void threadB(void) {
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
		}
	}
}

void threadC(void) {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 1; i < 72; ++i) {
			total += (i + 1)/i;
		}
	}
}

void threadD(void) {
	volatile double total;
	const float fact3 = 3*2;
	const float fact5 = 5*4*fact3;
	const float fact7 = 7*6*fact5;
	const float fact9 = 9*8*fact7;
	
	while(1) {
		total = 1 + 5/fact3 + 5/fact5 + 5/fact7	+ 5/fact9;	
	}
}

void threadE(void) {
	volatile double total;
	int i;
	
	while(1) {
		total = 0;
		
		for(i = 1; i < 100; ++i) {
			total += i*PI*PI;
		}
	}
}

void threadF(void) {
	volatile double total;
	double pot;
	int i;
	
	while(1) {
		total = 0;
		pot = 1;
		
		for(i = 1; i < 128; ++i) {
			pot *= 2;
			total += (i*i*i)/pot;
		}
	}
}
