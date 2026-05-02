/*
RESOURCES

https://github.com/NightHawk32/SmartAudio-testing/blob/master/tbs_test.ino
https://docs.google.com/document/d/1B97Ol2UQOWj8A2ZzzsRcIU3JlOwlX6hYAzeutjb6c1E/edit
https://www.akktek.com/akk-race-ranger-n-a-for-us-customer.html
https://github.com/betaflight/betaflight/blob/master/src/main/io/smartaudio_protocol.h
https://github.com/betaflight/betaflight/blob/master/src/main/io/smartaudio_protocol.c
https://github.com/Avamander/arduino-tvout
https://www.team-blacksheep.com/tbs_smartaudio_rev09.pdf
https://team-blacksheep.freshdesk.com/support/solutions/articles/4000100084-tbs-smartaudio-what-it-is-and-how-it-works
*/

#pragma once
#ifndef _SMARTAUDIO_H
#define _SMARTAUDIO_H

#include <HardwareSerial.h>
#include "crc.h"
#include "Arduino.h"

#define SMARTAUDIO_SERIAL_BAUD              4900
#define SMARTAUDIO_SERIAL_OPTIONS           SERIAL_8N2
#define SMARTAUDIO_REFRESH_MS               60000
#define SMARTAUDIO_RXTX_SWITCH_DELAY_MS     150
#define SMARTAUDIO_PACKET_MINIMUM_LENGTH    5
#define SMARTAUDIO_PACKET_MAXIMUM_LENGTH    32
#define SMARTAUDIO_PACKET_HEADER_BYTE0      0xAA
#define SMARTAUDIO_PACKET_HEADER_BYTE1      0x55
#define SMARTAUDIO_COMMAND_GET_SETTINGS_V10 0x01
#define SMARTAUDIO_COMMAND_GET_SETTINGS_V20 0x09
#define SMARTAUDIO_COMMAND_GET_SETTINGS_V21 0x10
#define SMARTAUDIO_COMMAND_SET_POWER        0x02
#define SMARTAUDIO_COMMAND_SET_CHANNEL      0x03
#define SMARTAUDIO_COMMAND_SET_FREQUENCY    0x04
#define SMARTAUDIO_COMMAND_SET_MODE         0x05


#define SMARTAUDIO_POWER_MAX                3

enum class SmartAudioPower : uint8_t
{
	Minimum = 0,
	Low = 1,
	Normal = 2,
	Maximum = 3
};

enum SMARTAUDIO_VERSION
{
	NONE,
	SA_V1,
	SA_V2
};

static const uint16_t V1_power_lookup[] = { 200, 400, 800, 1600 };

class SmartAudioVtx
{
	private:
	HardwareSerial*       _stream;
	SMARTAUDIO_VERSION    _version;
	uint8_t               _channel;
	uint8_t               _power;
	uint8_t               _mode;
	uint16_t              _frequency;
	uint8_t               _rxbuffer[SMARTAUDIO_PACKET_MAXIMUM_LENGTH];
	uint32_t              _nextRefreshDue = 0;

