/*
 * hcsr04p.hpp
 *
 *  Created on: Oct 24, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_DRIVERS_HCSR04P_HPP_
#define APP_INC_DRIVERS_HCSR04P_HPP_

#include <cstdio>
#include <cstdint>
#include "accuracy.hpp"
#include "component.hpp"
#include "alarm.hpp"
#include "stm32h7xx_it.h"
#include "movingaverage.hpp"
#include "altitude-ranged.hpp"

#define HCSR04P_SCALING_FACTOR			((float)(1.000000 / 1000.0))
#define HCSR04P_MINIMUM_DISTANCE		((float)0.03)
#define HCSR04P_MAXIMUM_DISTANCE		((float)3.00)
#define HCSR04P_RETRIGGER_MS			50
#define HCSR04P_TIMEOUT_MS				500

extern const char* textClassNameHcsr04p;
extern uint32_t HAL_GetHighResolutionTick();
extern "C" void interruptExternalReset(uint8_t id);
extern "C" uint8_t interruptExternalTestAndResetIfSet(uint8_t id);
extern "C" uint16_t interruptGetRisingEdge(uint8_t id);
extern "C" uint16_t interruptGetFallingEdge(uint8_t id);

class Hcsr04p : public HasStatusAndSetupAndCycle, public RangedAltitude
{
private:
	MovingAverage*  _average5        = NULL;
	Alarm*   		_alarm           = NULL;
	Alarm*          _timeout         = NULL;
	GPIO_TypeDef* 	_triggerPort     = NULL;
	uint16_t 		_triggerPin      = 0;
	uint8_t 		_interruptNumber = 0;
	uint8_t         _state           = 0;
public:
	Hcsr04p(GPIO_TypeDef* triggerPort, uint16_t triggerPin, uint8_t interruptNumber)
	{
		_triggerPort     = triggerPort;
		_triggerPin      = triggerPin;
		_interruptNumber = interruptNumber;
		_alarm           = new Alarm(HCSR04P_RETRIGGER_MS);
		_timeout         = new Alarm(HCSR04P_TIMEOUT_MS);
		_average5        = new MovingAverage();
	}
	virtual Status setup() override
	{
		return readyWithSet(textClassNameHcsr04p, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();
		switch (_state)
		{
		case 0:
			// we are waiting to send a trigger pulse...
			if (_alarm->alarm())
				_state = 1;
			break;
		case 1:
			// we are sending a trigger pulse (setting pin high)...
			_alarm->reset();
			interruptExternalReset(_interruptNumber);
			HAL_GPIO_WritePin(_triggerPort, _triggerPin, GPIO_PIN_SET);
			// get the timer value...
			_timeout->reset();
			_state = 2;
			break;
		case 2:
			// we are sending a trigger pulse (setting pin low)...
			HAL_GPIO_WritePin(_triggerPort, _triggerPin, GPIO_PIN_RESET);
			_state = 3;
			break;
		case 3:
			// we are waiting to receive an interrupt callback...
			if (interruptExternalTestAndResetIfSet(_interruptNumber))
			{
				_state = 0;

				uint16_t risingEdge  = interruptGetRisingEdge(_interruptNumber);
				uint16_t fallingEdge = interruptGetFallingEdge(_interruptNumber);
				uint16_t delta       = fallingEdge - risingEdge;

				// do some magical calculation...
				float scaled = delta * HCSR04P_SCALING_FACTOR;

				_average5->add(scaled);

				if (scaled > HCSR04P_MAXIMUM_DISTANCE)
					Accuracy::accuracy(ACCURACY_UNKNOWN);
				else if (scaled < 1.0)
					Accuracy::accuracy(ACCURACY_HIGHEST);
				else
					Accuracy::accuracy(ACCURACY_HIGH);

				scaled = LIMIT(_average5->average(), HCSR04P_MINIMUM_DISTANCE, HCSR04P_MAXIMUM_DISTANCE);

				rangedAltitude(scaled);
			}
			else
			{
				if (_timeout->alarm())
				{
					Accuracy::accuracy(ACCURACY_UNKNOWN);
					rangedAltitude(0);
					_state = 0;
				}
			}
			break;
		default:
			_state = 0;
			break;
		}
		return Status::Ready;
	}
};

#endif /* APP_INC_DRIVERS_HCSR04P_HPP_ */
