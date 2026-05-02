/*
 * inputdata.hpp
 *
 *  Created on: Oct 24, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_INPUT_HPP_
#define APP_INC_INPUT_HPP_

#include <cstdint>

#define DATUMS_PER_COMPONENT    24
#define DATUMS                  (4 * DATUMS_PER_COMPONENT)
#define USAGEDATUMS				((uint8_t)InputUsage::LastValueNotToBeUsed)

#define DATAFLAG_INVERT			0b00000001
#define DATAFLAG_OVERRIDABLE	0b00000010
#define DATAFLAG_LINKLOST		0b01000000
#define DATAFLAG_DIRTY			0b10000000

enum class InputSource : uint8_t
{
	Source1 = (0 * DATUMS_PER_COMPONENT),
	Source2 = (1 * DATUMS_PER_COMPONENT),
	Source3 = (2 * DATUMS_PER_COMPONENT),
	Source4 = (3 * DATUMS_PER_COMPONENT)
};
enum class InputState : uint8_t
{
	Unknown = 0,
	Low     = 1,
	Centre  = 2,
	High    = 3
};

enum class InputType : uint8_t
{
	Unknown  = 0,
	Analogue = 1,
	Digital2 = 2,
	Digital3 = 3
};

enum class InputUsage : uint8_t
{
	Unknown = 0,
	Throttle,
	Pitch,
	Roll,
	Yaw,
	Rssi,
	ThrottleDifferential,
	Override,
	FcuMode,

	Aux01,
	Aux02,
	Aux03,
	Aux04,
	Aux05,
	Aux06,
	Aux07,
	Aux08,
	Aux09,
	Aux10,
	Aux11,
	Aux12,
	Aux13,
	Aux14,
	Aux15,
	Aux16,

	LastValueNotToBeUsed
};

class InputDatum
{
public:
	//
	uint32_t rawReceived;
	// 000
	uint16_t rawValue;
	uint16_t rawRangeLow;
	// 032
	uint16_t rawRangeHigh;
	int16_t  normalisedValue;
	// 064
	uint8_t  inputType;
	uint8_t  inputUsage;
	uint8_t  inputState;
	uint8_t  deadzoneSizeLow;
	// 096
	uint8_t  deadzoneSizeCentre;
	uint8_t  deadzoneSizeHigh;
	uint8_t  flags;
	uint8_t  padding;
	// 128

	InputDatum()
	{
		this->rawReceived        = 0;
		this->rawValue           = 0;
		this->rawRangeLow        = 0;
		this->rawRangeHigh       = 0;
		this->normalisedValue    = 0;
		this->inputType          = 0;
		this->inputUsage         = 0;
		this->inputState         = 0;
		this->deadzoneSizeLow    = 0;
		this->deadzoneSizeCentre = 0;
		this->deadzoneSizeHigh   = 0;
		this->flags              = 0;
		this->padding            = 0;
	}
	InputDatum(uint16_t rawRangeLow, uint16_t rawRangeHigh, InputType inputType, InputUsage inputUsage, uint8_t deadzoneSizeLow, uint8_t deadzoneSizeCentre, uint8_t deadzoneSizeHigh, bool invert, bool overridable)
	{
		this->rawReceived        = 0;
		this->rawValue           = 0;
		this->rawRangeLow        = rawRangeLow;
		this->rawRangeHigh       = rawRangeHigh;
		this->normalisedValue    = 0;
		this->inputType          = (uint8_t)inputType;
		this->inputUsage         = (uint8_t)inputUsage;
		this->inputState         = 0;
		this->deadzoneSizeLow    = deadzoneSizeLow;
		this->deadzoneSizeCentre = deadzoneSizeCentre;
		this->deadzoneSizeHigh   = deadzoneSizeHigh;
		this->flags              = 0;
		this->padding            = 0;

		if (invert)      this->flags = this->flags | DATAFLAG_INVERT;
		if (overridable) this->flags = this->flags | DATAFLAG_OVERRIDABLE;
	}
//	void clearDynamic()
//	{
//		this->rawValue        = 0;
//		this->normalisedValue = 0;
//		this->inputState      = 0;
//	}
	void clear()
	{
		this->rawValue           = 0;
		this->rawRangeLow        = 0;
		this->rawRangeHigh       = 0;
		this->normalisedValue    = 0;
		this->inputType          = 0;
		this->inputUsage         = 0;
		this->inputState         = 0;
		this->deadzoneSizeLow    = 0;
		this->deadzoneSizeCentre = 0;
		this->deadzoneSizeHigh   = 0;
		this->flags              = 0;
		this->padding            = 0;
	}
};

class Input
{
public:
	int16_t normalisedValue;
	uint8_t datumId;
	uint8_t inputState;
	Input()
	{
		this->normalisedValue = 0;
		this->datumId         = 0;
		this->inputState      = 0;
	}
	Input(uint16_t normalisedValue, uint8_t datumId)
	{
		this->normalisedValue = normalisedValue;
		this->datumId         = datumId;
		this->inputState      = 0;
	}
};

#endif /* APP_INC_INPUT_HPP_ */
