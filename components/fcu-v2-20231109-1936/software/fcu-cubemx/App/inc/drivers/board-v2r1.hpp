/*
 * board-v2r1.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */
#ifndef BOARD_V2R1_H_
#define BOARD_V2R1_H_

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "interrupts.h"
#include "component.hpp"
#include "board.hpp"
#include "mcu.hpp"
#include "indicator.hpp"
#include "stm32h723vgt6.hpp"
#include "debug.hpp"
#include "blinker.hpp"
#include "iic.hpp"
#include "uart.hpp"
#include "altitude-ranged.hpp"

#include "temperature.hpp"
#include "gyroscope.hpp"
#include "acceleration.hpp"
#include "magnetometer.hpp"
#include "heading-pitch-roll.hpp"
#include "acceleration-gravity.hpp"

#include "rcrx.hpp"
#include "rcrx-dsmx.hpp"
#include "rcrx-sbus.hpp"

#include "pwm.hpp"
#include "pwmdriver.hpp"
#include "pca9685pw.hpp"
#include "servo.hpp"

#include "a3g4250d.hpp"
#include "lis2hh12.hpp"
#include "asm330lhxxh.hpp"
#include "iis2mdc.hpp"
#include "ilps22qs.hpp"

#include "bno055.hpp"
#include "bme280.hpp"
#include "mb85rc256v.hpp"

#include "gps.hpp"
#include "uartgps.hpp"

#include "vtxclient.hpp"

extern const char* textError;
extern const char* textClassNameBoardV2R1;

#define BOARD_ATTITUDE_ROLL_OFFSET				(2.06250F)

class BoardV2R1 : public Board
{
private:
	Stm32h723vgt6* _stm32h723vgt6;
	Blinker*       _activityBlinker;
	Indicator*     _indicatorActivity;
	Indicator*     _indicatorWarning;
	Indicator*     _indicatorError;

	// i2c driver 4...
	A3g4250d*      						_a3g4250d		= NULL;
	Lis2hh12*      						_lis2hh12		= NULL;
	Asm330lhxxh*   						_asm330lhxxh	= NULL;
	Iis2mdc*       						_iis2mdc		= NULL;
	Ilps22qs*      						_ilps22qs		= NULL;

	// i2c driver 5...
	Pca9685pw*     						_pca9685pw   	= NULL;
	Servo*								_r_esc			= NULL;
	Servo*								_l_esc			= NULL;
	Servo*								_r_aileron		= NULL;
	Servo*								_l_aileron		= NULL;
	Servo*								_r_vtail		= NULL;
	Servo*								_l_vtail		= NULL;

	HasStatusAndSetup**					_externalComponents;
	uint8_t 							_externalComponentsCount;

	HasStatusAndSetup*					_componentsToSetup[20];
	uint8_t								_componentsToSetup_count = 0;
	HasStatusAndSetupAndCycle*			_componentsToCycle[20];
	uint8_t                             _componentsToCycle_count = 0;
	PwmDriver* 							_pwm = NULL;
	Gyroscope* 							_gyroscopes[8];
	uint8_t                             _gyroscopes_count = 0;
	Acceleration* 						_accelerometers[8];
	uint8_t                             _accelerometers_count = 0;
	LinearAcceleration*					_linearAccelerometers[8];
	uint8_t                             _linearAccelerometers_count = 0;
	Magnetometer*						_magnetometers[8];
	uint8_t                             _magnetometers_count = 0;
	BarometricPressure*					_barometers[8];
	uint8_t                             _barometers_count = 0;
	GravityAcceleration*				_gravitometers[8];
	uint8_t                             _gravitometers_count = 0;
	Temperature*						_thermometers[8];
	uint8_t                             _thermometers_count = 0;
	Humidity*							_hygrometers[8];
	uint8_t                             _hygrometers_count = 0;
	Gps*								_gpss[8];
	uint8_t                             _gpss_count = 0;
	Heading*							_compasses[8];
	uint8_t                             _compasses_count = 0;
	HeadingPitchRoll*					_imus[8];
	uint8_t                             _imus_count = 0;
	RangedAltitude*						_rangedaltitude[8];
	uint8_t								_rangedaltitude_count = 0;


	MovingAverage						maGyroscopeX;
	MovingAverage						maGyroscopeY;
	MovingAverage						maGyroscopeZ;
	uint8_t								_cycleStage = 0;

