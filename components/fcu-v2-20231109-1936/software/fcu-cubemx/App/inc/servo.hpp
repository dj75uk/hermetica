/*
 * servo.hpp
 *
 *  Created on: Oct 13, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_SERVO_HPP_
#define APP_INC_SERVO_HPP_

#include <cstdint>
#include "pwmdriver.hpp"
#include "pwm.hpp"
#include "func.hpp"

class Servo
{
private:
	uint8_t    _channel       = 0;
	uint16_t   _hardLimitLow  = PWM_MINIMUM;
	uint16_t   _hardLimitHigh = PWM_MAXIMUM;
	uint16_t   _pwm           = PWM_CENTRE;
	float      _scaling       = 1.0;
	bool       _invert        = false;
public:
	Servo(const uint8_t outputChannel, const uint16_t initialPwm, const float scaling, const bool invert, const uint16_t hardLimitLow, const uint16_t hardLimitHigh)
	{
		_channel       = outputChannel;
		_hardLimitLow  = hardLimitLow;
		_hardLimitHigh = hardLimitHigh;
		_scaling       = scaling;
		_invert        = invert;
		pwm(initialPwm);
	}
	uint8_t channel() const
	{
		return _channel;
	}
	float scaling() const
	{
		return _scaling;
	}
	void scaling(const float value)
	{
		_scaling = value;
	}

	// pwm signal should always be normalised to 1000 to 2000...
	// hard limits are not scaled in any way
	// hard limits are to protect physical airframe and servos
	uint16_t pwm() const
	{
		return _pwm;
	}
	void pwm(const uint16_t pwm)
	{
		_pwm =
			bandpass<uint16_t>(
				unnormalise(
					(int16_t)multiply<float>(
						multiply<int16_t>(
							normalise(
								rescale<int16_t>(
									bandpass<int16_t>(pwm, PWM_MINIMUM, PWM_MAXIMUM),
									PWM_MINIMUM,
									PWM_MAXIMUM,
									_hardLimitLow,
									_hardLimitHigh
								)
							),
							-1,
							_invert
						),
						_scaling)
					),
				_hardLimitLow,
				_hardLimitHigh
			);
	}
};

#endif /* APP_INC_SERVO_HPP_ */
