#include <stdbool.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"

#include "cfaf128x128x16.h"
#include "buttons.h"
#include "joy.h"
#include "buzzer.h"

#include "car.h"

// ===========================================================================
// Imagens
// ===========================================================================

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

// Cores
static const uint32_t playerColor = ClrLightGrey;
static const uint32_t scoreBgColor = ClrRed;
static const uint32_t scoreFontColor = ClrBlack;
static const uint32_t scoreFontBgColor = ClrYellow;

static const uint32_t daySkyColor = ClrBlue;
static const uint32_t dayTerrainColor = ClrGreen;
static const uint32_t dayMountainColor = ClrDarkKhaki;
static const uint32_t dayRoadLineColor = ClrGray;

static const uint32_t snowSkyColor = ClrLightBlue;
static const uint32_t snowTerrainColor = ClrWhite;
static const uint32_t snowMountainColor = ClrLightGrey;
static const uint32_t snowRoadLineColor = ClrGray;

static const uint32_t nightSkyColor = ClrGray;
static const uint32_t nightTerrainColor = ClrBlack;
static const uint32_t nightMountainColor = ClrLightGrey;
static const uint32_t nightRoadLineColor = 0x00686868;

uint32_t skyColor = daySkyColor;
uint32_t terrainColor = dayTerrainColor;
uint32_t mountainColor = dayMountainColor;
uint32_t roadLineColor = dayRoadLineColor;

// ===========================================================================
// Constantes
// ===========================================================================

static const int16_t playerInitialPosX = 52; // Posição horizontal do jogador: 64 (meio da pista) - 12 (metade da largura do carro)
static const int16_t playerPosY = 84; // Posição vertical do jogador: 95 (terreno) - 11 (altura do carro)

static const int16_t boundsLeftX = 24; // Limite esquerdo da pista
static const int16_t boundsRightX = 103; // Limite direito da pista

static const int16_t minPlayerVelRoad = 25;  // Velocidade mínima do jogador na pista (após batida/freio)
static const int16_t maxPlayerVelRoad = 800; // Velocidade máxima do jogador na pista

static const uint32_t mainLoopDelayMs = 25; // Delay no loop principal

static const int32_t maxOffsetCurve = 32; // Máximo offset da curva a partir do centro
static const uint32_t curveChangeFactor = 50; // Frequência da alteração da curva (odometro/10000)

static const uint16_t maxBuzzerPeriod = 0x4000; // Período máximo do som do buzzer
static const uint16_t minBuzzerPeriod = 0x1800; // Período mínimo do som do buzzer

static const bool enableBumpRoad = true; // Habilita o "bump" ao bater nas bordas da pista

static const int16_t carDistance = 40; // Distância entre dois carros oponentes
static const int16_t carRespawnFactor = 160; // O quanto um carro vai para a frente no respawn

typedef enum {
	LEFT_CURVE,
	STRAIGHT,
	RIGHT_CURVE
} TipoCurva; // Tipo da curva

typedef enum {
	DAY,
	SNOW,
	NIGHT
} Weather; // Condições de tempo

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
char stringOdometro[6]; // Buffer para converter para string

TipoCurva tipoCurva = STRAIGHT; // Tipo atual da curva
int16_t offsetCurva = 0; // Offset atual da curva a partir do centro
int16_t xLeftCurve[64]; // Pontos da curva esquerda (em função de y)
int16_t xRightCurve[64]; // Pontos da curva direita (em função de y)

Weather weather = DAY; // Condições de tempo atuais
uint16_t numDia = 1; // Dia atual
uint16_t pontuacao = 200; // Carros a ultrapassar

uint16_t buzzerPeriod = maxBuzzerPeriod; // Período atual do buzzer

Car oponenteCar[4];
Car playerCar;

bool gameRunning = false;
bool playEndSound = false;

bool colisaoIrDireita = false;
bool colisaoIrEsquerda = false;

int8_t bateuLateral = 0;

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
	buzzer_init();
	buzzer_per_set(maxBuzzerPeriod);
}

uint32_t getRandomColor() {
	// Não precisa break porque retorna
	switch(rand()%5) {
	case 0:
		return ClrYellow;
	case 1:
		return ClrLightBlue;
	case 2:
		return ClrOrange;
	case 3:
		return ClrCyan;
	case 4:
	default:
		return ClrPink;
	}
}