	void processRxPacket(const uint8_t *packet, const uint8_t packetLength)
	{
		//verify packet
		//dumpBuffer("RX", &Serial, packet, packetLength);
		//dumpPacket("RX", &Serial, packet, packetLength);

		if (packet[0] != SMARTAUDIO_PACKET_HEADER_BYTE0)
			return;
		if (packet[1] != SMARTAUDIO_PACKET_HEADER_BYTE1)
			return;
		
		uint8_t command         = packet[2];
		uint8_t payloadLength   = packet[3];
		uint8_t actualCRC       = crc8(&packet[2], payloadLength + 1);
		uint8_t packetStart     = 0;

		for (int index = 0; index < packetLength; index++)
		{
			if (packet[index]==SMARTAUDIO_PACKET_HEADER_BYTE0 && packet[index+1]==SMARTAUDIO_PACKET_HEADER_BYTE1 && packet[index+3]<packetLength)
			{
				packetStart=index+2;
				uint8_t crcCalc=crc8(&packet[index+2], payloadLength + 1);
				uint8_t crc=packet[index+3+payloadLength];

				if(crcCalc==crc)
				{
					//Serial.println("CRC PASS");
				}
				else
				{
					//Serial.println("CRC FAIL");
				}
				switch (packet[packetStart])
				{
					case SMARTAUDIO_COMMAND_GET_SETTINGS_V10: //fall-through
					case SMARTAUDIO_COMMAND_GET_SETTINGS_V20:
					case SMARTAUDIO_COMMAND_GET_SETTINGS_V21:
						//Serial.println("SMARTAUDIO_COMMAND_GET_SETTINGS");
						this->_version = (packet[packetStart]==SMARTAUDIO_COMMAND_GET_SETTINGS_V10) ? SA_V1 : SA_V2;
						packetStart+=2; //skip cmd and length
						this->_channel = packet[packetStart++];
						this->_power = packet[packetStart++];
						this->_mode = packet[packetStart++];
						this->_frequency = ((uint16_t)packet[packetStart++]<<8)|packet[packetStart++];
						break;
					case SMARTAUDIO_COMMAND_SET_POWER:
						//Serial.println("SMARTAUDIO_COMMAND_SET_POWER");
						packetStart+=2;
						this->_power = packet[packetStart++];
						break;
					case SMARTAUDIO_COMMAND_SET_CHANNEL:
						//Serial.println("SMARTAUDIO_COMMAND_SET_CHANNEL");
						packetStart+=2;
						this->_channel = packet[packetStart++];
						break;
					case SMARTAUDIO_COMMAND_SET_FREQUENCY:
						//Serial.println("SMARTAUDIO_COMMAND_SET_FREQUENCY");
						//TBD: Pit mode Freq
						packetStart+=2;
						this->_frequency = ((uint16_t)packet[packetStart++]<<8)|packet[packetStart++];
						break;
					case SMARTAUDIO_COMMAND_SET_MODE:
						//SA V2 only!
						break;
				}
			}
		}
	}

	void createAndSendPacket(const uint8_t command, const uint32_t value)
	{
		//here: length --> only payload, without CRC
		//here: CRC --> calculated for complete packet 0xAA ... payload
		uint8_t packet[SMARTAUDIO_PACKET_MAXIMUM_LENGTH];
		for (auto index = 0; index < sizeof(packet); index++)
			packet[index] = 0;

		uint8_t packetLength = 0;
		packet[0] = SMARTAUDIO_PACKET_HEADER_BYTE0; // sync
		packet[1] = SMARTAUDIO_PACKET_HEADER_BYTE1; // sync
		packet[2] = (command << 1) | 0x01;          // command

		switch (command)
		{
			case SMARTAUDIO_COMMAND_GET_SETTINGS_V10:
				packet[3] = 0x00; // length
				packet[4] = crc8(packet, 4);
				packetLength = 5;
				break;
			case SMARTAUDIO_COMMAND_SET_POWER:
				packet[3] = 0x01; // length
				packet[4] = (this->_version == SA_V1) ? V1_power_lookup[value] : value;
				packet[5] = crc8(packet, 5);
				packetLength = 6;
				break;
			case SMARTAUDIO_COMMAND_SET_CHANNEL:
				packet[3] = 0x01; // length
				packet[4] = value;
				packet[5] = crc8(packet, 5);
				packetLength = 6;
				break;
			case SMARTAUDIO_COMMAND_SET_FREQUENCY:
				packet[3] = 0x02;
				packet[4] = (value>>8); //high byte first
				packet[5] = value;
				packet[6] = crc8(packet, 6);
				packetLength = 7;
				break;
			case SMARTAUDIO_COMMAND_SET_MODE: //supported for V2 only: UNIFY HV and newer
				if (this->_version == SA_V2)
				{
					//TBD --> Pit mode
					/*
					buffer[3] = 0x01; // length
					buffer[4] = value;
					buffer[5] = crc8(&buffer[0], 5);
					packetLength = 6;
					*/
				}
				break;
		}
		//dumpBuffer("TX", &Serial, packet, packetLength);
		//dumpPacket("TX", &Serial, packet, packetLength);
		
		this->_stream->begin(SMARTAUDIO_SERIAL_BAUD, SMARTAUDIO_SERIAL_OPTIONS);
		delay(SMARTAUDIO_RXTX_SWITCH_DELAY_MS);
		for (int index = 0; index < packetLength; index++)
		{
			this->_stream->write(packet[index]);
		}
		this->_stream->end(); //clear buffer, otherwise sa_tx_packet is received
		this->_stream->begin(SMARTAUDIO_SERIAL_BAUD, SMARTAUDIO_SERIAL_OPTIONS);
		UCSR3B &= ~(1<<TXEN3); //deactivate tx --> rx mode listening for response
		delay(SMARTAUDIO_RXTX_SWITCH_DELAY_MS);
	}

