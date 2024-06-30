#include "Thread.h"

std::vector<Thread*> Thread::_threads{};
bool Thread::_threads_launched{false};

void Thread::launchAllThreads() {
	_threads_launched = true;
	for (auto thread : _threads)
		thread->create();
}

void Thread::_thread_launch(ULONG argument) {
	Thread* thread = reinterpret_cast<Thread*>(argument);
	thread->main();
}

void Thread::main() {
	tx_thread_sleep(TX_WAIT_FOREVER);
}

void Thread::create() {
	char* pointer = nullptr;
    tx_byte_allocate(&TXMemory::getPool(), (void **) &pointer, stackSize, TX_NO_WAIT);
	auto result = tx_thread_create(&handle, (char*)name, _thread_launch, (ULONG)this, pointer, stackSize, priority, priority, TX_NO_TIME_SLICE, TX_AUTO_START);
	(void)result;
}

void Thread::start() {
	tx_thread_resume(&handle);
}

void Thread::stop() {
	tx_thread_suspend(&handle);
}

Thread::Thread(const char *name, uint32_t priority, uint32_t stackSize) :
		name{name},
		priority{priority},
		stackSize{stackSize}
{
	_threads.push_back(this);

	if (_threads_launched)
		create();
}

