/*
 * board.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */
#ifndef COMPONENTS_BOARD_H_
#define COMPONENTS_BOARD_H_

#include <cstdint>
#include "component.hpp"
#include "indicator.hpp"
#include "mcu.hpp"
#include "debug.hpp"
#include "rcrx.hpp"
#include "pwmdriver.hpp"
#include "servo.hpp"
#include "heading-pitch-roll.hpp"
#include "gyroscope.hpp"
#include "magnetometer.hpp"
#include "pressure-barometric.hpp"
#include "location.hpp"
#include "altitude-gps.hpp"
#include "altitude-ranged.hpp"
#include "temperature.hpp"
#include "hygrometer.hpp"
#include "acceleration.hpp"
#include "acceleration-linear.hpp"
#include "acceleration-gravity.hpp"
#include "armable.hpp"

extern const char* textClassNameBoard;
extern const char* textStop;

class Board
	: public HasStatusAndSetupAndCycle
	, public Armable
	, public Temperature
	, public HeadingPitchRoll
	, public Gyroscope
	, public BarometricPressure
	, public Location
	, public GpsAltitude
	, public RangedAltitude
	, public Humidity
	, public Acceleration
	, public LinearAcceleration
	, public GravityAcceleration
	, public Magnetometer
{
protected:
	Board()
	{
	}
public:
	virtual Mcu* mcu() const = 0;
	virtual Indicator* indicatorActivity() const = 0;
	virtual Indicator* indicatorWarning()  const = 0;
	virtual Indicator* indicatorError()    const = 0;
	virtual PwmDriver* pwm() const = 0;
	//
	virtual Servo* servoREsc()     const = 0;
	virtual Servo* servoLEsc()     const = 0;
	virtual Servo* servoRAileron() const = 0;
	virtual Servo* servoLAileron() const = 0;
	virtual Servo* servoRVTail()   const = 0;
	virtual Servo* servoLVTail()   const = 0;
	//
	virtual RcRx* rcrxA() const = 0;
	virtual RcRx* rcrxB() const = 0;
	virtual RcRx* rcrxC() const = 0;
	//
	virtual uint8_t throttlePercentageR() const = 0;
	virtual uint8_t throttlePercentageL() const = 0;
	virtual void throttlePercentageR(const uint8_t value) = 0;
	virtual void throttlePercentageL(const uint8_t value) = 0;
	//
	virtual Gps* gps() const = 0;
	//
	void fail()
	{
		fail(0);
	}
	void fail(uint8_t pulseCount)
	{
		debugWriteError(textClassNameBoard, __LINE__, textStop);
		indicatorActivity()->off();
		indicatorWarning()->off();
		indicatorError()->off();
		if (pulseCount == 0)
		{
			indicatorError()->on();
			while (1);
		}
		HAL_Delay(500);
		while (1)
		{
			for (int i = 0; i < pulseCount; i++)
			{
				indicatorError()->on();
				HAL_Delay(50);
				indicatorError()->off();
				HAL_Delay(250);
			}
			HAL_Delay(750);
		}
	}
};

#endif /* COMPONENTS_BOARD_H_ */
