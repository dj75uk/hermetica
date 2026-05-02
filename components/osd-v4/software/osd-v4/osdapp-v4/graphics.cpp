
/* A note about how Color is defined for this version of TVout
 *
 * Where ever choosing a color is mentioned the following are true:
 * 	BLACK	=0
 *	WHITE	=1
 *	INVERT	=2
 *	All others will be ignored.
*/
#include "config.h"
#include "graphics.h"
#include "display.h"
#include <math.h>
#include <avr/pgmspace.h>
#include <string.h>

uint8_t Graphics::fw() const { return font == NULL ? 0 : font[0]; }
uint8_t Graphics::fh() const { return font == NULL ? 0 : font[1]; }

/* call this to start video output with a specified resolution.
 *
 * Arguments:
 *	mode:
 *		The video standard to follow:
 *		PAL		=1	=_PAL
 *		NTSC	=0	=_NTSC
 *	x:
 *		Horizontal resolution must be divisible by 8.
 *	y:
 *		Vertical resolution.
 *
 *	Returns:
 *		0 if no error.
 *		1 if x is not divisible by 8.
 *		2 if y is to large (NTSC only cannot fill PAL vertical resolution by 8bit limit)
 *		4 if there is not enough memory for the frame buffer.
 */
uint8_t Graphics::begin(uint8_t ntscOrPal)
{
	//USART_EXT_writeS("Graphics::begin(): ENTRY\n");
	// check if x is divisible by 8
	if (!(W & 0xF8))
		return 1;
		
	cursor_x = 0;
	cursor_y = 0;
	
	displaySetup(ntscOrPal);
	
	//USART_EXT_writeS("Graphics::begin(): EXIT\n");
	return 0;
	
}

void Graphics::end()
{
	TIMSK1 = 0;
}

/* Fill the screen with some color.
 *
 * Arguments:
 *	color:
 *		The color to fill the screen with.
 *		(see color note at the top of this file)
*/
void Graphics::fill(uint8_t* screen)
{
	cursor_x = 0;
	cursor_y = 0;
	memset(screen, 0x00, SCREEN_BYTE_SIZE);
}

/* Return the number of characters that will fit on a line
 *
 * Returns:
 *	The number of characters that will fit on a text line starting from x=0.
 *	Will return -1 for dynamic width fonts as this cannot be determined.
*/
uint8_t Graphics::char_line()
{
	return (W / pgm_read_byte(font));
}

/* Set the color of a pixel
 * 
 * Arguments:
 *	x:
 *		The x coordinate of the pixel.
 *	y:
 *		The y coordinate of the pixel.
 *	c:
 *		The color of the pixel
 *		(see color note at the top of this file)
 */
void Graphics::setPixel(uint8_t* screen, uint8_t x, uint8_t y, char c)
{
	if (x >= W || y >= H)
		return;
	sp(screen, x, y, c);
}


void Graphics::rline(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t originX, uint8_t originY, float angle, char c)
{
	if (angle == 0)
	{
		line(screen, x0, y0, x1, y1, c);
	}
	else
	{
		XYi16 p0 = rotate(x0, y0, originX, originY, angle);
		XYi16 p1 = rotate(x1, y1, originX, originY, angle);
		line(screen, p0.x, p0.y, p1.x, p1.y, c);
	}
}



/* Draw a line from one point to another
 *
 * Arguments:
 *	x0:
 *		The x coordinate of point 0.
 *	y0:
 *		The y coordinate of point 0.
 *	x1:
 *		The x coordinate of point 1.
 *	y1:
 *		The y coordinate of point 1.
 *	c:
 *		The color of the line.
 *		(see color note at the top of this file)
 */
void Graphics::line(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c) {
	if (x0 > W || y0 > H || x1 > W || y1 > H)
		return;
	if (x0 == x1)
		vline(screen, x0, y0, y1, c);
	else if (y0 == y1)
		hline(screen, y0, x0, x1, c);
	else {
		int e;
		signed int dx,dy,j, temp;
		signed char s1,s2, xchange;
		signed int x,y;

		x = x0;
		y = y0;
	
		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}

		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}

		xchange = 0;   

		if (dy>dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		} 

		e = ((int)dy<<1) - dx;  
	 
		for (j=0; j<=dx; j++) {
			sp(screen, x, y, c);
		 
			if (e>=0) {
				if (xchange==1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx<<1);
			}
			if (xchange==1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy<<1);
		}
	}
} // end of draw_line


