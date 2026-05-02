/*
 * rcrx-sbus.h
 *
 *  Created on: Oct 6, 2023
 *      Author: Daniel Johnson
 */
#ifndef RCRX_SBUS_H_
#define RCRX_SBUS_H_

#include <cstdint>
#include <map>
#include "uart.hpp"
#include "component.hpp"
#include "rcrx.hpp"
#include "debug.hpp"
#include "main.h"
#include "input.hpp"

extern "C" void sprintf_binary16(char* s, uint16_t data);

#define SBUS_SERIAL_BAUD        100000
#define SBUS_PACKET_LENGTH      25
#define SBUS_PACKET_HEADER      0x0F
#define SBUS_PACKET_FOOTER      0x00
#define SBUS_CHANNELS           18

/*
 * http://www.ernstc.dk/arduino/sbus.html
 * https://github.com/mikeshub/FUTABA_SBUS/blob/master/FUTABA_SBUS/FUTABA_SBUS.cpp
 * https://quadmeup.com/simplest-hardware-inverter-for-flight-controller/
 * https://quadmeup.com/generate-s-bus-with-arduino-in-a-simple-way
 * https://www.codetd.com/en/article/13078381
 *
 * 100000 Baud rate (this is a non standard baud rate!)
 * 8E2 configuration, i.e.:
 * 1 start bit
 * 8 Data bits
 * 1 Even parity bit
 * 2 Stop bits
 *
 * STM32H7 Configuration:
 * Baud rate:                       100000
 * Word length:                     9 bits (including parity)
 * Parity:                          Even
 * Stop bits:                       1       <-- Definitely 1 not 2! Not a mistake
 * Auto baudrate:                   Disable
 * TX Pin Active Level Inversion:   Enable
 * RX Pin Active Level Inversion:   Enable
 * Data Inversion:                  Disable
 *
 *
 * 00 0x0F header
 * 01
 * 02
 * 03
 * 04
 * 05
 * 06
 * 07
 * 08
 * 09
 * 10
 * 11
 * 12
 * 13
 * 14
 * 15
 * 16
 * 17
 * 18
 * 19
 * 20
 * 21
 * 22
 * 23 flag byte: [0 0 0 0 fail-safe frame_lost ch18 ch17]
 * 24 0x00 footer
*/
class RcRxSbus : public RcRx
{
private:
	const char* className = "rcrxsbus";
	Uart*    _uart;
	uint8_t  _rssiChannel;
	uint8_t  _buffer[SBUS_PACKET_LENGTH];
	uint8_t  _flags;
	uint8_t  _position;
	uint32_t _packets;
	uint32_t _lastLocal;
	uint32_t _lastRemote;

