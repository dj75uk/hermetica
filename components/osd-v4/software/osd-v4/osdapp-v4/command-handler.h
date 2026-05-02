/*
 * command-handler.h
 *
 * Created: 04/05/2023 01:22:58
 *  Author: Daniel.Johnson
 */ 
#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

#include <Arduino.h>
//#include "config.h"
#include "packet.h"
#include "osdmodel.h"
#include "smartaudio.h"
#include "hardware-macros.h"

extern uint8_t limit_u8(uint8_t value, uint8_t low, uint8_t high);

class CommandHandler
{
	private:
		HardwareSerial*     _usart;
		uint32_t			_usartBaud;
		uint8_t				_usartOptions;
		OsdModel*			_model;
		SmartAudioVtx*		_vtx;
		uint8_t				_position;
		uint32_t			_lastDataRx;
		VtxCommandPacket	_builder;
		void ack();
		void nack();
		void apply(VtxCommandPacket* packet);
	public:
		CommandHandler(const HardwareSerial* stream, const uint32_t baud, const uint8_t options, const OsdModel* model, const SmartAudioVtx* vtx);
		void setup();
		uint8_t cycle();	
};

CommandHandler::CommandHandler(const HardwareSerial* stream, const uint32_t baud, const uint8_t options, const OsdModel* model, const SmartAudioVtx* vtx)
{
	_usart = (HardwareSerial*)stream;
	_usartBaud = baud;
	_usartOptions = options;
	_model = (OsdModel*)model;
	_vtx = (SmartAudioVtx*)vtx;
	_position = 0;
	_lastDataRx = 0;
}

void CommandHandler::ack() { _usart->write((uint8_t)VtxProtocol::Ack); }
void CommandHandler::nack() { _usart->write((uint8_t)VtxProtocol::Nack); }

void CommandHandler::setup()
{
	_usart->begin(_usartBaud, _usartOptions);
}

uint8_t CommandHandler::cycle()
{
	uint8_t significantDelay = 0;
	uint32_t entry = millis();
	
	if ((_lastDataRx == 0) || (entry > (_lastDataRx + 2000)))
	{
		nack();
		_lastDataRx = entry;
		significantDelay = 1;
	}
	while (_usart->available())
	{
		LED_CMD_ON
		_lastDataRx = entry;
		char szd[200];
		int dataAsInt = _usart->read();
		if ((dataAsInt < 0) || (dataAsInt > 255))
		{
			break;
		}

		significantDelay = 1;
		uint8_t data = (uint8_t)dataAsInt;

		if (_position == 0)
		{
			// wait for packet start...
			if (data != (uint8_t)VtxProtocol::Start)
			{
				_usart->flush();
				_model->synchronised = 0;
				nack();
				return significantDelay;
			}			
	
			// we have a new command...
			clearPacket(&_builder);
			_builder.start = data;

			// wait for the rest of the packet...
			_position++;
		}
		else
		{
			switch (_position)
			{
				case 1:
					_builder.command = data;
					break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					_builder.data.uint8s[_position - 2] = data;
					break;
				case 14:
					_builder.checksum = data;
					break;
				case 15:
					_builder.end = data;
					break;
			}
				
			_position++;
				
			if (_position >= COMMAND_PACKET_SIZE)
			{		
				// we now have a potential packet...
				if (_builder.end == (uint8_t)VtxProtocol::End)
				{
					// validate packet...
					if (calculateCrc(&_builder) == _builder.checksum)
					{
						_model->synchronised = 1;
						
						// process packet...
						_model->packets++;
						apply(&_builder);
						
						// we shouldn't have anything else incoming...
						_usart->flush();
						
						// send ACK...
						ack();
					}
					else
					{
						// packet crc fault...
						_model->packetCrcFaults++;
						_model->synchronised = 0;
						
						// we shouldn't have anything else incoming...
						_usart->flush();
						
						// send NACK...
						nack();
					}
				}
				else
				{
					// invalid packet...
					_model->packetOverruns++;
					_model->synchronised = 0;
					// we shouldn't have anything else incoming...
					_usart->flush();
					nack();
				}
				
				// reset...
				_position = 0;
			}
		}
	}
	return significantDelay;
}

