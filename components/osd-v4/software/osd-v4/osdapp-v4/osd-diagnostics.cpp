/*
 * osd_diagnostics.cpp
 *
 * Created: 06/05/2023 22:58:52
 *  Author: Daniel.Johnson
 */ 
#include "osd.hpp"

void OSD::drawStartup(uint8_t* screen)
{
	sprintf_P(_sz, PSTR("WAITING FOR CONNECTION"));
	_g.print(screen, CX - (strlen(_sz) * 6) / 2, CY - 5, _sz);
}

void OSD::drawDiagnostics(uint8_t* screen)
{
	uint8_t line = 0;
	
	_error = 0;
	_g.setFont(font4x6);

	sprintf_P(_sz, PSTR("MODE:  %u %u"), _model->fcuMode, _model->vtxMode);
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("ATTI: %+06d %+06d %+06d"), (int)(_model->attitude.pitch * 100), (int)(_model->attitude.roll * 100), (int)(_model->attitude.yaw * 100));
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("RATE: %+06d %+06d %+06d"), (int)(_model->attitude.pitchRate * 100), (int)(_model->attitude.rollRate * 100), (int)(_model->attitude.yawRate * 100));
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("HEAD: %+06u %+06u %+06u"), _model->heading.magnetic, _model->heading.gps, _model->heading.imu);
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("ASPD: %+06u %+06u %+06u"), _model->airspeed.pitot, _model->airspeed.gps, _model->airspeed.imu);
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("ALT:  %+06d %+06d %+06d"), (int)(_model->altitude.barometric * 100), (int)(_model->altitude.gps * 100), (int)(_model->altitude.ranged * 100));
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("VSI:  %+06d %+06d %+06d"), (int)(_model->vsi.barometric * 100), (int)(_model->vsi.gps * 100), (int)(_model->vsi.ranged * 100));
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("FPI:  %+06d  ----- %+06d"), (int)(_model->fpi.pitch * 100), (int)(_model->fpi.yaw * 100));
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	line++;
	sprintf_P(_sz, PSTR("GPS:   %u %u %u"), _model->gpsFix, _model->gpsSatellitesInUse, _model->gpsSatellitesInView);
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	sprintf_P(_sz, PSTR("CURR: %+09d %+09d"), _model->location.latitude, _model->location.longitude);
	_g.print(screen, 0, (6 * line++) + 6, _sz);
	
	sprintf_P(_sz, PSTR("HOME: %+09d %+09d"), _model->home.latitude, _model->home.longitude);
	_g.print(screen, 0, (6 * line) + 6, _sz);
	sprintf_P(_sz, PSTR("%03d %04d"), _model->home.bearing, _model->home.range);
	_g.print(screen, 110, (6 * line++) + 6, _sz);

	sprintf_P(_sz, PSTR("WAYP: %+09d %+09d"), _model->waypoint.latitude, _model->waypoint.longitude);
	_g.print(screen, 0, (6 * line) + 6, _sz);
	sprintf_P(_sz, PSTR("%03d %04d"), _model->waypoint.bearing, _model->waypoint.range);
	_g.print(screen, 110, (6 * line++) + 6, _sz);
}

