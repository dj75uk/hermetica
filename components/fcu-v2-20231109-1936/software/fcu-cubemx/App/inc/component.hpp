/*
 * component.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_COMPONENT_HPP_
#define APP_INC_COMPONENT_HPP_

enum class Status
{
	Unknown,
	Missing,
	Failed,
	Ready
};

#include <cstdint>
#include "debug.hpp"

extern const char* textOk;
extern const char* textError;

class HasStatusAndSetup
{
private:
	Status _status = Status::Unknown;
protected:
	Status status(const Status value)
	{
		_status = value;
		return _status;
	}
	Status missing(const char* className, const uint32_t useLineMacroHere)
	{
		debugWriteWarning(className, useLineMacroHere, textError);
		_status = Status::Missing;
		return Status::Missing;
	}
	Status failed(const char* className, const uint32_t useLineMacroHere)
	{
		debugWriteError(className, useLineMacroHere, textError);
		_status = Status::Failed;
		return Status::Failed;
	}
	Status readyWithSet(const char* className, const uint32_t useLineMacroHere)
	{
		debugWriteInfo(className, useLineMacroHere, textOk);
		_status = Status::Ready;
		return Status::Ready;
	}
	Status readyNoSet(const char* className, const uint32_t useLineMacroHere)
	{
		debugWriteTrace(className, useLineMacroHere, textOk);
		return Status::Ready;
	}
	Status failedNoSet(const char* className, const uint32_t useLineMacroHere)
	{
		debugWriteTrace(className, useLineMacroHere, textError);
		return Status::Ready;
	}
public:
	HasStatusAndSetup()
	{
		_status = Status::Unknown;
	}
	virtual ~HasStatusAndSetup()
	{
	}
	Status status() const
	{
		return _status;
	}
	virtual Status setup() = 0;
};

class HasStatusAndSetupAndCycle : public HasStatusAndSetup
{
public:
	virtual Status cycle() = 0;
};

#endif /* APP_INC_COMPONENT_HPP_ */
