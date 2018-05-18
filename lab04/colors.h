#ifndef __COLORS_LAB4_H__
#define __COLORS_LAB4_H__

#include <stdint.h>
#include "grlib/grlib.h"

#define MAX_COLORS_PER_GROUP 10
#define GROUP_COUNT 13
#define MAX_NAME_CHARS 23

typedef struct ColorGroup {
	uint32_t colorValues[MAX_COLORS_PER_GROUP];
	char colorNames[MAX_COLORS_PER_GROUP][MAX_NAME_CHARS];
	uint8_t count;
	char* groupName;
} ColorGroup;

// Blue Colors
const ColorGroup blueColors1 = {
	{ClrSlateBlue, ClrDarkSlateBlue, ClrMidnightBlue, ClrNavy, ClrDarkBlue, ClrMediumBlue, ClrBlue, ClrCornflowerBlue, ClrRoyalBlue, ClrDodgerBlue},
	{"Slate Blue\0", "Dark Slate Blue\0", "Midnight Blue\0", "Navy\0", "Dark Blue\0", "Medium Blue\0", "Blue\0", "Cornflower Blue\0", "Royal Blue\0", "Dodger Blue\0"},
	10,
	"Blue 1"
};
const ColorGroup blueColors2 = {
	{ClrDeepSkyBlue, ClrLightSkyBlue, ClrSkyBlue, ClrLightBlue, ClrSteelBlue, ClrLightSteelBlue, ClrSlateGray, ClrLightSlateGray},
	{"Deep Sky Blue\0", "Light Sky Blue\0", "Sky Blue\0", "Light Blue\0", "Steel Blue\0", "Light Steel Blue\0", "Slate Gray\0", "Light Slate Gray\0"},
	8,
	"Blue 2"
};

// Green Colors
const ColorGroup greenColors1 = {
	{ClrDarkSlateGray, ClrMediumSpringGreen, ClrSpringGreen, ClrPaleGreen, ClrLightGreen, ClrDarkSeaGreen, ClrMediumSeaGreen, ClrSeaGreen, ClrDarkGreen, ClrGreen},
	{"Dark Slate Gray\0", "Medium Spring Green\0", "Spring Green\0", "Pale Green\0", "Light Green\0", "Dark Sea Green\0", "Medium Sea Green\0", "Sea Green\0", "Dark Green\0", "Green\0"},
	10,
	"Green 1"
};
const ColorGroup greenColors2 = {
	{ClrForestGreen, ClrLimeGreen, ClrLime, ClrLawnGreen, ClrChartreuse, ClrGreenYellow, ClrYellowGreen, ClrOliveDrab, ClrDarkOliveGreen, ClrOlive},
	{"Forest Green\0", "Lime Green\0", "Lime\0", "Lawn Green\0", "Chartreuse\0", "Green Yellow\0", "Yellow Green\0", "Olive Drab\0", "Dark Olive Green\0", "Olive\0"},
	10,
	"Green 2"
};

// Brown Colors
const ColorGroup brownColors1 = {
	{ClrDarkKhaki, ClrGoldenrod, ClrDarkGoldenrod, ClrSaddleBrown, ClrSienna, ClrRosyBrown, ClrPeru, ClrChocolate, ClrSandyBrown, ClrNavajoWhite},
	{"Dark Khaki\0", "Goldenrod\0", "Dark Goldenrod\0", "Saddle Brown\0", "Sienna\0", "Rosy Brown\0", "Peru\0", "Chocolate\0", "Sandy Brown\0", "Navajo White\0"},
	10,
	"Brown 1"
};
const ColorGroup brownColors2 = {
	{ClrWheat, ClrBurlyWood, ClrTan},
	{"Wheat\0", "Burly Wood\0", "Tan\0"},
	3,
	"Brown 2"
};

// Purple Colors
const ColorGroup purpleColors1 = {
	{ClrMediumSlateBlue, ClrMediumPurple, ClrBlueViolet, ClrIndigo, ClrDarkViolet, ClrDarkOrchid, ClrMediumOrchid, ClrPurple, ClrDarkMagenta, ClrMagenta},
	{"Medium Slate Blue\0", "Medium Purple\0", "Blue Violet\0", "Indigo\0", "Dark Violet\0", "Dark Orchid\0", "Medium Orchid\0", "Purple\0", "Dark Magenta\0", "Magenta\0"},
	10,
	"Purple 1"
};
const ColorGroup purpleColors2 = {
	{ClrViolet, ClrOrchid, ClrPlum},
	{"Violet\0", "Orchid\0", "Plum\0"},
	3,
	"Purple 2"
};

// Cyan Colors
const ColorGroup cyanColors = {
	{ClrCyan, ClrDarkTurquoise, ClrTurquoise, ClrMediumTurquoise, ClrLightSeaGreen, ClrDarkCyan, ClrTeal, ClrAquamarine, ClrMediumAquamarine, ClrCadetBlue},
	{"Cyan\0", "Dark Turquoise\0", "Turquoise\0", "Medium Turquoise\0", "Light Sea Green\0", "Dark Cyan\0", "Teal\0", "Aquamarine\0", "Medium Aquamarine\0", "Cadet Blue\0"},
	10,
	"Cyan Colors"
};

// Orange Colors
const ColorGroup orangeColors = {
	{ClrOrangeRed, ClrDarkOrange, ClrOrange},
	{"Orange Red\0", "Dark Orange\0", "Orange\0"},
	3,
	"Orange Colors"
};

// Yellow Colors
const ColorGroup yellowColors = {
	{ClrGold, ClrYellow, ClrKhaki},
	{"Gold\0", "Yellow\0", "Khaki\0"},
	3,
	"Yellow Colors"
};

// Pink Colors
const ColorGroup pinkColors = {
	{ClrMediumVioletRed, ClrDeepPink, ClrHotPink, ClrPaleVioletRed, ClrLightPink, ClrPink, ClrLightCoral, ClrIndianRed, ClrCrimson},
	{"Medium Violet Red\0", "Deep Pink\0", "Hot Pink\0", "Pale Violet Red\0", "Light Pink\0", "Pink\0", "Light Coral\0", "Indian Red\0", "Crimson\0"},
	9,
	"Pink Colors"
};

// Red Colors
const ColorGroup redColors = {
	{ClrMaroon, ClrDarkRed, ClrFireBrick, ClrBrown, ClrSalmon, ClrDarkSalmon, ClrLightSalmon, ClrCoral, ClrTomato, ClrRed},
	{"Maroon\0", "Dark Red\0", "Fire Brick\0", "Brown\0", "Salmon\0", "Dark Salmon\0", "Light Salmon\0", "Coral\0", "Tomato\0", "Red\0"},
	10,
	"Red Colors"
};

// Grupo geral de cores
const ColorGroup* colorGroups[] = {&blueColors1, &blueColors2, &greenColors1, &greenColors2, &brownColors1, &brownColors2, &purpleColors1, &purpleColors2, &cyanColors, &orangeColors, &yellowColors, &pinkColors, &redColors};

#endif
