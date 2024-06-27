/*
 * Thread.h
 *
 *  Created on: Jun 23, 2024
 *      Author: mpeterson
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "stm32h5xx_nucleo.h"

#include <vector>

class Thread {
public:
	Thread(const char* name, osPriority_t priority, uint32_t stackSize);
	virtual ~Thread() = default;

	void create();
	void start();
	void stop();

	static void launchAllThreads();
protected:
	virtual void main();
private:
	osThreadId_t handle;
	const osThreadAttr_t attributes;

	static void _thread_launch(void *argument);
	static std::vector<Thread*> _threads;
};

#endif /* THREAD_H_ */
