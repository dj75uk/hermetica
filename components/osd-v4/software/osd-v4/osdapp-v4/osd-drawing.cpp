/*
 * osd_rendering.cpp
 *
 * Created: 06/05/2023 22:50:11
 *  Author: Daniel.Johnson
 */ 
#include "osd.hpp"

#define PSTR_c02u PSTR("%c%02u")
#define PSTR_02u PSTR("%02u")
#define PSTR_03u PSTR("%03u")
#define PSTR_05u PSTR("%05u")

void OSD::drawAlpha(uint8_t* screen)
{
	float alpha = _model->attitude.pitch - _model->fpi.pitch;
	int16_t  a = (int16_t)alpha;
	uint16_t b = (uint16_t)((abs(alpha) - abs(a)) * 10);
	_g.setFont(font4x6);
	sprintf_P(_sz, PSTR("op%+d."), a);
	uint8_t length = strlen(_sz);
	_g.print(screen, 0, 6, _sz);
	sprintf_P(_sz, PSTR("%01u"), b);
	_g.print(screen, (length * 4) - 2, 6, _sz);
	
	if (alpha > 30.0F)
	{	
		_g.setFont(font6x8);
		sprintf_P(_sz, PSTR("STALL WARNING"));
		_g.print(screen, CX - (6 * strlen(_sz) / 2), CY + 10, _sz);
	}
}	

void OSD::drawHeadingStrip(uint8_t* screen, uint8_t full)
{
	uint8_t xmin = CX-50;
	uint8_t xmax = CX+50;
	
	uint16_t heading = _model->heading.magnetic;
	_g.setFont(font6x8);
	sprintf_P(_sz, PSTR_03u, heading);
	_g.print(screen, CX - 8, 10, _sz);
	_g.line(screen, xmin, 6, xmax, 6, 1);
	sp(screen, CX, 7, 1);
	sp(screen, CX, 8, 1);
}

void OSD::drawAltitudeStrip(uint8_t* screen, uint8_t full)
{
	const uint8_t x = (W - (4 * 6)) - 4;
	char source = 'B';
	uint16_t altitude = (uint16_t)_model->altitude.barometric;
	
	if (_model->altitude.gps > 0)
	{
		altitude = _model->altitude.gps;
		source = 'G';
	}
	
	_g.setFont(font6x8);
	
	sprintf_P(_sz, PSTR("%4u"), altitude);
	_g.print(screen, x, CY - 2, _sz);
	
	_sz[0] = source;
	_sz[1] = 0;
	_g.print(screen, x + (3 * 6), CY + 5, _sz);
	
	if (full)
	{
		// altitude frame...
		_g.line(screen, W - 1, CY - 35, W - 1, CY + 35, 1);
		_g.line(screen, W - 1, CY, W - 4, CY, 1);
		
		uint16_t ranged = (uint16_t)(_model->altitude.ranged * 100.0F);
		if ((ranged > 0) && (ranged < 300))
		{
			_g.setFont(font6x8);
			sprintf_P(_sz, PSTR("AAA%03u"), ranged);
			//                   012345
			_sz[0] = 'R';
			_sz[1] = _sz[3];
			_sz[2] = 0;
			_g.print(screen, CX - 14, CY + 20, _sz);
			//
			_sz[0] = '.';
			_sz[1] = _sz[4];
			_sz[2] = _sz[5];
			_sz[3] = 0;
			_g.print(screen, CX - 3, CY + 20, _sz);
		}
	}
}

void OSD::drawVSI(uint8_t* screen)
{
	uint8_t x = (W - (5 * 4)) - 1;
	
	// vsi...
	int16_t y_vsi = CY - (int16_t)_model->vsi.barometric;
	if (y_vsi < 30)
		y_vsi = 30;
	if (y_vsi > 100)
		y_vsi = 100;
	
	_g.setFont(font4x6);
	_g.line(screen, x - 4, CY, x - 2, CY, 1);
	_g.line(screen, x - 4, CY, x - 4, y_vsi, 1);
	_g.setPixel(screen, x - 5, y_vsi, 1);
	sprintf_P(_sz, PSTR("%4d"), (int16_t)abs(_model->vsi.barometric));
	_g.print(screen, x - 21, y_vsi-2, _sz);
}