/* Fill a row from one point to another
 *
 * Argument:
 *	line:
 *		The row that fill will be performed on.
 *	x0:
 *		edge 0 of the fill.
 *	x1:
 *		edge 1 of the fill.
 *	c:
 *		the color of the fill.
 *		(see color note at the top of this file)
*/
void Graphics::hline(uint8_t* screen, uint8_t line, uint16_t x0, uint16_t x1, uint8_t c) {
	uint8_t lbit, rbit;
	if (x0 == x1)
		setPixel(screen, x0, line, c);
	else {
		if (x0 > x1) {
			lbit = x0;
			x0 = x1;
			x1 = lbit;
		}
		lbit = 0xff >> (x0&7);
		x0 = x0/8 + W_DIV8*line;
		rbit = ~(0xff >> (x1&7));
		x1 = x1/8 + W_DIV8*line;
		if (x0 == x1) {
			lbit = lbit & rbit;
			rbit = 0;
		}
		if (c == WHITE) {
			screen[x0++] |= lbit;
			while (x0 < x1)
				screen[x0++] = 0xff;
			screen[x0] |= rbit;
		}
		else if (c == BLACK) {
			screen[x0++] &= ~lbit;
			while (x0 < x1)
				screen[x0++] = 0;
			screen[x0] &= ~rbit;
		}
		else if (c == INVERT) {
			screen[x0++] ^= lbit;
			while (x0 < x1)
				screen[x0++] ^= 0xff;
			screen[x0] ^= rbit;
		}
	}
}


/* Fill a column from one point to another
 *
 * Argument:
 *	row:
 *		The row that fill will be performed on.
 *	y0:
 *		edge 0 of the fill.
 *	y1:
 *		edge 1 of the fill.
 *	c:
 *		the color of the fill.
 *		(see color note at the top of this file)
*/
void Graphics::vline(uint8_t* screen, uint8_t row, uint16_t y0, uint16_t y1, uint8_t c)
{
	unsigned char bit;
	int byte;
	
	if (y0 == y1)
		setPixel(screen, row, y0, c);
	else {
		if (y1 < y0) {
			bit = y0;
			y0 = y1;
			y1 = bit;
		}
		bit = 0x80 >> (row&7);
		byte = row/8 + y0*W_DIV8;
		if (c == WHITE) {
			while ( y0 <= y1) {
				screen[byte] |= bit;
				byte += W_DIV8;
				y0++;
			}
		}
		else if (c == BLACK) {
			while ( y0 <= y1) {
				screen[byte] &= ~bit;
				byte += W_DIV8;
				y0++;
			}
		}
		else if (c == INVERT) {
			while ( y0 <= y1) {
				screen[byte] ^= bit;
				byte += W_DIV8;
				y0++;
			}
		}
	}
}


/* draw a rectangle at x,y with a specified width and height
 * 
 * Arguments:
 *	x0:
 *		The x coordinate of upper left corner of the rectangle.
 *	y0:
 *		The y coordinate of upper left corner of the rectangle.
 *	w:
 *		The width of the rectangle.
 *	h:
 *		The height of the rectangle.
 *	c:
 *		The color of the rectangle.
 *		(see color note at the top of this file)
 *	fc:
 *		The fill color of the rectangle.
 *		(see color note at the top of this file)
 *		default =-1 (no fill)
*/
void Graphics::rectangle(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc)
{
	if (fc != -1)
	{
		for (unsigned char i = y0; i < y0+h; i++)
			hline(screen, i, x0, x0+w, fc);
	}
	line(screen, x0,y0,x0+w,y0,c);
	line(screen, x0,y0,x0,y0+h,c);
	line(screen, x0+w,y0,x0+w,y0+h,c);
	line(screen, x0,y0+h,x0+w,y0+h,c);
}


/* draw a circle given a coordinate x,y and radius both filled and non filled.
 *
 * Arguments:
 * 	x0:
 *		The x coordinate of the center of the circle.
 *	y0:
 *		The y coordinate of the center of the circle.
 *	radius:
 *		The radius of the circle.
 *	c:
 *		The color of the circle.
 *		(see color note at the top of this file)
 *	fc:
 *		The color to fill the circle.
 *		(see color note at the top of this file)
 *		default  =-1 (do not fill)
 */
