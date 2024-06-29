/*
 * UART.cpp
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#include "UART.h"
#include "Debug.h"
#include <cstdio>

std::vector<UART*> UART::_uarts {};

UART::UART(UART_HandleTypeDef* handle, RxCallack rxCallback) : handle{handle}, rxCallback{rxCallback}
{
	_uarts.push_back(this);
	uartLock.give();
}

UART* UART::findUARTByHandle(UART_HandleTypeDef* handle)
{
	for (auto uart : _uarts)
	{
		if (uart->getHandle() == handle || handle == nullptr)
			return uart;
	}

	return nullptr;
}

extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	UART::RxEventCallback(huart, Size);
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UART::TxCpltCallback(huart);
}

extern "C" int _write(int file, char *ptr, int len)
{
//	UART* uart = UART::findUARTByHandle(nullptr);
//	uart->transmit(reinterpret_cast<const uint8_t*>(ptr), len);
	ptr[len] = 0;
	Debug::getInstance().send(ptr);
	return len;
}

void UART::RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	UART* uart = UART::findUARTByHandle(huart);
	uart->startReceiving();
	uart->rxCallback(reinterpret_cast<char*>(huart->pRxBuffPtr));
}

void UART::TxCpltCallback(UART_HandleTypeDef *huart)
{
	UART* uart = UART::findUARTByHandle(huart);
	uart->txLock.give();
}

void UART::startReceiving()
{
	HAL_UARTEx_ReceiveToIdle_DMA(handle, dataBuffer, sizeof(dataBuffer));
}

void UART::transmit(const char* message)
{
	transmit(reinterpret_cast<const uint8_t*>(message), strlen(message));
}

void UART::transmit(const uint8_t* message, size_t len)
{
	uartLock.take();
	HAL_StatusTypeDef result;
	do
	{
		result = HAL_UART_Transmit_DMA(handle, message, len);
		if (result != HAL_OK)
			osThreadYield();
	} while (result == HAL_BUSY);

	if (result == HAL_OK)
		txLock.take();
	uartLock.give();
}
