/*
 * Thread.cpp
 *
 *  Created on: Jun 23, 2024
 *      Author: mpeterson
 */

#include "Thread.h"

std::vector<Thread*> Thread::_threads{};

void Thread::launchAllThreads() {
	for (auto thread : _threads)
	{
		thread->create();
	}
}

void Thread::_thread_launch(void *argument) {
	Thread* thread = reinterpret_cast<Thread*>(argument);
	thread->main();
}

void Thread::main() {
	osDelay(osWaitForever);
}

void Thread::create() {
	handle = osThreadNew(_thread_launch, this, &attributes);
}

void Thread::start() {
	osThreadResume(handle);
}

void Thread::stop() {
	osThreadSuspend(handle);
}

Thread::Thread(const char *name, osPriority_t priority, uint32_t stackSize) : attributes {
	.name = name,
	.stack_size = stackSize,
	.priority = priority
}
{
	_threads.push_back(this);
}

