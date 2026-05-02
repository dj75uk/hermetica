/*
 * storage.hpp
 *
 *  Created on: Oct 17, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_STORAGE_HPP_
#define APP_INC_STORAGE_HPP_

#include <cstdint>

class Storage
{
public:
	virtual bool read(const uint32_t address, const uint8_t* data)
	{
		UNUSED(address);
		UNUSED(data);
		return false;
	}
	virtual bool read(const uint32_t address, const uint8_t* data, const uint16_t length)
	{
		UNUSED(address);
		UNUSED(data);
		UNUSED(length);
		return false;
	}
	virtual bool verify(const uint32_t address, const uint8_t expected)
	{
		UNUSED(address);
		UNUSED(expected);
		return false;
	}
	virtual bool write(const uint32_t address, const uint8_t data)
	{
		UNUSED(address);
		UNUSED(data);
		return false;
	}
	virtual bool write(const uint32_t address, const uint8_t* data, const uint16_t length)
	{
		UNUSED(address);
		UNUSED(data);
		UNUSED(length);
		return false;
	}
};

#endif /* APP_INC_STORAGE_HPP_ */
