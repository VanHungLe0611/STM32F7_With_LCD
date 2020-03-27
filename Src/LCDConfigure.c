/*
 * LCDConfigure.c
 *
 *  Created on: 27.03.2020
 *      Author: hungv
 */

#include <LCDConfigure.h>
#include <fonts.h>

#define LCD_PIN LCD_D0_Pin | LCD_D1_Pin | LCD_D2_Pin | LCD_D3_Pin | LCD_D4_Pin | LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin
#define CONTROL_PIN RD_Pin | WR_Pin | RS_Pin | CS_Pin | RST_Pin

uint16_t offset = 0;
extern struct TABLE *ttable;
extern COLOR dotColor;

void wait(int time) {
	volatile int tmp;

	for (tmp = 0; tmp < 10800 * time; tmp++)
		; // ~ 1ms
}

void write_cmd_data(unsigned char data) {
	HAL_GPIO_WritePin(GPIOF, LCD_PIN, data);
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15);
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);

}

void write_command(unsigned char command) {
	HAL_GPIO_WritePin(GPIOF, LCD_PIN, command);
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x11);
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);

}

void write_data(COLOR color) {
	HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.red); // write data byte
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

	HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.green); // write data byte
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

	HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.blue); // write data byte
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

}

/******************************************************************************************************/
void write_position(short point1_x, short point1_y, short point2_x,
		short point2_y) {

	// offset (+ 1x272, + 2x272, + 3x272) points to screen 1, 2 or 3
	point1_y += offset;
	point2_y += offset;

	write_command(0x2A);                    // Set page address (x-axis)
	write_cmd_data(point1_x >> 8);   // Set start page address                HB
	write_cmd_data(point1_x);        //                                       LB
	write_cmd_data(point2_x >> 8); 	// Set display_stop page address         HB
	write_cmd_data(point2_x);        //                                       LB

	write_command(0x2B);                    // Set column address (y-axis)
	write_cmd_data(point1_y >> 8);   // Set start column address              HB
	write_cmd_data(point1_y);       //                                       LB
	write_cmd_data(point2_y >> 8); 	// Set display_stop column address       HB
	write_cmd_data(point2_y);        //                                       LB
}

/**************************** write_char()
 *******************************/
// writes a single letter with heigth 12 pixel                              //
/****************************************************************************/
void write_char(int w, COLOR color, COLOR backcolor) {
	int lv;
	for (lv = 0; lv < 12; lv++) {
		if (w & 1) {             // Write data byte
			HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.red); // write data byte
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

			HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.green); // write data byte
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

			HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.blue); // write data byte
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state
		} else {
			HAL_GPIO_WritePin(GPIOF, LCD_PIN, backcolor.red); // write data byte
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

			HAL_GPIO_WritePin(GPIOF, LCD_PIN, backcolor.green); // write data byte
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

			HAL_GPIO_WritePin(GPIOF, LCD_PIN, backcolor.blue); // write data byte
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
			HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state
		}
		w = w >> 1;
	}
}

/******************************************************************************************************/
void print_string(char *text, int row, int column, COLOR color, COLOR backcolor) {
	int w;
	int lv1, numChar;
	int length = strlen(text);
	int font_width = FONT_WIDTH_BIG;
	int font_hight = FONT_HIGHT_BIG;
	int columnStart = column;
	int columnStop = columnStart + font_width - 1;
	int rowStart = row;
	int rowStop = rowStart + font_hight - 1;

	for (numChar = 0; numChar < length; numChar++) {
		write_position(columnStart, rowStart, columnStop, rowStop);
		write_command(0x2C);
		for (lv1 = 0; lv1 < 32; lv1 = lv1 + 2) {
			w = (font_12_16[text[numChar]][lv1 + 1] << 4)
					| (font_12_16[text[numChar]][lv1] >> 4);
			write_char(w, color, backcolor);
		}
		columnStart += font_width;
		columnStop += font_width;
	}
}

/******************************************************************************************************/
//Writes the hole screen in one color
void write_screen_color(COLOR color) {
	int count = 0;
	write_position(0, 0, 799, 479);
	write_command(0x2C);

	while (count++ < 384000) {
		HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.red); // write data byte
		HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
		HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

		HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.green); // write data byte
		HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
		HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state

		HAL_GPIO_WritePin(GPIOF, LCD_PIN, color.blue); // write data byte
		HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x15); // chip select = 0 , write state = 0
		HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x1F);  // initial state
	}
}

