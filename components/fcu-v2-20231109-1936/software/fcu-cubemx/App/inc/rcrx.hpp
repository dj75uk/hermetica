/*
 * rcrx.h
 *
 *  Created on: Oct 4, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_RCRX_HPP_
#define APP_INC_RCRX_HPP_

#include <cstdint>
#include "component.hpp"
#include "input.hpp"
#include "macros.hpp"

#define RCRX_CHANNELS                   DATUMS_PER_COMPONENT
#define RCRX_NO_VALUE                   0
#define RCRX_MINIMUM_VALUE              1000
#define RCRX_CENTRE_VALUE               1500
#define RCRX_MAXIMUM_VALUE              2000
#define RCRX_FAILSAFE_RESET_DELAY_MS    250

#define RCRX_RSSI_MINIMUM				0
#define RCRX_RSSI_DEFAULT				50
#define RCRX_RSSI_MAXIMUM				100
#define RCRX_RSSI_LINK_LOST			    RCRX_RSSI_MINIMUM
#define RCRX_RSSI_LINK_LEVEL			(RCRX_RSSI_LINK_LOST+1)



class RcRx : public HasStatusAndSetupAndCycle
{
private:
	InputDatum* _config;
	uint8_t     _offset;
	uint8_t     _rssi;
	bool        _link;
protected:
	RcRx(InputDatum* config, InputSource source)
	{
		_config = config;
		_offset = (uint8_t)source;
		_rssi   = RCRX_RSSI_LINK_LOST;
		_link   = false;
		for (uint8_t id = 0; id < RCRX_CHANNELS; id++)
			_config[id + _offset].rawValue = RCRX_NO_VALUE;
	}
	void rawValue(const uint8_t id, const uint16_t value)
	{
		if (id >= RCRX_CHANNELS)
			return;

		uint32_t entry = HAL_GetTick();
		uint8_t index = id + _offset;
		if (link())
		{
			uint16_t existingValue = _config[index].rawValue;
			if (existingValue == value)
				return;
			_config[index].rawReceived  = entry;
			_config[index].rawValue     = value;
			_config[index].flags       |= DATAFLAG_DIRTY;
			_config[index].flags       &= ~DATAFLAG_LINKLOST;
		}
		else
		{
			_config[index].rawReceived  = entry;
			_config[index].rawValue     = 0;
			_config[index].flags       |= DATAFLAG_DIRTY;
			_config[index].flags       |= DATAFLAG_LINKLOST;
		}
	}
	void reset()
	{
//	    for (uint8_t id = 0; id < RCRX_CHANNELS; id++)
//			_config[id + _configOffset].rawValue = RCRX_NO_VALUE;
	}
	void rssi(const uint8_t value)
	{
		_rssi = LIMIT(value, RCRX_RSSI_MINIMUM, RCRX_RSSI_MAXIMUM);
	}
	void link(const bool value)
	{
		_link = value;
	}
	uint16_t rawValue(const uint8_t id) const
	{
		if (id < RCRX_CHANNELS)
			return _config[id + _offset].rawValue;
		return RCRX_NO_VALUE;
	}
public:
	virtual ~RcRx()
	{
	}
	uint16_t channel(const uint8_t id) const
	{
		if (id < RCRX_CHANNELS)
			return _config[id + _offset].normalisedValue;
		return RCRX_NO_VALUE;
	}
	InputUsage usage(const uint8_t id) const
	{
		if (id < RCRX_CHANNELS)
			return (InputUsage)_config[id + _offset].inputUsage;
		return InputUsage::Unknown;
	}
	uint8_t rssi() const
	{
		return _rssi;
	}
	bool link() const
	{
		return _link;
	}
};

#endif /* APP_INC_RCRX_HPP_ */
