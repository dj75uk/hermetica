/*
 * uartgps.hpp
 *
 *  Created on: Oct 15, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_UARTGPS_HPP_
#define APP_INC_UARTGPS_HPP_

#include <cstdio>
#include <cstdint>
#include <cstring>
#include "component.hpp"
#include "alarm.hpp"
#include "gps.hpp"
#include "uart.hpp"
#include "debug.hpp"

#define NMEA_TYPE_UNKNOWN     0
#define NMEA_TYPE_RMC         1
#define NMEA_TYPE_VTG         2
#define NMEA_TYPE_GGA         3
#define NMEA_TYPE_GSA         4
#define NMEA_TYPE_GSV         5

#define GPS_FIX_NONE          0
#define GPS_FIX_2D            1
#define GPS_FIX_3D            2


extern "C"
{
	static uint32_t parseNmeaDate(char* fragment)
	{
		// it's ddmmyy format, swap it...
		char d1 = fragment[0];
		char d2 = fragment[1];
		fragment[0] = fragment[4];
		fragment[1] = fragment[5];
		fragment[4] = d1;
		fragment[5] = d2;


		char *end;
		uint32_t result = strtoul(fragment, &end, 10);
		if (fragment == end)
			return 0;
		return result + 20000000;
	}

	static uint32_t parseNmeaTime(const char* fragment)
	{
		char *end;
		uint32_t result = strtoul(fragment, &end, 10);
		if (fragment == end)
			return 0;
		return result;
	}

	static int32_t parseNmeaLatLongToInt32(const char* fragment)
	{
		char replace[12];
		char *end;
		uint8_t j = 0;

		replace[0] = 0;
		for (uint8_t i = 0; i < strlen(fragment); i++)
			if (isdigit(fragment[i]))
				replace[j++] = fragment[i];
		replace[j] = 0;

		int32_t result = strtol(replace, &end, 10);

		if (replace == end)
			return 0;
		return result;
	}
}

class UartGps : public Gps
{
private:
	const char* className = "uartgps";
	Uart*   _uart;
	uint8_t _position = 0;
	uint8_t _buffer[90];
	uint8_t _nmeaType = NMEA_TYPE_UNKNOWN;
	uint8_t _valid = 0;
	float   _trueTrackMadeGood = 0;
	uint8_t _fixQuality = 0;
	float   _geoidalSeparation = 0;

	void processFragment(uint8_t number, char* fragment, uint8_t length)
	{
		if (length == 0)
			return;
		if (number == 0)
		{
			_valid = 1;
			_nmeaType = NMEA_TYPE_UNKNOWN;
			if (strncmp(fragment, "$GPRMC", 6) == 0)
				_nmeaType = NMEA_TYPE_RMC;
			if (strncmp(fragment, "$GPVTG", 6) == 0)
				_nmeaType = NMEA_TYPE_VTG;
			if (strncmp(fragment, "$GPGSA", 6) == 0)
				_nmeaType = NMEA_TYPE_GSA;
			if (strncmp(fragment, "$GPGSV", 6) == 0)
				_nmeaType = NMEA_TYPE_GSV;
			if (strncmp(fragment, "$GPGGA", 6) == 0)
				_nmeaType = NMEA_TYPE_GGA;

			if (_nmeaType == NMEA_TYPE_UNKNOWN)
			{
				char output[32];
				sprintf(output, "nmea: %s", fragment);
				debugWriteWarning(className, __LINE__, output);
			}

			char output[32];
			sprintf(output, "nmea: %s", fragment);
			debugWriteWarning(className, __LINE__, output);
		}
		else
		{
			char output[32];
			sprintf(output, "fragment %u.%u = %s", _nmeaType, number, fragment);
			debugWriteInfo(className, __LINE__, output);
			switch (_nmeaType)
			{
			case NMEA_TYPE_GGA:
				switch (number)
				{
				case 1:
					time(parseNmeaTime(fragment));
					break;
				case 2:
					latitude(parseNmeaLatLongToInt32(fragment));
					break;
				case 3:
					if (fragment[0] == 'S')
						latitude(latitude() * -1);
					break;
				case 4:
					longitude(parseNmeaLatLongToInt32(fragment));
					break;
				case 5:
					if (fragment[0] == 'E')
						longitude(longitude() * -1);
					break;
				case 6: // quality (0,1,2)
					_fixQuality = (uint8_t)atoi(fragment);
					break;
				case 7: // satellites in use
					satellitesInUse((uint8_t)atoi(fragment));
					break;
				case 8: // hdop
					if (fix() != GPS_FIX_NONE)
						hdop(parseFloat(fragment));
					break;
				case 9: // antenna height above sea level (meters)
					gpsAltitude(parseFloat(fragment));
					break;
				case 11: // geoidal separation (meters)
					_geoidalSeparation = parseFloat(fragment);
					break;
				default:
					break;
				}
				break;
			case NMEA_TYPE_GSV:
				if (number == 3)
					satellitesInView((uint8_t)atoi(fragment));
				break;
			case NMEA_TYPE_GSA:
				switch (number)
				{
				case 2:
					fix(GPS_FIX_NONE);
					if (fragment[0] == '2')
						fix(GPS_FIX_2D);
					if (fragment[0] == '3')
						fix(GPS_FIX_3D);
					break;
				case 15:
					if (fix() != GPS_FIX_NONE)
						pdop(parseFloat(fragment));
					break;
				case 16:
					if (fix() != GPS_FIX_NONE)
						hdop(parseFloat(fragment));
					break;
				case 17:
					if (fix() != GPS_FIX_NONE)
						vdop(parseFloat(fragment));
					break;
				default:
					break;
				}
				break;
			case NMEA_TYPE_VTG:
				switch (number)
				{
				case 1:
					_trueTrackMadeGood = parseFloat(fragment);
					break;
				case 5:
					groundspeed(parseFloat(fragment));
					break;
				default:
					break;
				}
				break;
			case NMEA_TYPE_RMC:
				switch (number)
				{
				case 1: // time: HHMMSS.000
					time(parseNmeaTime(fragment));
					break;
				case 2: // validity: A-ok
					if (fragment[0] != 'A')
							_valid = 0;
					break;
				case 3: // latitude: DDMM.SSSS
					latitude(parseNmeaLatLongToInt32(fragment));
					break;
				case 4: // N/S
					if (fragment[0] == 'S')
						latitude(latitude() * -1);
					break;
				case 5: // longitude: DDDMM.SSSS
					longitude(parseNmeaLatLongToInt32(fragment));
					break;
				case 6: // E/W
					if (fragment[0] == 'E')
						longitude(longitude() * -1);
					break;
				case 7: // speed (knots)
					airspeed(parseFloat(fragment));
					break;
				case 8: // heading (true)
					heading(parseFloat(fragment));
					break;
				case 9: // date: DDMMYY
					date(parseNmeaDate(fragment));
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
	}
	void processBuffer()
	{
		if (_buffer[00] != '$')
			return;

		char c = 0;
		uint8_t p = 0;

		uint8_t fragment[32];
		for (uint8_t i = 0; i < sizeof(fragment); i++)
			fragment[i] = 0;

		uint8_t f = 0;
		uint8_t fragmentNumber = 0;

		while (1)
		{
			c = _buffer[p++];
			if (c == 0)
			{
					// process fragment
					fragment[f++] = 0;
					processFragment(fragmentNumber, (char*)fragment, f - 1);
					break;
			}
			if (c == ',' || c == '*')
			{
				// process fragment
				fragment[f++] = 0;
				processFragment(fragmentNumber, (char*)fragment, f - 1);
				f = 0;
				fragmentNumber++;
				for (uint8_t i = 0; i < sizeof(fragment); i++)
					fragment[i] = 0;
			}
			else
			{
				fragment[f++] = c;
			}
		}
	}
public:
	UartGps(Uart *uart) : Gps()
	{
		_uart       = uart;
		_position   = 0;
		_fixQuality = 0;
		_valid      = 0;
		satellitesInUse(0);
		satellitesInView(0);
		pdop(0);
		hdop(0);
		vdop(0);
	}
	float    trueTrackMadeGood() const { return _trueTrackMadeGood; }
	uint8_t  fixQuality() const { return _fixQuality; }
	float    geoidalSeparation() const { return _geoidalSeparation; }

	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();


		char writeBuffer[64];

		// turn on antenna status messages...
		strcpy(writeBuffer, "$PGCMD,33,1*6C");
		_uart->write((uint8_t*)writeBuffer,  (uint16_t)strlen(writeBuffer));

		// enable sbas...
		strcpy(writeBuffer, "$PMTK313,1*2E");
		_uart->write((uint8_t*)writeBuffer,  (uint16_t)strlen(writeBuffer));

		// enable waas...
		strcpy(writeBuffer, "$PMTK301,2*2E");
		_uart->write((uint8_t*)writeBuffer,  (uint16_t)strlen(writeBuffer));

		// turn on all nmea messages...
		strcpy(writeBuffer, "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
		_uart->write((uint8_t*)writeBuffer,  (uint16_t)strlen(writeBuffer));



		return readyWithSet(className, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		while (_uart->available())
		{
			auto byteData = _uart->read();
			if (byteData.received != 0)
			{
				//debugWriteInfo(className, __LINE__, "rx");
				char c = (char)byteData.data;
				if (c == '$')
				{
					// new message...
					if (_position > 0)
						processBuffer();
					_position = 0;
				}
				_buffer[_position + 0] = c;
				_buffer[_position + 1] = '\0';
				_position++;
			}
		}
		return status(Status::Ready);
	}
};

#endif /* APP_INC_UARTGPS_HPP_ */
