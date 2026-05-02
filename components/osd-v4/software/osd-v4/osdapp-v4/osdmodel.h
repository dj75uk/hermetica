/*
 * osdmodel.h
 *
 * Created: 16/04/2023 17:05:44
 *  Author: Daniel.Johnson
 */ 
#ifndef OSDMODEL_H_
#define OSDMODEL_H_
#include <stdint-gcc.h>

#define OSD_MODEL_VERSION						14
#define OSD_MODEL_ENGINES						2
#define OSD_MODEL_FAULTBYTES					2
#define OSD_MODEL_SIZE							sizeof(OsdModel)

#define OSD_MODEL_SYNC0							0b01010101
#define OSD_MODEL_SYNC1							0b10101010
#define OSD_MODEL_SYNC2							0b00110011
#define OSD_MODEL_SYNC3							0b11001100
#define OSD_MODEL_SYNC4							0b00001111
#define OSD_MODEL_SYNC5							0b11110000
#define OSD_MODEL_SYNC6							0b11000011
#define OSD_MODEL_SYNC7							0b11111111

enum class FcuMode : uint8_t
{
	Startup					= 0,
	WaitingForArmStage1		= 1,
	WaitingForArmStage2		= 2,
	WaitingForArmStage3		= 3,
	WaitingForArmStage4		= 4,
	WaitingForArmStage5		= 5,
	Acrobatic				= 6,
	Takeoff					= 7,
	Stabilised				= 8,
	RTB						= 9,
	Waypoint				= 10,
	WaypointPrevious		= 11,
	WaypointNext			= 12,
	Landing					= 13,
	Loiter					= 14,
	Shutdown				= 15
};

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

typedef struct osdModelEngineData
{
	uint8_t  throttlePercentage;
	uint8_t  escTemperature;
	uint16_t rpm;
} OsdModelEngineData;

typedef struct osdModelAttitude
{
	float pitch;
	float pitchRate;
	float roll;
	float rollRate;
	float yaw;
	float yawRate;
} OsdModelAttitude;

typedef struct osdModelAltitudeData
{
	float barometric;
	float gps;
	float ranged;
} OsdModelAltitudeData;

typedef struct osdModelVsi
{
	float barometric;
	float gps;
	float ranged;
} OsdModelVsi;

typedef struct osdModelAirspeedData
{
	uint16_t pitot;
	uint16_t gps;
	uint16_t imu;
} OsdModelAirspeedData;

typedef struct osdModelHeadingData
{
	uint16_t magnetic;
	uint16_t gps;
	uint16_t imu;
} OsdModelHeadingData;



typedef struct osdModelFpi
{
	float pitch;
	float yaw;
} OsdModelFpi;



typedef struct osdLocation
{
	int32_t latitude;
	int32_t longitude;
	uint16_t bearing;
	uint16_t range;
	
} OsdLocation;

typedef struct osdModel
{
	uint8_t version;
	uint8_t faults[OSD_MODEL_FAULTBYTES];
	//
	FcuMode fcuMode;
	VtxMode vtxMode;
	//
	OsdModelAttitude     attitude;
	OsdModelFpi          fpi;
	OsdModelVsi          vsi;
	OsdModelHeadingData  heading;
	OsdModelAltitudeData altitude;
	OsdModelAirspeedData airspeed;
	OsdModelEngineData   engine[OSD_MODEL_ENGINES];
	//
	uint8_t armed;
	uint8_t rssi0;
	uint8_t rssi1;
	uint8_t rssi2;
	uint8_t rssi3;
	//
	OsdLocation location;
	OsdLocation home;
	OsdLocation waypoint;
	//
	uint8_t gpsFix;
	uint8_t gpsSatellitesInUse;
	uint8_t gpsSatellitesInView;
	//
	uint8_t  synchronised;
	uint32_t packets;
	uint32_t packetCrcFaults;
	uint32_t packetOverruns;
} OsdModel;

#endif /* OSDMODEL_H_ */
