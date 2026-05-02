/*
 * vtxclient.hpp
 *
 *  Created on: Oct 21, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_DRIVERS_VTXCLIENT_HPP_
#define APP_INC_DRIVERS_VTXCLIENT_HPP_

#include <cstdio>
#include <cstdint>
#include <string>
#include "uart.hpp"
#include "osdmodel-v13.hpp"
#include "alarm.hpp"
#include "board.hpp"
#include "ringbuffer.hpp"
#include "vtxcommand.hpp"
#include "fcumode.hpp"

class VtxClient : public HasStatusAndSetupAndCycle
{
private:
	const char* className = "vtxclient";
	Uart* _uart;
	Board* _board;
	uint8_t _cycleStage = 0;
	FcuMode _fcuMode;
	VtxMode _vtxMode;
	VtxCommandPacket cmd;
	jnk0le::Ringbuffer<uint8_t, 32> _txFifo;
	bool _waitingForAck = false;

	void writePacketBytes(VtxCommandPacket* packet)
	{
		if (_waitingForAck)
			return;
		if (_txFifo.readAvailable())
			return;
		uint8_t txBuffer[VTX_COMMAND_PACKET_SIZE];
		txBuffer[0]  = packet->start;
		txBuffer[1]  = packet->command;
		memcpy(txBuffer + 2, packet->data.uint8s, 12);
		txBuffer[14] = packet->checksum;
		txBuffer[15] = packet->end;

		for (uint8_t i = 0; i < VTX_COMMAND_PACKET_SIZE; i++)
			_txFifo.insert(txBuffer[i]);
	}
	uint8_t crcQ(const uint8_t *data, const uint8_t len)
	{
	  uint8_t crc = 0;
	  uint8_t i   = 0;
	  uint8_t j   = len;
	  //
	  while (j--)
	  {
	    crc ^= *data++;
	    for (i = 0; i < 8; i++)
	    {
	      if ((crc & 0x80) != 0)
	      {
	        crc = (uint8_t)((crc << 1) ^ 0x07);
	      }
	      else
	      {
	        crc <<= 1;
	      }
	    }
	  }
	  return crc;
	}
	uint8_t calculateCrc(VtxCommandPacket* command)
	{
		uint8_t data[16];
		data[0]  = command->command;
		data[1]  = command->data.uint8s[0];
		data[2]  = command->data.uint8s[1];
		data[3]  = command->data.uint8s[2];
		data[4]  = command->data.uint8s[3];
		data[5]  = command->data.uint8s[4];
		data[6]  = command->data.uint8s[5];
		data[7]  = command->data.uint8s[6];
		data[8]  = command->data.uint8s[7];
		data[9]  = command->data.uint8s[8];
		data[10] = command->data.uint8s[9];
		data[11] = command->data.uint8s[10];
		data[12] = command->data.uint8s[11];
		return crcQ(data, 13);
	}
	void setCrc(VtxCommandPacket* command)
	{
		command->start    = (uint8_t)VtxProtocol::Start;
		command->end      = (uint8_t)VtxProtocol::End;
		command->checksum = calculateCrc(command);
	}
	void clearPacket(VtxCommandPacket* command)
	{
		command->start = 0;
		command->command = 0;
		command->checksum = 0;
		command->end = 0;
		clearPayload(command);
	}
	void clearPayload(VtxCommandPacket* command)
	{
		for (uint8_t i = 0; i < 12; i++)
			command->data.uint8s[i] = 0;
	}
	VtxCommandPacket* fillMode(VtxCommandPacket* target, const FcuMode fcuMode, const VtxMode vtxMode)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::Mode;
		clearPayload(target);
		target->data.uint8s[0] = (uint8_t)fcuMode;
		target->data.uint8s[1] = (uint8_t)vtxMode;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillAttitude(VtxCommandPacket* target, const float pitch, const float roll, const float yaw)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::Attitude;;
		target->data.floats[0] = pitch;
		target->data.floats[1] = roll;
		target->data.floats[2] = yaw;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillAttitudeRates(VtxCommandPacket* target, const float pitchRate, const float rollRate, const float yawRate)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::AttitudeRates;
		target->data.floats[0] = pitchRate;
		target->data.floats[1] = rollRate;
		target->data.floats[2] = yawRate;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillAltitude(VtxCommandPacket* target, const float barometric, const float gps, const float ranged)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::Altitude;
		target->data.floats[0] = barometric;
		target->data.floats[1] = gps;
		target->data.floats[2] = ranged;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillHeading(VtxCommandPacket* target, const float magnetic, const float gps, const float imu)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::Heading;
		target->data.floats[0] = magnetic;
		target->data.floats[1] = gps;
		target->data.floats[2] = imu;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillRcRx(VtxCommandPacket* target, const uint8_t rssi0, const uint8_t rssi1, const uint8_t rssi2, const uint8_t rssi3)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::RcRx;
		target->data.uint8s[0] = rssi0;
		target->data.uint8s[1] = rssi1;
		target->data.uint8s[2] = rssi2;
		target->data.uint8s[3] = rssi3;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillEngineThrottle(VtxCommandPacket* target, const uint8_t throttlePercentageR, const uint8_t throttlePercentageL)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::EngineThrottle;
		target->data.uint8s[0] = throttlePercentageR;
		target->data.uint8s[1] = throttlePercentageL;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillFpi(VtxCommandPacket* target, const float pitch, const float roll, const float yaw)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::Fpi;
		target->data.floats[0] = pitch;
		target->data.floats[1] = roll;
		target->data.floats[2] = yaw;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillVsi(VtxCommandPacket* target, const float barometric, const float gps, const float ranged)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::Vsi;
		target->data.floats[0] = barometric;
		target->data.floats[1] = gps;
		target->data.floats[2] = ranged;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillGps(VtxCommandPacket* target, const int32_t longitude, const int32_t latitude, const uint8_t gpsFix, const uint8_t gpsSatsInUse, const uint8_t gpsSatsInView)
	{
		clearPacket(target);
		target->start          = (uint8_t)VtxProtocol::Start;
		target->end            = (uint8_t)VtxProtocol::End;
		target->command        = (uint8_t)VtxCommand::Gps;
		target->data.int32s[0] = longitude;
		target->data.int32s[1] = latitude;
		target->data.uint8s[8]  = gpsFix;
		target->data.uint8s[9]  = gpsSatsInUse;
		target->data.uint8s[10] = gpsSatsInView;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillHome(VtxCommandPacket* target, const int32_t longitude, const int32_t latitude, const uint16_t bearing, const uint16_t range)
	{
		clearPacket(target);
		target->start           = (uint8_t)VtxProtocol::Start;
		target->end             = (uint8_t)VtxProtocol::End;
		target->command         = (uint8_t)VtxCommand::Home;
		target->data.int32s[0]  = longitude;
		target->data.int32s[1]  = latitude;
		target->data.uint16s[4] = bearing;
		target->data.uint16s[5] = range;
		setCrc(target);
		return target;
	}
	VtxCommandPacket* fillWaypoint(VtxCommandPacket* target, const int32_t longitude, const int32_t latitude, const uint16_t bearing, const uint16_t range)
	{
		clearPacket(target);
		target->start           = (uint8_t)VtxProtocol::Start;
		target->end             = (uint8_t)VtxProtocol::End;
		target->command         = (uint8_t)VtxCommand::Waypoint;
		target->data.int32s[0]  = longitude;
		target->data.int32s[1]  = latitude;
		target->data.uint16s[4] = bearing;
		target->data.uint16s[5] = range;
		setCrc(target);
		return target;
	}
public:
	VtxClient(Uart* uart, Board* board)
	{
		_uart       = uart;
		_board      = board;
		_cycleStage = 0;
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		_fcuMode = FcuMode::Startup;
		_vtxMode = VtxMode::Decluttered;
		_waitingForAck = false;

		return status(Status::Ready);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		if (_waitingForAck)
		{
			while (_uart->available())
			{
				auto byteData = _uart->read();
				if (byteData.received != 0)
				{
					switch (byteData.data)
					{
					case (uint8_t)VtxProtocol::Ack:
					case (uint8_t)VtxProtocol::Nack:
						_waitingForAck = false;
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			if (_txFifo.readAvailable())
			{
				uint8_t txData;
				if (_txFifo.remove(&txData))
				{
					if (HAL_UART_Transmit(_uart->instance(), &txData, 1, 100) == HAL_OK)
					{
						if (!_txFifo.readAvailable())
							_waitingForAck = true;
					}
				}
			}
			else
			{
				// send something else...
				VtxCommandPacket* packet = &cmd;
				switch (_cycleStage++)
				{
				case 0:
					writePacketBytes(fillMode(packet, _fcuMode, _vtxMode));
					break;
				case 1:
					writePacketBytes(fillAttitude(packet, _board->pitch(), _board->roll(), 0));
					break;
				case 2:
					writePacketBytes(fillAttitudeRates(packet, _board->pitchRate(), _board->rollRate(), _board->yawRate()));
					break;
				case 3:
					writePacketBytes(fillAltitude(packet, _board->barometricAltitude(), _board->gpsAltitude(), _board->rangedAltitude()));
					break;
				case 4:
					writePacketBytes(fillHeading(packet, _board->heading(), _board->gps()->heading(), _board->heading()));
					break;
				case 5:
					writePacketBytes(fillVsi(packet, _board->barometricVsi(), 0, 0));
					break;
				case 6:
					writePacketBytes(fillFpi(packet, _board->fpiPitch(), 0, _board->fpiYaw()));
					break;
				case 7:
					writePacketBytes(fillRcRx(
						packet,
						_board->rcrxA()->rssi(),
						_board->rcrxB()->rssi(),
						0,
						0
					));
					break;
				case 8:
					writePacketBytes(fillEngineThrottle(packet, _board->throttlePercentageR(), _board->throttlePercentageL()));
					break;
				case 9:
					writePacketBytes(fillGps(
						packet,
						_board->gps()->latitude(),
						_board->gps()->longitude(),
						_board->gps()->fix(),
						_board->gps()->satellitesInUse(),
						_board->gps()->satellitesInView()
					));
					break;
				case 10:
					writePacketBytes(fillHome(
						packet,
						0,
						0,
						0,
						0
					));
					break;
				default:
					_cycleStage = 0;
					break;
				}
			}
		}
		return status(Status::Ready);
	}
	FcuMode mode() const
	{
		return _fcuMode;
	}
	VtxMode submode() const
	{
		return _vtxMode;
	}
	void mode(const FcuMode value)
	{
		_fcuMode = value;
	}
	void submode(const VtxMode value)
	{
		_vtxMode = value;
	}
};

#endif /* APP_INC_DRIVERS_VTXCLIENT_HPP_ */
