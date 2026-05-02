/*
 * config.h
 *
 * Created: 13/04/2023 11:01:13
 *  Author: Daniel.Johnson
 */
#ifndef CONFIG_H_
	#define CONFIG_H_

	// application version...
	#define					APP_VERSION_MAJOR			4
	#define					APP_VERSION_MINOR			4
	#define					APP_VERSION_BUILD			32
	
	// hardware configuration...
	#define					CMD_USART					Serial2
	#define					CMD_BAUD					57600
	#define					CMD_OPTIONS					SERIAL_8N1
	#define					VTX_USART					Serial3

	// display-driver configuration...
	//
	//ENABLE_FAST_OUTPUT chooses the highest bit of a port over the original output method
	//comment out this line to switch back to the original output pins.
	#define					ENABLE_FAST_OUTPUT
	#define					W_DIV8						26
	#define					W							(W_DIV8*8)
	#define					H							110
	#define					SCREEN_BYTE_SIZE			(W_DIV8*H)
	#define					MAXX						(W-1)
	#define					MAXY						(H-1)
	#define					CX							(W/2)
	#define					CY							((H/2)-1)

#endif /* CONFIG_H_ */