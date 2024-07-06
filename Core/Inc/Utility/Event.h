#ifndef USER_CORE_INC_UTILITY_EVENT_H_
#define USER_CORE_INC_UTILITY_EVENT_H_

#include "tx_api.h"

template<typename FlagType>
class Event
{
public:
	Event(const char* name = nullptr)
	{
		tx_event_flags_create(&handle, (char *)name);
	}

	bool waitForEventFlagsAny(FlagType waitForFlags, FlagType& outputFlags, ULONG ticks = TX_WAIT_FOREVER)
	{
		auto result = tx_event_flags_get(&handle, (ULONG)waitForFlags, TX_OR_CLEAR, (ULONG*)&outputFlags, ticks);
		return (result == TX_SUCCESS && (static_cast<ULONG>((outputFlags & waitForFlags)) != 0));
	}

	bool waitForEventFlagsAll(FlagType waitForFlags, FlagType& outputFlags, ULONG ticks = TX_WAIT_FOREVER)
	{
		auto result = tx_event_flags_get(&handle, (ULONG)waitForFlags, TX_AND_CLEAR, (ULONG*)&outputFlags, ticks);
		return (result == TX_SUCCESS && ((outputFlags & waitForFlags) == waitForFlags));
	}

	void post(FlagType flag)
	{
		tx_event_flags_set(&handle, (ULONG)flag, TX_OR);
	}
protected:
private:
	TX_EVENT_FLAGS_GROUP handle;
};




#endif /* USER_CORE_INC_UTILITY_EVENT_H_ */
