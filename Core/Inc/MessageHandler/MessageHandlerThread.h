/*
 * MessageHandlerTask.h
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#ifndef SRC_MESSAGEHANDLER_MESSAGEHANDLERTASK_H_
#define SRC_MESSAGEHANDLER_MESSAGEHANDLERTASK_H_

#include <vector>

#include <Message.h>
#include <QueueT.h>
#include <Thread.h>
#include <UART.h>

extern "C" { extern UART_HandleTypeDef huart3; }

using Message = char[512];

class MessageHandlerThread: public Thread {
public:
	MessageHandlerThread() : Thread("MessageHandler", osPriorityNormal, 1024 * 4) { }
	virtual ~MessageHandlerThread() = default;

	Queue<Message>& getQueue() { return messages; }
protected:
	virtual void main() override;
private:
	bool isEndOfMessage(char* check);

	bool parseJson(char* buffer);
	void uartRXReceived(char* rx);

	Queue<Message> messages {10};

	char stringBuffer[1024 * 24];
	size_t stringLength;

	UART uart{&huart3, [this](char* buff) { uartRXReceived(buff); }};
};

#endif /* SRC_MESSAGEHANDLER_MESSAGEHANDLERTASK_H_ */
