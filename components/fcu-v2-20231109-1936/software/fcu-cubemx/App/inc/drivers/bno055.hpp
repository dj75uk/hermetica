/*
 * bno055.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_BNO055_H_
#define APP_INC_BNO055_H_

#define BNO055_DEVICEID					0xA0
#define BNO055_BOOTTIME_MS				850
#define BNO055_CYCLE_MS  				50
#define BNO055_I2C_ADDRESS				0x52
#define BNO055_I2C_REG_WHOAMI			0x00
#define BNO055_I2C_REG_PAGE				0x07
#define BNO055_I2C_REG_TEMPERATURE		0x34
#define BNO055_I2C_REG_MODE				0x3D
#define BNO055_I2C_REG_PWRMODE			0x3E
#define BNO055_I2C_REG_TRIGGER			0x3F

#define BNO055_MODE_CONFIG      		0x00
#define BNO055_MODE_ACCONLY      		0x01
#define BNO055_MODE_MAGONLY      		0x02
#define BNO055_MODE_GYRONLY      		0x03
#define BNO055_MODE_ACCMAG      		0x04
#define BNO055_MODE_ACCGYRO      		0x05
#define BNO055_MODE_MAGGYRO      		0x06
#define BNO055_MODE_AMG      			0x07
#define BNO055_MODE_IMUPLUS      		0x08
#define BNO055_MODE_COMPASS      		0x09
#define BNO055_MODE_M4G      			0x0A
#define BNO055_MODE_NDOF_FMC_OFF      	0x0B
#define BNO055_MODE_NDOF      			0x0C

#include <cstdio>
#include <cstdint>
#include <string>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_i2c.h"
#include "iic.hpp"
#include "component.hpp"
#include "alarm.hpp"
#include "accuracy.hpp"
#include "temperature.hpp"
#include "gyroscope.hpp"
#include "acceleration.hpp"
#include "magnetometer.hpp"
#include "acceleration-gravity.hpp"
#include "acceleration-linear.hpp"
#include "heading-pitch-roll.hpp"
#include "debug.hpp"
#include "vector3.hpp"
#include "movingaverage.hpp"

extern const char* textClassNameBno055;

class Bno055 :
	public HasStatusAndSetupAndCycle,
	public Temperature,
	public Gyroscope,
	public Acceleration,
	public Magnetometer,
	public HeadingPitchRoll,
	public GravityAcceleration,
	public LinearAcceleration
{
private:
	Iic*    _iic;
	Alarm*  _readPriority1;
	Alarm*  _readPriority2;
	Alarm*  _readPriority3;
	uint8_t _address;
	uint8_t _buffer[48];
	uint8_t _bufferPosition;
	uint8_t _cycleStage;

	MovingAverage maGyroX;
	MovingAverage maGyroY;
	MovingAverage maGyroZ;

	uint8_t mode()
	{
		uint8_t result;
		_iic->readSingleByte(_address, BNO055_I2C_REG_MODE, &result);
		return result;
	}
	bool ensureOperationalMode(uint8_t value)
	{
		uint8_t temp = value;
		return _iic->writeSingleByteWithVerify(_address, BNO055_I2C_REG_MODE, &temp);
	}
	bool ensurePowerMode(uint8_t value)
	{
		uint8_t temp = value;
		return _iic->writeSingleByteWithVerify(_address, BNO055_I2C_REG_PWRMODE, &temp);
	}
	bool ensureMemoryPage(uint8_t value)
	{
		uint8_t temp = value;
		return _iic->writeSingleByteWithVerify(_address, BNO055_I2C_REG_PAGE, &temp);
	}
public:
	Bno055(Iic* iic)
	{
		_iic            = iic;
		_address        = BNO055_I2C_ADDRESS;
		_bufferPosition = 0;
		_readPriority1  = new Alarm(50);
		_readPriority2  = new Alarm(500);
		_readPriority3  = new Alarm(2000);
		_cycleStage     = 0;
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		HAL_Delay(BNO055_BOOTTIME_MS);

		uint8_t buffer[8];

		// wait for system-initialisation completion...
		for (uint8_t i = 0; i < 5; i++)
		{
			if (!_iic->readSingleByte(_address, 0x39, buffer))
				return failed(textClassNameBno055, __LINE__);
			buffer[0] = buffer[0] & 0b00011110;
			if (buffer[0] == 0)
				break;
			HAL_Delay(100);
		}




		// set page-0...
		if (!ensureMemoryPage(0))
			return failed(textClassNameBno055, __LINE__);

		// set configuration operating mode...
		if (!ensureOperationalMode(BNO055_MODE_CONFIG))
			return failed(textClassNameBno055, __LINE__);

		// set normal power operation...
		if (!ensurePowerMode(0))
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(400);

		// set usage of external crystal...
		HAL_Delay(10);
		buffer[0] = 0x80;
		if (!_iic->writeSingleByteWithVerify(_address, BNO055_I2C_REG_TRIGGER, buffer))
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(100);

		// verify identifiers...
		if (!_iic->verifySingleByte(_address, 0x00, 0xA0)) // chip_id = 0xA0
			return failed(textClassNameBno055, __LINE__);
		if (!_iic->verifySingleByte(_address, 0x01, 0xFB)) // acc_id = 0xFB
			return failed(textClassNameBno055, __LINE__);
		if (!_iic->verifySingleByte(_address, 0x02, 0x32)) // mag_id = 0x32
			return failed(textClassNameBno055, __LINE__);
		if (!_iic->verifySingleByte(_address, 0x03, 0x0F)) // gyro_id = 0x0F
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(100);

		// set output units (m/s, degrees and uT)...
		buffer[0] = 0x00;
		if (!_iic->bitClr(_address, 0x3B, 0))
			return failed(textClassNameBno055, __LINE__);
		if (!_iic->bitClr(_address, 0x3B, 1))
			return failed(textClassNameBno055, __LINE__);
		if (!_iic->bitClr(_address, 0x3B, 2))
			return failed(textClassNameBno055, __LINE__);
		if (!_iic->bitClr(_address, 0x3B, 4))
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(100);

		// set axis re-mapping to default...
		buffer[0] = 0x24;

		if (!_iic->writeSingleByteWithVerify(_address, 0x41, buffer))
			return failed(textClassNameBno055, __LINE__);
		buffer[0] = 0x00;
		if (!_iic->writeSingleByteWithVerify(_address, 0x42, buffer))
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(100);

		// set temperature source to default...
		buffer[0] = 0x00;
		if (!_iic->writeSingleByteWithVerify(_address, 0x40, buffer))
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(100);



		// run self-test...
		buffer[0] = 0x00;
		if (!_iic->readSingleByte(_address, 0x3F, buffer))
			return failed(textClassNameBno055, __LINE__);
		// buffer now contains the existing value for the 0x3F register...
		buffer[0] = buffer[0] | 1;
		if (!_iic->writeSingleByte(_address, 0x3F, buffer)) // don't verify write
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(100);

		// wait for self-test completion...
		for (uint8_t i = 0; i < 50; i++)
		{
			if (!_iic->readSingleByte(_address, 0x39, buffer))
				return failed(textClassNameBno055, __LINE__);
			if (buffer[0] == 1) // system error
				return failed(textClassNameBno055, __LINE__);
			if (buffer[0] >= 5) // system running fine
				break;
			HAL_Delay(100);
		}

		// check self-test results...
		//if (!_iic->readSingleByte(_address, 0x36, buffer))
		//	return failed(className, __LINE__);
		//if (buffer[0] != 0x0F)
		//	return failed(className, __LINE__);
		HAL_Delay(100);

		// get device status...
		for (uint8_t i = 0; i < 2; i++)
		{
			if (!_iic->readSingleByte(_address, 0x39 + i, buffer + i))
				return failed(textClassNameBno055, __LINE__);
		}
		if (buffer[0] == 1)
			return failed(textClassNameBno055, __LINE__);
		if (buffer[1] != 0)
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(100);

		// set normal operating mode...
		if (!ensureOperationalMode(BNO055_MODE_NDOF))
			return failed(textClassNameBno055, __LINE__);
		HAL_Delay(50);

		Temperature::accuracy(ACCURACY_LOW);

		return readyWithSet(textClassNameBno055, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();

		int16_t x = 0;
		int16_t y = 0;
		int16_t z = 0;

		if (_readPriority1->alarm())
		{
			_readPriority1->reset();

			if (!_iic->readByteRange(_address, 0x08, 0x1F, _buffer))
				return failedNoSet(textClassNameBno055, __LINE__);

			// acceleration (0x08 to 0x0D)...
			x = ((int16_t)_buffer[0]) | (((int16_t)_buffer[1]) << 8);
			y = ((int16_t)_buffer[2]) | (((int16_t)_buffer[3]) << 8);
			z = ((int16_t)_buffer[4]) | (((int16_t)_buffer[5]) << 8);
			accelerationX((float)x / (float)100.0);
			accelerationY((float)y / (float)100.0);
			accelerationZ((float)z / (float)100.0);

			// magnetometer (0x0E to 0x13)...
			x = ((int16_t)_buffer[6])  | (((int16_t)_buffer[7]) << 8);
			y = ((int16_t)_buffer[8])  | (((int16_t)_buffer[9]) << 8);
			z = ((int16_t)_buffer[10]) | (((int16_t)_buffer[11]) << 8);
			fieldX((float)x / (float)16.0);
			fieldY((float)y / (float)16.0);
			fieldZ((float)z / (float)16.0);

			// gyroscope (0x14 to 0x19)...
			x = ((int16_t)_buffer[12]) | (((int16_t)_buffer[13]) << 8);
			y = ((int16_t)_buffer[14]) | (((int16_t)_buffer[15]) << 8);
			z = ((int16_t)_buffer[16]) | (((int16_t)_buffer[17]) << 8);
			maGyroX.add((float)-1.0 * (float)y / (float)16.0);
			maGyroY.add((float)x / (float)16.0);
			maGyroZ.add((float)-1.0 * (float)z / (float)16.0);
			pitchRate(maGyroX.average());
			rollRate(maGyroY.average());
			yawRate(maGyroZ.average());

			// euler-vector (0x1A to 0x1F)...
			x = ((int16_t)_buffer[18]) | (((int16_t)_buffer[19]) << 8);
			y = ((int16_t)_buffer[20]) | (((int16_t)_buffer[21]) << 8);
			z = ((int16_t)_buffer[22]) | (((int16_t)_buffer[23]) << 8);
			heading((float)x / (float)16.0);
			pitch((float)y / (float)16.0);
			roll(-1.0F * (float)z / (float)16.0);
		}
		if (_readPriority2->alarm())
		{
			_readPriority2->reset();

			if (!_iic->readByteRange(_address, 0x28, 0x33, _buffer))
				return failedNoSet(textClassNameBno055, __LINE__);

			// linear acceleration (0x28 to 0x2D) DIV100...
			x = ((int16_t)_buffer[0]) | (((int16_t)_buffer[1]) << 8);
			y = ((int16_t)_buffer[2]) | (((int16_t)_buffer[3]) << 8);
			z = ((int16_t)_buffer[4]) | (((int16_t)_buffer[5]) << 8);
			linearAccelerationX((float)x / (float)100.0);
			linearAccelerationY((float)y / (float)100.0);
			linearAccelerationZ((float)z / (float)100.0);

			// gravity (0x2E to 0x33) DIV100...
			x = ((int16_t)_buffer[6])  | (((int16_t)_buffer[7])  << 8);
			y = ((int16_t)_buffer[8])  | (((int16_t)_buffer[9])  << 8);
			z = ((int16_t)_buffer[10]) | (((int16_t)_buffer[11]) << 8);
			gravityX((float)x / (float)100.0);
			gravityY((float)y / (float)100.0);
			gravityZ((float)z / (float)100.0);
		}
		if (_readPriority3->alarm())
		{
			_readPriority3->reset();

			if (!_iic->readByteRange(_address, 0x34, 0x35, _buffer))
				return failedNoSet(textClassNameBno055, __LINE__);

			// temperature (0x34 [1 byte])...
			temperature((int8_t)_buffer[0]);

			// calibration (0x35 [1 byte])...
			uint8_t calibrationS = (_buffer[1] & 0b11000000) >> 6;
			uint8_t calibrationG = (_buffer[1] & 0b00110000) >> 4;
			uint8_t calibrationA = (_buffer[1] & 0b00001100) >> 2;
			uint8_t calibrationM = (_buffer[1] & 0b00000011) >> 0;
			HeadingPitchRoll::accuracy(calibrationS == 3 ? ACCURACY_MEDIUM : ACCURACY_LOW);
			Gyroscope::accuracy(calibrationG == 3 ? ACCURACY_MEDIUM : ACCURACY_LOW);
			Acceleration::accuracy(calibrationA == 3 ? ACCURACY_MEDIUM : ACCURACY_LOW);
			GravityAcceleration::accuracy(calibrationA == 3 ? ACCURACY_MEDIUM : ACCURACY_LOW);
			Magnetometer::accuracy(calibrationM == 3 ? ACCURACY_MEDIUM : ACCURACY_LOW);
			LinearAcceleration::accuracy(calibrationA == 3 ? ACCURACY_MEDIUM : ACCURACY_LOW);
		}
		return readyNoSet(textClassNameBno055, __LINE__);
	}
};

#endif /* APP_INC_BNO055_H_ */
