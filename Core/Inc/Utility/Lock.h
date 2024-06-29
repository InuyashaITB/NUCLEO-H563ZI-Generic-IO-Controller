/*
 * Lock.h
 *
 *  Created on: Jun 25, 2024
 *      Author: mpeterson
 */

#ifndef INC_UTILITY_LOCK_H_
#define INC_UTILITY_LOCK_H_

#include "cmsis_os2.h"

class Lock
{
public:
	Lock(const char* name, size_t count) {
		osSemaphoreAttr_t attr{};
		attr.name = name;
		handle = osSemaphoreNew(count, 0, &attr);
	}

	void take() { osSemaphoreAcquire(handle, osWaitForever); }
	void take(uint32_t ticks) { osSemaphoreAcquire(handle, ticks); }
	void give() { osSemaphoreRelease(handle); }
protected:
private:
	osSemaphoreId_t handle;
};



#endif /* INC_UTILITY_LOCK_H_ */
