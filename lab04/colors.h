#include "grlib.h"

typedef struct Color {
	uint32_t colorValue;
	char* colorName;
} Color;

typedef struct ColorGroup {
	Color* color;
	uint8_t count;
	char* groupName;
} ColorGroup;

// Blue Colors
static const Color slateBlue = {ClrSlateBlue, "Slate Blue"};
static const Color darkSlateBlue = {ClrDarkSlateBlue, "Dark Slate Blue"};
static const Color midnightBlue = {ClrMidnightBlue, "Midnight Blue"};
static const Color navy = {ClrNavy, "Navy"};
static const Color darkBlue = {ClrDarkBlue, "Dark Blue"};
static const Color mediumBlue = {ClrMediumBlue, "Medium Blue"};
static const Color blue = {ClrBlue, "Blue"};
static const Color cornflowerBlue = {ClrCornflowerBlue, "Cornflower Blue"};
static const Color royalBlue = {ClrRoyalBlue, "Royal Blue"};
static const Color dodgerBlue = {ClrDodgerBlue, "Dodger Blue"};
static const Color deepSkyBlue = {ClrDeepSkyBlue, "Deep Sky Blue"};
static const Color lightSkyBlue = {ClrLightSkyBlue, "Light Sky Blue"};
static const Color skyBlue = {ClrSkyBlue, "Sky Blue"};
static const Color lightBlue = {ClrLightBlue, "Light Blue"};
static const Color steelBlue = {ClrSteelBlue, "Steel Blue"};
static const Color lightSteelBlue = {ClrLightSteelBlue, "Light Steel Blue"};
static const Color slateGray = {ClrSlateGray, "Slate Gray"};
static const Color lightSlateGray = {ClrLightSlateGray, "Light Slate Gray"};

static const ColorGroup blueColors1 = {
	{slateBlue, darkSlateBlue, midnightBlue, navy, darkBlue, mediumBlue, blue, cornflowerBlue, royalBlue, dodgerBlue},
	10,
	"Blue 1"
};
static const ColorGroup blueColors2 = {
	{deepSkyBlue, lightSkyBlue, skyBlue, lightBlue, steelBlue, lightSteelBlue, slateGray, lightSlateGray},
	8,
	"Blue 2"
};

// Green Colors
static const Color darkSlateGray = {ClrDarkSlateGray, "Dark Slate Gray"};
static const Color mediumSpringGreen = {ClrMediumSpringGreen, "Medium Spring Green"};
static const Color springGreen = {ClrSpringGreen, "Spring Green"};
static const Color paleGreen = {ClrPaleGreen, "Pale Green"};
static const Color lightGreen = {ClrLightGreen, "Light Green"};
static const Color darkSeaGreen = {ClrDarkSeaGreen, "Dark Sea Green"};
static const Color mediumSeaGreen = {ClrMediumSeaGreen, "Medium Sea Green"};
static const Color seaGreen = {ClrSeaGreen, "Sea Green"};
static const Color darkGreen = {ClrDarkGreen, "Dark Green"};
static const Color green = {ClrGreen, "Green"};
static const Color forestGreen = {ClrForestGreen, "Forest Green"};
static const Color limeGreen = {ClrLimeGreen, "Lime Green"};
static const Color lime = {ClrLime, "Lime"};
static const Color lawnGreen = {ClrLawnGreen, "Lawn Green"};
static const Color chartreuse = {ClrChartreuse, "Chartreuse"};
static const Color greenYellow = {ClrGreenYellow, "Green Yellow"};
static const Color yellowGreen = {ClrYellowGreen, "Yellow Green"};
static const Color oliveDrab = {ClrOliveDrab, "Olive Drab"};
static const Color darkOliveGreen = {ClrDarkOliveGreen, "Dark Olive Green"};
static const Color olive = {ClrOlive, "Olive"};

static const ColorGroup greenColors1 = {
	{darkSlateGray, mediumSpringGreen, springGreen, paleGreen, lightGreen, darkSeaGreen, mediumSeaGreen, seaGreen, darkGreen, green},
	10,
	"Green 1"
};
static const ColorGroup greenColors2 = {
	{forestGreen, limeGreen, lime, lawnGreen, chartreuse, greenYellow, yellowGreen, oliveDrab, darkOliveGreen, olive},
	10,
	"Green 2"
};

// Brown Colors
static const Color darkKhaki = {ClrDarkKhaki, "Dark Khaki"};
static const Color goldenrod = {ClrGoldenrod, "Goldenrod"};
static const Color darkGoldenrod = {ClrDarkGoldenrod, "Dark Goldenrod"};
static const Color saddleBrown = {ClrSaddleBrown, "Saddle Brown"};
static const Color sienna = {ClrSienna, "Sienna"};
static const Color rosyBrown = {ClrRosyBrown, "Rosy Brown"};
static const Color peru = {ClrPeru, "Peru"};
static const Color chocolate = {ClrChocolate, "Chocolate"};
static const Color sandyBrown = {ClrSandyBrown, "Sandy Brown"};
static const Color navajoWhite = {ClrNavajoWhite, "Navajo White"};
static const Color wheat = {ClrWheat, "Wheat"};
static const Color burlyWood = {ClrBurlyWood, "Burly Wood"};
static const Color tan = {ClrTan, "Tan"};

