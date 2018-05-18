#include <stdlib.h>
#include <stdbool.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"

#include "rgb.h"
#include "cfaf128x128x16.h"
#include "uart_lab4.h"
#include "colors.h"

// Contexto da tela
tContext sContext;

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

// ===========================================================================
// Message Queues
// ===========================================================================

osMessageQId redMsg;
osMessageQId greenMsg;
osMessageQId blueMsg;
osMessageQId menuMsg;
osMessageQId uartMsg;

osMessageQDef(redMsg, 16, uint32_t);
osMessageQDef(greenMsg, 16, uint32_t);
osMessageQDef(blueMsg, 16, uint32_t);
osMessageQDef(menuMsg, 16, uint32_t);
osMessageQDef(uartMsg, 16, uint32_t);

// ===========================================================================
// Threads
// ===========================================================================

// IDs das threads como variáveis globais
osThreadId idRedLed;
osThreadId idGreenLed;
osThreadId idBlueLed;
osThreadId idMenuManager;
osThreadId idUartManager;

void redLed() {
	osEvent event;
	
	while(1) {
		event = osMessageGet(redMsg, osWaitForever);
		if(event.status == osEventMessage) {
			rgb_write_r((uint8_t) event.value.v);
		}
	}
}

void greenLed() {
	osEvent event;
	
	while(1) {
		event = osMessageGet(greenMsg, osWaitForever);
		if(event.status == osEventMessage) {
			rgb_write_g((uint8_t) event.value.v);
		}
	}
}

void blueLed() {
	osEvent event;
	
	while(1) {
		event = osMessageGet(blueMsg, osWaitForever);
		if(event.status == osEventMessage) {
			rgb_write_b((uint8_t) event.value.v);
		}
	}
}

void menuManager() {
	
}

void uartManager() {
	
}

osThreadDef(redLed, osPriorityNormal, 1, 0);
osThreadDef(greenLed, osPriorityNormal, 1, 0);
osThreadDef(blueLed, osPriorityNormal, 1, 0);
osThreadDef(menuManager, osPriorityNormal, 1, 0);
osThreadDef(uartManager, osPriorityHigh, 1, 0);

int main(void) {
	init_all();
	
	osKernelInitialize();
	
	// Criando message queues
	redMsg = osMessageCreate(osMessageQ(redMsg), NULL);
	greenMsg = osMessageCreate(osMessageQ(greenMsg), NULL);
	blueMsg = osMessageCreate(osMessageQ(blueMsg), NULL);
	menuMsg = osMessageCreate(osMessageQ(menuMsg), NULL);
	uartMsg = osMessageCreate(osMessageQ(uartMsg), NULL);
	
	// Criando threads
	idRedLed = osThreadCreate(osThread(redLed), NULL);
	idGreenLed = osThreadCreate(osThread(greenLed), NULL);
	idBlueLed = osThreadCreate(osThread(blueLed), NULL);
	idMenuManager = osThreadCreate(osThread(menuManager), NULL);
	idUartManager = osThreadCreate(osThread(uartManager), NULL);
	
	osKernelStart();
	osDelay(osWaitForever);
	
}
