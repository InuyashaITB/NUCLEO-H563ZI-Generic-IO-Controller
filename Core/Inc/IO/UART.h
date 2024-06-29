/*
 * UART.h
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#ifndef INC_IO_UART_H_
#define INC_IO_UART_H_

#include <cstring>
#include <functional>
#include <vector>

#include "stm32h5xx.h"
#include "stm32h5xx_hal_uart.h"

#include "BinaryLock.h"

using RxCallack = std::function<void(char*)>;

class UART
{
public:
	UART(UART_HandleTypeDef* handle, RxCallack rxCallback);

	void transmit(const char* message);
	void transmit(const uint8_t* message, size_t len);
	void startReceiving();

	UART_HandleTypeDef* getHandle() { return handle; }

	static UART* findUARTByHandle(UART_HandleTypeDef* handle);
	static void RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
	static void TxCpltCallback(UART_HandleTypeDef *huart);
protected:

private:
	UART_HandleTypeDef* handle;
	RxCallack rxCallback;

	uint8_t dataBuffer[256];

	static std::vector<UART*> _uarts;
	BinaryLock txLock{"UART_TX_LOCK"};
	BinaryLock uartLock{nullptr};
};


#endif /* INC_IO_UART_H_ */
