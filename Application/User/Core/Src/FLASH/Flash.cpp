#include <algorithm>

#include "Debug.h"
#include "Flash.h"

extern "C"
{
extern int nmi_occurred;
}

template <typename T>
static bool isValuePresent(std::vector<T>& vector, T value)
{
	return std::find(vector.begin(), vector.end(), value) != vector.end();
}

template <typename T>
static void insertIfNotPresent(std::vector<T>& vec, T value) {
    if (!isValuePresent(vec, value)) {
        vec.push_back(value);
    }
}

Flash::Flash(FLASH_TypeDef* flashHandle) : handle{flashHandle}
{

}

bool Flash::addressRangeInFlash(const uint16_t* pDestination, size_t len)
{
	return (pDestination >= reinterpret_cast<const uint16_t*>(FLASH_EDATA_BEGIN) && (pDestination + len) <= reinterpret_cast<const uint16_t*>(FLASH_EDATA_END));
}

std::vector<const uint32_t*> Flash::getSectorsInRange(const uint16_t* pDestination, size_t len)
{
	const uint16_t* destinationStart = pDestination;
	const uint16_t* destinationEnd = pDestination + len;
	std::vector<const uint32_t*> regions{};
	for (uint32_t i = 0; i < FLASH_EDATA_SECTORS.size() - 1; i++)
	{
		const uint16_t* currentFlashSector = reinterpret_cast<const uint16_t*>(FLASH_EDATA_SECTORS[i]);
		const uint16_t* nextFlashSector = reinterpret_cast<const uint16_t*>(FLASH_EDATA_SECTORS[i + 1]);

		if ((destinationStart >= currentFlashSector && destinationStart < nextFlashSector)
		||  (regions.size() > 0 && destinationEnd > currentFlashSector))
		{
			regions.push_back(FLASH_EDATA_SECTORS[i]);
		}
	}

	return regions;
}

const uint32_t* Flash::findSectorForDestination(const uint16_t* pDestination)
{
    const uint32_t* addressUint32 = reinterpret_cast<const uint32_t*>(pDestination);
    for (size_t i = 0; i < FLASH_EDATA_SECTORS.size() - 1; ++i) {
        const uint32_t* sectorStart = FLASH_EDATA_SECTORS[i];
        const uint32_t* sectorEnd = FLASH_EDATA_SECTORS[i + 1];
        if (addressUint32 >= sectorStart && addressUint32 < sectorEnd) {
            return FLASH_EDATA_SECTORS[i];
        }
    }
    return nullptr; // Return nullptr if the address is not in any sector
}

bool Flash::erase(std::vector<EraseOptions> opts)
{
	bool success = true;
	uint32_t error = 0;

	unlock();
	for (auto& opt : opts)
	{
		FLASH_EraseInitTypeDef eraseOption;
		eraseOption.Banks = opt.bank;
		eraseOption.NbSectors = 1;
		eraseOption.Sector = opt.sector;
		eraseOption.TypeErase = FLASH_TYPEERASE_SECTORS;
		success = (success) && (HAL_FLASHEx_Erase(&eraseOption, &error) == HAL_OK) && (error == FLASH_ERASE_ERROR_NONE);
	}
	lock();

	return success;
}

void Flash::lock()
{
	HAL_FLASH_Lock();
}

void Flash::unlock()
{
	HAL_FLASH_Unlock();
}

