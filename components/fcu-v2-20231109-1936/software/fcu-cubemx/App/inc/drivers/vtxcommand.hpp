/*
 * vtxcommand.hpp
 *
 *  Created on: Oct 26, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_DRIVERS_VTXCOMMAND_HPP_
#define APP_INC_DRIVERS_VTXCOMMAND_HPP_

#include <cstdint>

enum class VtxProtocol : uint8_t
{
	Sync 	= 0x00,
	Start 	= 0x01,
	End 	= 0x04,
	Ack 	= 0x06,
	Nack 	= 0x21,
	Skip 	= 0xFF
};

enum class VtxCommand : uint8_t
{
	Mode					= 0x01,		// 2 uint8s (fcumode, vtxmode)
	VtxFrequency			= 0x02,
	VtxChannel				= 0x03,
	VtxPower				= 0x04,		// 1 uint8   (power [0..3])


	Attitude				= 0x10,		// 3 floats  (imu pitch, imu roll, imu heading)
	AttitudeRates			= 0x11,		// 3 floats  (pitch rate, roll rate, yaw rate)
	Heading					= 0x12,		// 3 floats  (magnetic heading, gps heading, imu heading)
	Altitude				= 0x13,		// 3 floats  (barometric altitude, gps altitude, ranged altitude)
	Vsi						= 0x14,		// 3 floats  (barometric vsi, gps vsi, ranged vsi)
	Airspeed				= 0x15,		// 3 floats  (pitot airspeed, gps airspeed, imu airspeed)
	Fpi						= 0x16,		// 3 floats  (pitch, roll, yaw)

	RcRx					= 0x20,		// 8 uint8s  (rssi0, rssi1, rssi2, rssi3, override?)
	Gps						= 0x21,		// 2 uint32s, 3 uint8s (latitude, longitude, fix, satellites in use, satellites in view)
	Home					= 0x22,		// 2 uint32s, 2 uint16s (latitude, longitude, bearing, range)
	Waypoint				= 0x23,		// 2 uint32s, 2 uint16s (latitude, longitude, bearing, range)

	EngineThrottle			= 0x30,		// 6 uint8s  (engine0, engine1, engine2, engine3, engine4, engine5)
	EngineRpm				= 0x31,		// 6 uint16s (engine0, engine1, engine2, engine3, engine4, engine5)
	EngineTemperature		= 0x32,		// 6 int16s  (engine0, engine1, engine2, engine3, engine4, engine5)
	EngineIntakeTemperature	= 0x33,		// 6 int16s  (engine0, engine1, engine2, engine3, engine4, engine5)
	EngineEscTemperature	= 0x34,		// 6 int16s  (engine0, engine1, engine2, engine3, engine4, engine5)

	BusVoltage				= 0x40,
	BusCurrent				= 0x41
};

typedef union vtxCommandPayload
{
	uint8_t  uint8s[12];
	uint16_t uint16s[6];
	int16_t  int16s[6];
	uint32_t uint32s[3];
	int32_t  int32s[3];
	float    floats[3];
} VtxCommandPayload;

typedef struct vtxCommandPacket
{
	uint8_t        		start;		// 00
	uint8_t        		command;	// 01
	VtxCommandPayload	data;		// 02..13
	uint8_t        		checksum;	// 14
	uint8_t        		end;		// 15
} VtxCommandPacket;


#define VTX_COMMAND_PACKET_SIZE     16

enum class VtxMode : uint8_t
{
	Diagnostics				= 0,
	Navigation				= 1,
	Decluttered				= 2,
	WaitingForArmTimer5		= 3,
	WaitingForArmTimer4		= 4,
	WaitingForArmTimer3		= 5,
	WaitingForArmTimer2		= 6,
	WaitingForArmTimer1		= 7
};

#endif /* APP_INC_DRIVERS_VTXCOMMAND_HPP_ */
