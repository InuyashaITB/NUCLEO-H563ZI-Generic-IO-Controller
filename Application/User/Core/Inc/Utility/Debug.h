#ifndef INC_UTILITY_DEBUG_H_
#define INC_UTILITY_DEBUG_H_

#include <cstdarg>
#include <cstdio>

#include <QueueT.h>
#include <Thread.h>
#include <UART.h>

using OutputType = char[128];

class Debug: public Thread {
public:
	static void send(const char *str, ...) {
		va_list args;
		va_start(args, str);
		getInstance().s(str, args);
		va_end(args);
	}
	static void send(const uint8_t *ptr, size_t size) {
		getInstance().s(ptr, size);
	}
protected:

	static Debug& getInstance() {
		static Debug instance;
		return instance;
	}

	virtual void main() override
	{
		OutputType m { };
		while (true) {
			queue.pop(m);
			uart->transmit(m);
		}
	}
private:
	Debug() : Thread { "Debug", TX_LOWEST_PRIORITY, 1024 } {
		uart = UART::findUARTByHandle(nullptr);
	}

	Queue<OutputType> queue { 5 };
	UART *uart;

	void s(const char *str, va_list args) {
		OutputType message;
		size_t bytes = vsnprintf(message, sizeof(OutputType), str, args);

		if (message[bytes - 1] != '\n')
		{
			message[bytes] = '\n';
			message[bytes+1] = '\0';
		}

		queue.push(message, 0);
	}
	void s(const uint8_t *ptr, size_t size) {
		OutputType message;
		memcpy(message, ptr, size);
		message[size] = 0;

		if (message[size - 1] != '\n')
		{
			message[size] = '\n';
			message[size+1] = '\0';
		}

		queue.push(message, 0);
	}
};

#endif /* INC_UTILITY_DEBUG_H_ */