void init_cars() {
	int i;
	
	setCarHitbox(&playerCar,
	             playerInitialPosX, playerPosY,
	             playerInitialPosX + carBigWidth, playerPosY + carBigHeight);
	playerCar.firstDraw = true;
	playerCar.player = true;
	playerCar.ultrapassado = false;
	playerCar.lane = 0;
	playerCar.color = playerColor;
	playerCar.image = carBigImage;
	
	for (i = 0; i < 4; i++) {
		setCarHitbox(&oponenteCar[i],
					 playerInitialPosX, 0 - carDistance*i,
					 playerInitialPosX + carBigWidth, 0 - carDistance*i + carTinyHeight);
		oponenteCar[i].firstDraw = true;
		oponenteCar[i].player = false;
		oponenteCar[i].ultrapassado = false;
		oponenteCar[i].lane = rand()%3 + 1;
		oponenteCar[i].color = getRandomColor();
		oponenteCar[i].image = carTinyImage;
	}
}

void show_scenario() {
	// Mostra o céu
	GrContextForegroundSet(&sContext, skyColor);
	GrRectFill(&sContext, &skyRect);
	
	// Mostra o terreno
	GrContextForegroundSet(&sContext, terrainColor);
	GrRectFill(&sContext, &terrainRect);
	
	// Mostra as montanhas
	GrContextForegroundSet(&sContext, mountainColor);
	GrContextBackgroundSet(&sContext, skyColor);
	GrImageDraw(&sContext, mountainImage, 0, skyRect.i16YMax - 5);
	GrImageDraw(&sContext, mountainImage, 64, skyRect.i16YMax - 5);
}

void init_tela() {
	GrContextInit(&sContext, &g_sCfaf128x128x16);
	
	GrFlush(&sContext);
	GrContextFontSet(&sContext, g_psFontFixed6x8);
	
	// Mostra o cenário inicial
	show_scenario();
	
	// Faz o desenho inicial do carro
	draw_car(&sContext, &playerCar, terrainColor, true);
	
	// Faz o desenho inicial do painel de instrumentos
	GrContextForegroundSet(&sContext, scoreBgColor);
	GrRectFill(&sContext, &pointsRect);
	GrContextForegroundSet(&sContext, scoreFontColor);
	GrContextBackgroundSet(&sContext, scoreFontBgColor);
	GrStringDrawCentered(&sContext, "00000", -1, 63, 104, true);
	GrStringDraw(&sContext, "1", -1, 48, 113, true);
	GrStringDraw(&sContext, "000", -1, 60, 113, true);
	
	GrContextForegroundSet(&sContext, playerColor);
	GrContextBackgroundSet(&sContext, terrainColor);
}

// Função de conversão de int para string (fornecida pelo professor)
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

// Calcula a posição de cada pixel da estrada (também faz a curva com o offset X)
void generateRoad(int32_t larguraJogo, int32_t alturaJogo, int32_t offsetX, int32_t distBorda) {
	int j;
	int x = 0;
	int y = 0;
	
	int xMedio;
	int xStartLeft;
	int xStartRight;
	int divNorm;
	
	xMedio = larguraJogo/2;
	
	xStartLeft = distBorda;
	xStartRight = larguraJogo - 1 - distBorda;
	
	divNorm = alturaJogo*alturaJogo*alturaJogo;
	
	for (j = alturaJogo - 1; j >= 0; --j) {
		y = alturaJogo - j; // Converte o "y da tela" (j) para o "y matemático" (y)
		
		x = xStartLeft;
		x += (y*(xMedio - xStartLeft))/alturaJogo; // Componente linear
		x += (y*y*y*offsetX)/divNorm;              // Componente de ordem 3
		xLeftCurve[j] = x;
		
		x = xStartRight;
		x += (y*(xMedio - xStartRight))/alturaJogo; // Componente linear
		x += (y*y*y*offsetX)/divNorm;               // Componente de ordem 3
		xRightCurve[j] = x;
	}
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
	int32_t iteracao = 0;
	
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Espera o player apertar o acelerador para iniciar
		if (!gameRunning && leituraBotao && numDia == 1) {
			gameRunning = true;
		}
		
		if (gameRunning) {
			// Aguarda mutex
			osMutexWait(idMutex, osWaitForever);
			
			// Posicionamento lateral
			if (iteracao % 5 == 0) {
				if (colisaoIrDireita || bateuLateral > 0) {
					playerVelX = 1;
				}
				else if (colisaoIrEsquerda || bateuLateral < 0) {
					playerVelX = -1;
				}
				else {
					if (weather == SNOW) {
						playerVelX = (leituraJoyX/1350 - 1) * (2 * playerVelRoad/maxPlayerVelRoad + 1);
					}
					else {
						playerVelX = (leituraJoyX/1350 - 1) * (4 * playerVelRoad/maxPlayerVelRoad + 1);
					}
				}
				
				playerPosX += playerVelX;
				playerCar.hitbox.i16XMin += playerVelX;
				playerCar.hitbox.i16XMax += playerVelX;
				
				if (bateuLateral != 0) {
					bateuLateral -= playerVelX;
				}
			}
			
			// Aceleração
			if (colisaoIrDireita || colisaoIrEsquerda || bateuLateral != 0) {
				aceleracao = 0;
			}
			else if (leituraBotao) {
				aceleracao = 2;
			}
			else if (leituraJoyY/1350 - 1 == -1) {
				aceleracao = -1;
			}
			else {
				aceleracao = 0;
			}
			
			playerVelRoad += aceleracao;
			if (playerVelRoad < minPlayerVelRoad) {
				playerVelRoad = minPlayerVelRoad;
			}
			else if (playerVelRoad > maxPlayerVelRoad) {
				playerVelRoad = maxPlayerVelRoad;
			}
			
			iteracao++;
			
			// Libera mutex
			osMutexRelease(idMutex);
		}
		
		// Envia sinal para a próxima thread
		osSignalSet(idVeiculoOutros, 0x1);
	}
}

