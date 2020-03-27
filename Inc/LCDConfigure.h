/*
 * LCDConfigure.h
 *
 *  Created on: 27.03.2020
 *      Author: hungv
 */

#ifndef INC_LCDCONFIGURE_H_
#define INC_LCDCONFIGURE_H_

#include <main.h>

#define FONT_WIDTH_BIG 12
#define FONT_HIGHT_BIG 16

// defines for LCD init
#define RST 0x10
#define INITIAL_STATE (0x1F)
#define SOFTWARE_RESET (0x01)
#define SET_PLL_MN (0xE2)
#define START_PLL (0xE0)
#define SET_LSHIFT (0xE6)
#define SET_LCD_MODE (0xB0)
#define SET_HORI_PERIOD (0xB4)
#define SET_VERT_PERIOD (0xB6)
#define SET_ADRESS_MODE (0x36)
#define SET_PIXEL_DATA_FORMAT (0xF0)
#define SET_DISPLAY_ON (0x29)

typedef union color {
	int all;
	struct {
		char red;
		char green;
		char blue;
	};
} COLOR;

// enum colors
enum {
	BLACK = 0x000000,
	RED = 0x0000FF,
	GREEN = 0x00FF00,
	YELLOW = 0x00FFFF,
	BLUE = 0xFF0000,
	WHITE = 0xFFFFFF,
	VIOLET = 0xEE82EE,
	MARINE = 0x7FFFD4
};

// Structs:
struct TABLE {
	char red;
	char green;
	char blue;
};

#endif /* INC_LCDCONFIGURE_H_ */