//draws a line from startpoint x to stoppoint y directly to the display
void write_line_normal(short startx, short starty, short stopx, short stopy,
		COLOR color) {
	short old_x, old_y, x, y, i;
	int start_x, stop_x, start_y, stop_y;
	double gain;

	// 90° line:
	if (startx == stopx) {
		if (starty > stopy) { // 90° from DOWN to UP   else: 270° from UP to DOWN
			start_y = stopy;
			stop_y = starty;
		} else {
			start_y = starty;
			stop_y = stopy;
		}
		write_position(startx, start_y, stopx, stop_y);
		write_command(0x2C);
		for (x = start_y; x <= stop_y; x++) {
			write_data(color);
		}
	}
	// 0° line:
	else if (starty == stopy) {
		if (startx > stopx) {
			start_x = stopx;
			stop_x = startx;
		} else {
			start_x = startx;
			stop_x = stopx;
		}
		write_position(start_x, starty, stop_x, stopy);
		write_command(0x2C);
		for (x = start_x; x <= stop_x; x++) {
			write_data(color);
		}
	}
/////////////////////////////////////////////////////////////////////////////////////////
	else {
		if (startx > stopx) { // running direction is negative ! => switch start and stop
			start_x = stopx;
			stop_x = startx;
			start_y = stopy;
			stop_y = starty;
		} else {
			start_x = startx;
			stop_x = stopx;
			start_y = starty;
			stop_y = stopy;
		}
		//(stop_y - start_y) can be positive or negative
		gain = (double) (stop_y - start_y) / (stop_x - start_x);

		if (gain >= 1) {
			old_y = start_y;

			for (x = start_x; x <= stop_x; x++) {
				y = gain * (x - start_x) + start_y;
				write_position(x, old_y, x, y);
				write_command(0x2C);
				for (i = old_y; i <= y; i++) {
					write_data(color);
				}
				old_y = y;
			}
		} else if (gain > 0) {
			old_x = start_x;

			for (y = start_y; y <= stop_y; y++) {
				x = (y - start_y) / gain + start_x;
				write_position(old_x, y, x, y);
				write_command(0x2C);
				for (i = old_x; i <= x; i++) {
					write_data(color);
				}
				old_x = x;
			}
		}

		else if (gain <= -1) {
			old_y = start_y;

			for (x = start_x; x <= stop_x; x++) {
				y = gain * (x - start_x) + start_y;
				write_position(x, y, x, old_y);
				write_command(0x2C);
				for (i = y; i <= old_y; i++) {
					write_data(color);
				}
				old_y = y;
			}
		}
		// start_y > stop_y
		else if (gain < 0) {
			old_x = start_x;

			for (y = start_y; y >= stop_y; y--) {
				x = (y - start_y) / gain + start_x;
				write_position(old_x, y, x, y);
				write_command(0x2C);
				for (i = old_x; i <= x; i++) {
					write_data(color);
				}
				old_x = x;
			}
		}
	}
}

void ConfigureLCD7Inch(void) {

	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, 0x00);
	wait(1);
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, INITIAL_STATE); // Initial state
	wait(10);                               //
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, ~RST);           // Hardware reset
	wait(1);                                //
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, RST);           // Hardware reset
	wait(1);                                //
	write_command(SOFTWARE_RESET);          // Software reset
	wait(10);                               //

	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, INITIAL_STATE); // Initial state
	wait(10);                               //

	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, ~RST);           // Hardware reset
	wait(1);                                //
	HAL_GPIO_WritePin(GPIOF, CONTROL_PIN, RST);           // Hardware reset
	wait(1);                                          //

	write_command(SOFTWARE_RESET);          // Software reset
	wait(10);

	write_command(SET_PLL_MN);              // Set PLL Freq to 120 MHz
	write_cmd_data(0x24);                   //
	write_cmd_data(0x02);                   //
	write_cmd_data(0x04);                   //

	write_command(START_PLL);               // Start PLL
	write_cmd_data(0x01);                   //
	wait(1);                                //

	write_command(START_PLL);               // Lock PLL
	write_cmd_data(0x03);                   //
	wait(1);                                //

	write_command(SOFTWARE_RESET);          // Software reset
	wait(10);

	//******************************************************************************

	write_command(0xe6);       // pixel clock frequency
	write_cmd_data(0x04); // LCD_FPR = 290985 = 33.300 Mhz Result for 7" Display
	write_cmd_data(0x70);    //
	write_cmd_data(0xA9);   //

	write_command(0xB0);          //SET LCD MODE   SIZE !!
	write_cmd_data(0x19); //19 TFT panel data width - Enable FRC or dithering for color depth enhancement 8 18  1f- 38
	write_cmd_data(0x20); //SET TFT MODE & hsync+Vsync+DEN MODE   20  or 00
	write_cmd_data(0x03); //SET horizontal size=800+1 HightByte   !!!!!!!!!!!!
	write_cmd_data(0x1F);      //SET horizontal size=800+1 LowByte
	write_cmd_data(0x01);      //SET vertical size=480+1 HightByte
	write_cmd_data(0xDF);      //SET vertical size=480+1 LowByte
	write_cmd_data(0x00); //Even line RGB sequence / Odd line RGB sequence RGB

	write_command(0xB4);            // Set Horizontal Period
	write_cmd_data(0x03); //03 High byte of horizontal total period (display + non-display)
	write_cmd_data(0x5E); //51 Low byte of the horizontal total period (display + non-display)
	write_cmd_data(0x00); //High byte of the non-display period between the start of the horizontal sync (LLINE) signal and the first display data.
	write_cmd_data(0x46); //**   // 46 Low byte of the non-display period between the start of the horizontal sync (LLINE) signal and the first display data
	write_cmd_data(0x09);       // Set the vertical sync
