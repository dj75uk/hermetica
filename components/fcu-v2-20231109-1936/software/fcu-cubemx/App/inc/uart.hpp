/*
 * uart.h
 *
 *  Created on: Oct 4, 2023
 *      Author: Daniel Johnson
 */
#ifndef UART_H_
#define UART_H_

#include <cstdint>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_uart.h"
#include "bytedata.hpp"
#include "ringbuffer.hpp"

extern "C" uint8_t interruptGetRxData(const uint8_t id);
extern "C" uint8_t* interruptGetRxBufferPointer(const uint8_t id);
extern "C" void interruptStartUartReceive(UART_HandleTypeDef* huart, uint8_t instanceId);

extern "C"
{
	// these queues are used by the uart class for reading...
	// there are 12 queues, one for each uart...
	jnk0le::Ringbuffer<ByteData, 256> interruptRxRingBuffer[12];

	// this is called from an interrupt handler...
	//
	// stm32h7xx_it.c:
	//   void USART10_IRQHandler()
	// uart.hpp:
	//     uartRxHandler(&huart10, 10)
	//
	void uartRxHandler(UART_HandleTypeDef* huart, uint8_t instanceId)
	{
		uint32_t entry = HAL_GetTick();
		if (huart->ErrorCode == 0)
		{
			// get the received byte...
			uint8_t receivedByte = interruptGetRxData(instanceId);

			ByteData data;
			data.data     = receivedByte;
			data.received = entry;

			// store the received byte in a queued buffer...
			interruptRxRingBuffer[instanceId].insert(data);
		}
		// wait for more data...
		interruptStartUartReceive(huart, instanceId);
	}
}

class Uart
{
private:
	UART_HandleTypeDef* _hal;
	uint8_t             _instanceId;
public:
	Uart(UART_HandleTypeDef* hal, uint8_t instanceId)
	{
		_hal        = hal;
		_instanceId = instanceId;
		interruptStartUartReceive(hal, instanceId);
	}
	bool available()
	{
		return interruptRxRingBuffer[_instanceId].readAvailable();
	}
	ByteData read()
	{
		ByteData result;
		if (interruptRxRingBuffer[_instanceId].readAvailable())
		{
			if (interruptRxRingBuffer[_instanceId].remove(&result))
				return result;
		}
		return result;
	}
	int write(uint8_t* buffer, uint16_t length)
	{
		return HAL_UART_Transmit(_hal, buffer, length, 10);
	}
	uint8_t instanceId() const
	{
		return _instanceId;
	}
	UART_HandleTypeDef* instance() const
	{
		return _hal;
	}
};

#endif /* UART_H_ */