void veiculoOutros(void const* args) {
	// Constantes
	static const int16_t limiarTiny = 50;//43;
	static const int16_t limiarSmall = 60;//54;
	static const int16_t limiarAvg = 70;//64;
	static const int16_t limiarNormal = 96;//74;
	
	static const int16_t outrosVelRoad = 300;//200;
	
	uint16_t iteracao = 0;
	int i;
	
	int16_t posy;
	int16_t larguraPistaY;
	int16_t carWidth;
	
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		if (gameRunning) {
			// Aguarda mutex
			osMutexWait(idMutex, osWaitForever);
			
			if (iteracao % 20 == 0) {
				for (i = 0; i < 4; i++) {
					// Altera a posição y do oponente
					oponenteCar[i].hitbox.i16YMin += (playerVelRoad - outrosVelRoad) / 100;
					oponenteCar[i].hitbox.i16YMax += (playerVelRoad - outrosVelRoad) / 100;
					
					// Se este passou de um determinado limiar
					if (oponenteCar[i].hitbox.i16YMin > 127) {
						// Volta ele para a frente e altera o rand da lane
						oponenteCar[i].hitbox.i16YMin -= carRespawnFactor;
						oponenteCar[i].hitbox.i16YMax -= carRespawnFactor;
						oponenteCar[i].lane = rand()%3 + 1;
						oponenteCar[i].ultrapassado = false;
						oponenteCar[i].color = getRandomColor();
					}
					
					// Altera a imagem e as dimensões da hitbox
					if (oponenteCar[i].hitbox.i16YMin < limiarTiny) {
						oponenteCar[i].image = weather == NIGHT ? carTinyNightImage : carTinyImage;
						carWidth = carTinyWidth;
						oponenteCar[i].hitbox.i16YMax = oponenteCar[i].hitbox.i16YMin + carTinyHeight;
					}
					else if (oponenteCar[i].hitbox.i16YMin < limiarSmall) {
						oponenteCar[i].image = weather == NIGHT ? carSmallNightImage : carSmallImage;
						carWidth = carSmallWidth;
						oponenteCar[i].hitbox.i16YMax = oponenteCar[i].hitbox.i16YMin + carSmallHeight;
					}
					else if (oponenteCar[i].hitbox.i16YMin < limiarAvg) {
						oponenteCar[i].image = weather == NIGHT ? carAvgNightImage : carAvgImage;
						carWidth = carAvgWidth;
						oponenteCar[i].hitbox.i16YMax = oponenteCar[i].hitbox.i16YMin + carAvgHeight;
					}
					else if (oponenteCar[i].hitbox.i16YMin < limiarNormal) {
						oponenteCar[i].image = weather == NIGHT ? carNormalNightImage : carNormalImage;
						carWidth = carNormalWidth;
						oponenteCar[i].hitbox.i16YMax = oponenteCar[i].hitbox.i16YMin + carNormalHeight;
					}
					else {
						oponenteCar[i].image = weather == NIGHT ? carBigNightImage : carBigImage;
						carWidth = carBigWidth;
						oponenteCar[i].hitbox.i16YMax = oponenteCar[i].hitbox.i16YMin + carBigHeight;
					}
					
					// Se o carro estiver dentro da pista, acerta o valor da sua posição x.
					posy = getCarPosY(&(oponenteCar[i]));
					if (posy >= 32 && posy <= 95) {
						larguraPistaY = xRightCurve[posy-32] - xLeftCurve[posy-32];
						oponenteCar[i].hitbox.i16XMin = xLeftCurve[posy-32] + larguraPistaY/4*oponenteCar[i].lane + 1 - carWidth/2;
						oponenteCar[i].hitbox.i16XMax = xLeftCurve[posy-32] + larguraPistaY/4*oponenteCar[i].lane + 1 + carWidth/2;
					}
				}
			}
			
			iteracao++;
			
			// Libera mutex
			osMutexRelease(idMutex);
		}
		
		// Envia sinal para a próxima thread
		osSignalSet(idGerenciadorTrajeto, 0x1);
	}
}

