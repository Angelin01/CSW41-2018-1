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

const int16_t maxPlayerVelRoad = 500; // Velocidade máxima do jogador na pista

const uint32_t mainLoopDelayMs = 25; // Delay no loop principal

// ===========================================================================
// Variáveis globais
// ===========================================================================

int16_t playerVelX; // Velocidade horizontal do jogador
int16_t playerPosX = playerInitialPosX; // Posição atual do jogador

int16_t playerVelRoad = 0; // Velocidade do jogador na pista (para o odômetro)
int16_t aceleracao = 0; // Aceleração do jogador (positiva ou negativa)

uint16_t leituraJoyX; // Leitura X do joystick
uint16_t leituraJoyY; // Leitura Y do joystick
bool leituraBotao; // Leitura do botão do acelerador

uint64_t odometro = 0; // Distância a mostrar no painel
char stringOdometro[6];

// ===========================================================================
// Mutexes e semáforos
// ===========================================================================

osMutexId idMutex;
osMutexDef(mutex);

// ===========================================================================
// Funções em geral
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

static void intToString(int64_t value, char * pBuf, uint32_t len, uint32_t base, uint8_t zeros){
	static const char* pAscii = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	bool n = false;
	int pos = 0, d = 0;
	int64_t tmpValue = value;

	// the buffer must not be null and at least have a length of 2 to handle one
	// digit and null-terminator
	if (pBuf == NULL || len < 2) return;

	// a valid base cannot be less than 2 or larger than 36
	// a base value of 2 means binary representation. A value of 1 would mean only zeros
	// a base larger than 36 can only be used if a larger alphabet were used.
	if (base < 2 || base > 36) return;

	if (zeros > len) return;
	
	// negative value
	if (value < 0) {
		tmpValue = -tmpValue;
		value = -value;
		pBuf[pos++] = '-';
		n = true;
	}

	// calculate the required length of the buffer
	do {
		pos++;
		tmpValue /= base;
	} while(tmpValue > 0);

	if (pos > len) return; // the len parameter is invalid.

	if(zeros > pos) {
		pBuf[zeros] = '\0';
		do {
			pBuf[d++ + (n ? 1 : 0)] = pAscii[0]; 
		}
		while(zeros > d + pos);
	}
	else {
		pBuf[pos] = '\0';
	}

	pos += d;
	do {
		pBuf[--pos] = pAscii[value % base];
		value /= base;
	} while(value > 0);
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
		leituraJoyY = joy_read_y();
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
		if (leituraBotao) {
			aceleracao = 10;
		}
		else if (leituraJoyY/1350 - 1 == -1) {
			aceleracao = -1;
		}
		else {
			aceleracao = 0;
		}
		
		playerVelRoad += aceleracao;
		if (playerVelRoad < 0) {
			playerVelRoad = 0;
		}
		else if (playerVelRoad > maxPlayerVelRoad) {
			playerVelRoad = maxPlayerVelRoad;
		}
		
		odometro += playerVelRoad;
		
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
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Aguarda mutex
		osMutexWait(idMutex, osWaitForever);
		
		
		
		GrContextForegroundSet(&sContext, ClrBlack);
		GrContextBackgroundSet(&sContext, ClrYellow);
		intToString(odometro/10000, stringOdometro, 6, 10, 5);
		GrStringDrawCentered(&sContext, stringOdometro, -1, 63, 104, true);
		
		// Libera mutex
		osMutexRelease(idMutex);
	}
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
osTimerId idTimerPainelInstrumentos;

void tHandleSaida(void const* args) {
	osSignalSet(idSaida, 0x1);
}

void tHandlePainelInstrumentos(void const* args) {
	osSignalSet(idPainelInstrumentos, 0x1);
}

// Define os timers
osTimerDef(timerSaida, tHandleSaida);
osTimerDef(timerPainelInstrumentos, tHandlePainelInstrumentos);

// Tempo dos timers
const uint32_t msTimerSaida = 50;
const uint32_t msTimerPainelInstrumentos = 300;

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
	idPainelInstrumentos = osThreadCreate(osThread(painelInstrumentos), NULL);
	
	idTimerSaida = osTimerCreate(osTimer(timerSaida), osTimerPeriodic, NULL);
	idTimerPainelInstrumentos = osTimerCreate(osTimer(timerPainelInstrumentos), osTimerPeriodic, NULL);
	
	osTimerStart(idTimerSaida, msTimerSaida);
	osTimerStart(idTimerPainelInstrumentos, msTimerPainelInstrumentos);
	
	osKernelStart();
	
	osDelay(1000);
	osSignalSet(idEntrada, 0x1);
	osDelay(osWaitForever);
}
