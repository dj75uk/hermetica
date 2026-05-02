/*
 * osdmodel-v13.hpp
 *
 *  Created on: Oct 21, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_DRIVERS_OSDMODEL_V13_HPP_
#define APP_INC_DRIVERS_OSDMODEL_V13_HPP_

#include <cstdio>
#include <cstdint>
#include <string>

#define OSD_MODEL_VERSION				13
#define OSD_MODEL_ENGINES				2
#define OSD_MODEL_FAULTBYTES			2
#define OSD_MODEL_SIZE					sizeof(OsdModel)

#define OSD_MODEL_SYNC0					0b01010101
#define OSD_MODEL_SYNC1					0b10101010
#define OSD_MODEL_SYNC2					0b00110011
#define OSD_MODEL_SYNC3					0b11001100
#define OSD_MODEL_SYNC4					0b00001111
#define OSD_MODEL_SYNC5					0b11110000
#define OSD_MODEL_SYNC6					0b11000011
#define OSD_MODEL_SYNC7					0b11111111

#define OSD_MODE_STARTUP				0
#define OSD_MODE_PIT					1
#define OSD_MODE_TAKEOFF				2
#define OSD_MODE_FLIGHT					3
#define OSD_MODE_LANDING				4

#define OSD_SUBMODE_DIAGNOSTICS			0
#define OSD_SUBMODE_NAVIGATION			1
#define OSD_SUBMODE_MINIMAL				2
#define OSD_SUBMODE_LOWLEVEL			3

typedef struct osdModelEngineData
{
	uint8_t  throttlePercentage;
	uint8_t  escTemperature;
	uint16_t rpm;
} OsdModelEngineData;

typedef struct osdModelAltitudeData
{
	uint16_t target;
	uint16_t barometric;
	uint16_t gps;
	uint16_t ranged;
	float    vsi;
} OsdModelAltitudeData;

typedef struct osdModelAirspeedData
{
	uint16_t target;
	uint16_t pitot;
	uint16_t imu;
	uint16_t gps;
	float    acceleration;
} OsdModelAirspeedData;

typedef struct osdModelHeadingData
{
	uint16_t target;
	uint16_t magnetic;
	uint16_t gps;
	uint16_t home;
} OsdModelHeadingData;

typedef struct osdModelAttitude
{
	float pitch;
	float pitchRate;
	float roll;
	float rollRate;
	float yaw;
	float yawRate;
	float fpiPitch;
	float fpiYaw;

} OsdModelAttitude;

typedef struct osdModel
{
	uint8_t version;
	uint8_t faults[OSD_MODEL_FAULTBYTES];
	//
	uint8_t mode;
	uint8_t submode;
	//
	OsdModelAttitude     attitude;
	OsdModelHeadingData  heading;
	OsdModelAltitudeData altitude;
	OsdModelAirspeedData airspeed;
	OsdModelEngineData   engine[OSD_MODEL_ENGINES];
	//
	uint8_t  synchronised;
	uint32_t packets;
	uint32_t packetCrcFaults;
	uint32_t packetOverruns;
} OsdModel;



#endif /* APP_INC_DRIVERS_OSDMODEL_V13_HPP_ */