void OSD::drawAirspeedStrip(uint8_t* screen, uint8_t full)
{
	const uint8_t x = 6;
	char source = 'P';
	uint16_t airspeed = (uint16_t)_model->airspeed.pitot;
	if (airspeed == 0)
	{
		airspeed = (uint16_t)_model->airspeed.gps;
		if (airspeed > 0)
			source = 'G';
	}
	
	_g.setFont(font6x8);
	
	sprintf_P(_sz, PSTR("%u"), airspeed);
	_g.print(screen, x, CY - 2, _sz);
	
	_sz[0] = source;
	_sz[1] = 0;
	_g.print(screen, x, CY + 5, _sz);
	
	// frame...
	_g.line(screen, 2, CY - 35, 2, CY + 35, 1);
	_g.line(screen, 2, CY,      5, CY, 1);
	
	// stall...
	_g.line(screen, 0, CY - 20, 0, CY + 35, 1);
}

void OSD::drawNosePointer(uint8_t* screen)
{
	_g.line(screen, CX, CY - 2, CX, CY + 2, 1);
	_g.line(screen, CX - 2, CY, CX + 3, CY, 1);
}
void OSD::drawNosePointer(uint8_t* screen, float pitch)
{
	drawNosePointer(screen);
	uint8_t p = (uint8_t)abs(pitch);
	if (p <= 1)
		return;
	_g.setFont(font4x6);
	sprintf_P(_sz, PSTR("%c%u"), pitch >= 0 ? '+' : '-', p);
	_g.print(screen, CX + 2, CY - 6, _sz);
}

void OSD::drawPitchRateIndicator(uint8_t* screen)
{
	float absRate = abs(_model->attitude.pitchRate);
	if (absRate > 0.2F)
	{
		uint16_t y = (uint16_t)(OSD_PITCHRATE_FACTOR * absRate);
		if (y > 30)
			y = 30;
		if (_model->attitude.pitchRate > 0)
		{
			y = CY - y;
			_g.line(screen, CX, CY, CX, y, 1);
			_g.line(screen, CX, y, CX - 2, y + 2, 1);
			_g.line(screen, CX, y, CX + 2, y + 2, 1);
		}
		else
		{
			y = CY + y;
			_g.line(screen, CX, CY, CX, y, 1);
			_g.line(screen, CX, y, CX - 2, y - 2, 1);
			_g.line(screen, CX, y, CX + 2, y - 2, 1);
		}
	}
}

void OSD::drawRollRateIndicator(uint8_t* screen, float rollRate)
{
	float absRate = abs(rollRate);
	if (absRate > 0.2F)
	{
		uint16_t x = (uint16_t)(OSD_ROLLRATE_FACTOR  * absRate);
		if (x > 50)
			x = 50;
		if (rollRate > 0)
		{
			x = CX + x;
			_g.line(screen, CX, CY, x, CY, 1);
			_g.line(screen, x, CY, x - 2, CY - 2, 1);
			_g.line(screen, x, CY, x - 2, CY + 2, 1);
		}
		else
		{
			x = CX - x;
			_g.line(screen, CX, CY, x, CY, 1);
			_g.line(screen, x, CY, x + 2, CY - 2, 1);
			_g.line(screen, x, CY, x + 2, CY + 2, 1);
		}
	}
}

void OSD::drawFlightPathIndicator(uint8_t* screen, float fpiPitch, float fpiYaw)
{
	uint8_t x = CX - fpiYaw;
	uint8_t y = (uint8_t)( (int8_t)CY - (int8_t)(1.0F * fpiPitch));
	_g.circle(screen, x, y, OSD_FPI_RADIUS, 1);
	_g.line(screen,x, y - (OSD_FPI_RADIUS+3), x, y-OSD_FPI_RADIUS, 1);
	_g.line(screen,x - (OSD_FPI_RADIUS+5), y, x-OSD_FPI_RADIUS,y, 1);
	_g.line(screen,x + (OSD_FPI_RADIUS), y, x+OSD_FPI_RADIUS+6,y, 1);
}

