/*
 * Pin.h
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#ifndef SRC_IO_PIN_H_
#define SRC_IO_PIN_H_

#include "stm32h5xx.h"
#include "stm32h5xx_hal_gpio.h"

#include "Message.h"

class Pin {
public:
	static GPIO_TypeDef* getPort(Port_e p)
	{
		switch(p)
		{
			default:
			case Port_e::A: return GPIOA;
			case Port_e::B: return GPIOB;
			case Port_e::C: return GPIOC;
			case Port_e::D: return GPIOD;
			case Port_e::E: return GPIOE;
			case Port_e::F: return GPIOF;
			case Port_e::G: return GPIOG;
			case Port_e::H: return GPIOH;
			case Port_e::I: return GPIOI;
		}
	}

	static void handleConfiguration(PinConfiguration &config) {
		auto port = getPort(config.port);
		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		switch(config.pinType)
		{
		case PinType_e::OpenDrain: 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; break;
		case PinType_e::PushPull: 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; break;
		default:					GPIO_InitStruct.Mode = GPIO_MODE_INPUT; break;
		}
		GPIO_InitStruct.Pin = config.pinNumber;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(port, &GPIO_InitStruct);

		switch(config.pinState)
		{
		default:
		case PinState_e::NoChange : break;
		case PinState_e::Low      : HAL_GPIO_WritePin(port, config.pinNumber, GPIO_PIN_RESET); break;
		case PinState_e::High     : HAL_GPIO_WritePin(port, config.pinNumber, GPIO_PIN_SET); break;
		}
	}
protected:
private:
};

#endif /* SRC_IO_PIN_H_ */
