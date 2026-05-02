/*
 * bme280.h
 *
 *  Created on: Oct 8, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_BME280_H_
#define APP_INC_BME280_H_

#define BME280_DEVICEID					0x60
#define BME280_BOOTTIME_MS				850
#define BME280_CYCLE_MS  				293
#define BME280_I2C_ADDRESS				0xEE
#define BME280_I2C_REG_WHOAMI			0xD0
#define BME280_I2C_REG_PAGE				0x07
#define BME280_I2C_REG_MODE				0x3D
#define BME280_I2C_REG_PWRMODE			0x3E
#define BME280_I2C_REG_TRIGGER			0x3F

#include <cstdio>
#include <cstdint>
#include <string>
#include "component.hpp"
#include "alarm.hpp"
#include "iic.hpp"
#include "debug.hpp"
#include "temperature.hpp"
#include "pressure-barometric.hpp"
#include "hygrometer.hpp"

class Bme280 :
	public HasStatusAndSetupAndCycle,
	public Temperature,
	public BarometricPressure,
	public Humidity
{
private:
	const char* className = "bme280";
	Iic*     _iic;
	Alarm*   _alarm;

	uint8_t  _buffer[8];
	uint8_t  _bufferPosition = 0;
	uint16_t _digT1 = 0;
	int16_t  _digT2 = 0;
	int16_t  _digT3 = 0;
	uint8_t  _digH1 = 0;
	int16_t  _digH2 = 0;
	uint8_t  _digH3 = 0;
	int16_t  _digH4 = 0;
	int16_t  _digH5 = 0;
	uint16_t _digP1 = 0;
	int16_t  _digP2 = 0;
	int16_t  _digP3 = 0;
	int16_t  _digP4 = 0;
	int16_t  _digP5 = 0;
	int16_t  _digP6 = 0;
	int16_t  _digP7 = 0;
	int16_t  _digP8 = 0;
	int16_t  _digP9 = 0;

	int32_t calculateTF(const int32_t rawTemperatureData)
	{
		int32_t var1 = ((((rawTemperatureData>>3) - ((int32_t)_digT1<<1))) * ((int32_t)_digT2)) >> 1;
		int32_t var2 = (((((rawTemperatureData>>4) - ((int32_t)_digT1   )) * ((rawTemperatureData>>4) - ((int32_t)_digT1))) >> 12) * ((int32_t)_digT3)) >> 14;
		return var1 + var2;
	}
	float compensateTemperature(const int32_t tf)
	{
		return ((float)((tf * 5 + 128) >> 8)) / (float)100.0;
	}
	float compensatePressure(const int32_t raw, const int32_t tf)
	{
		UNUSED(raw);
		UNUSED(tf);
		return (float)0.0;
	}
	float compensateHumidity(const int32_t raw, const int32_t tf)
	{
		UNUSED(raw);
		UNUSED(tf);
		return (float)0.0;
	}
public:
	Bme280(Iic* iic)
	{
		_iic            = iic;
		_bufferPosition = 0;
		_alarm          = new Alarm(450);
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		HAL_Delay(BME280_BOOTTIME_MS);

		uint8_t buffer[24];

		// verify the device is a bme280...
		if (!_iic->verifySingleByte(BME280_I2C_ADDRESS, BME280_I2C_REG_WHOAMI, BME280_DEVICEID))
			return missing(className, __LINE__);

		buffer[0] = 0b00000110;
		if (!_iic->writeSingleByteWithVerify(BME280_I2C_ADDRESS, 0xF5, buffer)) // configuration
			return failed(className, __LINE__);
		HAL_Delay(10);

		buffer[0] = 0b00000100;
		if (!_iic->writeSingleByteWithVerify(BME280_I2C_ADDRESS, 0xF2, buffer)) // ctrl_hum
			return failed(className, __LINE__);
		HAL_Delay(10);

		buffer[0] = 0b10010011;
		if (!_iic->writeSingleByteWithVerify(BME280_I2C_ADDRESS, 0xF4, buffer)) // ctrl_meas
			return failed(className, __LINE__);
		HAL_Delay(10);

		// read temperature calibration data...
		if (!_iic->readByteRange(BME280_I2C_ADDRESS, 0x88, 0x8D, buffer))
			return failed(className, __LINE__);
		_digT1 = (uint16_t)((buffer[1] << 8) + buffer[0]);
		_digT2 =  (int16_t)((buffer[3] << 8) + buffer[2]);
		_digT3 =  (int16_t)((buffer[5] << 8) + buffer[4]);

		// read pressure calibration data...
		if (!_iic->readByteRange(BME280_I2C_ADDRESS, 0x8E, 0x9F, buffer))
			return failed(className, __LINE__);
		_digP1 = (uint16_t)((buffer[1]  << 8) + buffer[0]);
		_digP2 =  (int16_t)((buffer[5]  << 8) + buffer[2]);
		_digP3 =  (int16_t)((buffer[7]  << 8) + buffer[4]);
		_digP4 =  (int16_t)((buffer[9]  << 8) + buffer[6]);
		_digP5 =  (int16_t)((buffer[11] << 8) + buffer[8]);
		_digP6 =  (int16_t)((buffer[13] << 8) + buffer[10]);
		_digP7 =  (int16_t)((buffer[15] << 8) + buffer[12]);
		_digP8 =  (int16_t)((buffer[17] << 8) + buffer[16]);
		_digP9 =  (int16_t)((buffer[19] << 8) + buffer[18]);

		// read humidity calibration data...
		if (!_iic->readSingleByte(BME280_I2C_ADDRESS, 0xA1, &_digH1))
			return failed(className, __LINE__);
		if (!_iic->readByteRange(BME280_I2C_ADDRESS, 0xE1, 0xE6, buffer))
			return failed(className, __LINE__);
		_digH2 =  (int16_t)((buffer[1] << 8) + buffer[0]);
		_digH3 =  buffer[2];
		_digH4 =  (int16_t)((buffer[4] << 8) + buffer[3]);
		_digH5 =  (int16_t)((buffer[6] << 8) + buffer[5]);

		return readyWithSet(className, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		if (!_alarm->alarm())
			return status();
		_alarm->reset();

		// read output registers...
		uint8_t readByte;
		if (!_iic->readSingleByte(BME280_I2C_ADDRESS, 0xF7 + _bufferPosition, &readByte))
			return Status::Failed;
		_buffer[_bufferPosition++] = readByte;
		if (_bufferPosition < 8)
			return status();
		_bufferPosition = 0;

		// determine temperature (should be done first to calculate tf)...
		// 3 MSB
		// 4 LSB
		// 5 XLSB
		int32_t tf = calculateTF((int32_t)((_buffer[3] << 16) + (_buffer[4] << 8) + _buffer[5]) >> 4);
		temperature(compensateTemperature(tf));

		// determine pressure (should be done after temperature)...
		// 0 MSB
		// 1 LSB
		// 2 XLSB
		BarometricPressure::accuracy(0.0);
		barometricPressure(compensatePressure((int32_t)(((_buffer[0] << 16) + (_buffer[1] << 8) + _buffer[2]) >> 4), tf));

		// determine humidity (should be done after temperature)...
		// 6 MSB
		// 7 LSB
		humidity(compensateHumidity((int16_t)((_buffer[6] << 8) + _buffer[7]), tf));


		return status(Status::Ready);
	}
};

#endif /* APP_INC_BME280_H_ */