void OSD::drawRollTrim(uint8_t* screen, float roll)
{
	uint8_t x = CX;
	uint8_t y = H - OSD_ROLLTRIM_Y_OFFSET;
	
	float absRoll = (roll < 0) ? -1.0F * roll : roll;
	if (absRoll > 3.3F)
		absRoll = 3.3F;
	
	// angle is scaled to 45 degrees for 3 degrees...
	// scaled to radians
	float theta = (45.0F / 3.0F) * absRoll * 0.01746F;
	
	uint8_t dy = (uint8_t)(OSD_ROLLTRIM_NEEDLE_LENGTH * cosf(theta));
	uint8_t dx = (uint8_t)(OSD_ROLLTRIM_NEEDLE_LENGTH * sinf(theta));

	if (roll > 0)
		_g.line(screen, x, y, x+dx, y+dy, 1);
	else
		_g.line(screen, x, y, x-dx, y+dy, 1);

	_g.setPixel(screen, x-8, y+8,  1);
	_g.setPixel(screen, x-6, y+10, 1);
	_g.setPixel(screen, x-3, y+11, 1);
	_g.setPixel(screen, x,   y+12, 1);
	_g.setPixel(screen, x+3, y+11, 1);
	_g.setPixel(screen, x+6, y+10, 1);
	_g.setPixel(screen, x+8, y+8,  1);
	
	_g.setFont(font4x6);
	_g.print(screen, x - 11, y +  10, "3");
	_g.print(screen, x - 1,  y +  14, "0");
	_g.print(screen, x + 9,  y +  10, "3");
}

void OSD::drawLadder(uint8_t* screen, const float pitch, const float roll, const float fpiPitch, const float fpiYaw)
{
	const uint8_t horizonLine_width = 60;
	uint8_t  absPitch    = (uint8_t)abs(pitch);
	float    pitchFactor = 1.0;
	float    rollFactor  = 0.01746F;
	uint16_t horizonY    = CY - (pitch * pitchFactor);
	uint16_t plus20Y     = horizonY + (20 * pitchFactor);
	
	_g.rline(screen, CX-horizonLine_width, horizonY, CX-9, horizonY, CX,CY, (roll*rollFactor), 1);
	_g.rline(screen, CX+9, horizonY, CX+horizonLine_width, horizonY, CX,CY, (roll*rollFactor), 1);

	// clear border around ladder...
	for (uint16_t index = 0; index < 24; index++)
		_g.line(screen, index, 0, index, MAXY, 0);
	for (uint16_t index = (W-25); index < W; index++)
		_g.line(screen, index, 0, index, MAXY, 0);
	for (uint16_t index = 0; index < 15; index++)
		_g.line(screen, 0, index, MAXX, index, 0);
	for (uint16_t index = (H-15); index < H; index++)
		_g.line(screen, 0, index, MAXX, index, 0);
		
	// draw alpha...
	float alpha = pitch - fpiPitch;
	int16_t  a = (int16_t)alpha;
	uint16_t b = (uint16_t)((abs(alpha) - abs(a)) * 10);
	_g.setFont(font4x6);
	sprintf_P(_sz, PSTR("op%+d."), a);
	uint8_t length = strlen(_sz);
	_g.print(screen, 0, 6, _sz);
	sprintf_P(_sz, PSTR("%01u"), b);
	_g.print(screen, (length * 4) - 2, 6, _sz);
	if (alpha > 30.0F)
	{
		_g.setFont(font6x8);
		sprintf_P(_sz, PSTR("STALL WARNING"));
		_g.print(screen, CX - (6 * strlen(_sz) / 2), CY + 10, _sz);
	}
	
	// draw nose pointer...
	_g.line(screen, CX, CY - 2, CX, CY + 2, 1);
	_g.line(screen, CX - 2, CY, CX + 3, CY, 1);
	if (absPitch > 0)
	{
		_g.setFont(font4x6);
		sprintf_P(_sz, PSTR("%c%u"), pitch >= 0 ? '+' : '-', absPitch);
		_g.print(screen, CX + 2, CY - 6, _sz);
	}

	// draw flight-path indicator...
	uint8_t x = CX - fpiYaw;
	uint8_t y = (uint8_t)( (int8_t)CY - (int8_t)(1.0F * fpiPitch));
	_g.circle(screen, x, y, OSD_FPI_RADIUS, 1);
	_g.line(screen,x, y - (OSD_FPI_RADIUS+3), x, y-OSD_FPI_RADIUS, 1);
	_g.line(screen,x - (OSD_FPI_RADIUS+5), y, x-OSD_FPI_RADIUS,y, 1);
	_g.line(screen,x + (OSD_FPI_RADIUS), y, x+OSD_FPI_RADIUS+6,y, 1);
	
	// draw rssi...
	_g.setFont(font4x6);
	sprintf_P(_sz, PSTR("%c%c%c%c"),
		_model->rssi0 > 0 ? 'A' : '-',
		_model->rssi1 > 0 ? 'B' : '-',
		_model->rssi2 > 0 ? 'C' : '-',
		_model->rssi3 > 0 ? 'D' : '-'
	);
	_g.print(screen, W - 17, 6, _sz);
	
	// draw gps...
	sprintf_P(_sz, PSTR("%u:%u/%u"), _model->gpsFix, _model->gpsSatellitesInUse, _model->gpsSatellitesInView);
	_g.print(screen, W - ((strlen(_sz) * 4) + 1), 12, _sz);
}

