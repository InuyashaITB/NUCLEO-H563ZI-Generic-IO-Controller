#include "ThreadXMemory.h"

char TXMemory::pool_memory[TXMemory::BYTE_POOL_SIZE];
TX_BYTE_POOL TXMemory::byte_pool;

TXMemory::TXMemory()
{
    tx_byte_pool_create(&TXMemory::byte_pool, (char*)"app byte pool", TXMemory::pool_memory, TXMemory::BYTE_POOL_SIZE);
}

TX_BYTE_POOL& TXMemory::getPool()
{
	return TXMemory::byte_pool;
}