bool Flash::write(const uint16_t* pDestination, const uint16_t* pInput, size_t len)
{
	bool success = true;
	const uint16_t* pDestinationEnd = pDestination + len;
	nmi_occurred = 0;

	// Start by validating the destination and length remain within the limits of our flash
	if (!addressRangeInFlash(pDestination, len))
	{
		Debug::send("Destination (%p) is not within EDATA region: (%p --> %p)", pDestination, FLASH_EDATA_BEGIN, FLASH_EDATA_END);
		return false;
	}

	if ((reinterpret_cast<std::uintptr_t>(pDestination) % 2 != 0)
	||  (reinterpret_cast<std::uintptr_t>(pInput) % 2 != 0))
	{
		Debug::send("Destination (%p) or Input (%p) is not half-word aligned!", pDestination, pInput);
		return false;
	}

	std::vector<const uint32_t*> allAffectedSectors = getSectorsInRange(pDestination, len);

	std::vector<const uint32_t*> sectorsNeedingErased{};
	for (size_t i = 0; i < len; i += sizeof(uint16_t))
	{
		uint16_t inputValue = *(pInput + i);
		uint16_t flashData = *(pDestination + i);
		if ((flashData & inputValue) != inputValue)
		{
			insertIfNotPresent(sectorsNeedingErased, findSectorForDestination(pDestination));
		}
	}

	const uint16_t* flashPtr = pDestination;
	const uint16_t* inputPtr = pInput;

	for (size_t i = 0; i < allAffectedSectors.size(); i++)
	{
		const uint16_t* currentSector = reinterpret_cast<const uint16_t*>(allAffectedSectors[i]);
		const uint16_t* endCurrentSector = currentSector + (FLASH_EDATA_SECTOR_SIZE / sizeof(uint16_t));

		static uint16_t flashDataCopy[FLASH_EDATA_SECTOR_SIZE / sizeof(uint16_t)];
		for (size_t i = 0; i < FLASH_EDATA_SECTOR_SIZE / sizeof(uint16_t); i++)
		{
			flashDataCopy[i] = *(currentSector + i);
		}

		bool eraseOccured = false;

		// If current sector is listed in the sectors to be erased...
		if (isValuePresent(sectorsNeedingErased, reinterpret_cast<const uint32_t*>(currentSector)))
		{
			Flash::Banks bank = static_cast<Flash::Banks>(FLASH_EDATA_BANK);
			auto sector = FLASH_EDATA_SECTOR_MAP.at(allAffectedSectors[i]);
			EraseOptions opt { bank, sector };
			// start by erasing the sector
			eraseOccured = erase({ opt });

			// Yield to any on coming threads
			tx_thread_relinquish();
		}

		unlock();
		// There is some 'unprogrammed' data at the start of this sector that will need to be retained
		if (eraseOccured && i == 0 && pDestination > currentSector)
		{
			ssize_t numberOfHalfWords = std::floor(((uint32_t)pDestination - (uint32_t)currentSector) / (float)sizeof(uint16_t));
			for (const uint16_t* pRetainedData = reinterpret_cast<const uint16_t*>(&flashDataCopy[0]),
					*pRetainedFlashDestination = reinterpret_cast<const uint16_t*>(currentSector);
					numberOfHalfWords > 0;
					pRetainedData++,
					numberOfHalfWords--)
			{
				success = success && (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD_EDATA, (uint32_t)pRetainedFlashDestination, (uint32_t)pRetainedData) == HAL_OK);
			}
		}
		// There is some 'unprogrammed' data at the end of this sector that will need to be retained
		if (eraseOccured && i == (allAffectedSectors.size() - 1) && (pDestinationEnd < endCurrentSector - 1))
		{
			size_t remainingHalfWords = (size_t)(endCurrentSector - pDestinationEnd);
			size_t offsetHalfWords = (size_t)(pDestinationEnd - currentSector);
			for (const uint16_t* pRetainedData = reinterpret_cast<const uint16_t*>(flashDataCopy + offsetHalfWords),
					*pRetainedFlashDestination = reinterpret_cast<const uint16_t*>(currentSector + offsetHalfWords);
					remainingHalfWords > 0;
					pRetainedData++,
					remainingHalfWords--)
			{
				success = success && (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD_EDATA, (uint32_t)pRetainedFlashDestination, (uint32_t)pRetainedData) == HAL_OK);
			}
		}

		// At this point we are programming an entire sector and don't need to do anything special
		for (; flashPtr < pDestinationEnd; flashPtr++, inputPtr++)
		{
			if (*flashPtr != *inputPtr)
				success = success && (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD_EDATA, (uint32_t)flashPtr, (uint32_t)inputPtr) == HAL_OK);
		}

		// Yield to any on coming threads
		tx_thread_relinquish();
	}

	lock();

	return success;
}