void gerenciadorTrajeto(void const* args) {
	static const uint32_t moduloAtualizaCurva = 8;
	static const uint32_t moduloAtualizaWeather = 10000;
	static const uint32_t moduloCentrifuga = 32;
	static const int32_t fatorVel = 4;
	
	uint32_t iteracao = 0;
	uint32_t contadorCurva = 1;
	
	int32_t i;
	
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		if (gameRunning) {
			// Aguarda mutex
			osMutexWait(idMutex, osWaitForever);
			
			// Aumenta o odômetro do painel
			odometro += playerVelRoad;
			
			// Troca o tipo da curva quando necessário
			if (odometro/10000 > contadorCurva*curveChangeFactor) {
				contadorCurva++;
				tipoCurva = rand()%3;
			}
			
			// Troca a condição climática quando necessário
			if (iteracao != 0 && iteracao % moduloAtualizaWeather == 0) {
				weather = (weather + 1) % 3;
				if (weather == 0) {
					++numDia;
					if (numDia > 2) {
						gameRunning = false;
						playEndSound = true;
						buzzerPeriod = 0x200;
						osDelay(1000);
						playEndSound = false;
					}
				}
			}
			
			// Computa a nova curva caso ela ainda não esteja no ponto desejado
			// Apenas atualiza a curva a cada "moduloAtualizaCurva" iterações
			if (iteracao % moduloAtualizaCurva == 0) {
				// Apenas recalcula a curva se a velocidade for maior que zero
				if (playerVelRoad > 0) {
					// Seleciona o tipo da curva
					switch (tipoCurva) {
					case STRAIGHT:
						// Se a curva está para a direita, diminui o offset
						if (offsetCurva > 0) {
							// Altera o offset de acordo com a velocidade
							offsetCurva -= 1 + playerVelRoad * fatorVel / maxPlayerVelRoad;
							if (offsetCurva < 0) {
								offsetCurva = 0; // Corrige se passar do ponto desejado
							}
							generateRoad(128, 64, offsetCurva, 16);
						}
						// Se a curva está para a esquerda, aumenta o offset
						else if (offsetCurva < 0) {
							offsetCurva += 1 + playerVelRoad * fatorVel / maxPlayerVelRoad;
							if (offsetCurva > 0) {
								offsetCurva = 0;
							}
							generateRoad(128, 64, offsetCurva, 16);
						}
						break;
					case LEFT_CURVE:
						if (offsetCurva > (-1)*maxOffsetCurve) {
							offsetCurva -= 1 + playerVelRoad * fatorVel / maxPlayerVelRoad;
							if (offsetCurva < (-1)*maxOffsetCurve) {
								offsetCurva = (-1)*maxOffsetCurve;
							}
							generateRoad(128, 64, offsetCurva, 16);
						}
						break;
					case RIGHT_CURVE:
						if (offsetCurva < maxOffsetCurve) {
							offsetCurva += 1 + playerVelRoad * fatorVel / maxPlayerVelRoad;
							if (offsetCurva > maxOffsetCurve) {
								offsetCurva = maxOffsetCurve;
							}
							generateRoad(128, 64, offsetCurva, 16);
						}
						break;
					default:
						break;
					}
				}
			}
			
			// "Força centrífuga"
			if (tipoCurva == LEFT_CURVE || tipoCurva == RIGHT_CURVE) {
				if (playerVelRoad != 0 && iteracao % moduloCentrifuga == 0) {
					if (tipoCurva == LEFT_CURVE) {
						++playerPosX;
						++(playerCar.hitbox.i16XMin);
						++(playerCar.hitbox.i16XMax);
					}
					else if (tipoCurva == RIGHT_CURVE) {
						--playerPosX;
						--(playerCar.hitbox.i16XMin);
						--(playerCar.hitbox.i16XMax);
					}
				}
			}
			
			// Check de colisão e de ultrapassagem
			for (i = 0; i < 4; ++i) {
				if (GrRectOverlapCheck(&(playerCar.hitbox), &(oponenteCar[i].hitbox))) {
					if (!colisaoIrDireita && !colisaoIrEsquerda) {
						playerVelRoad = minPlayerVelRoad;
						if (getCarPosX(&playerCar) < 64) {
							colisaoIrDireita = true;
						}
						else {
							colisaoIrEsquerda = true;
						}
					}
				}
				
				if (!(oponenteCar[i].ultrapassado) && playerCar.hitbox.i16YMax < oponenteCar[i].hitbox.i16YMin) {
					if (pontuacao > 0) {
						--pontuacao;
					}
					oponenteCar[i].ultrapassado = true;
				}
				else if (oponenteCar[i].ultrapassado && playerCar.hitbox.i16YMax > oponenteCar[i].hitbox.i16YMin) {
					if (pontuacao != 0) {
						++pontuacao;
					}
					oponenteCar[i].ultrapassado = false;
				}
			}
			
			// Player bateu no lado esquerdo da pista
			if (playerPosX < boundsLeftX) {
				playerPosX = boundsLeftX;
				playerCar.hitbox.i16XMin = boundsLeftX;
				playerCar.hitbox.i16XMax = boundsLeftX + carBigWidth;
				colisaoIrEsquerda = false;
				
				// Reduz a velocidade do carro e dá um "bump" para o lado
				if (enableBumpRoad) {
					bateuLateral = 2;
					playerVelRoad -= 10;
					if (playerVelRoad < minPlayerVelRoad) {
						playerVelRoad = minPlayerVelRoad;
					}
				}
			}
			// Player bateu no lado direito da pista
			else if (playerPosX > boundsRightX - carBigWidth) {
				playerPosX = boundsRightX - carBigWidth;
				playerCar.hitbox.i16XMin = boundsRightX - carBigWidth;
				playerCar.hitbox.i16XMax = boundsRightX;
				colisaoIrDireita = false;
				
				// Reduz a velocidade do carro e dá um "bump" para o lado
				if (enableBumpRoad) {
					bateuLateral = -2;
					playerVelRoad -= 10;
					if (playerVelRoad < minPlayerVelRoad) {
						playerVelRoad = minPlayerVelRoad;
					}
				}
			}
			
			// Incrementa a iteração
			iteracao++;
			
			// Libera mutex
			osMutexRelease(idMutex);
		}
		
		// Delay no input
		osDelay(mainLoopDelayMs);
		
		// Envia sinal para a próxima thread
		osSignalSet(idEntrada, 0x1);
	}
}

