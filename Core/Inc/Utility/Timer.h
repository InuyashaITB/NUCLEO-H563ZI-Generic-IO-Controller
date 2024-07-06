#ifndef INC_UTILITY_TIMER_H_
#define INC_UTILITY_TIMER_H_

#include <functional>

#include "app_threadx.h"
#include "ThreadXMemory.h"

class Timer
{
public:
	Timer(const char* name, bool repeating, std::function<void(void)> onLoop) : onLoop{onLoop}, repeating{repeating}, name{name}
	{
	}

	void start(UINT ticks)
	{
		if (handle.tx_timer_id == 0)
		{
			init();
			tx_timer_create(&handle, (char*)name, s_TimerCallback, (ULONG)this, ticks, repeating ? ticks : 0, TX_NO_ACTIVATE);
		}
		tx_timer_change(&handle, ticks, repeating ? ticks : 0);
		tx_timer_activate(&handle);
	}

	void stop()
	{
		tx_timer_deactivate(&handle);
	}
protected:
	virtual void init() {}
private:
	std::function<void(void)> onLoop;
	bool repeating;
	const char* name;

	static void s_TimerCallback(ULONG arg)
	{
		reinterpret_cast<Timer*>(arg)->onLoop();
	}

	TX_TIMER handle;
};



#endif /* INC_UTILITY_TIMER_H_ */
