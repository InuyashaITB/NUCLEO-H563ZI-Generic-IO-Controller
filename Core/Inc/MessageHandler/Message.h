/*
 * Message.h
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#ifndef SRC_MESSAGEHANDLER_MESSAGE_H_
#define SRC_MESSAGEHANDLER_MESSAGE_H_

#include <map>
#include <stm32h5xx_hal.h>
#include <stm32h5xx_hal_tim.h>

#define PORT_NAME "Port"
#define PIN_STATE_NAME "PinState"
#define PIN_TYPE_NAME "PinType"
#define PIN_NUMBER_NAME "PinNumber"
#define PWM_TIMER_NAME "Timer"
#define PWM_CHANNEL_NAME "Channel"
#define PWM_FREQUENCY_NAME "Frequency"
#define PWM_DUTY_CYCLE_NAME "DutyCycle"

enum Port_e {
	A, B, C, D, E, F, G, H, I
};

enum PinType_e {
	PWM,
	OpenDrain,
	PushPull,
	HighZ
};

enum PinState_e {
	NoChange,
	Low,
	High
};

enum Timer_e {
	None,
	Timer1,
	Timer2,
	Timer3,
	Timer4,
	Timer5,
	Timer8,
	Timer12,
	Timer13,
	Timer14,
	Timer15,
	Timer16,
};

struct PinConfiguration
{
	Port_e port;
	uint8_t pinNumber;
	PinType_e pinType;
	PinState_e pinState;
	Timer_e timer;
	uint16_t channel;
	float frequency;
	float duty;
};

static constexpr std::pair<const char*, Port_e> portMap[] = {
	{ "A", Port_e::A },
	{ "B", Port_e::B },
	{ "C", Port_e::C },
	{ "D", Port_e::D },
	{ "E", Port_e::E },
	{ "F", Port_e::F },
	{ "G", Port_e::G },
	{ "H", Port_e::H },
	{ "I", Port_e::I },
};

static constexpr std::pair<const char*, PinType_e> pinTypeMap[] = {
	{ "PWM", PinType_e::PWM },
	{ "OpenDrain", PinType_e::OpenDrain },
	{ "PushPull", PinType_e::PushPull },
	{ "HighZ", PinType_e::HighZ },
};

static constexpr std::pair<const char*, PinState_e> pinStateMap[] = {
	{ "NoChange", PinState_e::NoChange },
	{ "Low", PinState_e::Low },
	{ "High", PinState_e::High },
};

static constexpr std::pair<const char*, Timer_e> timerMap[] = {
	{ "1", Timer_e::Timer1 },
	{ "2", Timer_e::Timer2 },
	{ "3", Timer_e::Timer3 },
	{ "4", Timer_e::Timer4 },
	{ "5", Timer_e::Timer5 },
	{ "8", Timer_e::Timer8 },
	{ "12", Timer_e::Timer12 },
	{ "13", Timer_e::Timer13 },
	{ "14", Timer_e::Timer14 },
	{ "15", Timer_e::Timer15 },
	{ "16", Timer_e::Timer16 },
};

static constexpr std::pair<const char*, uint16_t> channelMap[] = {
	{ "1", TIM_CHANNEL_1 },
	{ "2", TIM_CHANNEL_2 },
	{ "3", TIM_CHANNEL_3 },
	{ "4", TIM_CHANNEL_4 },
	{ "5", TIM_CHANNEL_5 },
	{ "6", TIM_CHANNEL_6 },
};


#endif /* SRC_MESSAGEHANDLER_MESSAGE_H_ */
