/*
 * Queue.h
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#ifndef SRC_UTILITY_QUEUE_H_
#define SRC_UTILITY_QUEUE_H_

#include "cmsis_os2.h"

template <typename T> class Queue
{
public:
	Queue(uint32_t maxCount, const char* name = nullptr) {
		osMessageQueueAttr_t attributes { name };
		handle = osMessageQueueNew(maxCount, sizeof(T), &attributes);
	}

	const char* getName() {
		return osMessageQueueGetName(handle);
	}

	bool push(T& message) {
		return osMessageQueuePut(handle, &message, 0, 0) == osOK;
	}

	void pop(T& message) {
		osMessageQueueGet(handle, &message, nullptr, osWaitForever);
	}
protected:
private:
	osMessageQueueId_t handle;
};


#endif /* SRC_UTILITY_QUEUE_H_ */
