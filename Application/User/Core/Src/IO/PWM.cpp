#include <cstdio>

#include "stm32h5xx_hal.h"

#include "PWM.h"

extern "C"
{
	extern TIM_HandleTypeDef htim1;
	extern TIM_HandleTypeDef htim2;
	extern TIM_HandleTypeDef htim3;
	extern TIM_HandleTypeDef htim4;
	extern TIM_HandleTypeDef htim5;
	extern TIM_HandleTypeDef htim8;
	extern TIM_HandleTypeDef htim12;
	extern TIM_HandleTypeDef htim13;
	extern TIM_HandleTypeDef htim14;
	extern TIM_HandleTypeDef htim15;
	extern TIM_HandleTypeDef htim16;
}

/**
 * Values taken from https://www.st.com/resource/en/datasheet/stm32h573ai.pdf
 */
std::vector<PWM::PinDef> PWM::_pin_defenitions {
	PWM::PinDef { GPIOA, GPIO_PIN_0 , &htim5 , TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOA, GPIO_PIN_0 , &htim2 , TIM_CHANNEL_1, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_1 , &htim5 , TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOA, GPIO_PIN_1 , &htim2 , TIM_CHANNEL_2, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_2 , &htim5 , TIM_CHANNEL_3, 2 },
	PWM::PinDef { GPIOA, GPIO_PIN_2 , &htim2 , TIM_CHANNEL_3, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_3 , &htim5 , TIM_CHANNEL_4, 2 },
	PWM::PinDef { GPIOA, GPIO_PIN_3 , &htim2 , TIM_CHANNEL_4, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_5 , &htim2 , TIM_CHANNEL_1, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_6 , &htim3 , TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOA, GPIO_PIN_7 , &htim3 , TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOA, GPIO_PIN_8 , &htim1 , TIM_CHANNEL_1, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_9 , &htim1 , TIM_CHANNEL_2, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_10, &htim1 , TIM_CHANNEL_3, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_11, &htim1 , TIM_CHANNEL_4, 1 },
	PWM::PinDef { GPIOA, GPIO_PIN_15, &htim2 , TIM_CHANNEL_1, 1 },

	PWM::PinDef { GPIOB, GPIO_PIN_0 , &htim3 , TIM_CHANNEL_3, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_1 , &htim3 , TIM_CHANNEL_4, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_4 , &htim3 , TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_5 , &htim3 , TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_6 , &htim4 , TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_7 , &htim4 , TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_8 , &htim4 , TIM_CHANNEL_3, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_8 , &htim16, TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_9 , &htim4 , TIM_CHANNEL_4, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_10, &htim2 , TIM_CHANNEL_3, 1 },
	PWM::PinDef { GPIOB, GPIO_PIN_11, &htim2 , TIM_CHANNEL_4, 1 },
	PWM::PinDef { GPIOB, GPIO_PIN_14, &htim12, TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOB, GPIO_PIN_15, &htim12, TIM_CHANNEL_2, 2 },

	PWM::PinDef { GPIOC, GPIO_PIN_2 , &htim4 , TIM_CHANNEL_4, 2 },
	PWM::PinDef { GPIOC, GPIO_PIN_4 , &htim2 , TIM_CHANNEL_4, 1 },
	PWM::PinDef { GPIOC, GPIO_PIN_6 , &htim3 , TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOC, GPIO_PIN_6 , &htim8 , TIM_CHANNEL_1, 3 },
	PWM::PinDef { GPIOC, GPIO_PIN_7 , &htim3 , TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOC, GPIO_PIN_7 , &htim8 , TIM_CHANNEL_2, 3 },
	PWM::PinDef { GPIOC, GPIO_PIN_8 , &htim3 , TIM_CHANNEL_3, 2 },
	PWM::PinDef { GPIOC, GPIO_PIN_8 , &htim8 , TIM_CHANNEL_3, 3 },
	PWM::PinDef { GPIOC, GPIO_PIN_9 , &htim3 , TIM_CHANNEL_4, 2 },
	PWM::PinDef { GPIOC, GPIO_PIN_9 , &htim8 , TIM_CHANNEL_4, 3 },

	PWM::PinDef { GPIOD, GPIO_PIN_13, &htim4 , TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOD, GPIO_PIN_14, &htim4 , TIM_CHANNEL_3, 2 },
	PWM::PinDef { GPIOD, GPIO_PIN_15, &htim4 , TIM_CHANNEL_4, 2 },

	PWM::PinDef { GPIOE, GPIO_PIN_5 , &htim15, TIM_CHANNEL_1, 4 },
	PWM::PinDef { GPIOE, GPIO_PIN_6 , &htim15, TIM_CHANNEL_2, 4 },
	PWM::PinDef { GPIOE, GPIO_PIN_9 , &htim1 , TIM_CHANNEL_1, 1 },
	PWM::PinDef { GPIOE, GPIO_PIN_11, &htim1 , TIM_CHANNEL_2, 1 },
	PWM::PinDef { GPIOE, GPIO_PIN_13, &htim1 , TIM_CHANNEL_3, 1 },
	PWM::PinDef { GPIOE, GPIO_PIN_14, &htim1 , TIM_CHANNEL_4, 1 },

	PWM::PinDef { GPIOF, GPIO_PIN_6 , &htim16, TIM_CHANNEL_1, 1 },

	PWM::PinDef { GPIOH, GPIO_PIN_6 , &htim12, TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOH, GPIO_PIN_6 , &htim8 , TIM_CHANNEL_1, 3 },
	PWM::PinDef { GPIOH, GPIO_PIN_7 , &htim1 , TIM_CHANNEL_3, 1 },
	PWM::PinDef { GPIOH, GPIO_PIN_8 , &htim8 , TIM_CHANNEL_2, 3 },
	PWM::PinDef { GPIOH, GPIO_PIN_9 , &htim1 , TIM_CHANNEL_2, 1 },
	PWM::PinDef { GPIOH, GPIO_PIN_9 , &htim12, TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOH, GPIO_PIN_10, &htim8 , TIM_CHANNEL_3, 3 },
	PWM::PinDef { GPIOH, GPIO_PIN_10, &htim5 , TIM_CHANNEL_1, 2 },
	PWM::PinDef { GPIOH, GPIO_PIN_11, &htim5 , TIM_CHANNEL_2, 2 },
	PWM::PinDef { GPIOH, GPIO_PIN_11, &htim1 , TIM_CHANNEL_1, 1 },
	PWM::PinDef { GPIOH, GPIO_PIN_12, &htim5 , TIM_CHANNEL_3, 2 },

	PWM::PinDef { GPIOI, GPIO_PIN_0 , &htim5 , TIM_CHANNEL_4, 2 },
	PWM::PinDef { GPIOI, GPIO_PIN_2 , &htim8 , TIM_CHANNEL_4, 3 },
	PWM::PinDef { GPIOI, GPIO_PIN_5 , &htim8 , TIM_CHANNEL_1, 3 },
	PWM::PinDef { GPIOI, GPIO_PIN_6 , &htim8 , TIM_CHANNEL_2, 3 },
	PWM::PinDef { GPIOI, GPIO_PIN_7 , &htim8 , TIM_CHANNEL_3, 3 },
};

