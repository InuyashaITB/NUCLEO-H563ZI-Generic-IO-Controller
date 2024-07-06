#ifndef USER_CORE_INC_IO_HEARTBEAT_H_
#define USER_CORE_INC_IO_HEARTBEAT_H_

#include "Timer.h"
#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_gpio.h"

class HeartBeat : public Timer
{
public:
	HeartBeat(GPIO_TypeDef* port, UINT pin) : Timer { "Heartbeat", true, [this] { thump(); } }, port{port}, pin{pin}
	{
	}
protected:
	virtual void init() override {
		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(port, &GPIO_InitStruct);
	}
private:
	GPIO_TypeDef* port;
	UINT pin;
	void thump() { HAL_GPIO_TogglePin(port, pin); }
};



#endif /* USER_CORE_INC_IO_HEARTBEAT_H_ */