	RcRx* 								_rcrxA = NULL;
	RcRx* 								_rcrxB = NULL;
	RcRx* 								_rcrxC = NULL;
	Gps*								_gpsA  = NULL;
	uint8_t								_throttlePercentageR = 0;
	uint8_t								_throttlePercentageL = 0;
public:
	BoardV2R1(
		Stm32h723vgt6* mcu,
		Servo* r_esc,
		Servo* l_esc,
		Servo* r_aileron,
		Servo* l_aileron,
		Servo* r_vtail,
		Servo* l_vtail,
		HasStatusAndSetup** externalComponents,
		uint8_t externalComponentsCount
	)
	{
		_stm32h723vgt6           = mcu;
		_r_esc                   = r_esc;
		_l_esc                   = l_esc;
		_r_aileron               = r_aileron;
		_l_aileron               = l_aileron;
		_r_vtail                 = r_vtail;
		_l_vtail   				 = l_vtail;
		_externalComponents      = externalComponents;
		_externalComponentsCount = externalComponentsCount;
		_indicatorActivity       = new Indicator(GPIOC, GPIO_PIN_10);
		_indicatorWarning        = new Indicator(GPIOC, GPIO_PIN_11);
		_indicatorError          = new Indicator(GPIOC, GPIO_PIN_12);
		_activityBlinker         = new Blinker(_indicatorActivity, 5, 1000);
		_cycleStage              = 0;
	}

	Mcu*       mcu()               const override { return _stm32h723vgt6;     }
	PwmDriver* pwm()               const override { return _pca9685pw;         }
	Indicator* indicatorActivity() const override { return _indicatorActivity; }
	Indicator* indicatorWarning()  const override { return _indicatorWarning;  }
	Indicator* indicatorError()    const override { return _indicatorError;    }

	Servo* servoREsc()     const override { return _r_esc;     }
	Servo* servoLEsc()     const override { return _l_esc;     }
	Servo* servoRAileron() const override { return _r_aileron; }
	Servo* servoLAileron() const override { return _l_aileron; }
	Servo* servoRVTail()   const override { return _r_vtail;   }
	Servo* servoLVTail()   const override { return _l_vtail;   }

	virtual uint8_t throttlePercentageR() const override { return _throttlePercentageR; }
	virtual uint8_t throttlePercentageL() const override { return _throttlePercentageL; }
	virtual void throttlePercentageR(const uint8_t value) override { _throttlePercentageR = value; }
	virtual void throttlePercentageL(const uint8_t value) override { _throttlePercentageL = value; }

	virtual RcRx* rcrxA() const override { return _rcrxA; }
	virtual RcRx* rcrxB() const override { return _rcrxB; }
	virtual RcRx* rcrxC() const override { return _rcrxC; }

	virtual Gps* gps() const override { return _gpsA; }

