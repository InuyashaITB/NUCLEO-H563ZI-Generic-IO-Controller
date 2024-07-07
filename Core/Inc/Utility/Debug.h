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
		static char buffer[4096];
		char* pBuffer = buffer;
		size_t byteCount = vsnprintf(buffer, sizeof(buffer), str, args);

		if (byteCount >= 2 && (buffer[byteCount - 2] != '\r' || buffer[byteCount - 1] != '\n'))
		{
			buffer[byteCount] = '\r';
			buffer[byteCount + 1] = '\n';
			buffer[byteCount + 2] = 0;
			byteCount += 3;
		}

		while (byteCount > 0)
		{
			size_t len = std::min(byteCount, sizeof(OutputType) - 1);
			OutputType message;
			std::copy(pBuffer, pBuffer + len, message);
			message[len] = 0;
			queue.push(message, 0);

			pBuffer += len;
			byteCount -= len;
		}
	}
	void s(const uint8_t *ptr, size_t size) {
		while (size > 0)
		{
			size_t len = std::min(sizeof(OutputType) - 1, size);
			OutputType message;

			memcpy(message, ptr, len);

			message[len] = 0;

			queue.push(message, 0);

			ptr += len;
			size -= len;

			if (size == 0 && (message[len - 2] != '\r' || message[len - 1] != '\n'))
			{
				message[0] = '\r';
				message[1] = '\n';
				message[2] = 0;
				queue.push(message, 0);
			}
		}
	}
};

#endif /* INC_UTILITY_DEBUG_H_ */