void Graphics::circle(uint8_t* screen, uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int	ddF_y = -2 * radius;
	int x = 0;
	int y = radius;
	uint8_t pyy = y,pyx = x;
	
	
	//there is a fill color
	if (fc != -1)
		hline(screen, y0, x0-radius, x0+radius, fc);
	
	sp(screen, x0, y0 + radius,c);
	sp(screen, x0, y0 - radius,c);
	sp(screen, x0 + radius, y0,c);
	sp(screen, x0 - radius, y0,c);
	
	while(x < y) {
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		//there is a fill color
		if (fc != -1) {
			//prevent double draws on the same rows
			if (pyy != y) {
				hline(screen, y0+y, x0-x, x0+x, fc);
				hline(screen, y0-y, x0-x, x0+x, fc);
			}
			if (pyx != x && x != y) {
				hline(screen, y0+x, x0-y, x0+y, fc);
				hline(screen, y0-x, x0-y, x0+y, fc);
			}
			pyy = y;
			pyx = x;
		}
		sp(screen, x0 + x, y0 + y,c);
		sp(screen, x0 - x, y0 + y,c);
		sp(screen, x0 + x, y0 - y,c);
		sp(screen, x0 - x, y0 - y,c);
		sp(screen, x0 + y, y0 + x,c);
		sp(screen, x0 - y, y0 + x,c);
		sp(screen, x0 + y, y0 - x,c);
		sp(screen, x0 - y, y0 - x,c);
	}
} // end of draw_circle


/* place a bitmap at x,y where the bitmap is defined as {width,height,imagedata....}
 *
 * Arguments:
 *	x:
 *		The x coordinate of the upper left corner.
 *	y:
 *		The y coordinate of the upper left corner.
 *	bmp:
 *		The bitmap data to print.
 *	i:
 *		The offset into the image data to start at.  This is mainly used for fonts.
 *		default	=0
 *	width:
 *		Override the bitmap width. This is mainly used for fonts.
 *		default =0 (do not override)
 *	height:
 *		Override the bitmap height. This is mainly used for fonts.
 *		default	=0 (do not override)
*/
void Graphics::blitBitmap(uint8_t* screen, uint8_t x, uint8_t y, const unsigned char * bmp, uint16_t i, uint8_t width, uint8_t lines)
{

	uint8_t temp, lshift, rshift, save, xtra;
	uint16_t si = 0;
	
	rshift = x&7;
	lshift = 8-rshift;
	if (width == 0) {
		width = pgm_read_byte((uint32_t)(bmp) + i);
		i++;
	}
	if (lines == 0) {
		lines = pgm_read_byte((uint32_t)(bmp) + i);
		i++;
	}
		
	if (width&7) {
		xtra = width&7;
		width = width/8;
		width++;
	}
	else {
		xtra = 8;
		width = width/8;
	}

	
	for (uint8_t l = 0; l < lines; l++) {
		si = (y + l)*W_DIV8 + x/8;
		if (width == 1)
			temp = 0xff >> rshift + xtra;
		else
			temp = 0;
		save = screen[si];
		screen[si] &= ((0xff << lshift) | temp);
		temp = pgm_read_byte((uint32_t)(bmp) + i++);
		screen[si++] |= temp >> rshift;
		for ( uint16_t b = i + width-1; i < b; i++) {
			save = screen[si];
			screen[si] = temp << lshift;
			temp = pgm_read_byte((uint32_t)(bmp) + i);
			screen[si++] |= temp >> rshift;
		}
		if (rshift + xtra < 8)
			screen[si-1] |= (save & (0xff >> rshift + xtra));	//test me!!!
		if (rshift + xtra - 8 > 0)
			screen[si] &= (0xff >> rshift + xtra - 8);
		screen[si] |= temp << lshift;
	}
} // end of bitmap


