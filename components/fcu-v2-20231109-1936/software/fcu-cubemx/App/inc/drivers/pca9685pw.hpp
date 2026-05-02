/*
 * pca9685pw.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_PCA9685PW_H_
#define APP_INC_PCA9685PW_H_

#include <cstdio>
#include <cstdint>
#include <cstring>
#include "component.hpp"
#include "alarm.hpp"
#include "pwmdriver.hpp"
#include "iic.hpp"
#include "debug.hpp"
#include "macros.hpp"
#include "func.hpp"

extern const char* textClassNamePca9685pw;
extern const char* textError;

#define PCA9685PW_I2C_ADDRESS 			0b10000000
#define PCA9685PW_OSC_FREQUENCY_HZ 		25000000
#define PCA9685PW_PWM_FREQUENCY_HZ 		200
#define PCA9685PW_PWM_STAGGER			50
#define PCA9685PW_CHANNELS            	16
#define PCA9685PW_MINIMUM_OUTPUT      	0
#define PCA9685PW_MAXIMUM_OUTPUT      	4095
#define PCA9685PW_REG_MODE1				0x00
#define PCA9685PW_REG_MODE2				0x01
#define PCA9685PW_REG_LEDBASE 			0x06
#define PCA9685PW_REG_PRESCALE			0xFE
#define PCA9685PW_TESTMODE 				0xFF

#define PCA9685PW_PWM_SERVO_MIN			835
#define PCA9685PW_PWM_SERVO_MAX			(2*PCA9685PW_PWM_SERVO_MIN)

// MODE1 bits
#define MODE1_BIT_ALLCALL					0	/**< respond to LED All Call I2C-bus address */
#define MODE1_BIT_SUB3						1	/**< respond to I2C-bus subaddress 3 */
#define MODE1_BIT_SUB2						2	/**< respond to I2C-bus subaddress 2 */
#define MODE1_BIT_SUB1						3	/**< respond to I2C-bus subaddress 1 */
#define MODE1_BIT_SLEEP						4	/**< Low power mode. Oscillator off */
#define MODE1_BIT_AI						5	/**< Auto-Increment enabled */
#define MODE1_BIT_EXTCLK					6	/**< Use EXTCLK pin clock */
#define MODE1_BIT_RESTART					7	/**< Restart enabled */

// MODE2 bits
#define MODE2_BIT_OUTNE0					0	/**< Active LOW output enable input */
#define MODE2_BIT_OUTNE1					1	/**< Active LOW output enable input - high impedience */
#define MODE2_BIT_OUTDRV					2	/**< totem pole structure vs open-drain */
#define MODE2_BIT_OCH						3	/**< Outputs change on ACK vs STOP */
#define MODE2_BIT_INVERT					4	/**< Output logic state inverted */

class Pca9685pw : public PwmDriver
{
private:
	Iic*     _iic;
	uint8_t  _address  = PCA9685PW_I2C_ADDRESS;
	uint16_t _channels[PCA9685PW_CHANNELS];
	bool     _dirty[PCA9685PW_CHANNELS];
	uint8_t  _prescale = 0x00;
	Alarm*   _alarm;

	bool prescale(const uint8_t value)
	{
		uint8_t buffer = value;
		if (!_iic->writeSingleByteWithVerify(_address, PCA9685PW_REG_PRESCALE, &buffer))
			return false;
		_prescale = value;
		return true;
	}
	uint8_t remap(const uint8_t channel) const
	{
		if (channel ==  1) return 0;
		if (channel ==  2) return 1;
		if (channel ==  3) return 2;
		if (channel ==  4) return 3;
		if (channel ==  5) return 4;
		if (channel ==  6) return 5;
		if (channel ==  7) return 6;
		if (channel ==  8) return 7;
		if (channel ==  9) return 15;
		if (channel == 10) return 14;
		if (channel == 11) return 13;
		if (channel == 12) return 12;
		if (channel == 13) return 11;
		if (channel == 14) return 8;
		if (channel == 15) return 9;
		if (channel == 16) return 10;
		return 0;
	}
	bool ensureAsleep()
	{
		return _iic->bitSet(_address, PCA9685PW_REG_MODE1, MODE1_BIT_SLEEP);
	}
	bool ensureAwake()
	{
		return _iic->bitClr(_address, PCA9685PW_REG_MODE1, MODE1_BIT_SLEEP);
	}
	bool writeStartStop(uint8_t deviceRegister, uint16_t start, uint16_t stop)
	{
		bool success = true;
		uint8_t buffer[4];
		buffer[0] = (uint8_t)start;
		buffer[1] = (uint8_t)(start >> 8);
		buffer[2] = (uint8_t)stop;
		buffer[3] = (uint8_t)(stop >> 8);
		for (uint8_t i = 0; i < sizeof(buffer); i++)
			success = success && _iic->writeSingleByteWithVerify(_address, deviceRegister + i, buffer + i);
		return success;
	}
	bool writeRange(uint8_t pcaChannel, uint16_t value)
	{
		return writeStartStop(PCA9685PW_REG_LEDBASE + (uint8_t)(4 * pcaChannel), 0x0000, value);
	}
	Status cycle(bool forced)
	{
		if (!forced)
		{
			if (status() != Status::Ready)
				return status();
			if (!_alarm->alarm())
				return status();
			_alarm->reset();
		}
		bool success = true;
		for (uint8_t channel = 0; channel < PCA9685PW_CHANNELS; channel++)
		{
			if (!forced && !_dirty[channel])
				continue;
			success = success && writeRange(channel, _channels[channel]);
			if (success)
				_dirty[channel] = false;
		}
		if (!success)
		{
			debugWriteError(textClassNamePca9685pw, __LINE__, textError);
		}
		return Status::Ready;
	}
public:
	Pca9685pw(Iic* iic)
	{
		_iic   = iic;
		_alarm = new Alarm(10);

		for (uint8_t channel = 0; channel < PCA9685PW_CHANNELS; channel++)
		{
			_channels[channel] = 0;
			_dirty[channel] = true;
		}
	}
	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		memset(_channels, PCA9685PW_MINIMUM_OUTPUT, PCA9685PW_CHANNELS * sizeof(uint16_t));

