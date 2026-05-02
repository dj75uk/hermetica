/*
 * interrupts.c
 *
 *  Created on: Oct 25, 2023
 *      Author: Daniel Johnson
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdatomic.h>
#include "stm32h7xx.h"
#include "stm32h7xx_it.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_uart.h"
#include "interrupts.h"

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

extern TIM_HandleTypeDef htim3;

uint32_t HAL_GetHighResolutionTick(void)
{
	return htim3.Instance->CNT;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

#define UART_RX_BUFFER_COUNT 12

// these buffers are used for the actual uart-receives to write to...
// there are 12 single-byte buffers, one for each uart...
static volatile uint8_t __uartRxInterruptBuffer[UART_RX_BUFFER_COUNT];

void interruptSetRxData(const uint8_t id, const uint8_t value)
{
	if (id < UART_RX_BUFFER_COUNT)
		__uartRxInterruptBuffer[id] = value;
}
uint8_t interruptGetRxData(const uint8_t id)
{
	if (id < UART_RX_BUFFER_COUNT)
		return __uartRxInterruptBuffer[id];
	return 0;
}
uint8_t* interruptGetRxBufferPointer(const uint8_t id)
{
	uint8_t* result = NULL;
	if (id < UART_RX_BUFFER_COUNT)
		result = (uint8_t*)&(__uartRxInterruptBuffer[id]);
	return result;
}
void interruptStartUartReceive(UART_HandleTypeDef* huart, uint8_t instanceId)
{
	// ensure the rx interrupt is enabled...
	huart->Instance->CR1 = huart->Instance->CR1 | USART_CR1_RXNEIE;
	// start the interrupt-based receive...
	HAL_UART_Receive_IT(huart, interruptGetRxBufferPointer(instanceId), 1);
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

#define INTERRUPTS_COUNT 16

static volatile uint8_t  __interruptSet[INTERRUPTS_COUNT];
static volatile uint16_t __interruptRisingEdge[INTERRUPTS_COUNT];
static volatile uint16_t __interruptFallingEdge[INTERRUPTS_COUNT];

void interruptExternalSet(uint8_t id)
{
	if (id < INTERRUPTS_COUNT)
		__interruptSet[id] = 1;
}
void interruptExternalReset(uint8_t id)
{
	if (id < INTERRUPTS_COUNT)
	{
		//__disable_irq();
		__interruptSet[id] = 0;
		//__enable_irq();
	}
}
uint8_t interruptExternalTestAndResetIfSet(uint8_t id)
{
	if (id < INTERRUPTS_COUNT)
	{
		//__disable_irq();
		if (__interruptSet[id] == 1)
		{
			__interruptSet[id] = 0;
			return 1;
		}
		//__enable_irq();
	}
	return 0;
}
void interruptSetRisingEdge(uint8_t id, uint16_t value)
{
	if (id < INTERRUPTS_COUNT)
		__interruptRisingEdge[id] = value;
}
uint16_t interruptGetRisingEdge(uint8_t id)
{
	if (id < INTERRUPTS_COUNT)
		return __interruptRisingEdge[id];
	return 0;
}
void interruptSetFallingEdge(uint8_t id, uint16_t value)
{
	if (id < INTERRUPTS_COUNT)
		__interruptFallingEdge[id] = value;
}
uint16_t interruptGetFallingEdge(uint8_t id)
{
	if (id < INTERRUPTS_COUNT)
		return __interruptFallingEdge[id];
	return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_9)
	{
		uint8_t pin = 9;
		if (HAL_GPIO_ReadPin(GPIOE, GPIO_Pin) == GPIO_PIN_SET)
		{
			// rising edge...
			interruptSetRisingEdge(pin, (uint16_t)HAL_GetHighResolutionTick());
		}
		else
		{
			// falling edge...
			interruptSetFallingEdge(pin, (uint16_t)HAL_GetHighResolutionTick());
			interruptExternalSet(pin);
		}
	}
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static UART_HandleTypeDef* __debugOutputUart = NULL;
static uint8_t             __debugTraceEnabled = 0;
static char                __debugWriteBuffer[DEBUG_TX_BUFFER_LENGTH];

static void __debugWriteRaw(const char* data)
{
	if (__debugOutputUart == NULL)
		return;
	//__disable_irq();
	HAL_UART_Transmit(__debugOutputUart, (uint8_t*)data, (uint16_t)strlen(data), DEBUG_TX_TIMEOUT);
	//__enable_irq();
}
static void __debugWriteFormat(const char level, const char* className, const uint32_t lineNumber, const char* data)
{
	if (__debugOutputUart == NULL)
		return;
	sprintf(__debugWriteBuffer, DEBUG_FORMAT_TEXT, level, className, lineNumber, data);
	__debugWriteRaw(__debugWriteBuffer);
}
void debugSetUart(UART_HandleTypeDef* outputUart)
{
	__debugOutputUart = outputUart;
}
void debugWritePlot(const char* data)
{
	__debugWriteRaw(data);
}
void debugWriteTrace(const char* className, const uint32_t lineNumber, const char* data)
{
	if (__debugTraceEnabled)
		__debugWriteFormat('t', className, lineNumber, data);
}
void debugWriteInfo(const char* className, const uint32_t lineNumber, const char* data)
{
	__debugWriteFormat('i', className, lineNumber, data);
}
void debugWriteWarning(const char* className, const uint32_t lineNumber, const char* data)
{
	__debugWriteFormat('w', className, lineNumber, data);
}
void debugWriteError(const char* className, const uint32_t lineNumber, const char* data)
{
	__debugWriteFormat('e', className, lineNumber, data);
}
void debugEnableTrace()
{
	__debugTraceEnabled = 1;
}
void debugDisableTrace()
{
	__debugTraceEnabled = 0;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void sprintf_binary8(char* s, uint8_t data)
{
	s[0] = (data & 0x80) ? '1' : '0';
	s[1] = (data & 0x40) ? '1' : '0';
	s[2] = (data & 0x20) ? '1' : '0';
	s[3] = (data & 0x10) ? '1' : '0';
	s[4] = (data & 0x08) ? '1' : '0';
	s[5] = (data & 0x04) ? '1' : '0';
	s[6] = (data & 0x02) ? '1' : '0';
	s[7] = (data & 0x01) ? '1' : '0';
	s[8] = 0;
}
void sprintf_binary16(char* s, uint16_t data)
{
	s[0]  = (data & 0x8000) ? '1' : '0';
	s[1]  = (data & 0x4000) ? '1' : '0';
	s[2]  = (data & 0x2000) ? '1' : '0';
	s[3]  = (data & 0x1000) ? '1' : '0';
	s[4]  = (data & 0x0800) ? '1' : '0';
	s[5]  = (data & 0x0400) ? '1' : '0';
	s[6]  = (data & 0x0200) ? '1' : '0';
	s[7]  = (data & 0x0100) ? '1' : '0';
	s[8]  = (data & 0x0080) ? '1' : '0';
	s[9]  = (data & 0x0040) ? '1' : '0';
	s[10] = (data & 0x0020) ? '1' : '0';
	s[11] = (data & 0x0010) ? '1' : '0';
	s[12] = (data & 0x0008) ? '1' : '0';
	s[13] = (data & 0x0004) ? '1' : '0';
	s[14] = (data & 0x0002) ? '1' : '0';
	s[15] = (data & 0x0001) ? '1' : '0';
	s[16] = 0;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
