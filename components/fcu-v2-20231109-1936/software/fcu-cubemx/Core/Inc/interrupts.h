/*
 * interrupts.h
 *
 *  Created on: Oct 25, 2023
 *      Author: Daniel Johnson
 */
#ifndef INC_INTERRUPTS_H_
#define INC_INTERRUPTS_H_

#include "stdint.h"
#include "stm32h7xx_hal_uart.h"

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

uint32_t HAL_GetHighResolutionTick();

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

#define UART_RX_BUFFER_COUNT 			12

void interruptSetRxData(const uint8_t id, const uint8_t value);
uint8_t interruptGetRxData(const uint8_t id);
uint8_t* interruptGetRxBufferPointer(const uint8_t id);
void interruptStartUartReceive(UART_HandleTypeDef* huart, uint8_t instanceId);

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

#define INTERRUPTS_COUNT 				16

void interruptSetRisingEdge(uint8_t id, uint16_t value);
uint16_t interruptGetRisingEdge(uint8_t id);
void interruptSetFallingEdge(uint8_t id, uint16_t value);
uint16_t interruptGetFallingEdge(uint8_t id);
void interruptExternalSet(uint8_t id);
void interruptExternalReset(uint8_t id);
uint8_t interruptExternalTestAndResetIfSet(uint8_t id);

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

#define DEBUG_TX_BUFFER_LENGTH 			512
#define DEBUG_TX_TIMEOUT				100
#define DEBUG_FORMAT_TEXT				"%c: %s:%lu: %s\n"

void debugSetUart(UART_HandleTypeDef* outputUart);
void debugWritePlot(const char* data);
void debugWriteTrace(const char* className, const uint32_t lineNumber, const char* data);
void debugWriteInfo(const char* className, const uint32_t lineNumber, const char* data);
void debugWriteWarning(const char* className, const uint32_t lineNumber, const char* data);
void debugWriteError(const char* className, const uint32_t lineNumber, const char* data);
void debugEnableTrace();
void debugDisableTrace();

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void sprintf_binary8(char* s, uint8_t data);
void sprintf_binary16(char* s, uint16_t data);

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

#endif /* INC_INTERRUPTS_H_ */
