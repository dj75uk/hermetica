#pragma once
#ifndef _OSD_HPP
#define _OSD_HPP

#include "graphics.h"
#include "font6x8.h"
#include "font4x6.h"
#include <string.h>
#include <stdio.h>
#include "osdmodel.h"
#include "smartaudio.h"

#define OSD_PITCHRATE_FACTOR			50.0F
#define OSD_ROLLRATE_FACTOR				30.0F
#define OSD_FPI_RADIUS					2
#define OSD_ROLLTRIM_Y_OFFSET			20
#define OSD_ROLLTRIM_NEEDLE_LENGTH		12
#define OSD_ENGINEDATA_X_OFFSET			18
#define OSD_ENGINEDATA_Y_OFFSET			6

class OSD
{
private:
	bool			_enabled;
	Graphics		_g;
	OsdModel*		_model;
	SmartAudioVtx*	_vtx;
	uint8_t			_error;
	char			_sz[32];

	void drawStartup(uint8_t* screen);
	void drawWaitingForArm(uint8_t* screen, FcuMode fcuMode, VtxMode submode);
	void drawDiagnostics(uint8_t* screen);
	void drawHeadingStrip(uint8_t* screen, uint8_t full);
	void drawAltitudeStrip(uint8_t* screen, uint8_t full);
	void drawAirspeedStrip(uint8_t* screen, uint8_t full);
	void drawNosePointer(uint8_t* screen);
	void drawNosePointer(uint8_t* screen, float pitch);
	void drawFlightPathIndicator(uint8_t* screen, float fpiPitch, float fpiYaw);
	void drawLadder(uint8_t* screen, const float pitch, const float roll, const float fpiPitch, const float fpiYaw);
	void drawMode(uint8_t* screen);
	void drawEngineData(uint8_t* screen);
	void drawRollTrim(uint8_t* screen, float roll);
	void drawVSI(uint8_t* screen);
	void drawAlpha(uint8_t* screen);
	void drawPitchRateIndicator(uint8_t* screen);
	void drawRollRateIndicator(uint8_t* screen, float rollRate);

public:
	OSD(OsdModel* model, SmartAudioVtx* vtx);
	void setup(uint8_t ntscOrPal);
	void enable();
	void disable();
	void cycle();
	uint8_t error() const;
};

#endif // _OSD_HPP
