#ifndef APPLICATION_USER_CORE_SRC_UTILITY_THREADXMEMORY_H_
#define APPLICATION_USER_CORE_SRC_UTILITY_THREADXMEMORY_H_

#include "tx_api.h"
#include "nx_api.h"

class TXMemory
{
public:
	TXMemory();
	static TX_BYTE_POOL& getBytePool();
	static NX_PACKET_POOL& getPacketPool();
protected:
private:
	static constexpr size_t BYTE_POOL_SIZE = 1024 * 16;
	static char pool_memory[BYTE_POOL_SIZE];
	static TX_BYTE_POOL byte_pool;

	static constexpr size_t PACKET_POOL_SIZE = 1024 * 8;
	static char packet_memory[PACKET_POOL_SIZE];
	static NX_PACKET_POOL packet_pool;
};



#endif /* APPLICATION_USER_CORE_SRC_UTILITY_THREADXMEMORY_H_ */
