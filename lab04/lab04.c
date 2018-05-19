#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"
#include "inc/hw_ints.h"

#include "rgb.h"
#include "cfaf128x128x16.h"
#include "uart_lab4.h"
#include "colors.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

// Outras estruturas
#define UP 0
#define DOWN 1
#define NEXT_GROUP 2 
#define PREVIOUS_GROUP 3
#define INVALID 99

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

void redLed(void const* args) {
	osEvent event;
	
	while(1) {
		event = osMessageGet(redMsg, osWaitForever);
		if(event.status == osEventMessage) {
			rgb_write_r((uint8_t) event.value.v);
		}
	}
}

void greenLed(void const* args) {
	osEvent event;
	
	while(1) {
		event = osMessageGet(greenMsg, osWaitForever);
		if(event.status == osEventMessage) {
			rgb_write_g((uint8_t) event.value.v);
		}
	}
}

void blueLed(void const* args) {
	osEvent event;
	
	while(1) {
		event = osMessageGet(blueMsg, osWaitForever);
		if(event.status == osEventMessage) {
			rgb_write_b((uint8_t) event.value.v);
		}
	}
}

void drawMenu(uint8_t selectedGroup, uint8_t selectedColor) {
	int i;
	char strHex[8];
	static const tRectangle telaInteira = {0, 0, 128, 128};
	
	// Limpa tela
	GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &telaInteira);
	GrContextForegroundSet(&sContext, ClrWhite);
	
	// Printa a cor selecionada em HEX com cor que eh
	GrContextForegroundSet(&sContext, colorGroups[selectedGroup]->colorValues[selectedColor]);
	sprintf(strHex, "0x%0.6X", colorGroups[selectedGroup]->colorValues[selectedColor]);
	GrStringDraw(&sContext, strHex, -1, 80, 4, true);
	GrContextForegroundSet(&sContext, ClrWhite);
	
	GrStringDraw(&sContext, colorGroups[selectedGroup]->groupName, -1, 4, 4, true);
	GrLineDraw(&sContext, 0, 13, 128, 13);
	
	for(i = 0; i < colorGroups[selectedGroup]->count; ++i) {
		if(i == selectedColor) {
			GrContextForegroundSet(&sContext, ClrBlack);
			GrContextBackgroundSet(&sContext, ClrWhite);
			GrStringDraw(&sContext, colorGroups[selectedGroup]->colorNames[i], -1, 4, 16 + 10*i, true);
			GrContextForegroundSet(&sContext, ClrWhite);
			GrContextBackgroundSet(&sContext, ClrBlack);
		}
		else {
			GrStringDraw(&sContext, colorGroups[selectedGroup]->colorNames[i], -1, 4, 16 + 10*i, true);
		}
	}
}

void menuManager(void const* args) {
	osEvent event;
	volatile int8_t selectedGroup = 0; 
	volatile int8_t selectedColor = 0;
	
	osMessagePut(redMsg, rgb_color_r(colorGroups[selectedGroup]->colorValues[selectedColor]), osWaitForever);
	osMessagePut(greenMsg, rgb_color_g(colorGroups[selectedGroup]->colorValues[selectedColor]), osWaitForever);
	osMessagePut(blueMsg, rgb_color_b(colorGroups[selectedGroup]->colorValues[selectedColor]), osWaitForever);
	drawMenu(selectedGroup, selectedColor);
	
	while(1) {
		event = osMessageGet(menuMsg, osWaitForever);
		if(event.status == osEventMessage) {
			switch(event.value.v) {
				case DOWN:
					if(++selectedColor >= colorGroups[selectedGroup]->count) {
						selectedColor = 0;
					}
					break;
					
				case UP:
					if(--selectedColor == -1) {
						selectedColor = colorGroups[selectedGroup]->count - 1;
					}
					break;
					
				case NEXT_GROUP:
					selectedColor = 0;
					if(++selectedGroup == GROUP_COUNT) {
						selectedGroup = 0;
					}
					break;
					
				case PREVIOUS_GROUP:
					selectedColor = 0;
					if(--selectedGroup == -1) {
						selectedGroup = GROUP_COUNT - 1;
					}
					break;
					
				default:
					// Stuffs?
					break;
			}
			osMessagePut(redMsg, rgb_color_r(colorGroups[selectedGroup]->colorValues[selectedColor]), osWaitForever);
			osMessagePut(greenMsg, rgb_color_g(colorGroups[selectedGroup]->colorValues[selectedColor]), osWaitForever);
			osMessagePut(blueMsg, rgb_color_b(colorGroups[selectedGroup]->colorValues[selectedColor]), osWaitForever);
			drawMenu(selectedGroup, selectedColor);
		}
	}
}

void uartManager(void const* args) {
	osEvent event;
	
	while(1) {
		event = osMessageGet(uartMsg, osWaitForever);
		if(event.status == osEventMessage) {
			switch((char)event.value.v) {
				case 'w':
				case 'W':
					osMessagePut(menuMsg, UP, osWaitForever);
					break;
					
				case 'a':
				case 'A':
					osMessagePut(menuMsg, PREVIOUS_GROUP, osWaitForever);
					break;
					
				case 's':
				case 'S':
					osMessagePut(menuMsg, DOWN, osWaitForever);
					break;
					
				case 'd':
				case 'D':
					osMessagePut(menuMsg, NEXT_GROUP, osWaitForever);
					break;
					
				default:
					osMessagePut(menuMsg, INVALID, osWaitForever);
					break;
			}
		}
	}
}

osThreadDef(redLed, osPriorityNormal, 1, 0);
osThreadDef(greenLed, osPriorityNormal, 1, 0);
osThreadDef(blueLed, osPriorityNormal, 1, 0);
osThreadDef(menuManager, osPriorityNormal, 1, 0);
osThreadDef(uartManager, osPriorityHigh, 1, 0);

void uartIntHandler() {
	osMessagePut(uartMsg, uart_getChar(), 0);
}

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
	
	uart_regIntHandler(uartIntHandler);
	
	osKernelStart();
	osDelay(osWaitForever);
	
}