//    write_cmd_data(0x00);       //SET Hsync pulse start
//    write_cmd_data(0x00);                                       //00
//    write_cmd_data(0x00); //SET Hsync pulse subpixel start position  //00
	write_cmd_data(0x00);             // Set horiz.Sync pulse start pos= 8    HB
	write_cmd_data(0x08);             // Set horiz.Sync pulse start pos= 8    LB
	write_cmd_data(0x00);                   //
	//   ** too small will give you half a PICTURE !!

	write_command(0xB6);          //Set Vertical Period
	write_cmd_data(0x01); //01 High byte of the vertical total (display + non-display)
	write_cmd_data(0xFE); //F4 Low byte F5 INCREASES SYNC TIME AND BACK PORCH
	write_cmd_data(0x00);      // 00
	write_cmd_data(0x0C); //0C =12 The non-display period in lines between the start of the frame and the first display data in line.
//    write_cmd_data(0x00); //Set the vertical sync pulse width (LFRAME) in lines.
//    write_cmd_data(0x00);      //SET Vsync pulse start position
//    write_cmd_data(0x00);
	write_cmd_data(0x00);              // Set vert.Sync pulse start pos= 8    HB
	write_cmd_data(0x00);              // Set vert.Sync pulse start pos= 8    LB
	write_cmd_data(0x04);                   //

	write_command(0x36);
	write_cmd_data(0x01);

	// PWM signal frequency = PLL clock
	write_command(0xBE);
	write_cmd_data(0x08);
	write_cmd_data(0x80);
	write_cmd_data(0x01);

//    write_command(0xBC);   //Set Post Proc
//    write_cmd_data(0x40); //40 Set the contrast value
//    write_cmd_data(0x80); //80 Set the brightness value
//    write_cmd_data(0x40); //40 Set the saturation value
//    write_cmd_data(0x01);  //1 Enable the postprocessor
//
//    write_command(0xE2);
//    write_cmd_data(60);    //35 PLLclk = REFclk * 36 (360MHz)
//    write_cmd_data(5);     // SYSclk = PLLclk / 3  (120MHz)
//    write_cmd_data(0x54);  // validate M and N      dec 84
//
//    write_command(0x26);  //    Set Gamma Curve
//    //  write_cmd_data(0x01); // Gamma curve selection =0
//    write_cmd_data(0x08);  // Gamma curve selection =3
//
//    write_command(0x0B);          //SET SCAN MODE
//    write_cmd_data(0x00); //SET TFT MODE   top to bottom, left to right normal etc

	write_command(0x0A);
	write_cmd_data(0x1C);         //Power Mode

//    write_command(0x3A);          //SET Pixel Format
//    write_cmd_data(0x50);       //16 bit pixel
//    write_cmd_data(0x60);       //18 bit pixel
//    write_cmd_data(0x70);       //24 bit pixel
	write_command(0xF0); //set pixel data format 8bit
	write_cmd_data(0x00);

//    write_command(0xF0);      //  Set Pixel Data Interface
//    write_cmd_data(0x03); // 16-bit (565 format)   011 16-bit (565 format)
	write_command(0x29);                    // Set display on

}

