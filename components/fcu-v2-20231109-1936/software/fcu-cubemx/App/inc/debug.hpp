/*
 * debug.h
 *
 *  Created on: Oct 5, 2023
 *      Author: Daniel Johnson
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

	extern void debugEnableTrace();
	extern void debugDisableTrace();
	extern void debugWriteTrace(const char* className, const uint32_t lineNumber, const char* data);
	extern void debugWriteInfo(const char* className, const uint32_t lineNumber, const char* data);
	extern void debugWriteWarning(const char* className, const uint32_t lineNumber, const char* data);
	extern void debugWriteError(const char* className, const uint32_t lineNumber, const char* data);
	extern void sprintf_binary8(char* s, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */
