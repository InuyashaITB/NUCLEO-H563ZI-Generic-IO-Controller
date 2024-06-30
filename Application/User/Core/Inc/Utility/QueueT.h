#ifndef SRC_UTILITY_QUEUE_H_
#define SRC_UTILITY_QUEUE_H_

#include "app_threadx.h"

template <typename T> class Queue
{
public:
	Queue(UINT maxCount, const char* name = nullptr)
	{
		size_t byteCount = maxCount * sizeof(DataNode*);

		char* pointer{nullptr};
		tx_byte_allocate(&TXMemory::getPool(), (void **) &pointer, byteCount, TX_NO_WAIT);
		tx_queue_create(&handle, (char*)name, sizeof(DataNode*) / sizeof(UINT), pointer, byteCount);

		data = new DataNode[nodeCount = maxCount];
	}

	const char* getName()
	{
		char* name{nullptr};
		tx_queue_info_get(&handle, &name, nullptr, nullptr, nullptr, nullptr, nullptr);
		return name;
	}

	bool push(T& message, ULONG waitTime = TX_WAIT_FOREVER)
	{
		auto nextNode = findAvailableNode();
		if (!nextNode)
			return false;

		nextNode->inUse = true;
		memcpy(&nextNode->data, &message, sizeof(T));
		return tx_queue_send(&handle, &nextNode, waitTime) == TX_SUCCESS;
	}

	void pop(T& message)
	{
		DataNode* poppedItem{nullptr};
		tx_queue_receive(&handle, &poppedItem, TX_WAIT_FOREVER);

		poppedItem->inUse = false;
		memcpy(&message, &poppedItem->data, sizeof(T));
	}
protected:
private:
	struct DataNode {
		T data{};
		bool inUse{false};
	};

	TX_QUEUE handle;

	DataNode* data;
	ULONG nodeCount;

	DataNode* findAvailableNode()
	{
		DataNode* endNode = data + nodeCount;
		for (DataNode* node = data; node < endNode; node++)
		{
			if (!node->inUse)
				return node;
		}

		return nullptr;
	}
};


#endif /* SRC_UTILITY_QUEUE_H_ */
