#ifdef DEBUG
	#pragma GCC error "Incorrect build configuration: DEBUG defined. Hint: Try switching to Release configuration."
#endif
#ifndef NDEBUG
	#pragma GCC error "Incorrect build configuration: NDEBUG not defined. Hint: Try switching to Release configuration."
#endif
#include <Arduino.h>
#include "config.h"
#include "smartaudio.h"
#include "osdmodel.h"
#include "display.h"
#include "osd.hpp"
#include "command-handler.h"
#include "hardware-macros.h"

OsdModel		model;
SmartAudioVtx	vtx(&VTX_USART);
OSD				osd(&model, &vtx);
CommandHandler  cmdh(&CMD_USART, CMD_BAUD, CMD_OPTIONS, &model, &vtx);

uint8_t limit_u8(uint8_t value, uint8_t low, uint8_t high)
{
	return (value < low) ? low : ((value > high) ? high : value);
}

void setup()
{
	// set K0..K2 to output for LEDs...
	DDRK |= (1<<0);
	DDRK |= (1<<1);
	DDRK |= (1<<2);
	
	// drive K0..K2 low...
	LED_OSD_OFF
	LED_CMD_OFF
	LED_ERR_OFF

	// clear the model...
	memset(&model, (uint8_t)0, OSD_MODEL_SIZE);
	
	// start the primary usart handler...
	cmdh.setup();
  
	// start the vtx...
	vtx.setup();
	vtx.power(SmartAudioPower::Minimum);
	vtx.channel(0);
	
	// start the osd...
	osd.setup(PAL);
	osd.enable();
}

void loop()
{
    for (;;)
    {	
		displayData.screenSwitchEnabled = ((
			cmdh.cycle() +
			vtx.cycle()
		) == 0) ? 1 : 0;

	    if (displayData.screenSwitchEnabled == 1)
	    {
		    LED_CMD_OFF
	    }
		osd.cycle();
    }
}