#include "config.h"
#include "avr\pgmspace.h"

#ifndef DISPLAY_H
	#define DISPLAY_H
	
	#define _CYCLES_PER_US				(F_CPU / 1000000UL)

	// device specific settings...
	#if defined(ENABLE_FAST_OUTPUT)
		#define PORT_VID	PORTA
		#define	DDR_VID		DDRA
		#define VID_PIN		7
		#define BLD_HWS		"bld	r16,7\n\t"
		#define BST_HWS		"bst	r16,7\n\t"
		#define ANDI_HWS	"andi	r16,0x7F\n"
	#else
		#define PORT_VID	PORTB
		#define	DDR_VID		DDRB
		#define VID_PIN		6
		#define BLD_HWS		"bld	r16,6\n\t"
		#define BST_HWS		"bst	r16,6\n\t"
		#define ANDI_HWS	"andi	r16,0xBF\n"
	#endif
	
	// sync
	#define PORT_SYNC					PORTB
	#define DDR_SYNC					DDRB
	#define	SYNC_PIN					5

	#define _TIME_HORZ_SYNC				4.7
	#define _TIME_VIRT_SYNC				58.85
	#define _TIME_ACTIVE				46
	#define _CYCLES_VIRT_SYNC			((_TIME_VIRT_SYNC * _CYCLES_PER_US) - 1)
	#define _CYCLES_HORZ_SYNC			((_TIME_HORZ_SYNC * _CYCLES_PER_US) - 1)

	//Timing settings for NTSC
	#define _NTSC_TIME_SCANLINE			63.55
	#define _NTSC_TIME_OUTPUT_START		12
	#define _NTSC_LINE_FRAME			262
	#define _NTSC_LINE_START_VSYNC		0
	#define _NTSC_LINE_STOP_VSYNC		3
	#define _NTSC_LINE_DISPLAY			216
	#define _NTSC_LINE_MID				((_NTSC_LINE_FRAME - _NTSC_LINE_DISPLAY)/2 + _NTSC_LINE_DISPLAY/2)
	#define _NTSC_CYCLES_SCANLINE		((_NTSC_TIME_SCANLINE * _CYCLES_PER_US) - 1)
	#define _NTSC_CYCLES_OUTPUT_START	((_NTSC_TIME_OUTPUT_START * _CYCLES_PER_US) - 1)

	//Timing settings for PAL
	#define _PAL_TIME_SCANLINE			64
	#define _PAL_TIME_OUTPUT_START		12.5
	#define _PAL_LINE_FRAME				312
	#define _PAL_LINE_START_VSYNC		0
	#define _PAL_LINE_STOP_VSYNC		7
	#define _PAL_LINE_DISPLAY			260
	#define _PAL_LINE_MID				((_PAL_LINE_FRAME - _PAL_LINE_DISPLAY)/2 + _PAL_LINE_DISPLAY/2)
	#define _PAL_CYCLES_SCANLINE		((_PAL_TIME_SCANLINE * _CYCLES_PER_US) - 1)
	#define _PAL_CYCLES_OUTPUT_START	((_PAL_TIME_OUTPUT_START * _CYCLES_PER_US) - 1)

	typedef struct
	{
		uint8_t x;
		uint8_t y;
	} XYu8;

	typedef struct
	{
		int16_t x;
		int16_t y;	
	} XYi16;


	typedef struct
	{
		volatile uint8_t vsyncs;
		volatile uint16_t scanLine;
		unsigned char start_render;
		uint16_t lines_frame;   //remove me
		uint8_t output_delay; 	//remove me
		char vscale_const;		//combine me with status switch
		char vscale;			//combine me too.
		char vsync_end;			//remove me
		
		uint8_t  screenBuffer1[SCREEN_BYTE_SIZE];
		uint8_t  screenBuffer2[SCREEN_BYTE_SIZE];
		uint8_t  currentScreenToInject;
		uint8_t* pointerToScreenToInject;
		uint8_t* pointerToScreenToDrawOn;
		uint8_t  screenSwitchEnabled;
		uint8_t  screenSwitchPending;
		uint8_t  drawingInProgress;
		
	} DisplayData;

	extern DisplayData displayData;

	void sp(uint8_t* screen, uint8_t x, uint8_t y, char c);
	int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max);
	void displaySwitchScreenBuffer();
	void displaySyncFrames(unsigned int x);
	void displaySync();
	void displaySetup(uint8_t ntscOrPal);
	void displayHandleBlankLine();
	void displayHandleActiveLine();
	void displayHandleSyncLine();
	void displayRenderLine6();
	void displayRenderLine5();
	void displayRenderLine4();
	void displayRenderLine3();
	void displayWaitUntil(uint8_t time);

	XYi16 rotate(int16_t x0, int16_t y0, int16_t originX, int16_t originY, float angle);

	// delay macros
	__asm__ __volatile__ (
		// delay 1 clock cycle.
		".macro delay1\n\t"
			"nop\n"
		".endm\n"

		// delay 2 clock cycles
		".macro delay2\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 3 clock cyles
		".macro delay3\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 4 clock cylces
		".macro delay4\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 5 clock cylces
		".macro delay5\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 6 clock cylces
		".macro delay6\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 7 clock cylces
		".macro delay7\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 8 clock cylces
		".macro delay8\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 9 clock cylces
		".macro delay9\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"

		// delay 10 clock cylces
		".macro delay10\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n"
		".endm\n"
	);

	// common output macros, specific output macros at top of file
	__asm__ __volatile__ (
		// save port 16 and clear the video bit
		".macro svprt p\n\t"
			"in		r16,\\p\n\t"
			ANDI_HWS
			".endm\n"

			// output 1 bit port safe
			".macro o1bs p\n\t"
			BLD_HWS
			"out	\\p,r16\n"
		".endm\n"
	);

#endif // DISPLAY_H