void saida(void const* args) {
	bool firstIter = true;
	bool weatherChange = false;
	int16_t j;
	
	// Para identificar se o carro se moveu depois do último draw
	int8_t oldCarX = playerInitialPosX;
	
	// Para limpar a curva anterior
	int16_t oldXLeftCurve[64];
	int16_t oldXRightCurve[64];
	int16_t oldOffsetCurva;
	
	// Para a mudança de condição de tempo
	Weather oldWeather = DAY;
	
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Aguarda mutex
		osMutexWait(idMutex, osWaitForever);
		
		// Faz buzz se bateu ou está acelerando ou acabou
		if (playEndSound == true) {
			buzzer_write(true);
			buzzerPeriod += 0x20;
			buzzer_per_set(buzzerPeriod);
		}
		else if (colisaoIrDireita || colisaoIrEsquerda || bateuLateral != 0) {
			buzzer_per_set(0x8000);
			buzzer_write(true);
		}
		else if (aceleracao > 0) {
			buzzerPeriod -= 0x80;
			if (buzzerPeriod < minBuzzerPeriod) {
				buzzerPeriod = maxBuzzerPeriod;
			}
			buzzer_per_set(buzzerPeriod);
			buzzer_write(true);
		}
		else {
			buzzer_write(false);
		}
		
		// Copia dados iniciais da curva e mostra, caso seja a primeira iteração
		if (firstIter) {
			firstIter = false;
			
			oldOffsetCurva = offsetCurva;
			for (j = 0; j < 64; ++j) {
				oldXLeftCurve[j] = xLeftCurve[j];
				oldXRightCurve[j] = xRightCurve[j];
			}
			
			for (j = 63; j >= 0; --j) {
				GrContextForegroundSet(&sContext, j/16*0x181818 + roadLineColor);
				GrPixelDraw(&sContext, xLeftCurve[j], j+32);
				GrPixelDraw(&sContext, xRightCurve[j], j+32);
			}
		}
		
		// Altera as cores de acordo com a condição de tempo
		if (oldWeather != weather) {
			oldWeather = weather;
			
			switch (weather) {
			case DAY:
				skyColor = daySkyColor;
				terrainColor = dayTerrainColor;
				mountainColor = dayMountainColor;
				roadLineColor = dayRoadLineColor;
				break;
			case SNOW:
				skyColor = snowSkyColor;
				terrainColor = snowTerrainColor;
				mountainColor = snowMountainColor;
				roadLineColor = snowRoadLineColor;
				break;
			case NIGHT:
				skyColor = nightSkyColor;
				terrainColor = nightTerrainColor;
				mountainColor = nightMountainColor;
				roadLineColor = nightRoadLineColor;
				break;
			}
			
			show_scenario();
			weatherChange = true;
		}
		
		if (oldOffsetCurva != offsetCurva || weatherChange) {
			oldOffsetCurva = offsetCurva;
			for (j = 63; j >= 0; --j) {
				// Apaga a curva anterior
				GrContextForegroundSet(&sContext, terrainColor);
				GrPixelDraw(&sContext, oldXLeftCurve[j], j+32);
				GrPixelDraw(&sContext, oldXRightCurve[j], j+32);
				
				// Mostra a nova curva
				GrContextForegroundSet(&sContext, j/16*0x181818 + roadLineColor);
				GrPixelDraw(&sContext, xLeftCurve[j], j+32);
				GrPixelDraw(&sContext, xRightCurve[j], j+32);
				
				// Altera o vetor antigo
				oldXLeftCurve[j] = xLeftCurve[j];
				oldXRightCurve[j] = xRightCurve[j];
			}
		}
		
		for (j = 0; j < 4; ++j) {
			draw_car(&sContext, &(oponenteCar[j]), terrainColor, weatherChange);
		}
		
		if (oldCarX != playerPosX || weatherChange) {			
			// Mostra o carro e já apaga o rastro
			draw_car(&sContext, &playerCar, terrainColor, true);
			
			// Guarda a posição X dessa iteração para utilizar na próxima
			oldCarX = playerPosX;
			
			weatherChange = false;
		}
		
		// Libera mutex
		osMutexRelease(idMutex);
	}
}

