/*
 * rcrx-dsmx.h
 *
 *  Created on: Oct 4, 2023
 *      Author: Daniel Johnson
 */

#ifndef RCRX_DSMX_H_
#define RCRX_DSMX_H_

#include <cstdio>
#include <cstdint>
#include <string>
#include <map>
#include <debug.hpp>
#include "macros.hpp"
#include "main.h"
#include "uart.hpp"
#include "component.hpp"
#include "rcrx.hpp"
#include "debug.hpp"
#include "alarm.hpp"

#define DSMX_SERIAL_BAUD_RATE           125000
#define DSMX_PACKET_SIZE_IN_BYTES       16
#define DSMX_CHANNELS                   6
#define DSMX_MINIMUM_PACKET_DELAY_MS    19
#define DSMX_MAXIMUM_PACKET_DELAY_MS    25
#define DSMX_DEFAULT_RSSI               50
#define DSMX_MINIMUM_VALUE              410
#define DSMX_MAXIMUM_VALUE              1642

class RcRxDsmx : public RcRx
{
private:
	const char* className = "rcrxdsmx";
	Uart*    _uart;
    uint32_t _lastPacketEndReceived = 0;
    uint32_t _lastInterPacketDelay  = 0;
    uint32_t _previousByteReceived  = 0;
    uint8_t  _rxBuffer[DSMX_PACKET_SIZE_IN_BYTES + 2];
    uint8_t  _position = 0;

    void processPacket(const uint32_t interPacketTime)
    {
    	if ((interPacketTime >= DSMX_MINIMUM_PACKET_DELAY_MS) && (interPacketTime <= DSMX_MAXIMUM_PACKET_DELAY_MS))
    	{
    		// process as 22ms packet...
    		process22msPacket();
    	}
    }
	void process22msPacket()
	{
		debugWriteTrace(className, __LINE__, "22ms");
		uint8_t i = 2;
		while (i < DSMX_PACKET_SIZE_IN_BYTES)
		{
			uint8_t channel = (_rxBuffer[i] & 0b01111000) >> 3; // TODO: use constants
			if (channel < 16)
			{
				// some channels are weirdly duplicated...
				if (channel == 8)
					channel = 4;
				if (channel == 9)
					channel = 5;
				if (channel < DSMX_CHANNELS)
				{
					uint16_t value = ((_rxBuffer[i] << 8) + _rxBuffer[i + 1]) & 0b0000011111111111;
					rawValue(channel, value);
					link(true);
				}
			}
			i = i + 2;
		}
	}
public:
	RcRxDsmx(Uart* uart, InputDatum* config, InputSource source) : RcRx(config, source)
	{
		_uart  = uart;
		_position = 0;
	}
	virtual ~RcRxDsmx()
	{
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();
		return readyWithSet(className, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();

		uint32_t delta = HAL_GetTick() - _lastPacketEndReceived;
		if ((delta > DSMX_MINIMUM_PACKET_DELAY_MS) && (delta < DSMX_MAXIMUM_PACKET_DELAY_MS))
		{
			rssi(DSMX_DEFAULT_RSSI);
		}
		if (delta > RCRX_FAILSAFE_RESET_DELAY_MS)
		{
			reset();
			link(false);
			rssi(0);
		}
		else
		{
			rssi(50);
		}
		while (_uart->available())
		{
			auto byteData = _uart->read();
			if (byteData.received == 0)
				break;

			delta = byteData.received - _previousByteReceived;
			if (delta > 8)
			{
				// probably a new packet...
				_position = 0;
			}
			_rxBuffer[_position++] = byteData.data;
			if (_position >= DSMX_PACKET_SIZE_IN_BYTES)
			{
				// process packet...
				processPacket(byteData.received - _lastPacketEndReceived);

				// reset builder...
				_position = 0;
				for (uint8_t i = 0; i < sizeof(_rxBuffer); i++)
					_rxBuffer[i] = 0;

				_lastPacketEndReceived = byteData.received;
				_lastInterPacketDelay  = delta;
			}
			_previousByteReceived = byteData.received;
		}
	    return status(Status::Ready);
	}
};

#endif /* RCRX_DSMX_H_ */
