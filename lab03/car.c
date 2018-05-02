#include "car.h"

// Imagem do carro grande
const uint8_t carBigImage[] = {
	IMAGE_FMT_1BPP_COMP,
	24, 0,
	11, 0,
	
	0x02, 0x1c, 0x7e, 0x38, 0x1f, 0xff, 0xf8, 0xe9, 0xe0, 0x9c, 0xba, 0xc7,
	0xe3, 0xa2, 0xd4, 0xa2, 0x1c, 0x7e, 0x00, 0x07,
};
const uint8_t carBigNightImage[] = {
	IMAGE_FMT_1BPP_COMP,
	24, 0,
	11, 0,

	0xe3, 0x07, 0x07, 0x01, 0x03, 0xc3, 0xc0, 0xe9, 0x07,
};
const uint8_t carBigWidth = 24;
const uint8_t carBigHeight = 11;

// Imagem do carro normal
const uint8_t carNormalImage[] = {
	IMAGE_FMT_1BPP_COMP,
	18, 0,
	8, 0,
	
	0x00, 0x03, 0xf0, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xf1, 0x02, 0xc0, 0xff,
	0xfe, 0x00, 0x1f, 0xff, 0xd4, 0xc0, 0x40, 0xe3, 0x5a,
};
const uint8_t carNormalNightImage[] = {
	IMAGE_FMT_1BPP_COMP,
	18, 0,
	8, 0,

	0xcc, 0x07, 0x01, 0x1e, 0x1e, 0xe9, 0x07,
};
const uint8_t carNormalWidth = 18;
const uint8_t carNormalHeight = 8;

// Imagem do carro media
const uint8_t carAvgImage[] = {
	IMAGE_FMT_1BPP_COMP,
	10, 0,
	6, 0,
	
	0x00, 0x1e, 0x00, 0x7f, 0x80, 0xbf, 0x80, 0x7f, 0x40, 0x80, 0xe2,
};
const uint8_t carAvgNightImage[] = {
	IMAGE_FMT_1BPP_COMP,
	10, 0,
	6, 0,

	0xa0, 0x04, 0x33, 0x07,
};
const uint8_t carAvgWidth = 10;
const uint8_t carAvgHeight = 6;

// Imagem do carro pequeno
const uint8_t carSmallImage[] = {
	IMAGE_FMT_1BPP_UNCOMP,
	6, 0,
	3, 0,
	
	0x78, 0xfc, 0x78,
};
const uint8_t carSmallNightImage[] = {
	IMAGE_FMT_1BPP_UNCOMP,
	6, 0,
	3, 0,

	0x00, 0x48, 0x00,
};
const uint8_t carSmallWidth = 6;
const uint8_t carSmallHeight = 3;

// Imagem do carro minusculo
const uint8_t carTinyImage[] = {
	IMAGE_FMT_1BPP_UNCOMP,
	4, 0,
	1, 0,
	
	0xf0,
};
const uint8_t carTinyNightImage[] = {
	IMAGE_FMT_1BPP_UNCOMP,
	4, 0,
	1, 0,

	0x60,
};
const uint8_t carTinyWidth = 4;
const uint8_t carTinyHeight = 1;

void draw_car(tContext* context, Car* car, uint32_t terrainColor, bool forceRedraw) {
	tRectangle rastroPlayer;
	
	if (car == 0) {
		return;
	}
	
	if (car->player) {
		if (car->firstDraw) {
			car->firstDraw = false;
		}
		else {
			// Calcula o rastro gerado
			if (car->clearRect.i16XMin < car->hitbox.i16XMin) {
				rastroPlayer.i16XMin = car->clearRect.i16XMin;
				rastroPlayer.i16XMax = car->hitbox.i16XMin;
			}
			else {
				rastroPlayer.i16XMin = car->hitbox.i16XMax;
				rastroPlayer.i16XMax = car->clearRect.i16XMax;
			}
			rastroPlayer.i16YMin = car->hitbox.i16YMin;
			rastroPlayer.i16YMax = car->hitbox.i16YMax;

			// Apaga o rastro
			GrContextForegroundSet(context, terrainColor);
			GrRectFill(context, &rastroPlayer);
			GrContextForegroundSet(context, car->color);
		}
			
		// Mostra o carro
		GrContextForegroundSet(context, car->color);
		GrContextBackgroundSet(context, terrainColor);
		GrImageDraw(context, car->image, car->hitbox.i16XMin, car->hitbox.i16YMin);
			
		// Guarda a localização desse draw
		car->clearRect.i16XMax = car->hitbox.i16XMax;
		car->clearRect.i16XMin = car->hitbox.i16XMin;
		car->clearRect.i16YMax = car->hitbox.i16YMax;
		car->clearRect.i16YMin = car->hitbox.i16YMin;
	}
	else {
		if (car->firstDraw || forceRedraw ||
			car->hitbox.i16XMax != car->clearRect.i16XMax ||
			car->hitbox.i16XMin != car->clearRect.i16XMin ||
			car->hitbox.i16YMax != car->clearRect.i16YMax ||
			car->hitbox.i16YMin != car->clearRect.i16YMin) {
			
			// Troca a região de clipping temporariamente para não desenhar fora da estrada
			context->sClipRegion.i16YMin = 36;
			context->sClipRegion.i16YMax = 95;
			
			if (car->firstDraw) {
				car->firstDraw = false;
			}
			else {
				GrContextForegroundSet(context, terrainColor);
				GrRectFill(context, &(car->clearRect));
			}
				
			GrContextForegroundSet(context, car->color);
			GrContextBackgroundSet(context, terrainColor);
			GrImageDraw(context, car->image, car->hitbox.i16XMin, car->hitbox.i16YMin);
			
			// Restaura a região de clipping anterior
			context->sClipRegion.i16YMin = 0;
			context->sClipRegion.i16YMax = 127;
			
			car->clearRect.i16XMax = car->hitbox.i16XMax;
			car->clearRect.i16XMin = car->hitbox.i16XMin;
			car->clearRect.i16YMax = car->hitbox.i16YMax;
			car->clearRect.i16YMin = car->hitbox.i16YMin;
		}
	}
}

void setCarHitbox(Car* car, int16_t xMin, int16_t yMin, int16_t xMax, int16_t yMax) {
	car->hitbox.i16XMin = xMin;
	car->hitbox.i16YMin = yMin;
	car->hitbox.i16XMax = xMax;
	car->hitbox.i16YMax = yMax;
}

int16_t getCarPosX(Car* car) {
	return (car->hitbox.i16XMin + car->hitbox.i16XMax)/2;
}

int16_t getCarPosY(Car* car) {
	return (car->hitbox.i16YMin + car->hitbox.i16YMax)/2;
}
