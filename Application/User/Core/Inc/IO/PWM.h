#ifndef SRC_IO_PWM_H_
#define SRC_IO_PWM_H_

#include <vector>

#include "stm32h5xx.h"

#include "Pin.h"
#include "UART.h"

class PWM
{
public:
	struct PinDef
	{
		GPIO_TypeDef* gpioPortHandle;
		uint16_t pinNumber;
		TIM_HandleTypeDef* timerHandle;
		uint32_t timerChannel;
		uint8_t alternateFunctionValue;
	};

	static PinDef* findPinDef(Port_e port, uint32_t pin, Timer_e timer = Timer_e::None, uint16_t channel = TIM_CHANNEL_ALL);
	static void startPWM(PinDef* pd, float frequency, float dutyCycle);
	static void printHelpMessage(UART& uart);
protected:
private:
	static std::vector<PinDef> _pin_defenitions;
};



#endif /* SRC_IO_PWM_H_ */
