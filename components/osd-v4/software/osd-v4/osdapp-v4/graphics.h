#ifndef GRAPHICS_H
	#define GRAPHICS_H

	#include "config.h"
	#include <stdint.h>
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include <avr/interrupt.h>
	#include <stdlib.h>
	#include "display.h"

	// macros for readability when selecting mode.
	#define PAL						1
	#define	NTSC					0
	#define _PAL					1
	#define _NTSC					0

	#define WHITE					1
	#define BLACK					0
	#define INVERT					2

	#define UP						0
	#define DOWN					1
	#define LEFT					2
	#define RIGHT					3

	#define DEC						10
	#define HEX						16
	#define OCT						8
	#define BIN						2
	#define BYTE					0

	class Graphics
	{
	private:
		uint8_t cursor_x,cursor_y;
		const unsigned char * font;
		void inc_txtline(uint8_t* screen);
		void printNumber(unsigned long, uint8_t);
		void printFloat(double, uint8_t);

	public:
		uint8_t fw() const;
		uint8_t fh() const;
		uint8_t begin(uint8_t ntscOrPal);
		void end();
	
		//accessor functions
		//uint8_t hres();
		//uint8_t vres();
		uint8_t char_line();
	
		//flow control functions
		void vsync();
		void delay_frame(unsigned int x);
	
		//basic rendering functions
		void setPixel(uint8_t* screen, uint8_t x, uint8_t y, char c);
		void fill(uint8_t* screen);
		void shift(uint8_t* screen, uint8_t distance, uint8_t direction);
		void line(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c);
		void hline(uint8_t* screen, uint8_t line, uint16_t x0, uint16_t x1, uint8_t c);
		void vline(uint8_t* screen, uint8_t row, uint16_t y0, uint16_t y1, uint8_t c);
		void rectangle(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc = -1); 
		void circle(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc = -1);

		void rline(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t originX, uint8_t originY, float angle, char c);

		void blitBitmap(uint8_t* screen, uint8_t x, uint8_t y, const unsigned char * bmp, uint16_t i = 0, uint8_t width = 0, uint8_t lines = 0);
		void blitCharacter(uint8_t* screen, uint8_t x, uint8_t y, unsigned char c);
		void setCursor(uint8_t x, uint8_t y);
		void setFont(const unsigned char * f);


		void writeCharacter(uint8_t* screen, uint8_t c);
		void writeString(uint8_t* screen, const char *str);
		void writeStringWithLength(uint8_t* screen, const uint8_t *buffer, uint8_t size);
		
		void print(uint8_t* screen, uint8_t x, uint8_t y, const char[]);
	
		void printPGM(uint8_t* screen, const char[]);
		void printPGM(uint8_t* screen, uint8_t x, uint8_t y, const char[]);
	};

#endif // GRAPHICS_H