#ifndef INC_UTILITY_DEBUG_H_
#define INC_UTILITY_DEBUG_H_

#include <cstdarg>
#include <cstdio>

#include <CDC.h>
#include <QueueT.h>
#include <Socket.h>
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

	template<typename T>
	static void transmitDirect(T m)
	{
		for (auto uart : UART::getAllChannels())
			uart->transmit(m);

		for (auto socket : Socket::getAllSockets())
			socket->transmit(m);

		CDC::getInstance().transmit(m);
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
			Debug::transmitDirect(m);
		}
	}
private:
	Debug() : Thread { "Debug", TX_LOWEST_PRIORITY, 1024 } {
	}

	Queue<OutputType> queue { 50 };
	UART *uart;
	Socket *socket;

	void s(const char *str, va_list args) {
		OutputType message;
		size_t byteCount = vsnprintf(message, sizeof(OutputType), str, args);

		if (byteCount >= 2 && (message[byteCount - 2] != '\r' || message[byteCount - 1] != '\n'))
			strcat(message, "\r\n");

		queue.push(message, 0);
	}
	void s(const uint8_t *ptr, size_t size) {
		OutputType message;
		memcpy(message, ptr, size);
		message[size] = 0;

		if (size >= 2 && (message[size - 2] != '\r' || message[size - 1] != '\n'))
			strcat(message, "\r\n");

		queue.push(message, 0);
	}
};

#endif /* INC_UTILITY_DEBUG_H_ */
