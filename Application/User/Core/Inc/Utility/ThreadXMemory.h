#ifndef APPLICATION_USER_CORE_SRC_UTILITY_THREADXMEMORY_H_
#define APPLICATION_USER_CORE_SRC_UTILITY_THREADXMEMORY_H_

#include "tx_api.h"

class TXMemory
{
public:
	TXMemory();
	static TX_BYTE_POOL& getPool();
protected:
private:
	static constexpr size_t BYTE_POOL_SIZE = 1024*16;
	static char pool_memory[BYTE_POOL_SIZE];
	static TX_BYTE_POOL byte_pool;
};



#endif /* APPLICATION_USER_CORE_SRC_UTILITY_THREADXMEMORY_H_ */