	public:
	SmartAudioVtx(const HardwareSerial* stream)
	{
		this->_stream = (HardwareSerial*)stream;
		this->_version = SMARTAUDIO_VERSION::NONE;
		this->_channel = 0;
		this->_power = 0;
		this->_mode = 0;
		this->_frequency = 0;
	}

	uint8_t version()   const { return this->_version; }
	uint8_t channel()   const { return this->_channel; }
	SmartAudioPower power()     const { return (SmartAudioPower)this->_power; }
	uint8_t mode()      const { return this->_mode; }
	uint8_t frequency() const { return this->_frequency; }

	void channel(uint8_t value)
	{
		if (value >= 0 && value <= 8)
		this->createAndSendPacket(SMARTAUDIO_COMMAND_SET_CHANNEL, value);
	}

	void power(SmartAudioPower value)
	{
		if ((uint8_t)value >= 0 && (uint8_t)value <= 3)
		{
			this->createAndSendPacket(SMARTAUDIO_COMMAND_SET_POWER, (uint8_t)value);
			_power = (uint8_t)value;
		}
	}

	void refresh()
	{
		this->createAndSendPacket(SMARTAUDIO_COMMAND_GET_SETTINGS_V10, 0);
	}

	void setup()
	{
		// start in rx mode...
		this->_stream->begin(SMARTAUDIO_SERIAL_BAUD, SMARTAUDIO_SERIAL_OPTIONS);
		UCSR3B &= ~(1<<TXEN3);
	}

	uint8_t cycle()
	{
		uint8_t significantDelay = 0;
		
		if (millis() > _nextRefreshDue)
		{
			refresh();
			_nextRefreshDue = millis() + SMARTAUDIO_REFRESH_MS;
			significantDelay = 1;
		}


		for (auto index = 0; index < sizeof(_rxbuffer); index++)
			_rxbuffer[index] = 0;

		uint8_t packetLength = 0;
		while (this->_stream->available())
		{
			int dataAsInteger = this->_stream->read();
			if (dataAsInteger == -1)
			continue;
			significantDelay = 1;
			uint8_t dataAsByte = (uint8_t)dataAsInteger;
			
			switch (packetLength)
			{
				case 0:
					if (dataAsByte == SMARTAUDIO_PACKET_HEADER_BYTE0)
					{
						_rxbuffer[packetLength] = dataAsByte;
						packetLength++;
					}
					break;
				case 1:
					if (dataAsByte == SMARTAUDIO_PACKET_HEADER_BYTE1)
					{
						_rxbuffer[packetLength] = dataAsByte;
						packetLength++;
					}
					break;
				default:
					_rxbuffer[packetLength] = dataAsByte;
					packetLength++;
			}
		}
		if (packetLength > 0)
		{
			//Serial.print("RX ");
			//Serial.print(packetLength);
			//Serial.println(" bytes");
		}
		if (packetLength >= SMARTAUDIO_PACKET_MINIMUM_LENGTH) // min packet size
		{
			uint8_t indicatedPayloadLength = _rxbuffer[3];
			uint8_t expectedPacketSize     = indicatedPayloadLength + SMARTAUDIO_PACKET_MINIMUM_LENGTH;
			if (packetLength < expectedPacketSize)
			{
				//Serial.print("DATA MISSING: PAYLOAD LENGTH=");
				//Serial.print(indicatedPayloadLength);
				//Serial.print(", INDICATED PACKET LENGTH=");
				//Serial.print(expectedPacketSize);
				//Serial.print(", ACTUAL PACKET LENGTH=");
				//Serial.println(packetLength);
			}
			else
			{
				processRxPacket(_rxbuffer, expectedPacketSize);
				//Serial.print("Version:");
				//Serial.print(this->_version);
				//Serial.print(", Channel:");
				//Serial.print(this->_channel);
				//Serial.print(", Power:");
				//Serial.print(this->_power);
				//Serial.print(", Mode:");
				//Serial.print(this->_mode);
				//Serial.print(", Frequency:");
				//.println(this->_frequency);
			}
		}
		return significantDelay;
	}
};

#endif // _SMARTAUDIO_H
