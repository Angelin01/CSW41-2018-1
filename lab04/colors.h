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
	{"Slate Blue", "Dark Slate Blue", "Midnight Blue", "Navy", "Dark Blue", "Medium Blue", "Blue", "Cornflower Blue", "Royal Blue", "Dodger Blue"},
	10,
	"Blue 1"
};
const ColorGroup blueColors2 = {
	{ClrDeepSkyBlue, ClrLightSkyBlue, ClrSkyBlue, ClrLightBlue, ClrSteelBlue, ClrLightSteelBlue, ClrSlateGray, ClrLightSlateGray},
	{"Deep Sky Blue", "Light Sky Blue", "Sky Blue", "Light Blue", "Steel Blue", "Light Steel Blue", "Slate Gray", "Light Slate Gray"},
	8,
	"Blue 2"
};

// Green Colors
const ColorGroup greenColors1 = {
	{ClrDarkSlateGray, ClrMediumSpringGreen, ClrSpringGreen, ClrPaleGreen, ClrLightGreen, ClrDarkSeaGreen, ClrMediumSeaGreen, ClrSeaGreen, ClrDarkGreen, ClrGreen},
	{"Dark Slate Gray", "Medium Spring Green", "Spring Green", "Pale Green", "Light Green", "Dark Sea Green", "Medium Sea Green", "Sea Green", "Dark Green", "Green"},
	10,
	"Green 1"
};
const ColorGroup greenColors2 = {
	{ClrForestGreen, ClrLimeGreen, ClrLime, ClrLawnGreen, ClrChartreuse, ClrGreenYellow, ClrYellowGreen, ClrOliveDrab, ClrDarkOliveGreen, ClrOlive},
	{"Forest Green", "Lime Green", "Lime", "Lawn Green", "Chartreuse", "Green Yellow", "Yellow Green", "Olive Drab", "Dark Olive Green", "Olive"},
	10,
	"Green 2"
};

// Brown Colors
const ColorGroup brownColors1 = {
	{ClrDarkKhaki, ClrGoldenrod, ClrDarkGoldenrod, ClrSaddleBrown, ClrSienna, ClrRosyBrown, ClrPeru, ClrChocolate, ClrSandyBrown, ClrNavajoWhite},
	{"Dark Khaki", "Goldenrod", "Dark Goldenrod", "Saddle Brown", "Sienna", "Rosy Brown", "Peru", "Chocolate", "Sandy Brown", "Navajo White"},
	10,
	"Brown 1"
};
const ColorGroup brownColors2 = {
	{ClrWheat, ClrBurlyWood, ClrTan},
	{"Wheat", "Burly Wood", "Tan"},
	3,
	"Brown 2"
};

// Purple Colors
const ColorGroup purpleColors1 = {
	{ClrMediumSlateBlue, ClrMediumPurple, ClrBlueViolet, ClrIndigo, ClrDarkViolet, ClrDarkOrchid, ClrMediumOrchid, ClrPurple, ClrDarkMagenta, ClrMagenta},
	{"Medium Slate Blue", "Medium Purple", "Blue Violet", "Indigo", "Dark Violet", "Dark Orchid", "Medium Orchid", "Purple", "Dark Magenta", "Magenta"},
	10,
	"Purple 1"
};
const ColorGroup purpleColors2 = {
	{ClrViolet, ClrOrchid, ClrPlum},
	{"Violet", "Orchid", "Plum"},
	3,
	"Purple 2"
};

// Cyan Colors
const ColorGroup cyanColors = {
	{ClrCyan, ClrDarkTurquoise, ClrTurquoise, ClrMediumTurquoise, ClrLightSeaGreen, ClrDarkCyan, ClrTeal, ClrAquamarine, ClrMediumAquamarine, ClrCadetBlue},
	{"Cyan", "Dark Turquoise", "Turquoise", "Medium Turquoise", "Light Sea Green", "Dark Cyan", "Teal", "Aquamarine", "Medium Aquamarine", "Cadet Blue"},
	10,
	"Cyan Colors"
};

// Orange Colors
const ColorGroup orangeColors = {
	{ClrOrangeRed, ClrDarkOrange, ClrOrange},
	{"Orange Red", "Dark Orange", "Orange"},
	3,
	"Orange Colors"
};

// Yellow Colors
const ColorGroup yellowColors = {
	{ClrGold, ClrYellow, ClrKhaki},
	{"Gold", "Yellow", "Khaki"},
	3,
	"Yellow Colors"
};

// Pink Colors
const ColorGroup pinkColors = {
	{ClrMediumVioletRed, ClrDeepPink, ClrHotPink, ClrPaleVioletRed, ClrLightPink, ClrPink, ClrLightCoral, ClrIndianRed, ClrCrimson},
	{"Medium Violet Red", "Deep Pink", "Hot Pink", "Pale Violet Red", "Light Pink", "Pink", "Light Coral", "Indian Red", "Crimson"},
	9,
	"Pink Colors"
};

// Red Colors
const ColorGroup redColors = {
	{ClrMaroon, ClrDarkRed, ClrFireBrick, ClrBrown, ClrSalmon, ClrDarkSalmon, ClrLightSalmon, ClrCoral, ClrTomato, ClrRed},
	{"Maroon", "Dark Red", "Fire Brick", "Brown", "Salmon", "Dark Salmon", "Light Salmon", "Coral", "Tomato", "Red"},
	10,
	"Red Colors"
};

// Grupo geral de cores
const ColorGroup* colorGroups[] = {&blueColors1, &blueColors2, &greenColors1, &greenColors2, &brownColors1, &brownColors2, &purpleColors1, &purpleColors2, &cyanColors, &orangeColors, &yellowColors, &pinkColors, &redColors};

#endif
