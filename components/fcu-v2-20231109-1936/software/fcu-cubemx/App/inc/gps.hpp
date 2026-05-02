/*
 * gps.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_GPS_H_
#define APP_INC_GPS_H_

#include <cstdint>
#include "component.hpp"
#include "accuracy.hpp"
#include "heading.hpp"
#include "airspeed.hpp"
#include "groundspeed.hpp"
#include "altitude-gps.hpp"
#include "calendar.hpp"
#include "location.hpp"

static float parseFloat(const char* input)
{
	float result = 0;
	char lhs[8];
	char rhs[6];
	uint8_t point = 255;

	for (uint8_t i = 0; i < sizeof(lhs); i++)
		lhs[i] = 0;
	for (uint8_t i = 0; i < sizeof(rhs); i++)
		rhs[i] = 0;

	// find the position of the decimal point...
	for (uint8_t i = 0; i < strlen(input); i++)
	{
		if (input[i] == '.')
		{
			point = i;
			break;
		}
	}
	if (point == 255)
		return 0;

	// split the strings into lhs and rhs...
	uint8_t j = 0;
	for (uint8_t i = 0; i < point; i++)
		lhs[i] = input[i];
	for (uint8_t i = (point + 1); i < strlen(input); i++)
		rhs[j++] = input[i];

	char * end;

	uint32_t tmp = strtol(lhs, &end, 10);
	if (lhs == end)
	{
		// failed
		return 0;
	}

	result = (float)tmp;
	tmp = strtol(rhs, &end, 10);
	if (rhs == end)
	{
		// failed
		return 0;
	}
	uint8_t rhslen = (uint8_t)strlen(rhs);
	float divisor = 1;
	while (rhslen--)
		divisor *= 10;
	result += ((float)tmp) / divisor;
	return result;
}

class Gps
	: public HasStatusAndSetupAndCycle
	, public Heading
	, public Airspeed
	, public Groundspeed
	, public GpsAltitude
	, public ProvidesDate
	, public ProvidesTime
	, public Location
{
private:
    uint8_t _satellitesInView;
    uint8_t _satellitesInUse;
    float   _pdop;
    float   _hdop;
    float   _vdop;
    uint8_t _fix;
protected:
    Gps()
    {
    	_fix              = 0;
    	_satellitesInView = 0;
    	_satellitesInUse  = 0;
    	_pdop             = 0.0;
    	_hdop             = 0.0;
    	_vdop             = 0.0;
    }
    void satellitesInView(const uint8_t value) { _satellitesInView = value; }
    void satellitesInUse(const uint8_t value)  { _satellitesInUse  = value; }
    void pdop(const float value) { _pdop = value; }
    void hdop(const float value) { _hdop = value; }
    void vdop(const float value) { _vdop = value; }
    void fix(const uint8_t value) { _fix = value; }
public:
    uint8_t fix() const { return _fix; }
    uint8_t satellitesInView() const { return _satellitesInView; }
    uint8_t satellitesInUse() const  { return _satellitesInUse;  }
    float pdop() const { return _pdop; }
    float hdop() const { return _hdop; }
    float vdop() const { return _vdop; }
};

#endif /* APP_INC_GPS_H_ */
