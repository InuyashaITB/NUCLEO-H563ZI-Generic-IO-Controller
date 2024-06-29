/*
 * Debug.h
 *
 *  Created on: Jun 27, 2024
 *      Author: mpeterson
 */

#ifndef INC_UTILITY_DEBUG_H_
#define INC_UTILITY_DEBUG_H_

#include <string>

#include <QueueT.h>
#include <Thread.h>
#include <UART.h>

using OutputType = char[128];

class Debug : public Thread
{
public:
	static Debug& getInstance()
	{
		static Debug instance;
		return instance;
	}

	void send(const char* str) { OutputType message; memcpy(message, str, strlen(str) + 1); queue.push(message); }
	void send(const uint8_t* ptr, size_t size) { send(reinterpret_cast<const char*>(ptr)); }
protected:
	virtual void main() override
	{
		while (true)
		{
			OutputType m{};
			queue.pop(m);
			uart->transmit(m);
		}
	}
private:
	Debug() : Thread { "Debug", osPriorityLow, 1024 }
	{
		uart = UART::findUARTByHandle(nullptr);
	}
	Queue<OutputType> queue {50};
	UART* uart;
};



#endif /* INC_UTILITY_DEBUG_H_ */