void OSD::drawMode(uint8_t* screen)
{
	VtxMode enforcedSubmode = (VtxMode)_model->vtxMode;
	//
	_g.setFont(font4x6);
	sprintf_P(_sz, PSTR("P%u"), (uint8_t)_vtx->power());
	_g.print(screen, 0, H - 12, _sz);
	//
	switch ((FcuMode)_model->fcuMode)
	{
		case FcuMode::Startup:
		case FcuMode::WaitingForArmStage1:
		case FcuMode::WaitingForArmStage2:
		case FcuMode::WaitingForArmStage3:
		case FcuMode::WaitingForArmStage4:
		case FcuMode::WaitingForArmStage5:
			strcpy_P(_sz, PSTR("STARTUP"));
			enforcedSubmode = VtxMode::Diagnostics;
			break;
		case FcuMode::Takeoff:
			strcpy_P(_sz, PSTR("TAKEOFF"));
			enforcedSubmode = VtxMode::Decluttered;
			break;
		case FcuMode::Acrobatic:
			strcpy_P(_sz, PSTR("ACROBATIC"));
			break;
		case FcuMode::Stabilised:
			strcpy_P(_sz, PSTR("STABILISED"));
			break;
		case FcuMode::Landing:
			strcpy_P(_sz, PSTR("LANDING"));
			enforcedSubmode = VtxMode::Decluttered;
			break;
		case FcuMode::RTB:
			strcpy_P(_sz, PSTR("RTB"));
			break;
		case FcuMode::Waypoint:
		case FcuMode::WaypointPrevious:
		case FcuMode::WaypointNext:
			strcpy_P(_sz, PSTR("WAYPOINT"));
			break;
		case FcuMode::Loiter:
			strcpy_P(_sz, PSTR("LOITER"));
			break;
		case FcuMode::Shutdown:
			strcpy_P(_sz, PSTR("SHUTDOWN"));
			enforcedSubmode = VtxMode::Decluttered;
			break;
		default:
			strcpy_P(_sz, PSTR("???"));
	}
	_g.print(screen, W - ((strlen(_sz) * 4) + 1), H - 6, _sz);
	//
	switch (enforcedSubmode)
	{
		case VtxMode::Diagnostics:
			strcpy_P(_sz, PSTR("TST"));
			break;
		case VtxMode::Navigation:
			strcpy_P(_sz, PSTR("NAV"));
			break;
		case VtxMode::Decluttered:
			strcpy_P(_sz, PSTR("MIN"));
			break;
		default:
			strcpy_P(_sz, PSTR("???"));
	}
	_g.print(screen, 0, H - 6, _sz);
}

void OSD::drawEngineData(uint8_t* screen)
{
	_g.setFont(font4x6);
	sprintf_P(_sz, PSTR("%03u%%"), _model->engine[1].throttlePercentage);
	_g.print(screen, CX - (OSD_ENGINEDATA_X_OFFSET + 13), H - OSD_ENGINEDATA_Y_OFFSET, _sz);
	sprintf_P(_sz, PSTR("%03u%%"), _model->engine[0].throttlePercentage);
	_g.print(screen, CX + OSD_ENGINEDATA_X_OFFSET, H - OSD_ENGINEDATA_Y_OFFSET, _sz);
}