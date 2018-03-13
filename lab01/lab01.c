#include "cmsis_os.h"
#include "TM4C129.h"                    // Device header
#include <stdbool.h>
#include "grlib/grlib.h"

/*----------------------------------------------------------------------------
 * include libraries from drivers
 *----------------------------------------------------------------------------*/

#include "cfaf128x128x16.h"
#include "buttons.h"
#include "joy.h"

// Arquivos
#include "airplane.h"

// Contexto da tela
tContext sContext;

int vel;
bool aviao = true;
bool sentidoNormal = true;
char buffer[5] = {0, 0, 0, 0, 0};

extern void mov_image(void);

void init_all() {
	cfaf128x128x16Init();
	button_init();
	joy_init();
}

void init_tela() {
	GrContextInit(&sContext, &g_sCfaf128x128x16);
	
	GrFlush(&sContext);
	GrContextFontSet(&sContext, g_psFontFixed6x8);
	
	GrContextBackgroundSet(&sContext, ClrWhite);
	GrContextForegroundSet(&sContext, ClrBlack);
}

int main (void) {
	// Variáveis da imagem
	const uint8_t* img = (uint8_t*) airplaneImage;
	const int largura = 96;
	const int altura = 64;
	
	// Variáveis da tela
	int8_t posx;
	int8_t posy;
	uint8_t* imgIterator;
	
	// Variáveis de movimentação
	uint8_t i;
	
	// Variáveis dos botões
	bool botaoCor = false;
	bool corNormal = true;
	bool corApertado = false;
	
	bool botaoVolta = false;
	bool sentidoApertado = false;
	
	bool botaoAviao = false;
	bool aviaoApertado = false;
	
	// Inicialização
	init_all();
	init_tela();

	while(1){
		vel = joy_read_y()/900 + 1;
		
		corApertado = button_read_s1();
		sentidoApertado = button_read_s2();
		aviaoApertado = joy_read_center();
		
		if(corApertado && !botaoCor) {
			corNormal = !corNormal;
			botaoCor = true;
		}
		else if(!corApertado) {
			botaoCor = false;
		}
		
		if(sentidoApertado && !botaoVolta) {
			sentidoNormal = !sentidoNormal;
			botaoVolta = true;
		}
		else if(!sentidoApertado) {
			botaoVolta = false;
		}
		
		if(aviaoApertado && !botaoAviao) {
			aviao = !aviao;
			botaoAviao = true;
		}
		else if(!aviaoApertado) {
			botaoAviao = false;
		}
		
		imgIterator = ((uint8_t*) img);
		for (posy = 0; posy < altura; ++posy) {
			for (posx = 0; posx < largura; ++posx) {
				// Mostra o pixel
				if (*imgIterator < 128) {
					if (!corNormal) {
						DpyPixelDraw(sContext.psDisplay, posx, posy, sContext.ui32Background);
					}
					else {
						DpyPixelDraw(sContext.psDisplay, posx, posy, sContext.ui32Foreground);
					}
				}
				else {
					if (!corNormal) {
						DpyPixelDraw(sContext.psDisplay, posx, posy, sContext.ui32Foreground);
					}
					else {
						DpyPixelDraw(sContext.psDisplay, posx, posy, sContext.ui32Background);
					}
				}
				
				// Vai para o próximo pixel
				++imgIterator;
			}
		}
		
		// ASSEMBLY !ftw
		mov_image();
		// Lembrar de tentar recolocar o const nas imagens
		
//		if (vel > 0 && vel < 6) {
//			if (aviao) {
//				/* LÓGICA PARA MOVIMENTAÇÃO DO AVIÃO */
//				// O movimento vertical é mais complicado, porque o acesso sequencial ao vetor é horizontal.
//				if (sentidoNormal) {
//					// Move para CIMA de acordo com a velocidade
//					// Para cada coluna...
//					for (posx = 0; posx < largura; ++posx) {
//						// Retira os "vel" primeiros e coloca no buffer
//						for (posy = 0; posy < vel; ++posy) {
//							buffer[posy] = airplaneImage[posx + posy*largura];
//						}

//						// Move os restantes para cima
//						for (posy = vel; posy < altura; ++posy) {
//							airplaneImage[posx + (posy - vel)*largura] = airplaneImage[posx + posy*largura];
//						}

//						// Coloca o buffer nas últimas posições
//						for (posy = altura - vel, i = 0; posy < altura; ++posy, ++i) {
//							airplaneImage[posx + posy*largura] = buffer[i];
//						}
//					}
//				}
//				else {
//					// Move para BAIXO de acordo com a velocidade
//					// Para cada coluna...
//					for (posx = 0; posx < largura; ++posx) {
//						// Retira os "vel" últimos e coloca no buffer
//						for (posy = altura - vel, i = 0; posy < altura; ++posy, ++i) {
//							buffer[i] = airplaneImage[posx + posy*largura];
//						}

//						// Move os restantes para baixo
//						for (posy = (altura-1) - vel; posy >= 0; --posy) {
//							airplaneImage[posx + (posy + vel)*largura] = airplaneImage[posx + posy*largura];
//						}

//						// Coloca o buffer nas primeiras posições
//						for (posy = 0; posy < vel; ++posy) {
//							airplaneImage[posx + posy*largura] = buffer[posy];
//						}
//					}
//				}
//			}
//			else {
//				/* LÓGICA PARA MOVIMENTAÇÃO DO ÔNIBUS */
//				// Como o movimento é horizontal, podemos tratar a imagem como um vetor unidimensional.
//				if (sentidoNormal) {
//					// Move para a ESQUERDA de acordo com a velocidade
//					// Para cada linha...
//					for (posy = 0; posy < altura; ++posy) {
//						// Retira os "vel" primeiros e coloca no buffer
//						for (posx = 0; posx < vel; ++posx) {
//							buffer[posx] = airplaneImage[posx + posy*largura];
//						}

//						// Move os restantes para a esquerda
//						for (posx = vel; posx < largura; ++posx) {
//							airplaneImage[posx - vel + posy*largura] = airplaneImage[posx + posy*largura];
//						}

//						// Coloca o buffer nas últimas posições
//						for (posx = largura - vel, i = 0; posx < largura; ++posx, ++i) {
//							airplaneImage[posx + posy*largura] = buffer[i];
//						}
//					}
//				}
//				else {
//					// Move para a DIREITA de acordo com a velocidade
//					// Para cada linha...
//					for (posy = 0; posy < altura; ++posy) {
//						// Retira os "vel" últimos e coloca no buffer
//						for (posx = largura - vel, i = 0; posx < largura; ++posx, ++i) {
//							buffer[i] = airplaneImage[posx + posy*largura];
//						}

//						// Move os restantes para a direita
//						for (posx = (largura-1) - vel; posx >= 0; --posx) {
//							airplaneImage[posx + vel + posy*largura] = airplaneImage[posx + posy*largura];
//						}

//						// Coloca o buffer nas primeiras posições
//						for (posx = 0; posx < vel; ++posx) {
//							airplaneImage[posx + posy*largura] = buffer[posx];
//						}
//					}
//				}
//			}
//		}
		
	}	
}
