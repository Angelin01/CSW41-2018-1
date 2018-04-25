#ifndef __CAR_LAB_3_H__
#define __CAR_LAB_3_H__

#include <stdbool.h>
#include "TM4C129.h"
#include "grlib/grlib.h"

typedef struct Car {
	bool firstDraw;
	bool player;
	
	uint8_t lane;
	
	uint32_t color;
	
	const uint8_t* image;
	
	tRectangle hitbox;
	tRectangle clearRect;
} Car;

extern const uint8_t carBigImage[];
extern const uint8_t carBigWidth;
extern const uint8_t carBigHeight;

extern const uint8_t carNormalImage[];
extern const uint8_t carNormalWidth;
extern const uint8_t carNormalHeight;

extern const uint8_t carAvgImage[];
extern const uint8_t carAvgWidth;
extern const uint8_t carAvgHeight;

extern const uint8_t carSmallImage[];
extern const uint8_t carSmallWidth;
extern const uint8_t carSmallHeight;

extern const uint8_t carTinyImage[];
extern const uint8_t carTinyWidth;
extern const uint8_t carTinyHeight;

extern void draw_car(tContext* context, Car* car, uint32_t terrainColor, bool forceRedraw);

extern void setCarHitbox(Car* car, int16_t xMin, int16_t yMin, int16_t xMax, int16_t yMax);

extern int16_t getCarPosX(Car* car);

extern int16_t getCarPosY(Car* car);

#endif // __CAR_LAB_3_H__
