/*
 * pwmdriver.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Daniel Johnson
 */

#ifndef APP_INC_PWMDRIVER_H_
#define APP_INC_PWMDRIVER_H_

#include "component.hpp"

class PwmDriver : public HasStatusAndSetupAndCycle
{
public:
	virtual void drive(const uint8_t channel, const uint16_t value) = 0;
};

#endif /* APP_INC_PWMDRIVER_H_ */
