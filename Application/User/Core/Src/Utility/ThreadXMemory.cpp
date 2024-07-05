#include "ThreadXMemory.h"

char TXMemory::pool_memory[TXMemory::BYTE_POOL_SIZE];
TX_BYTE_POOL TXMemory::byte_pool;

char TXMemory::packet_memory[TXMemory::PACKET_POOL_SIZE];
NX_PACKET_POOL TXMemory::packet_pool;

TXMemory::TXMemory()
{
    tx_byte_pool_create(&TXMemory::byte_pool, (char*)"app byte pool", TXMemory::pool_memory, TXMemory::BYTE_POOL_SIZE);
    nx_packet_pool_create(&TXMemory::packet_pool, (char*)"packet pool", 4096, TXMemory::packet_memory, TXMemory::PACKET_POOL_SIZE);
}

TX_BYTE_POOL& TXMemory::getBytePool()
{
	return TXMemory::byte_pool;
}

NX_PACKET_POOL& TXMemory::getPacketPool()
{
	return TXMemory::packet_pool;
}
