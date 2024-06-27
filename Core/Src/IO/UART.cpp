/*
 * UART.cpp
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#include "UART.h"
#include <cstdio>

std::vector<UART*> UART::_uarts {};

UART::UART(UART_HandleTypeDef* handle, RxCallack rxCallback) : handle{handle}, rxCallback{rxCallback}
{
	_uarts.push_back(this);
}

UART* UART::findUARTByHandle(UART_HandleTypeDef* handle)
{
	for (auto uart : _uarts)
	{
		if (uart->getHandle() == handle)
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
	HAL_StatusTypeDef result;
	do
	{
		result = HAL_UART_Transmit_DMA(handle, reinterpret_cast<const uint8_t*>( message ), strlen(message));
	} while (result == HAL_BUSY);

	if (result == HAL_OK)
		txLock.take();
	else
		printf("debug %d", result);
	__NOP();
}