static const char* portToString(GPIO_TypeDef* gpio)
{
	if (gpio == GPIOA) return "A";
	else if (gpio == GPIOB) return "B";
	else if (gpio == GPIOC) return "C";
	else if (gpio == GPIOD) return "D";
	else if (gpio == GPIOE) return "E";
	else if (gpio == GPIOF) return "F";
	else if (gpio == GPIOG) return "G";
	else if (gpio == GPIOH) return "H";
	else if (gpio == GPIOI) return "I";
	else return "?";
}

static const char* gpioPinToString(uint16_t pin)
{
	if (pin == GPIO_PIN_0) return "0";
	else if (pin == GPIO_PIN_1) return "1";
	else if (pin == GPIO_PIN_2) return "2";
	else if (pin == GPIO_PIN_3) return "3";
	else if (pin == GPIO_PIN_4) return "4";
	else if (pin == GPIO_PIN_5) return "5";
	else if (pin == GPIO_PIN_6) return "6";
	else if (pin == GPIO_PIN_7) return "7";
	else if (pin == GPIO_PIN_8) return "8";
	else if (pin == GPIO_PIN_9) return "9";
	else if (pin == GPIO_PIN_10) return "10";
	else if (pin == GPIO_PIN_11) return "11";
	else if (pin == GPIO_PIN_12) return "12";
	else if (pin == GPIO_PIN_13) return "13";
	else if (pin == GPIO_PIN_14) return "14";
	else if (pin == GPIO_PIN_15) return "15";
	else return "?";
}