	virtual Status setup() override
	{
		if (status() != Status::Unknown)
			return status();

		debugWriteInfo(textClassNameBoardV2R1, __LINE__, "setup");

		// create the NXP PCA9685DW pwm driver...
		_pca9685pw = new Pca9685pw(_stm32h723vgt6->iic5());
		_pca9685pw->drive(servoREsc()->channel(),     servoREsc()->pwm());
		_pca9685pw->drive(servoLEsc()->channel(),     servoLEsc()->pwm());
		_pca9685pw->drive(servoRAileron()->channel(), servoRAileron()->pwm());
		_pca9685pw->drive(servoLAileron()->channel(), servoLAileron()->pwm());
		_pca9685pw->drive(servoRVTail()->channel(),   servoRVTail()->pwm());
		_pca9685pw->drive(servoLVTail()->channel(),   servoLVTail()->pwm());
		if (_pca9685pw->setup() == Status::Ready)
			buildComponentLists(_pca9685pw);
		else
			fail(2);
		_pca9685pw->cycle();

		// create STM A3G4250D gyroscope...
		_a3g4250d = new A3g4250d(_stm32h723vgt6->iic4());
		if (_a3g4250d->setup() == Status::Ready)
			buildComponentLists(_a3g4250d);

		//  create STM LIS2HH12 accelerometer...
		_lis2hh12 = new Lis2hh12(_stm32h723vgt6->iic4());
		if (_lis2hh12->setup() == Status::Ready)
			buildComponentLists(_lis2hh12);

		//  create STM ASM330LHXXH imu...
		_asm330lhxxh = new Asm330lhxxh(_stm32h723vgt6->iic4());
		if (_asm330lhxxh->setup() == Status::Ready)
			buildComponentLists(_asm330lhxxh);

		//  create STM IIS2MDC magnetometer...
		_iis2mdc = new Iis2mdc(_stm32h723vgt6->iic4());
		if (_iis2mdc->setup() == Status::Ready)
			buildComponentLists(_iis2mdc);

		//  create STM ILPS22QS barometer...
		_ilps22qs = new Ilps22qs(_stm32h723vgt6->iic4());
		if (_ilps22qs->setup() == Status::Ready)
			buildComponentLists(_ilps22qs);


		indicateStartupSequence();

		// add in the external components...
		for (uint8_t i = 0; i < _externalComponentsCount; i++)
		{
			buildComponentsRequiringSetup(_externalComponents[i]);
		}

		// call setup() on every component supporting it...
		for (uint8_t i = 0; i < _componentsToSetup_count; i++)
		{
			if (_componentsToSetup[i]->setup() == Status::Ready)
			{
				// component looks good - check what it actually is...
				buildComponentLists(_componentsToSetup[i]);
			}
		}

		// check failure conditions...
		if (_componentsToCycle_count == 0)
			fail(2);

		if (_gyroscopes_count == 0)
			fail(2);

		if (_accelerometers_count == 0)
			fail(2);

		// everything looks right so far...
		return readyWithSet(textClassNameBoardV2R1, __LINE__);
	}
	virtual Status cycle() override
	{
		if (status() != Status::Ready)
			return status();

		// flashy-flashy-flash-flash...
		_activityBlinker->cycle();

		bool failure = false;

		debugWriteTrace(textClassNameBoardV2R1, __LINE__, "ok");

		pwm()->drive(servoREsc()->channel(),     servoREsc()->pwm());
		pwm()->drive(servoLEsc()->channel(),     servoLEsc()->pwm());
		pwm()->drive(servoRAileron()->channel(), servoRAileron()->pwm());
		pwm()->drive(servoLAileron()->channel(), servoLAileron()->pwm());
		pwm()->drive(servoRVTail()->channel(),   servoRVTail()->pwm());
		pwm()->drive(servoLVTail()->channel(),   servoLVTail()->pwm());

		// call cycle() on every component supporting it...
		for (uint8_t i = 0; i < _componentsToCycle_count; i++)
		{
			switch (_componentsToCycle[i]->cycle())
			{
			case Status::Unknown:
			case Status::Missing:
			case Status::Failed:
				failure = true;
				break;
			case Status::Ready:
			default:
				break;
			}
		}

		// check if we need to work out failures...
		if (failure)
		{
			// at least one component failed...
			debugWriteWarning(textClassNameBoardV2R1, __LINE__, textError);

		}

		float    total = 0;
		uint16_t count = 0;
		float p = 0;
		float r = 0;
		float y = 0;
		float x = 0;
		float z = 0;


//		char a[200];
//		char b[64];
		switch (_cycleStage++)
		{
		case 0:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "imus");
			count = 0; p = 0; r = 0; y = 0;
			for (uint8_t i = 0; i < _imus_count; i++)
			{
				count += _imus[i]->accuracy();
				p     += _imus[i]->pitch()   * _imus[i]->accuracy();
				r     += _imus[i]->roll()    * _imus[i]->accuracy();
				y     += _imus[i]->heading() * _imus[i]->accuracy();
			}
			pitch(  (count > 0) ? p / (float)count : (float)0.0);
			roll(   (count > 0) ? BOARD_ATTITUDE_ROLL_OFFSET + (r / (float)count) : (float)0.0);
			heading((count > 0) ? y / (float)count : (float)0.0);
			break;
		case 1:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "gyroscopes");
			count = 0; p = 0; r = 0; y = 0;
//			a[0] = 0;
			for (uint8_t i = 0; i < _gyroscopes_count; i++)
			{
				count += _gyroscopes[i]->accuracy();
				p     += _gyroscopes[i]->pitchRate() * _gyroscopes[i]->accuracy();
				r     += _gyroscopes[i]->rollRate()  * _gyroscopes[i]->accuracy();
				y     += _gyroscopes[i]->yawRate()   * _gyroscopes[i]->accuracy();
			}

