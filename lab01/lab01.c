// Arquivos
#include "airplane.h"
#include "bus.h"

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
	const uint8_t* img;
	const int largura = 96;
	const int altura = 64;
	
	// Variáveis da tela
	int8_t posx;
	int8_t posy;
	uint8_t* imgIterator;
	
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
		
		
		if (aviao) {
			img = (uint8_t*) airplaneImage;
		}
		else {
			img = (uint8_t*) busImage;
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
		
		// ASSEMBLY IS COOL!!!
		mov_image();
		
	}	
}
