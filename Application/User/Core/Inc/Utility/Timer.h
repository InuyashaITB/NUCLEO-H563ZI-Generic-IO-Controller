#ifndef INC_UTILITY_TIMER_H_
#define INC_UTILITY_TIMER_H_

#include <functional>

#include "app_threadx.h"

class Timer
{
public:
	Timer(osTimerType_t type, std::function<void(void)> onLoop) : onLoop{onLoop}
	{
		handle = osTimerNew(s_TimerCallback,
							type,
							this,
							nullptr);
	}

	void start(uint32_t ticks)
	{
		osTimerStart(handle, ticks);
	}

	void stop()
	{
		osTimerStop(handle);
	}
protected:
private:
	std::function<void(void)> onLoop;

	static void s_TimerCallback(void* arg)
	{
		reinterpret_cast<Timer*>(arg)->onLoop();
	}

	osTimerId_t handle;
};



#endif /* INC_UTILITY_TIMER_H_ */