/* shift the pixel buffer in any direction
 * This function will shift the screen in a direction by any distance.
 *
 * Arguments:
 *	distance:
 *		The distance to shift the screen
 *	direction:
 *		The direction to shift the screen the direction and the integer values:
 *		UP		=0
 *		DOWN	=1
 *		LEFT	=2
 *		RIGHT	=3
*/
void Graphics::shift(uint8_t* screen, uint8_t distance, uint8_t direction)
{
	uint8_t * src;
	uint8_t * dst;
	uint8_t * end;
	uint8_t shift;
	uint8_t tmp;
	switch(direction) {
		case UP:
			dst = screen;
			src = screen + distance*W_DIV8;
			end = screen + SCREEN_BYTE_SIZE;
				
			while (src <= end) {
				*dst = *src;
				*src = 0;
				dst++;
				src++;
			}
			break;
		case DOWN:
			dst = screen + SCREEN_BYTE_SIZE;
			src = dst - distance*W_DIV8;
			end = screen;
				
			while (src >= end) {
				*dst = *src;
				*src = 0;
				dst--;
				src--;
			}
			break;
		case LEFT:
			shift = distance & 7;
			
			for (uint8_t line = 0; line < H; line++) {
				dst = screen + W_DIV8*line;
				src = dst + distance/8;
				end = dst + W_DIV8-2;
				while (src <= end) {
					tmp = 0;
					tmp = *src << shift;
					*src = 0;
					src++;
					tmp |= *src >> (8 - shift);
					*dst = tmp;
					dst++;
				}
				tmp = 0;
				tmp = *src << shift;
				*src = 0;
				*dst = tmp;
			}
			break;
		case RIGHT:
			shift = distance & 7;
			
			for (uint8_t line = 0; line < H; line++) {
				dst = screen + W_DIV8-1 + W_DIV8*line;
				src = dst - distance/8;
				end = dst - W_DIV8+2;
				while (src >= end) {
					tmp = 0;
					tmp = *src >> shift;
					*src = 0;
					src--;
					tmp |= *src << (8 - shift);
					*dst = tmp;
					dst--;
				}
				tmp = 0;
				tmp = *src >> shift;
				*src = 0;
				*dst = tmp;
			}
			break;
	}
}

void Graphics::setFont(const unsigned char * f)
{
	font = f;
}

/*
 * print an 8x8 char c at x,y
 * x must be a multiple of 8
 */
void Graphics::blitCharacter(uint8_t* screen, uint8_t x, uint8_t y, unsigned char c)
{

	c -= pgm_read_byte(font+2);
	blitBitmap(screen, x, y, font, (c*pgm_read_byte(font+1))+3, pgm_read_byte(font),pgm_read_byte(font+1));
}

void Graphics::inc_txtline(uint8_t* screen)
{
	if (cursor_y >= (H - pgm_read_byte(font+1)))
		shift(screen, pgm_read_byte(font+1),UP);
	else
		cursor_y += pgm_read_byte(font+1);
}

/* default implementation: may be overridden */
void Graphics::writeString(uint8_t* screen, const char *str)
{
	while (*str)
		writeCharacter(screen, *str++);
}

/* default implementation: may be overridden */
void Graphics::writeStringWithLength(uint8_t* screen, const uint8_t *buffer, uint8_t size)
{
	while (size--)
		writeCharacter(screen, *buffer++);
}

void Graphics::writeCharacter(uint8_t* screen, uint8_t c)
{
	uint8_t fontWidth = pgm_read_byte(font);
	
	switch(c) {
		case '\0':			//null
			break;
		case '\n':			//line feed
			cursor_x = 0;
			inc_txtline(screen);
			break;
		case 8:				//backspace
			cursor_x -= fontWidth;
			blitCharacter(screen, cursor_x, cursor_y, ' ');
			break;
		case 13:			//carriage return !?!?!?!VT!?!??!?!
			cursor_x = 0;
			break;
		case 14:			//form feed new page(clear screen)
			//clear_screen();
			break;
		default:
			if (cursor_x >= (W - fontWidth)) {
				cursor_x = 0;
				inc_txtline(screen);
				blitCharacter(screen, cursor_x, cursor_y, c);
			}
			else
				blitCharacter(screen, cursor_x, cursor_y, c);
			cursor_x += fontWidth;
	}
}

void Graphics::printPGM(uint8_t* screen, const char str[])
{
	char c;
	while ((c = pgm_read_byte(str)))
	{
		str++;
		writeCharacter(screen, c);
	}
}

void Graphics::printPGM(uint8_t* screen, uint8_t x, uint8_t y, const char str[])
{
	char c;
	cursor_x = x;
	cursor_y = y;
	while ((c = pgm_read_byte(str)))
	{
		str++;
		writeCharacter(screen, c);
	}
}

void Graphics::setCursor(uint8_t x, uint8_t y)
{
	cursor_x = x;
	cursor_y = y;
}

void Graphics::print(uint8_t* screen, uint8_t x, uint8_t y, const char str[])
{
	cursor_x = x;
	cursor_y = y;
	writeString(screen, str);
}
