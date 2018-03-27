#include <stdbool.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"

#include "cfaf128x128x16.h"

// PAra diagramas de Gantt
#define TICKS_FACTOR 10000
FILE *ganttFile;
uint_32t time;

// Número da próxima thread a executar
int threadNum = 0;

// Número da chave atual
int key = 3;

// Número primo anterior para o segundo teste
int previousPrime = 2;

// Sinaliza se a mensagem foi decodificada com sucesso
bool deuboa = true;

// Contexto da tela
tContext sContext;

// Mensagens codificadas
uint32_t msg1[35] = {0x5a990100,0x6667feff,0x76990100,0x6867feff,0x7b990100,0x6167feff,0x27990100,0x2667feff,0x27990100,0x4c67feff,0x68990100,0x6767feff,0x7b990100,0x5a67feff,0x75990100,0x5a67feff,0x27990100,0x5f67feff,0x6c990100,0x5a67feff,0x7b990100,0x2767feff,0x27990100,0x4b67feff,0x76990100,0x5b67feff,0x27990100,0x4d67feff,0x6f990100,0x6867feff,0x74990100,0x5a67feff,0x7a990100,0x8a650200,0x8ebffeff};;
uint32_t msg2[35] = {0x8820ae8b,0x33e05174,0xa820ae8b,0x2fe05174,0xbc20ae8b,0xdddf5174,0x8520ae8b,0x2fe05174,0xa820ae8b,0x1ee05174,0xb720ae8b,0x25e05174,0x6320ae8b,0x16e05174,0xb220ae8b,0x32e05174,0x6320ae8b,0x11e05174,0xa420ae8b,0x28e05174,0xa820ae8b,0xdddf5174,0x7020ae8b,0xdddf5174,0x9720ae8b,0x25e05174,0xa820ae8b,0xdddf5174,0x9320ae8b,0x2ce05174,0xaf20ae8b,0x26e05174,0xa620ae8b,0x643085d1,0xbedf5174};

// Buffer para guardar a mensagem codificada
uint32_t decoded[35];

// ---String para debug---
char string[4];

// ---Preenche o buffer com a string correspondente a um byte---
void simpleByteToString(uint8_t value, char* buffer) {
	buffer[3] = '\0';
	buffer[2] = (value%10) + '0';
	value /= 10;
	buffer[1] = (value%10) + '0';
	value /= 10;
	buffer[0] = value + '0';
}

// Thread 1: gera uma chave
void generateKey(void const* arg) {
	uint8_t i;
	for (i = 0; true; ++i) {
		time = osKernelSysTick()/TICKS_FACTOR;
		while (threadNum != 1) {
			//osThreadYield();
			osDelay(10000);
		}
		
		//printf("Thread 1: iter %u\n", i);
		simpleByteToString(i, string);
		GrStringDraw(&sContext, (char*)string, -1, (sContext.psFont->ui8MaxWidth)*16,  (sContext.psFont->ui8Height+2)*threadNum, true);
		threadNum = 2;
		//fprintf(ganttFile, "generateKey: %i, %i\n", (int)time, (int)osKernelSysTick()/TICKS_FACTOR);
	}
}

// Thread 2: faz a decodificação usando a chave, preenchendo o buffer "decoded"
void decodeMsg(void const* arg) {
	uint8_t i;
	for (i = 0; true; ++i) {
		time = osKernelSysTick()/TICKS_FACTOR;
		while (threadNum != 2) {
			//osThreadYield();
			osDelay(10000);
		}
		
		//printf("Thread 2: iter %u\n", i);
		simpleByteToString(i, string);
		GrStringDraw(&sContext, (char*)string, -1, (sContext.psFont->ui8MaxWidth)*16,  (sContext.psFont->ui8Height+2)*threadNum, true);
		threadNum = 3;
		//fprintf(ganttFile, "decodeMsg: %i, %i\n", (int)time, (int)osKernelSysTick()/TICKS_FACTOR);
	}
}

// Thread 3: faz o teste da penúltima word (divisão por dois)
void testDivTwo(void const* arg) {
	uint8_t i;
	for (i = 0; true; ++i) {
		time = osKernelSysTick()/TICKS_FACTOR;
		while (threadNum != 3) {
			//osThreadYield();
			osDelay(10000);
		}
		
		//printf("Thread 3: iter %u\n", i);
		simpleByteToString(i, string);
		GrStringDraw(&sContext, (char*)string, -1, (sContext.psFont->ui8MaxWidth)*16,  (sContext.psFont->ui8Height+2)*threadNum, true);
		threadNum = 4;
		//fprintf(ganttFile, "testDivTwo: %i, %i\n", (int)time, (int)osKernelSysTick()/TICKS_FACTOR);
	}
}

