#ifndef USER_CORE_INC_FLASH_CONFIGURATION_H_
#define USER_CORE_INC_FLASH_CONFIGURATION_H_

#include "Message.h"
#include "Pin.h"
#include "PWM.h"
#include "WearLeveling.h"

class Configuration {
public:
	static constexpr uint32_t CONFIGURATION_VERSION = 1;

	struct Data {
		uint32_t version;
		PinConfiguration pinConfigurations[Port_e::COUNT][16];

		Data()
		{
			version = 0xFFFF'FFFF;
			memset(pinConfigurations, 0xFF, sizeof(pinConfigurations));
		}
	};

	Configuration() {
		storage.readLastElement(data);
		if (data.version != CONFIGURATION_VERSION)
		{
			data = {};
			data.version = CONFIGURATION_VERSION;
		}
	}

	void handleConfiguration(PinConfiguration p)
	{
		if (p.valid())
		{
			data.pinConfigurations[p.port][p.pinNumber] = p;

			if (p.pinType == PinType_e::PWM)
			{
				auto pinDesc = PWM::findPinDef(p.port, p.pinNumber, p.timer, p.channel);
				if (pinDesc)
					PWM::startPWM(pinDesc, p.frequency, p.duty);
			}
			else
			{
				Pin pin;
				pin.handleConfiguration(p);
			}
		}
	}

	bool store()
	{
		return storage.store(data);
	}

	void reset()
	{
		storage.reset();
	}

	Data data;
protected:
private:
	Flash flash { FLASH_NS };
	WearLeveling<Data> storage { flash };
};



#endif /* USER_CORE_INC_FLASH_CONFIGURATION_H_ */