const char* hTimerToString(TIM_HandleTypeDef* htim)
{
	if (htim == &htim1) return "1";
	else if (htim == &htim2) return "2";
	else if (htim == &htim3) return "3";
	else if (htim == &htim4) return "4";
	else if (htim == &htim5) return "5";
	else if (htim == &htim8) return "8";
	else if (htim == &htim12) return "12";
	else if (htim == &htim13) return "13";
	else if (htim == &htim14) return "14";
	else if (htim == &htim15) return "15";
	else if (htim == &htim16) return "16";
	else return "?";
}

const char* channelToString(uint32_t channel)
{
	switch(channel)
	{
	case TIM_CHANNEL_1: return "1";
	case TIM_CHANNEL_2: return "2";
	case TIM_CHANNEL_3: return "3";
	case TIM_CHANNEL_4: return "4";
	case TIM_CHANNEL_5: return "5";
	case TIM_CHANNEL_6: return "6";
	}

	return "?";
}

void PWM::printHelpMessage(UART& uart)
{
	char buffer[256];
	uart.transmit("PWM able pins: [\r\n");

	for (auto& pindef : _pin_defenitions)
	{
		sprintf(buffer, "\t{\"Port\": \"%s\", \"PinNumber\": %s, \"Timer\": %s, \"Channel\": %s}%s\r\n",
				portToString(pindef.gpioPortHandle),
				gpioPinToString(pindef.pinNumber),
				hTimerToString(pindef.timerHandle),
				channelToString(pindef.timerChannel),
				&pindef != &_pin_defenitions.back() ? "," : "");
		uart.transmit(buffer);
	}

	uart.transmit("]\r\n");
}

static const std::pair<Timer_e, TIM_HandleTypeDef*> TimerEnumMap[] =
{
	{ Timer_e::None, nullptr },
	{ Timer_e::Timer1, &htim1 }
};

PWM::PinDef* PWM::findPinDef(Port_e port, uint32_t pin, Timer_e timer, uint16_t channel)
{
	GPIO_TypeDef* gpioPort = Pin::getPort(port);
	for (auto& pinDef : PWM::_pin_defenitions)
	{
		if (gpioPort == pinDef.gpioPortHandle && pin == pinDef.pinNumber)
		{
			if ((timer == Timer_e::None || TimerEnumMap[timer].second == pinDef.timerHandle) &&
				(channel == TIM_CHANNEL_ALL || channel == pinDef.timerChannel))
			{
				return &pinDef;
			}
		}
	}

	return nullptr;
}

void PWM::startPWM(PWM::PinDef* pd, float frequency, float dutyCycle)
{
	// Start by programming the GPIO pin itself
	GPIO_InitTypeDef ginit{};
	ginit.Pin = pd->pinNumber;
	ginit.Mode = GPIO_MODE_AF_PP;
	ginit.Pull = GPIO_NOPULL;
	ginit.Speed = GPIO_SPEED_FREQ_HIGH;
	ginit.Alternate = pd->alternateFunctionValue;
	HAL_GPIO_Init(pd->gpioPortHandle, &ginit);
//	HAL_TIM_Base_Init(pd->timerHandle);
//	HAL_TIM_PWM_Init(pd->timerHandle);

	SystemCoreClockUpdate();
	float prescaler = 0;
	float period = 0;
	while (true)
	{
		period = ((HAL_GetTick() / (prescaler + 1)) / frequency);
		if (period < 65536)
		{
			break;
		}
		else
		{
			prescaler++;
		}
	}

	pd->timerHandle->Instance->PSC = prescaler;
	pd->timerHandle->Instance->ARR = period;
	pd->timerHandle->Instance->CNT = 0;

	HAL_TIM_Base_Start(pd->timerHandle);

	TIM_OC_InitTypeDef ocConfig{};
	ocConfig.OCMode = TIM_OCMODE_PWM1;
	ocConfig.Pulse = period * dutyCycle;
	ocConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	ocConfig.OCFastMode = TIM_OCFAST_DISABLE;

	HAL_TIM_PWM_ConfigChannel(pd->timerHandle, &ocConfig, pd->timerChannel);
	HAL_TIM_PWM_Start(pd->timerHandle, pd->timerChannel);
}
