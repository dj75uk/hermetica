/*
 * a3g4250d.h
 *SS
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_A3G4250D_H_
#define APP_INC_A3G4250D_H_

#define A3G4250D_DEVICEID				0xD3
#define A3G4250D_BOOTTIME_MS			300
#define A3G4250D_CYCLE_MS				30
#define A3G4250D_I2C_ADDRESS			0xD0

#define A3G4250D_I2C_REG_WHOAMI			0x0F
#define A3G4250D_I2C_REG_CTRL1			0x20
#define A3G4250D_I2C_REG_CTRL2			0x21
#define A3G4250D_I2C_REG_CTRL3			0x22
#define A3G4250D_I2C_REG_CTRL4			0x23
#define A3G4250D_I2C_REG_CTRL5			0x24
#define A3G4250D_I2C_REG_REFERENCE		0x25
#define A3G4250D_I2C_REG_TEMP			0x26
#define A3G4250D_I2C_REG_STATUS			0x27
#define A3G4250D_I2C_REG_OUT_X_L		0x28
#define A3G4250D_I2C_REG_OUT_X_H		0x29
#define A3G4250D_I2C_REG_OUT_Y_L		0x2A
#define A3G4250D_I2C_REG_OUT_Y_H		0x2B
#define A3G4250D_I2C_REG_OUT_Z_L		0x2C
#define A3G4250D_I2C_REG_OUT_Z_H		0x2D
#define A3G4250D_I2C_REG_FIFO_CTRL		0x2E
#define A3G4250D_I2C_REG_FIFO_SRC		0x2F
#define A3G4250D_I2C_REG_INT1_CFG		0x30
#define A3G4250D_I2C_REG_INT1_SRC		0x31
#define A3G4250D_I2C_REG_INT1_THS_XH	0x32
#define A3G4250D_I2C_REG_INT1_THS_XL	0x33
#define A3G4250D_I2C_REG_INT1_THS_YH	0x34
#define A3G4250D_I2C_REG_INT1_THS_YL	0x35
#define A3G4250D_I2C_REG_INT1_THS_ZH	0x36
#define A3G4250D_I2C_REG_INT1_THS_ZL	0x37
#define A3G4250D_I2C_REG_INT1_DURATION	0x38

#include <cstdint>
#include "component.hpp"
#include "gyroscope.hpp"
#include "iic.hpp"
#include "debug.hpp"
#include "alarm.hpp"
#include "movingaverage.hpp"

class A3g4250d
	: public HasStatusAndSetupAndCycle
	, public Gyroscope
{
private:
	const char* className = "a3g4250d";
	Iic*    _iic;
	uint8_t _address;
	Alarm*  _alarm;
	MovingAverage maX;
	MovingAverage maY;
	MovingAverage maZ;
public:
	A3g4250d(Iic* iic)
	{
		_iic     = iic;
		_address = A3G4250D_I2C_ADDRESS;
		_alarm   = new Alarm(A3G4250D_CYCLE_MS);
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		HAL_Delay(A3G4250D_BOOTTIME_MS);

		// check device identifier...
		if (!_iic->verifySingleByte(_address, A3G4250D_I2C_REG_WHOAMI, A3G4250D_DEVICEID))
			return missing(className, __LINE__);

		// DEVICE FOUND AND IDENTIFIED AS A3G4250D

		// write all control registers, leaving CTRL_REG1:BIT3 to last as it wakes the device...

		// set bits of control register 1 (0x20)...
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL1, 0)) return failed(className, __LINE__);
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL1, 1)) return failed(className, __LINE__);
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL1, 2)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL1, 4)) return failed(className, __LINE__);
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL1, 5)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL1, 6)) return failed(className, __LINE__);
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL1, 7)) return failed(className, __LINE__);

		// set bits of control register 2 (0x21)...
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL2, 0)) return failed(className, __LINE__);
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL2, 1)) return failed(className, __LINE__);
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL2, 2)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL2, 3)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL2, 4)) return failed(className, __LINE__);
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL2, 5)) return failed(className, __LINE__);

		// set bits of control register 3 (0x22)...
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 0)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 1)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 2)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 3)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 4)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 5)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 6)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL3, 7)) return failed(className, __LINE__);

		// set bits of control register 4 (0x23)...
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL4, 0)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL4, 1)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL4, 2)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL4, 6)) return failed(className, __LINE__);

		// set bits of control register 5 (0x24)...
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL5, 0)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL5, 1)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL5, 2)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL5, 3)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL5, 4)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL5, 6)) return failed(className, __LINE__);
		if (!_iic->bitClr(_address, A3G4250D_I2C_REG_CTRL5, 7)) return failed(className, __LINE__);

		// ensure the device is awake...
		if (!_iic->bitSet(_address, A3G4250D_I2C_REG_CTRL1, 3))
			return failed(className, __LINE__);

		// device starting from power-down mode can take roughly 250ms...
		HAL_Delay(300);

		Gyroscope::accuracy(ACCURACY_HIGH);
		return readyWithSet(className, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		if (!_alarm->alarm())
			return status();
		_alarm->reset();

		// read all 6 output bytes in one go...
		uint8_t buffer[6];
		if (!_iic->readByteRange(_address, A3G4250D_I2C_REG_OUT_X_L, A3G4250D_I2C_REG_OUT_Z_H, buffer))
			return Status::Failed;
		int16_t x = (int16_t)((uint16_t)buffer[1] << 8) | buffer[0];
		int16_t y = (int16_t)((uint16_t)buffer[3] << 8) | buffer[2];
		int16_t z = (int16_t)((uint16_t)buffer[5] << 8) | buffer[4];

		maX.add((float)x / (float)100.0);
		maY.add((float)y / (float)100.0);
		maZ.add((float)z / (float)100.0);

		// store the results...
		pitchRate(maX.average());
		rollRate(maY.average());
		yawRate(maZ.average());

		return status(Status::Ready);
	}
};

#endif /* APP_INC_A3G4250D_H_ */
