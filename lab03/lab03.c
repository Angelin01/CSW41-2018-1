#include <stdbool.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"

#include "cfaf128x128x16.h"
#include "buttons.h"
#include "joy.h"

// ===========================================================================
// Imagens
// ===========================================================================

const uint8_t carImage[] = {
	IMAGE_FMT_1BPP_COMP,
	24, 0,
	11, 0,
	
	0x02, 0x1c, 0x7e, 0x38, 0x1f, 0xff, 0xf8, 0xe9, 0xe0, 0x9c, 0xba, 0xc7,
	0xe3, 0xa2, 0xd4, 0xa2, 0x1c, 0x7e, 0x00, 0x07,
};

const uint8_t carWidth = 24;
const uint8_t carHeight = 11;

const uint8_t mountainImage[] = {
	IMAGE_FMT_1BPP_COMP,
	64, 0,
	6, 0,
	
	0x09, 0x00, 0x00, 0x0f, 0xf0, 0x04, 0xff, 0xff, 0x8c, 0x11, 0xff, 0xff,
	0xf0, 0x6a, 0xff, 0xff, 0xff, 0xe9, 0xd8, 0x8a, 0xb2, 0xf0, 0x8c, 0x79,
};

// ===========================================================================
// Retângulos para desenhar
// ===========================================================================

const tRectangle skyRect = {
	0,   // i16XMin
	0,   // i16YMin
	127, // i16XMax
	31   // i16YMax
};
const tRectangle terrainRect = {0, 32, 127, 95};
const tRectangle pointsRect = {40, 100, 86, 121};

// ===========================================================================
// Tela
// ===========================================================================

// Contexto da tela
tContext sContext;

// ===========================================================================
// Constantes
// ===========================================================================

const int16_t playerInitialPosX = 52; // Posição horizontal do jogador: 64 (meio da pista) - 12 (metade da largura do carro)
const int16_t playerPosY = 84; // Posição vertical do jogador: 95 (terreno) - 11 (altura do carro)

const int16_t boundsLeftX = 22; //16; // Limite esquerdo da pista
const int16_t boundsRightX = 105; //111; // Limite direito da pista

const uint32_t mainLoopDelayMs = 25; // Delay no loop principal

// ===========================================================================
// Variáveis globais
// ===========================================================================

int16_t playerVelX; // Velocidade horizontal do jogador
int16_t playerPosX = playerInitialPosX; // Posição atual do jogador

uint16_t leituraJoyX; // Leitura X do joystick
bool leituraBotao; // Leitura do botão do acelerador

// ===========================================================================
// Mutexes e semáforos
// ===========================================================================

osMutexId idMutex;
osMutexDef(mutex);

// ===========================================================================
// Funções de inicialização
// ===========================================================================

void init_all() {
	cfaf128x128x16Init();
	button_init();
	joy_init();
}

void init_tela() {
	GrContextInit(&sContext, &g_sCfaf128x128x16);
	
	GrFlush(&sContext);
	GrContextFontSet(&sContext, g_psFontFixed6x8);
	
	// Mostra o céu
	GrContextForegroundSet(&sContext, ClrBlue);
	GrRectFill(&sContext, &skyRect);
	
	// Mostra o terreno
	GrContextForegroundSet(&sContext, ClrGreen);
	GrRectFill(&sContext, &terrainRect);
	
	// Mostra as montanhas
	GrContextForegroundSet(&sContext, ClrDarkKhaki);
	GrContextBackgroundSet(&sContext, ClrBlue);
	GrImageDraw(&sContext, mountainImage, 0, skyRect.i16YMax - 5);
	GrImageDraw(&sContext, mountainImage, 64, skyRect.i16YMax - 5);
	
	// Mostra as linhas
	GrContextForegroundSet(&sContext, ClrGray);
	GrContextBackgroundSet(&sContext, ClrGreen);
	GrLineDraw(&sContext, 63, 32, 15, 95);
	GrLineDraw(&sContext, 64, 32, 112, 95);
	
	// Faz o desenho inicial do carro
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextBackgroundSet(&sContext, ClrGreen);
	GrImageDraw(&sContext, carImage, playerPosX, playerPosY);
	
	// Faz o desenho inicial do painel de instrumentos
	GrContextForegroundSet(&sContext, ClrRed);
	GrRectFill(&sContext, &pointsRect);
	GrContextForegroundSet(&sContext, ClrBlack);
	GrContextBackgroundSet(&sContext, ClrYellow);
	GrStringDrawCentered(&sContext, "00000", -1, 63, 104, true);
	GrStringDraw(&sContext, "0", -1, 48, 113, true);
	GrStringDraw(&sContext, "000", -1, 60, 113, true);
	
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextBackgroundSet(&sContext, ClrGreen);
}

// ===========================================================================
// Threads
// ===========================================================================

// IDs das threads como variáveis globais
osThreadId idEntrada;
osThreadId idVeiculoJogador;
osThreadId idVeiculoOutros;
osThreadId idGerenciadorTrajeto;
osThreadId idSaida;
osThreadId idPainelInstrumentos;