			maGyroscopeX.add((count > 0) ? p / (float)count : (float)0.0);
			maGyroscopeY.add((count > 0) ? r / (float)count : (float)0.0);
			maGyroscopeZ.add((count > 0) ? y / (float)count : (float)0.0);
			pitchRate(maGyroscopeX.average());
			rollRate(maGyroscopeY.average());
			yawRate(maGyroscopeZ.average());
//			sprintf(a, "%.4f %.4f %.4f 5 -5\n", pitchRate(), rollRate(), yawRate());
//			mcu()->uart4()->write((uint8_t*)a, strlen(a));
			break;
		case 2:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "accelerometers");
			count = 0; x = 0; y = 0; z = 0;
			for (uint8_t i = 0; i < _accelerometers_count; i++)
			{
				count += _accelerometers[i]->accuracy();
				x     += _accelerometers[i]->accelerationX() * _accelerometers[i]->accuracy();
				y     += _accelerometers[i]->accelerationY() * _accelerometers[i]->accuracy();
				z     += _accelerometers[i]->accelerationZ() * _accelerometers[i]->accuracy();
			}
			accelerationX((count > 0) ? x / (float)count : (float)0.0);
			accelerationY((count > 0) ? y / (float)count : (float)0.0);
			accelerationZ((count > 0) ? z / (float)count : (float)0.0);
			break;
		case 3:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "magnetometers");
			count = 0; x = 0; y = 0; z = 0;
			for (uint8_t i = 0; i < _magnetometers_count; i++)
			{
				count += _magnetometers[i]->accuracy();
				x     += _magnetometers[i]->fieldX() * _magnetometers[i]->accuracy();
				y     += _magnetometers[i]->fieldY() * _magnetometers[i]->accuracy();
				z     += _magnetometers[i]->fieldZ() * _magnetometers[i]->accuracy();
			}
			fieldX((count > 0) ? x / (float)count : (float)0.0);
			fieldY((count > 0) ? y / (float)count : (float)0.0);
			fieldZ((count > 0) ? z / (float)count : (float)0.0);
			break;
		case 4:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "linearaccelerometers");
			count = 0; x = 0; y = 0; z = 0;
			for (uint8_t i = 0; i < _linearAccelerometers_count; i++)
			{
				count += _linearAccelerometers[i]->accuracy();
				x     += _linearAccelerometers[i]->linearAccelerationX() * _linearAccelerometers[i]->accuracy();
				y     += _linearAccelerometers[i]->linearAccelerationY() * _linearAccelerometers[i]->accuracy();
				z     += _linearAccelerometers[i]->linearAccelerationZ() * _linearAccelerometers[i]->accuracy();
			}
			linearAccelerationX((count > 0) ? x / (float)count : (float)0.0);
			linearAccelerationY((count > 0) ? y / (float)count : (float)0.0);
			linearAccelerationZ((count > 0) ? z / (float)count : (float)0.0);
			break;
		case 5:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "gravitometers");
			count = 0; x = 0; y = 0; z = 0;
			for (uint8_t i = 0; i < _gravitometers_count; i++)
			{
				count += _gravitometers[i]->accuracy();
				x     += _gravitometers[i]->gravityX() * _gravitometers[i]->accuracy();
				y     += _gravitometers[i]->gravityY() * _gravitometers[i]->accuracy();
				z     += _gravitometers[i]->gravityZ() * _gravitometers[i]->accuracy();
			}
			gravityX((count > 0) ? x / (float)count : (float)0.0);
			gravityY((count > 0) ? y / (float)count : (float)0.0);
			gravityZ((count > 0) ? z / (float)count : (float)0.0);
			break;
		case 6:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "thermometers");
			count = 0; total = 0;
			for (uint8_t i = 0; i < _thermometers_count; i++)
			{
				count += _thermometers[i]->accuracy();
				total += _thermometers[i]->temperature() * _thermometers[i]->accuracy();
			}
			temperature((count > 0) ? total / (float)count : (float)0.0);
			break;
		case 7:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "barometers");
			count = 0; total = 0;
			for (uint8_t i = 0; i < _barometers_count; i++)
			{
				count += _barometers[i]->accuracy();
				total += _barometers[i]->barometricPressure() * _barometers[i]->accuracy();
			}
			barometricPressure((count > 0) ? total / (float)count : (float)0.0);
			break;
		case 8:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "hygrometers");
			count = 0; total = 0;
			for (uint8_t i = 0; i < _hygrometers_count; i++)
			{
				count += _hygrometers[i]->accuracy();
				total += _hygrometers[i]->humidity() * _hygrometers[i]->accuracy();
			}
			humidity((count > 0) ? total / (float)count : (float)0.0);
			break;
		case 9:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "gpss");
			for (uint8_t i = 0; i < _gpss_count; i++)
			{
				latitude(_gpss[i]->latitude());
				longitude(_gpss[i]->longitude());
				gpsAltitude(_gpss[i]->gpsAltitude());
			}
			break;
		case 10:
			debugWriteTrace(textClassNameBoardV2R1, __LINE__, "rangedaltitude");
			for (uint8_t i = 0; i < _rangedaltitude_count; i++)
			{
				rangedAltitude(_rangedaltitude[i]->rangedAltitude());
			}
			break;
		default:
			_cycleStage = 0;
			break;
		}



		// servo control range seems to be around 95 to 715
		//
		//const uint16_t min_pca = 95;
		//const uint16_t max_pca = 715;
		//
		return readyNoSet(textClassNameBoardV2R1, __LINE__);
	}
