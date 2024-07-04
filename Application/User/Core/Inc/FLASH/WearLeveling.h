#ifndef USER_CORE_INC_FLASH_WEARLEVELING_H_
#define USER_CORE_INC_FLASH_WEARLEVELING_H_

#include "Flash.h"

template <typename ElementType>
class WearLeveling
{
public:
static_assert(sizeof(ElementType) % (sizeof(uint16_t)) == 0, "ElementType must be aligned to 2 bytes");

	WearLeveling(Flash& flash) : flash{flash}
	{
		readIndex();
	}

	bool store(ElementType& data)
	{
		auto nextIndex = findNextIndex();
		if (nextIndex > MAX_ELEMENTS)
		{
			reset();
			nextIndex = 1;
		}

		return storeData(nextIndex, data);
	}

	bool readLastElement(ElementType& data)
	{
		auto nextIndex = findNextIndex();
		if (nextIndex == 0)
			return false;
		else
			return readData(nextIndex - 1, data);
	}
protected:
private:
	static constexpr size_t INDEX_SIZE = sizeof(uint32_t);
	static constexpr size_t BITS_PER_INDEX = INDEX_SIZE * 8;
	static constexpr size_t MAX_POSSIBLE_ELEMENTS_STORABLE = TOTAL_FLASH_EDATA_SIZE / sizeof(ElementType);
	static constexpr size_t NUMBER_OF_WORDS_NEEDED_FOR_INDEXING = MAX_POSSIBLE_ELEMENTS_STORABLE / INDEX_SIZE;
	static constexpr size_t MAX_ELEMENTS = MAX_POSSIBLE_ELEMENTS_STORABLE - (NUMBER_OF_WORDS_NEEDED_FOR_INDEXING * (sizeof(ElementType) / INDEX_SIZE));

	struct Indexer {
		uint32_t indecies[NUMBER_OF_WORDS_NEEDED_FOR_INDEXING];
	};

	const uint16_t* FLASH_INDECIES_LOCATION = reinterpret_cast<const uint16_t*>(FLASH_EDATA_BEGIN);
	const uint16_t* FLASH_DATA_LOCATION = FLASH_INDECIES_LOCATION + (sizeof(Indexer) / sizeof(uint16_t));

	Flash& flash;
	Indexer index;

	void reset()
	{
		flash.eraseAll();
		std::fill(std::begin(index.indecies), std::end(index.indecies), 0xFFFF'FFFF);
	}

	void readIndex()
	{
		flash.read(FLASH_INDECIES_LOCATION, reinterpret_cast<uint16_t*>(&index), sizeof(Indexer) / sizeof(uint16_t));
	}

	void setIndexUsed(size_t i)
	{
		size_t word = i / BITS_PER_INDEX;
		size_t bit = i - (word * BITS_PER_INDEX);
		index.indecies[word]  &= ~(1 << bit);

		flash.write(FLASH_INDECIES_LOCATION, reinterpret_cast<uint16_t*>(&index), sizeof(index) / sizeof(uint16_t));
	}

	bool readData(size_t index, ElementType& data)
	{
		const uint16_t* dataLocation = FLASH_DATA_LOCATION + (index * sizeof(ElementType) / sizeof(uint16_t));
		uint16_t* pData = reinterpret_cast<uint16_t*>(&data);
		size_t lenHalfWords = sizeof(ElementType) / sizeof(uint16_t);
		return flash.read(dataLocation, pData, lenHalfWords);
	}

	bool storeData(size_t index, ElementType& data)
	{
		const uint16_t* dataLocation = FLASH_DATA_LOCATION + (index * sizeof(ElementType) / sizeof(uint16_t));
		uint16_t* pData = reinterpret_cast<uint16_t*>(&data);
		size_t lenHalfWords = sizeof(ElementType) / sizeof(uint16_t);
		bool dataWritten = flash.write(dataLocation, pData, lenHalfWords);
		if (dataWritten)
			setIndexUsed(index);
		return dataWritten;
	}

	size_t findNextIndex()
	{
		size_t result = 0;

		for (size_t i = 0; i < sizeof(index.indecies) / INDEX_SIZE; i++)
		{
			for (uint8_t bit = 0; bit < BITS_PER_INDEX; bit++)
			{
				if (index.indecies[i] & (1 << bit))
					return result;
				else
					result++;
			}
		}

		return result;
	}
};



#endif /* USER_CORE_INC_FLASH_WEARLEVELING_H_ */
