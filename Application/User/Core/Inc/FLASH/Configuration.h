#ifndef USER_CORE_INC_FLASH_CONFIGURATION_H_
#define USER_CORE_INC_FLASH_CONFIGURATION_H_

#include "WearLeveling.h"

class Configuration {
public:
	struct Data {
		uint32_t bootCount;
	};

	Configuration() {
		if (!storage.readLastElement(data))
			data.bootCount = 0xFFFF'FFFF;
	}

	bool store()
	{
		return storage.store(data);
	}

	Data data;
protected:
private:
	Flash flash { FLASH_NS };
	WearLeveling<Data> storage { flash };
};



#endif /* USER_CORE_INC_FLASH_CONFIGURATION_H_ */