void painelInstrumentos(void const* args) {
	uint16_t oldNumDia = 1;
	
	while (true) {
		// Aguarda sinal
		osSignalWait(0x1, osWaitForever);
		
		// Aguarda mutex
		osMutexWait(idMutex, osWaitForever);
		
		GrContextForegroundSet(&sContext, scoreFontColor);
		GrContextBackgroundSet(&sContext, scoreFontBgColor);
		
		intToString(odometro/10000, stringOdometro, 6, 10, 5);
		GrStringDrawCentered(&sContext, stringOdometro, -1, 63, 104, true);
		
		intToString(pontuacao, stringOdometro, 6, 10, 3);
		GrStringDraw(&sContext, stringOdometro, 3, 60, 113, true);
		
		if (oldNumDia != numDia) {
			oldNumDia = numDia;
			intToString(numDia, stringOdometro, 6, 10, 0);
			GrStringDraw(&sContext, stringOdometro, 1, 48, 113, true);
		}
		
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
const uint32_t msTimerSaida = 100;
const uint32_t msTimerPainelInstrumentos = 300;

// ===========================================================================
// Função main
// ===========================================================================

int main (void) {
	osKernelInitialize();
	
	// Inicialização
	init_cars();
	init_all();
	init_tela();
	
	generateRoad(128, 64, offsetCurva, 16);
	
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