static const ColorGroup brownColors1 = {
	{darkKhaki, goldenrod, darkGoldenrod, saddleBrown, sienna, rosyBrown, peru, chocolate, sandyBrown, navajoWhite},
	10,
	"Brown 1"
};
static const ColorGroup brownColors2 = {
	{wheat, burlyWood, tan},
	3,
	"Brown 2"
};

// Purple Colors
static const Color mediumSlateBlue = {ClrMediumSlateBlue, "Medium Slate Blue"};
static const Color mediumPurple = {ClrMediumPurple, "Medium Purple"};
static const Color blueViolet = {ClrBlueViolet, "Blue Violet"};
static const Color indigo = {ClrIndigo, "Indigo"};
static const Color darkViolet = {ClrDarkViolet, "Dark Violet"};
static const Color darkOrchid = {ClrDarkOrchid, "Dark Orchid"};
static const Color mediumOrchid = {ClrMediumOrchid, "Medium Orchid"};
static const Color purple = {ClrPurple, "Purple"};
static const Color darkMagenta = {ClrDarkMagenta, "Dark Magenta"};
static const Color magenta = {ClrMagenta, "Magenta"};
static const Color violet = {ClrViolet, "Violet"};
static const Color orchid = {ClrOrchid, "Orchid"};
static const Color plum = {ClrPlum, "Plum"};

static const ColorGroup purpleColors1 = {
	{mediumSlateBlue, mediumPurple, blueViolet, indigo, darkViolet, darkOrchid, mediumOrchid, purple, darkMagenta, magenta},
	10,
	"Purple 1"
};
static const ColorGroup purpleColors2 = {
	{violet, orchid, plum},
	3,
	"Purple 2"
};

// Cyan Colors
static const Color cyan = {ClrCyan, "Cyan"};
static const Color darkTurquoise = {ClrDarkTurquoise, "Dark Turquoise"};
static const Color turquoise = {ClrTurquoise, "Turquoise"};
static const Color mediumTurquoise = {ClrMediumTurquoise, "Medium Turquoise"};
static const Color lightSeaGreen = {ClrLightSeaGreen, "Light Sea Green"};
static const Color darkCyan = {ClrDarkCyan, "Dark Cyan"};
static const Color teal = {ClrTeal, "Teal"};
static const Color aquamarine = {ClrAquamarine, "Aquamarine"};
static const Color mediumAquamarine = {ClrMediumAquamarine, "Medium Aquamarine"};
static const Color cadetBlue = {ClrCadetBlue, "Cadet Blue"};

static const ColorGroup cyanColors = {
	{cyan, darkTurquoise, turquoise, mediumTurquoise, lightSeaGreen, darkCyan, teal, aquamarine, mediumAquamarine, cadetBlue},
	10,
	"Cyan Colors"
};

// Orange Colors
static const Color orangeRed = {ClrOrangeRed, "Orange Red"};
static const Color darkOrange = {ClrDarkOrange, "Dark Orange"};
static const Color orange = {ClrOrange, "Orange"};

static const ColorGroup orangeColors = {
	{orangeRed, darkOrange, orange},
	3,
	"Orange Colors"
};

// Yellow Colors
static const Color gold = {ClrGold, "Gold"};
static const Color yellow = {ClrYellow, "Yellow"};
static const Color khaki = {ClrKhaki, "Khaki"};

static const ColorGroup yellowColors = {
	{gold, yellow, khaki},
	3,
	"Yellow Colors"
};

// Pink Colors
static const Color mediumVioletRed = {ClrMediumVioletRed, "Medium Violet Red"};
static const Color deepPink = {ClrDeepPink, "Deep Pink"};
static const Color hotPink = {ClrHotPink, "Hot Pink"};
static const Color paleVioletRed = {ClrPaleVioletRed, "Pale Violet Red"};
static const Color lightPink = {ClrLightPink, "Light Pink"};
static const Color pink = {ClrPink, "Pink"};
static const Color lightCoral = {ClrLightCoral, "Light Coral"};
static const Color indianRed = {ClrIndianRed, "Indian Red"};
static const Color crimson = {ClrCrimson, "Crimson"};

static const ColorGroup pinkColors = {
	{mediumVioletRed, deepPink, hotPink, paleVioletRed, lightPink, pink, lightCoral, indianRed, crimson},
	9,
	"Pink Colors"
};

// Red Colors
static const Color maroon = {ClrMaroon, "Maroon"};
static const Color darkRed = {ClrDarkRed, "Dark Red"};
static const Color fireBrick = {ClrFireBrick, "Fire Brick"};
static const Color brown = {ClrBrown, "Brown"};
static const Color salmon = {ClrSalmon, "Salmon"};
static const Color darkSalmon = {ClrDarkSalmon, "Dark Salmon"};
static const Color lightSalmon = {ClrLightSalmon, "Light Salmon"};
static const Color coral = {ClrCoral, "Coral"};
static const Color tomato = {ClrTomato, "Tomato"};
static const Color red = {ClrRed, "Red"};

static const ColorGroup redColors = {
	{maroon, darkRed, fireBrick, brownm, salmon, darkSalmon, lightSalmon, coral, tomato, red},
	10,
	"Red Colors"
};

// Grupo geral de cores
static const ColorGroup colorGroups[] = {blueColors1, blueColors2, greenColors1, greenColors2, brownColors1, brownColors2, purpleColors1, purpleColors2, cyanColors, orangeColors, yellowColors, pinkColors, redColors};
