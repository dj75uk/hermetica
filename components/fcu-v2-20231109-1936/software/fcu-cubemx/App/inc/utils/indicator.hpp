/*
 * indicator.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */

#ifndef INDICATOR_H_
#define INDICATOR_H_

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"

class Indicator
{
private:
	GPIO_TypeDef* _port;
	uint16_t      _pin;
public:
	Indicator(GPIO_TypeDef *port, uint16_t pin)
	{
		_port = port;
		_pin  = pin;
	}
	void on()     { HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_SET);   }
	void off()    { HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET); }
	void toggle() { HAL_GPIO_TogglePin(_port, _pin); }
};

#endif /* INDICATOR_H_ */
