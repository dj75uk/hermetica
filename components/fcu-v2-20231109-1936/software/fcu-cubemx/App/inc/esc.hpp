/*
 * esc.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_ESC_HPP_
#define APP_INC_ESC_HPP_

#include <cstdio>
#include <cstdint>
#include <string>
#include "pwm.hpp"
#include "servo.hpp"

class Esc : public Servo
{
private:
	const char* className = "esc";
protected:
public:
	Esc(const uint8_t outputChannel)
		: Servo(outputChannel, PWM_MINIMUM, 1.0F, false, PWM_MINIMUM, PWM_MAXIMUM)
	{
	}
};

#endif /* APP_INC_ESC_HPP_ */
