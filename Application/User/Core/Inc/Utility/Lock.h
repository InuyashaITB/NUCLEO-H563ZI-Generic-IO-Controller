#ifndef INC_UTILITY_LOCK_H_
#define INC_UTILITY_LOCK_H_

#include "app_threadx.h"

class Lock
{
public:
	Lock(const char* name, size_t count) {
		tx_semaphore_create(&handle, (char*)name, count);
	}

	void take() { tx_semaphore_get(&handle, TX_WAIT_FOREVER); }
	void take(UINT ticks) { tx_semaphore_get(&handle, ticks); }
	void give() { tx_semaphore_put(&handle); }
protected:
private:
	TX_SEMAPHORE handle;
};



#endif /* INC_UTILITY_LOCK_H_ */