private:
	void createInternalComponents()
	{
		// add the components to the setup list...
		enqueueComponentForSetup(_a3g4250d);
		enqueueComponentForSetup(_lis2hh12);
		enqueueComponentForSetup(_asm330lhxxh);
		enqueueComponentForSetup(_iis2mdc);
		enqueueComponentForSetup(_ilps22qs);
		enqueueComponentForSetup(_pca9685pw);
	}
	void enqueueComponentForSetup(HasStatusAndSetup* component)
	{
		_componentsToSetup[_componentsToSetup_count++] = component;
	}
	void enqueueComponentForCycle(HasStatusAndSetupAndCycle* component)
	{
		_componentsToCycle[_componentsToCycle_count++] = component;
	}
	template <typename T>
	void buildComponentsRequiringSetup(T const & component)
	{
		auto hasSetup = dynamic_cast<HasStatusAndSetup*>(component);
		if (hasSetup)
			enqueueComponentForSetup(hasSetup);
	}
	template <typename T>
	void buildComponentLists(T const & component)
	{
		if (auto hasCyle = dynamic_cast<HasStatusAndSetupAndCycle*>(component))
			enqueueComponentForCycle(hasCyle);

		if (auto temp = dynamic_cast<RcRx*>(component))
		{
			if (_rcrxA == NULL)
			{
				_rcrxA = temp;
			}
			else if (_rcrxB == NULL)
			{
				_rcrxB = temp;
			}
			else if (_rcrxC == NULL)
			{
				_rcrxC = temp;
			}
		}

		if (auto temp = dynamic_cast<PwmDriver*>(component))
		{
			if (_pwm == NULL)
				_pwm = temp;
			else
				fail(2);
		}

		if (auto temp = dynamic_cast<Gyroscope*>(component))
			_gyroscopes[_gyroscopes_count++] = temp;

		if (auto temp = dynamic_cast<Acceleration*>(component))
			_accelerometers[_accelerometers_count++] = temp;

		if (auto temp = dynamic_cast<LinearAcceleration*>(component))
			_linearAccelerometers[_linearAccelerometers_count++] = temp;

		if (auto temp = dynamic_cast<Magnetometer*>(component))
			_magnetometers[_magnetometers_count++] = temp;

		if (auto temp = dynamic_cast<BarometricPressure*>(component))
			_barometers[_barometers_count++] = temp;

		if (auto temp = dynamic_cast<GravityAcceleration*>(component))
			_gravitometers[_gravitometers_count++] = temp;

		if (auto temp = dynamic_cast<Temperature*>(component))
			_thermometers[_thermometers_count++] = temp;

		if (auto temp = dynamic_cast<Humidity*>(component))
			_hygrometers[_hygrometers_count++] = temp;

		if (auto temp = dynamic_cast<Gps*>(component))
		{
			_gpss[_gpss_count++] = temp;
			if (_gpsA == NULL)
				_gpsA = temp;
		}

		if (auto temp = dynamic_cast<Heading*>(component))
			_compasses[_compasses_count++] = temp;

		if (auto temp = dynamic_cast<HeadingPitchRoll*>(component))
			_imus[_imus_count++] = temp;

		if (auto temp = dynamic_cast<RangedAltitude*>(component))
			_rangedaltitude[_rangedaltitude_count++] = temp;


	}
	void indicateStartupSequence()
	{
		indicatorActivity()->off();
		indicatorWarning()->off();
		indicatorError()->off();
		HAL_Delay(100);
		for (uint8_t i = 0; i < 5; i++)
		{
			indicatorActivity()->on();
			indicatorWarning()->on();
			indicatorError()->on();
			HAL_Delay(10);
			indicatorActivity()->off();
			indicatorWarning()->off();
			indicatorError()->off();
			HAL_Delay(100);
		}
		indicatorActivity()->on();
		indicatorWarning()->on();
		indicatorError()->off();
	}
};

#endif /* BOARD_V2R1_H_ */
