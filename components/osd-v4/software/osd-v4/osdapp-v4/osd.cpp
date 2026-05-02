/*
 * osd.cpp
 *
 * Created: 06/05/2023 22:53:42
 *  Author: Daniel.Johnson
 */ 
#include "osd.hpp"
#include "graphics.h"
#include "font6x8.h"
#include "font4x6.h"
#include <string.h>
#include <stdio.h>
#include "osdmodel.h"
#include "smartaudio.h"

OSD::OSD(OsdModel* model, SmartAudioVtx* vtx)
{
	_model          = model;
	_vtx            = vtx;
	_enabled        = false;
	_model->fcuMode = FcuMode::Startup;
	_model->vtxMode = VtxMode::Decluttered;
}

void OSD::enable()
{
	if (_model->fcuMode == FcuMode::Startup)
		_model->fcuMode = FcuMode::WaitingForArmStage1;
	_enabled = true;
}

void OSD::disable()
{
	_enabled = false;
}

uint8_t OSD::error() const
{
	return _error;
}

void OSD::setup(uint8_t ntscOrPal)
{
	_g.begin(ntscOrPal);
	TCCR1A  = 0;
	TCCR1B  = _BV(CS10);
	TIMSK1 |= _BV(ICIE1);
	_g.setFont(font4x6);
	_g.fill(displayData.pointerToScreenToInject);
	_g.fill(displayData.pointerToScreenToDrawOn);
}

void OSD::cycle()
{
	if (displayData.screenSwitchPending)
		return;
	if (displayData.drawingInProgress) // should never happen...
		return;
	
	displayData.drawingInProgress = 1;

	// wait for synchronisation...
	displaySyncFrames(1);
	uint8_t* screen = displayData.pointerToScreenToDrawOn;
	_g.setFont(font6x8);
	_g.fill(screen);

	if (!_enabled)
		return;

	SmartAudioPower requiredVtxPower = SmartAudioPower::Maximum;

	// enforce the osd submode if required...
	VtxMode enforcedVtxMode = _model->vtxMode;
	switch (_model->fcuMode)
	{
		case FcuMode::Startup:
		case FcuMode::WaitingForArmStage1:
		case FcuMode::WaitingForArmStage2:
		case FcuMode::WaitingForArmStage3:
		case FcuMode::WaitingForArmStage4:
		case FcuMode::WaitingForArmStage5:
		case FcuMode::Shutdown:
			requiredVtxPower = SmartAudioPower::Minimum;
			break;
		
		default:
			requiredVtxPower = SmartAudioPower::Maximum;
			break;
	}

	_model->fcuMode  = FcuMode::RTB;
	_model->vtxMode  = VtxMode::Diagnostics;
	enforcedVtxMode  = _model->vtxMode;
	requiredVtxPower = SmartAudioPower::Minimum;
	
	
	switch (_model->fcuMode)
	{
	case FcuMode::Startup:
		drawStartup(screen);
		break;
	case FcuMode::WaitingForArmStage1:
	case FcuMode::WaitingForArmStage2:
	case FcuMode::WaitingForArmStage3:
	case FcuMode::WaitingForArmStage4:
	case FcuMode::WaitingForArmStage5:
		drawWaitingForArm(screen, _model->fcuMode, _model->vtxMode);
		break;
	case FcuMode::Takeoff:
		drawLadder(screen, _model->attitude.pitch, _model->attitude.roll, _model->fpi.pitch, _model->fpi.yaw);
		drawMode(screen);
		drawRollTrim(screen, _model->attitude.roll);
		drawHeadingStrip(screen, 1);
		drawAltitudeStrip(screen, 1);
		drawAirspeedStrip(screen, 1);
		drawEngineData(screen);
		break;
	default:
		// render the specific osd components...
		switch (enforcedVtxMode)
		{
			case VtxMode::Diagnostics:
				drawDiagnostics(screen);
				break;
			case VtxMode::Navigation:
				drawLadder(screen, _model->attitude.pitch, _model->attitude.roll, _model->fpi.pitch, _model->fpi.yaw);
				drawMode(screen);
				drawRollTrim(screen, _model->attitude.roll);
				drawHeadingStrip(screen, 1);
				drawAltitudeStrip(screen, 1);
				drawAirspeedStrip(screen, 1);
				drawEngineData(screen);
				break;
			case VtxMode::Decluttered:
				drawLadder(screen, _model->attitude.pitch, _model->attitude.roll, _model->fpi.pitch, _model->fpi.yaw);
				drawMode(screen);
				drawHeadingStrip(screen, 1);
				drawAltitudeStrip(screen, 1);
				drawAirspeedStrip(screen, 1);
				break;
		}
		break;
	}
	
	// set vtx power...
	if (_vtx->power() != requiredVtxPower)
		_vtx->power(requiredVtxPower);

	// drawing complete...
	displaySyncFrames(1);
	displayData.drawingInProgress   = 0;
	displayData.screenSwitchPending = 1;
}