// Thread 4: faz o teste da última word (quadrado da chave dividido pelo primo anterior)
void testSquareDiv(void const* arg) {
	uint8_t i;
	for (i = 0; true; ++i) {
		time = osKernelSysTick()/TICKS_FACTOR;
		while (threadNum != 4) {
			//osThreadYield();
			osDelay(10000);
		}
		
		//printf("Thread 4: iter %u\n", i);
		simpleByteToString(i, string);
		GrStringDraw(&sContext, (char*)string, -1, (sContext.psFont->ui8MaxWidth)*16,  (sContext.psFont->ui8Height+2)*threadNum, true);
		threadNum = 5;
		//fprintf(ganttFile, "testSquareDiv: %i, %i\n", (int)time, (int)osKernelSysTick()/TICKS_FACTOR);
	}
}

// Thread 5: faz o teste de primalidade da chave
void testPrime(void const* arg) {
	uint8_t i;
	for (i = 0; true; ++i) {
		time = osKernelSysTick()/TICKS_FACTOR;
		while (threadNum != 5) {
			//osThreadYield();
			osDelay(10000);
		}
		
		//printf("Thread 5: iter %u\n", i);
		simpleByteToString(i, string);
		GrStringDraw(&sContext, (char*)string, -1, (sContext.psFont->ui8MaxWidth)*16,  (sContext.psFont->ui8Height+2)*threadNum, true);
		threadNum = 6;
		//fprintf(ganttFile, "testPrime: %i, %i\n", (int)time, (int)osKernelSysTick()/TICKS_FACTOR);
	}
}

// Thread 6: Mostra os resultados no console
void printResult(void const* arg) {
	uint8_t i;
	for (i = 0; true; ++i) {
		time = osKernelSysTick()/TICKS_FACTOR;
		while (threadNum != 6) {
			//osThreadYield();
			osDelay(10000);
		}
		
		//printf("Thread 6: iter %u\n", i);
		simpleByteToString(i, string);
		GrStringDraw(&sContext,(char*)string, -1, (sContext.psFont->ui8MaxWidth)*16,  (sContext.psFont->ui8Height+2)*threadNum, true);
		threadNum = 1;
		//fprintf(printResult, "testPrime: %i, %i\n", (int)time, (int)osKernelSysTick()/TICKS_FACTOR);
	}
}

// Define as threads
osThreadDef(generateKey, osPriorityNormal, 1, 0);
osThreadDef(decodeMsg, osPriorityNormal, 1, 0);
osThreadDef(testDivTwo, osPriorityNormal, 1, 0);
osThreadDef(testSquareDiv, osPriorityNormal, 1, 0);
osThreadDef(testPrime, osPriorityNormal, 1, 0);
osThreadDef(printResult, osPriorityNormal, 1, 0);


void init_tela() {
	GrContextInit(&sContext, &g_sCfaf128x128x16);
	
	GrFlush(&sContext);
	GrContextFontSet(&sContext, g_psFontFixed6x8);
	
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextBackgroundSet(&sContext, ClrBlack);
	
	//Escreve menu lateral:
	GrStringDraw(&sContext,"---------------------", -1, 0, (sContext.psFont->ui8Height+2)*0, true);
	GrStringDraw(&sContext,"Thread 1: iter ", -1, 0, (sContext.psFont->ui8Height+2)*1, true);
	GrStringDraw(&sContext,"Thread 2: iter ", -1, 0, (sContext.psFont->ui8Height+2)*2, true);
	GrStringDraw(&sContext,"Thread 3: iter ", -1, 0, (sContext.psFont->ui8Height+2)*3, true);
	GrStringDraw(&sContext,"Thread 4: iter ", -1, 0, (sContext.psFont->ui8Height+2)*4, true);
	GrStringDraw(&sContext,"Thread 5: iter ", -1, 0, (sContext.psFont->ui8Height+2)*5, true);
	GrStringDraw(&sContext,"Thread 6: iter ", -1, 0, (sContext.psFont->ui8Height+2)*6, true);
	GrStringDraw(&sContext,"---------------------", -1, 0, (sContext.psFont->ui8Height+2)*7, true);
	GrStringDraw(&sContext,"---------------------", -1, 0, (sContext.psFont->ui8Height+2)*8, true);

}

// Função main: apenas faz inicializações
int main(void) {
	// Inicializa o kernel
	osKernelInitialize();
	
	// Inicializa os recursos
	cfaf128x128x16Init();
	init_tela();
	
	// Cria as threads
	osThreadCreate(osThread(generateKey), NULL);
	osThreadCreate(osThread(decodeMsg), NULL);
	osThreadCreate(osThread(testDivTwo), NULL);
	osThreadCreate(osThread(testSquareDiv), NULL);
	osThreadCreate(osThread(testPrime), NULL);
	osThreadCreate(osThread(printResult), NULL);
	
	// Indica a primeira thread a executar
	threadNum = 1;
	
	osKernelStart();
	
	osDelay(osWaitForever);
}