void CommandHandler::apply(VtxCommandPacket* packet)
{
	if (packet->command == (uint8_t)VtxProtocol::Sync)
		return;
	if (packet->command == (uint8_t)VtxProtocol::Skip)
		return;
	
	switch ((VtxCommand)packet->command)
	{
		case VtxCommand::Mode:
			_model->fcuMode = (FcuMode)packet->data.uint8s[0];
			_model->vtxMode = (VtxMode)packet->data.uint8s[1];
			break;
		case VtxCommand::VtxFrequency:
			break;
		case VtxCommand::VtxChannel:
			break;
		case VtxCommand::VtxPower:
			_vtx->power((SmartAudioPower)limit_u8(packet->data.uint8s[0], 0, 3));
			break;
		case VtxCommand::Attitude:
			_model->attitude.pitch = packet->data.floats[0];
			_model->attitude.roll  = packet->data.floats[1];
			_model->attitude.yaw   = packet->data.floats[2];
			break;
		case VtxCommand::AttitudeRates:
			_model->attitude.pitchRate = packet->data.floats[0];
			_model->attitude.rollRate  = packet->data.floats[1];
			_model->attitude.yawRate   = packet->data.floats[2];
			break;
		case VtxCommand::Heading:
			_model->heading.magnetic = packet->data.floats[0];
			_model->heading.gps      = packet->data.floats[1];
			_model->heading.imu      = packet->data.floats[2];
			break;
		case VtxCommand::Altitude:
			_model->altitude.barometric = packet->data.floats[0];
			_model->altitude.gps        = packet->data.floats[1];
			_model->altitude.ranged     = packet->data.floats[2];
			break;
		case VtxCommand::Vsi:
			_model->vsi.barometric = packet->data.floats[0];
			_model->vsi.gps        = packet->data.floats[1];
			_model->vsi.ranged     = packet->data.floats[2];
			break;
		case VtxCommand::Airspeed:
			_model->airspeed.pitot  = packet->data.floats[0];
			_model->airspeed.gps    = packet->data.floats[1];
			_model->airspeed.imu    = packet->data.floats[2];
			break;
		case VtxCommand::Fpi:
			_model->fpi.pitch = packet->data.floats[0];
			_model->fpi.yaw   = packet->data.floats[2];
			break;

		case VtxCommand::RcRx:
			_model->rssi0               = packet->data.uint8s[0];
			_model->rssi1               = packet->data.uint8s[1];
			_model->rssi2               = packet->data.uint8s[2];
			_model->rssi3               = packet->data.uint8s[3];
			break;
			
		// 32.32.32.32 32.32.32.32 32.32.32.32
		// 16.16 16.16 16.16 16.16 16.16 16.16
		// 08 08 08 08 08 08 08 08 08 08 08 08
		case VtxCommand::Gps:
			_model->location.latitude   = packet->data.int32s[0];
			_model->location.longitude  = packet->data.int32s[1];
			_model->gpsFix              = packet->data.uint8s[8];
			_model->gpsSatellitesInUse  = packet->data.uint8s[9];
			_model->gpsSatellitesInView = packet->data.uint8s[10];
			break;
			
		// 32.32.32.32 32.32.32.32 32.32.32.32
		// 16.16 16.16 16.16 16.16 16.16 16.16
		case VtxCommand::Home:
			_model->home.latitude  = packet->data.int32s[0];
			_model->home.longitude = packet->data.int32s[1];
			_model->home.bearing   = packet->data.uint16s[4];
			_model->home.range     = packet->data.uint16s[5];
			break;
		
		// 32.32.32.32 32.32.32.32 32.32.32.32
		// 16.16 16.16 16.16 16.16 16.16 16.16
		case VtxCommand::Waypoint:
			_model->waypoint.latitude  = packet->data.int32s[0];
			_model->waypoint.longitude = packet->data.int32s[1];
			_model->waypoint.bearing   = packet->data.uint16s[4];
			_model->waypoint.range     = packet->data.uint16s[5];
			break;
			
		case VtxCommand::EngineThrottle:
			_model->engine[0].throttlePercentage = packet->data.uint8s[0];
			_model->engine[1].throttlePercentage = packet->data.uint8s[1];
			break;
		default:
			// unhandled command
			break;
	}
}

#endif /* COMMAND_HANDLER_H_ */