	bool append(uint8_t b)
	{
		//debugWriteInfo(className, __LINE__, "append");


		// wait for packet header to start things off...
		if ((_position == 0) && (b != SBUS_PACKET_HEADER))
			return false;

		// append supplied data...
		_buffer[_position++] = b;

		// wait for packet end...
		if (_position < SBUS_PACKET_LENGTH)
			return false;

		// we have a full buffer so we can reset the state machine...
		_position  = 0;
		_buffer[0] = ~SBUS_PACKET_HEADER;

		// ensure the packet footer is correct...
		if (_buffer[SBUS_PACKET_LENGTH - 1] != SBUS_PACKET_FOOTER)
			return false;

		// we now seem to have a valid packet...
		_lastLocal = HAL_GetTick();
		_packets++;

		// decode the packet...
		uint16_t value00 = ((_buffer[1]|_buffer[2]<< 8) & 0x07FF);						// roll
		uint16_t value01 = ((_buffer[2]>>3|_buffer[3]<<5) & 0x07FF);					// pitch
		uint16_t value02 = ((_buffer[3]>>6|_buffer[4]<<2|_buffer[5]<<10) & 0x07FF);		// throttle
		uint16_t value03 = ((_buffer[5]>>1|_buffer[6]<<7) & 0x07FF);					// yaw
		uint16_t value04 = ((_buffer[6]>>4|_buffer[7]<<4) & 0x07FF);
		uint16_t value05 = ((_buffer[7]>>7|_buffer[8]<<1|_buffer[9]<<9) & 0x07FF);
		uint16_t value06 = ((_buffer[9]>>2|_buffer[10]<<6) & 0x07FF);
		uint16_t value07 = ((_buffer[10]>>5|_buffer[11]<<3) & 0x07FF);
		uint16_t value08 = ((_buffer[12]|_buffer[13]<< 8) & 0x07FF);
		uint16_t value09 = ((_buffer[13]>>3|_buffer[14]<<5) & 0x07FF);
		uint16_t value10 = ((_buffer[14]>>6|_buffer[15]<<2|_buffer[16]<<10) & 0x07FF);
		uint16_t value11 = ((_buffer[16]>>1|_buffer[17]<<7) & 0x07FF);
		uint16_t value12 = ((_buffer[17]>>4|_buffer[18]<<4) & 0x07FF);
		uint16_t value13 = ((_buffer[18]>>7|_buffer[19]<<1|_buffer[20]<<9) & 0x07FF);
		uint16_t value14 = ((_buffer[20]>>2|_buffer[21]<<6) & 0x07FF);
		uint16_t value15 = ((_buffer[21]>>5|_buffer[22]<<3) & 0x07FF);

		rawValue( 0, value00); // roll
		rawValue( 1, value01); // pitch
		rawValue( 2, value02); // throttle
		rawValue( 3, value03); // yaw
		rawValue( 4, value04);
		rawValue( 5, value05);
		rawValue( 6, value06);
		rawValue( 7, value07);
		rawValue( 8, value08);
		rawValue( 9, value09);
		rawValue(10, value10);
		rawValue(11, value11);
		rawValue(12, value12);
		rawValue(13, value13);
		rawValue(14, value14);
		rawValue(15, value15);

//		char s[200];
//		sprintf(s, "                       %04u", value00);
//		debugWriteInfo(className, __LINE__, s);
//		sprintf(s, "                       %04u", rawValue(0));
//		debugWriteInfo(className, __LINE__, s);

		// 23 flag byte: [0 0 0 0 fail-safe frame_lost ch18 ch17]
		_flags = _buffer[23] & 0b00001111;

		// packet represents remote control...
		if ((_flags & 0b1100) == 0)
			_lastRemote = _lastLocal;
		// TODO: set link() here based upon flags...

		// process the two digital channels...
		if (_flags & 0b0001) { rawValue(16, RCRX_MAXIMUM_VALUE); } else { rawValue(16, RCRX_MINIMUM_VALUE); }
		if (_flags & 0b0010) { rawValue(17, RCRX_MAXIMUM_VALUE); } else { rawValue(17, RCRX_MINIMUM_VALUE); }





		// set rssi...
		if (IsRemoteLost())
		{
			rssi(RCRX_RSSI_LINK_LOST);
			link(false);
			reset();
		}
		else
		{
			if (_rssiChannel < SBUS_CHANNELS)
				rssi(
						(uint8_t)
						(
								100.0F * (float)(channel(_rssiChannel) - 1000.0F) / 1000.0F
						)
				);
			else
				rssi(50);
			link(true);
		}
		return true;
	}
public:
	RcRxSbus(Uart* uart, InputDatum* config, InputSource source, uint8_t rssiChannel) : RcRx(config, source)
	{
		_uart        = uart;
		_rssiChannel = rssiChannel;
		_flags       = 0;
		_position    = 0;
		_packets     = 0;
		_lastLocal   = 0;
		_lastRemote  = 0;
		rssi(RCRX_RSSI_LINK_LOST);
	}
	virtual ~RcRxSbus()
	{
	}
	bool IsLocalOk()     const { return (_packets > 0) && (LocalAge() < 100); }
	bool IsRemoteOk()    const { return !(_flags & 0b1100);                   }
	bool IsRemoteLost()  const { return _flags & 0b0100;                      }
	bool IsFailsafe()    const { return _flags & 0b1000;                      }
	uint32_t LocalAge()  const { return HAL_GetTick() - _lastLocal;           }
	uint32_t RemoteAge() const { return HAL_GetTick() - _lastRemote;          }

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
		while (_uart->available())
		{
			auto byteData = _uart->read();
			if (byteData.received != 0)
				append(byteData.data);
		}
		return status(Status::Ready);
	}
};

#endif /* RCRX_SBUS_H_ */
