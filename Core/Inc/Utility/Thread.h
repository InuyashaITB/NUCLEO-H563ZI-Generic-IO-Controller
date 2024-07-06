#ifndef THREAD_H_
#define THREAD_H_

#include "app_threadx.h"

#include <cstdint>
#include <vector>

class Thread {
public:
	Thread(const char* name, uint32_t priority, uint32_t stackSize);
	virtual ~Thread() = default;

	void create();
	void start();
	void stop();

	static void launchAllThreads();
protected:
	virtual void main();
private:
	TX_THREAD handle;
	const char* name;
	uint32_t priority;
	uint32_t stackSize;

	static void _thread_launch(ULONG argument);
	static std::vector<Thread*> _threads;
	static bool _threads_launched;
};

#endif /* THREAD_H_ */