		HAL_Delay(100);

		// device must be asleep to set configuration...
		if (!ensureAsleep())
			return failed(textClassNamePca9685pw, __LINE__);
		HAL_Delay(50);

		// configure mode1 bits...
		_iic->bitClr(_address, PCA9685PW_REG_MODE1, MODE1_BIT_ALLCALL); // ALLCALL
		_iic->bitClr(_address, PCA9685PW_REG_MODE1, MODE1_BIT_SUB3);    // SUB3
		_iic->bitClr(_address, PCA9685PW_REG_MODE1, MODE1_BIT_SUB2);    // SUB2
		_iic->bitClr(_address, PCA9685PW_REG_MODE1, MODE1_BIT_SUB1);    // SUB1
		_iic->bitClr(_address, PCA9685PW_REG_MODE1, MODE1_BIT_AI);      // AI
		_iic->bitClr(_address, PCA9685PW_REG_MODE1, MODE1_BIT_EXTCLK);  // EXTCLK

		// configure mode2 bits...
		_iic->bitClr(_address, PCA9685PW_REG_MODE2, MODE2_BIT_OUTNE0);  // OUTNE[0]
		_iic->bitClr(_address, PCA9685PW_REG_MODE2, MODE2_BIT_OUTNE1);  // OUTNE[1]
		_iic->bitSet(_address, PCA9685PW_REG_MODE2, MODE2_BIT_OUTDRV);  // OUTDRV
		_iic->bitClr(_address, PCA9685PW_REG_MODE2, MODE2_BIT_OCH);     // OCH
		_iic->bitClr(_address, PCA9685PW_REG_MODE2, MODE2_BIT_INVERT);  // INVRT

		// set pre-scaling frequency...
		uint8_t prescaler = (uint8_t)((float)PCA9685PW_OSC_FREQUENCY_HZ / ((float)PCA9685PW_PWM_FREQUENCY_HZ * 4096.0F)) - 1.0;
		if (!prescale(prescaler))
			return failed(textClassNamePca9685pw, __LINE__);
		HAL_Delay(50);

		// waking device...
		if (!ensureAwake())
			return failed(textClassNamePca9685pw, __LINE__);
		HAL_Delay(50);

		// ensure all channels are initially written to on the first cycle...
		for (uint8_t channel = 0; channel < PCA9685PW_CHANNELS; channel++)
		{
			_dirty[channel] = true;
		}

		// force a cycle to set the initial pwm outputs...
		cycle(true);

		// done...
		return readyWithSet(textClassNamePca9685pw, __LINE__);
	}
	virtual Status cycle() override
	{
		return status(cycle(false));
	}
	virtual void drive(const uint8_t channel, const uint16_t value) override
	{
		if (channel >= PCA9685PW_CHANNELS)
			return;
		uint8_t remappedChannel = remap(channel);

		uint16_t scaledValue = value;
		if (scaledValue > 0)
		{
			scaledValue = bandpass<uint16_t>(scaledValue, PWM_MINIMUM, PWM_MAXIMUM);
			scaledValue = rescale<uint16_t>(scaledValue, PWM_MINIMUM, PWM_MAXIMUM, PCA9685PW_PWM_SERVO_MIN, PCA9685PW_PWM_SERVO_MAX);
			scaledValue = bandpass<uint16_t>(scaledValue, PCA9685PW_MINIMUM_OUTPUT, PCA9685PW_MAXIMUM_OUTPUT);
		}
		if (_channels[remappedChannel] == scaledValue)
			return;
		_channels[remappedChannel] = scaledValue;
		_dirty[remappedChannel]    = true;
	}
};

#endif /* APP_INC_PCA9685PW_H_ */
