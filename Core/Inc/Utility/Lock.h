#ifndef INC_UTILITY_LOCK_H_
#define INC_UTILITY_LOCK_H_

#include "app_threadx.h"

class Lock
{
public:
	Lock(const char* name, size_t count) {
		tx_semaphore_create(&handle, (char*)name, count);
	}

	bool take() { return tx_semaphore_get(&handle, TX_WAIT_FOREVER) == TX_SUCCESS; }
	bool take(UINT ticks) { return tx_semaphore_get(&handle, ticks) == TX_SUCCESS; }
	void give() { tx_semaphore_put(&handle); }
protected:
private:
	TX_SEMAPHORE handle;
};



#endif /* INC_UTILITY_LOCK_H_ */