void entrada(void const* args) {
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		leituraJoyX = joy_read_x();
		leituraBotao = button_read_s1();
		
		// Envia sinal para a próxima thread
		osSignalSet(idVeiculoJogador, 0x1);
	}
}

void veiculoJogador(void const* args) {
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Aguarda mutex
		osMutexWait(idMutex, osWaitForever);
		
		// Posicionamento lateral
		playerVelX = leituraJoyX/1350 - 1;
		playerPosX += playerVelX;
		
		// TODO: Aceleração
		
		// Libera mutex
		osMutexRelease(idMutex);
		
		// Envia sinal para a próxima thread
		osSignalSet(idVeiculoOutros, 0x1);
	}
}

void veiculoOutros(void const* args) {
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Aguarda mutex
		osMutexWait(idMutex, osWaitForever);
		
		// TODO: Outros veículos
		
		// Libera mutex
		osMutexRelease(idMutex);
		
		// Envia sinal para a próxima thread
		osSignalSet(idGerenciadorTrajeto, 0x1);
	}
}

void gerenciadorTrajeto(void const* args) {
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Aguarda mutex
		osMutexWait(idMutex, osWaitForever);
		
		// TODO: Muitas coisas:
		//  - Curvas
		//  - Condições de tempo
		//  - Colisão com outros veículos
		//  - Pontuação
		//  - Quilometragem
		
		// Player se moveu lateralmente
		if (playerVelX != 0) {
			// Player bateu no lado esquerdo da pista
			if (playerPosX < boundsLeftX) {
				playerPosX = boundsLeftX;
			}
			// Player bateu no lado direito da pista
			else if (playerPosX > boundsRightX - carWidth) {
				playerPosX = boundsRightX - carWidth;
			}
		}
		
		// Libera mutex
		osMutexRelease(idMutex);
		
		// Delay no input
		osDelay(mainLoopDelayMs);
		
		// Envia sinal para a próxima thread
		osSignalSet(idEntrada, 0x1);
	}
}

void saida(void const* args) {
	// Para limpar o rastro do carro
	tRectangle clearRect;
	int8_t oldCarX = playerInitialPosX;
	
	clearRect.i16YMin = playerPosY;
	clearRect.i16YMax = playerPosY + carHeight;
	
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Aguarda mutex
		osMutexWait(idMutex, osWaitForever);
		
		if (oldCarX != playerPosX) {
			// Calcula o rastro gerado se o player se moveu
			if (oldCarX < playerPosX) {
				clearRect.i16XMin = oldCarX;
				clearRect.i16XMax = playerPosX;
			}
			else {
				clearRect.i16XMin = playerPosX + carWidth;
				clearRect.i16XMax = oldCarX + carWidth;
			}
			
			// Mostra o carro
			GrContextForegroundSet(&sContext, ClrWhite);
			GrContextBackgroundSet(&sContext, ClrGreen);
			GrImageDraw(&sContext, carImage, playerPosX, playerPosY);
			
			// Apaga o rastro
			GrContextForegroundSet(&sContext, ClrGreen);
			GrRectFill(&sContext, &clearRect);
			GrContextForegroundSet(&sContext, ClrWhite);
			
			// Guarda a posição X dessa iteração para utilizar na próxima
			oldCarX = playerPosX;
		}
		
		// Libera mutex
		osMutexRelease(idMutex);
	}
}

void painelInstrumentos(void const* args) {
	// TODO: tudo
}

// Define as threads
osThreadDef(entrada, osPriorityNormal, 1, 0);
osThreadDef(veiculoJogador, osPriorityNormal, 1, 0);
osThreadDef(veiculoOutros, osPriorityNormal, 1, 0);
osThreadDef(gerenciadorTrajeto, osPriorityNormal, 1, 0);
osThreadDef(saida, osPriorityNormal, 1, 0);
osThreadDef(painelInstrumentos, osPriorityNormal, 1, 0);

// ===========================================================================
// Timers
// ===========================================================================

osTimerId idTimerSaida;

void tHandleSaida(void const* args) {
	osSignalSet(idSaida, 0x1);
}

// Define os timers
osTimerDef(timerSaida, tHandleSaida);

// Tempo dos timers
const uint32_t msTimerSaida = 50;

// ===========================================================================
// Função main
// ===========================================================================

int main (void) {
	osKernelInitialize();
	
	// Inicialização
	init_all();
	init_tela();
	
	idMutex = osMutexCreate(osMutex(mutex));
	
	idEntrada = osThreadCreate(osThread(entrada), NULL);
	idVeiculoJogador = osThreadCreate(osThread(veiculoJogador), NULL);
	idVeiculoOutros = osThreadCreate(osThread(veiculoOutros), NULL);
	idGerenciadorTrajeto = osThreadCreate(osThread(gerenciadorTrajeto), NULL);
	idSaida = osThreadCreate(osThread(saida), NULL);
	
	idTimerSaida = osTimerCreate(osTimer(timerSaida), osTimerPeriodic, NULL);
	
	osTimerStart(idTimerSaida, msTimerSaida);
	
	osKernelStart();
	
	osDelay(1000);
	osSignalSet(idEntrada, 0x1);
	osDelay(osWaitForever);
}
