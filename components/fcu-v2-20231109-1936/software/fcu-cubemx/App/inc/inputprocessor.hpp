/*
 * inputprocessor.hpp
 *
 *  Created on: Oct 24, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_INPUTPROCESSOR_HPP_
#define APP_INC_INPUTPROCESSOR_HPP_

#include <cstdio>
#include <cstdint>
#include <string>
#include "input.hpp"
#include "func.hpp"

class InputProcessor
{
private:
	const char* className = "inputprocessor";
	InputDatum* _inputData;
	Input*      _inputs;
	bool        _overriding = false;
public:
	InputProcessor(InputDatum* inputData, Input* inputs)
	{
		_inputData  = inputData;
		_inputs     = inputs;
		_overriding = false;
	}
	Input* input(const InputUsage inputUsage) const
	{
		uint8_t usageId = (uint8_t)inputUsage;
		if (usageId < USAGEDATUMS)
			return &_inputs[usageId];
		return NULL;
	}
	uint16_t value(const InputUsage inputUsage) const
	{
		uint8_t usageId = (uint8_t)inputUsage;
		if (usageId < USAGEDATUMS)
			return _inputs[usageId].normalisedValue;
		return 0;
	}
	InputState state(const InputUsage inputUsage) const
	{
		uint8_t usageId = (uint8_t)inputUsage;
		if (usageId < USAGEDATUMS)
			return (InputState)_inputs[usageId].inputState;
		return InputState::Low;
	}
	bool overriding() const
	{
		return _overriding;
	}
	void cycle()
	{
		uint32_t timeout = HAL_GetTick() - 1000;

		for (uint8_t datumId = 0; datumId < DATUMS; datumId++)
		{
			// check this entry is valid...
			uint16_t rawRangeLow  = _inputData[datumId].rawRangeLow;
			uint16_t rawRangeHigh = _inputData[datumId].rawRangeHigh;
			if (rawRangeLow == rawRangeHigh)
				continue;

			// check the time the datum was received...
			if ((_inputData[datumId].rawReceived < timeout))
			{
				if (_inputData[datumId].rawValue != 0)
				{
					_inputData[datumId].rawValue = 0;
				}
				_inputData[datumId].normalisedValue = 0;
				_inputData[datumId].inputState      = (uint8_t)InputState::Unknown;
				continue;
			}

			// check the dirty flag...
			if ((_inputData[datumId].flags & DATAFLAG_DIRTY) == 0)
				continue;

			// reset the dirty flag...
			_inputData[datumId].flags = _inputData[datumId].flags & ~DATAFLAG_DIRTY;

			// get the raw value...
			int16_t raw = _inputData[datumId].rawValue;
			if (raw == 0)
			{
				_inputData[datumId].normalisedValue = 0;
				_inputData[datumId].inputState      = (uint8_t)InputState::Unknown;
				continue;
			}

			// ensure the value is trimmed to be within the expected range...
			raw = bandpass<int16_t>(raw, rawRangeLow, rawRangeHigh);

			// scale the value to the standard range...
			raw = rescale<int16_t>(raw, rawRangeLow, rawRangeHigh, RCRX_MINIMUM_VALUE, RCRX_MAXIMUM_VALUE);

			// apply low dead-zone if configured...
			uint8_t deadzoneSizeLow  = _inputData[datumId].deadzoneSizeLow;
			if (deadzoneSizeLow > 0)
			{
				// low dead-zone...
				if (raw < (RCRX_MINIMUM_VALUE + deadzoneSizeLow))
					raw = RCRX_MINIMUM_VALUE;
			}

			// apply centre dead-zone if configured...
			uint8_t deadzoneSizeCentre  = _inputData[datumId].deadzoneSizeCentre;
			if (deadzoneSizeCentre > 0)
			{
				// centre dead-zone...
				if (raw > (RCRX_CENTRE_VALUE - deadzoneSizeCentre))
					if (raw < (RCRX_CENTRE_VALUE + deadzoneSizeCentre))
						raw = RCRX_CENTRE_VALUE;
			}

			// apply high dead-zone if configured...
			uint8_t deadzoneSizeHigh  = _inputData[datumId].deadzoneSizeHigh;
			if (deadzoneSizeHigh > 0)
			{
				// high dead-zone...
				if (raw > (RCRX_MAXIMUM_VALUE - deadzoneSizeHigh))
					raw = RCRX_MAXIMUM_VALUE;
			}

			uint8_t inputType  = _inputData[datumId].inputType;
			if (inputType == (uint8_t)InputType::Digital2)
			{
				if (raw > RCRX_CENTRE_VALUE)
					raw = RCRX_MAXIMUM_VALUE;
				else
					raw = RCRX_MINIMUM_VALUE;
			}
			if (inputType == (uint8_t)InputType::Digital3)
			{
				if (raw > (RCRX_MAXIMUM_VALUE - 200))
					raw = RCRX_MAXIMUM_VALUE;
				else if (raw < (RCRX_MINIMUM_VALUE + 200))
					raw = RCRX_MINIMUM_VALUE;
				else
					raw = RCRX_CENTRE_VALUE;
			}

			// apply value-inversion if configured...
			if (_inputData[datumId].flags & DATAFLAG_INVERT)
			{
				raw = (RCRX_MAXIMUM_VALUE - raw) + RCRX_MINIMUM_VALUE;
			}

			// set the channel value ensuring it is limited to the standard range...
			raw = bandpass<int16_t>(raw, RCRX_MINIMUM_VALUE, RCRX_MAXIMUM_VALUE);

			// set the channel...
			_inputData[datumId].normalisedValue = normalise(raw);

			// set the state...
			if (raw > (RCRX_MAXIMUM_VALUE - 200))
				_inputData[datumId].inputState = (uint8_t)InputState::High;
			else if (raw < (RCRX_MINIMUM_VALUE + 200))
				_inputData[datumId].inputState = (uint8_t)InputState::Low;
			else
				_inputData[datumId].inputState = (uint8_t)InputState::Centre;

			// set the override flag...
			if (_inputData[datumId].inputUsage == (uint8_t)InputUsage::Override)
			{
				if (_inputData[datumId].inputState == (uint8_t)InputState::High)
				{
					_overriding = true;
				}
				else
				{
					_overriding = false;
				}
			}
		}

		// clear the "outputs"...
		for (int usageId = 0; usageId < USAGEDATUMS; usageId++)
		{
			_inputs[usageId].normalisedValue = 0;
			_inputs[usageId].datumId         = 0xFF;
			_inputs[usageId].inputState      = (uint8_t)InputState::Unknown;
		}

		// create the "outputs"...
		for (uint8_t datumId = 0; datumId < DATUMS; datumId++)
		{
			uint8_t usageId = (uint8_t)_inputData[datumId].inputUsage;

			if (usageId == 0)
				continue;
			if (usageId >= USAGEDATUMS)
				continue;
			if (_inputData[datumId].rawValue == 0)
				continue;
			if (_inputData[datumId].rawRangeLow == _inputData[datumId].rawRangeHigh)
				continue;

			bool overwrite = true;

			if (_inputs[usageId].datumId != 0xFF)
			{
				// we already have a value for the usageId...
				overwrite = false;

				// is overriding currently enabled?
				if (_overriding)
				{
					uint8_t existingDatumId = _inputs[usageId].datumId;
					uint8_t existingFlags   = _inputData[existingDatumId].flags;

					// does the existing input have the overridable flag set?
					if (existingFlags & DATAFLAG_OVERRIDABLE)
					{
						overwrite = true;
					}
				}
			}
			if (overwrite)
			{
				_inputs[usageId].normalisedValue = _inputData[datumId].normalisedValue;
				_inputs[usageId].datumId         = datumId;
				_inputs[usageId].inputState      = _inputData[datumId].inputState;
			}
		}

//		for (int i = DATUM_OFFSET_SOURCE_1; i < (DATUM_OFFSET_SOURCE_1 + 6); i++)
//		{
//			if (_inputData[i].rawRangeHigh == 0)
//				continue;
//
//			char s[200];
//			sprintf(s, "DatumId: %02d %08lu %04u %04u %04u %02u %02u %03u %03u %03u -> %04u %u",
//				i,
//				_inputData[i].rawReceived,
//				_inputData[i].rawRangeLow,
//				_inputData[i].rawValue,
//				_inputData[i].rawRangeHigh,
//				(uint8_t)_inputData[i].inputType,
//				(uint8_t)_inputData[i].inputUsage,
//				_inputData[i].deadzoneSizeLow,
//				_inputData[i].deadzoneSizeCentre,
//				_inputData[i].deadzoneSizeHigh,
//				_inputData[i].normalisedValue,
//				(uint8_t)_inputData[i].inputState
//			);
//			//debugWriteInfo(className, __LINE__, s);
//		}
//		for (int i = 0; i < USAGEDATUMS; i++)
//		{
//			if (_inputs[i].datumId == 0xFF)
//				continue;
//
//			char s[64];
//			sprintf(s, "UsageId: %02d %04u %02u %02u",
//				i,
//				_inputs[i].normalisedValue,
//				_inputs[i].inputState,
//				_inputs[i].datumId
//			);
//			debugWriteInfo(className, __LINE__, s);
//		}

//		char s[200];
//		sprintf(s, "outputs:");
//		char t[32];
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::Throttle].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::Yaw].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::Pitch].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::Roll].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::Override].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::FcuMode].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::ThrottleDifferential].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " %+04d", _inputs[(uint8_t)InputUsage::Aux01].normalisedValue);
//		strcat(s, t);
//		sprintf(t, " OR:%d", overriding() ? 1 : 0);
//		strcat(s, t);
//		debugWriteInfo(className, __LINE__, s);
	}
};

#endif /* APP_INC_INPUTPROCESSOR_HPP_ */
