/*
 * MessageHandlerTask.cpp
 *
 *  Created on: Jun 24, 2024
 *      Author: mpeterson
 */

#include <cstring>
#include <gason.h>

#include <MessageHandlerThread.h>
#include "Pin.h"
#include "PWM.h"

using namespace gason;

static constexpr char END_OF_MESSAGE[] = "\r\r\r\r\0";
static const char* HELP_MESSAGE = "Example Structure:"
								"\r\n["
								"\r\n\t{"
								"\r\n\t\t\"" PORT_NAME "\":\"G\","
								"\r\n\t\t\"" PIN_STATE_NAME "\":\"NoChange or Low or High\","
								"\r\n\t\t\"" PIN_TYPE_NAME "\":\"OpenDrain or PushPull or HighZ\","
								"\r\n\t\t\"" PIN_NUMBER_NAME "\":#"
								"\r\n\t}"
								"\r\n]\r\n"
								"\r\r\r\r\0";


static Port_e convertPort(JsonValue v)
{
	const char* value = v ? const_cast<const char*>(v.toString()) : "";

	for (auto pair : portMap)
	{
		if (strcmp(pair.first, value) == 0)
		{
			return pair.second;
		}
	}

	return Port_e::A;
}

static PinType_e convertPinType(JsonValue v)
{
	const char* value = v ? const_cast<const char*>(v.toString()) : "";

	for (auto pair : pinTypeMap)
	{
		if (strcmp(pair.first, value) == 0)
		{
			return pair.second;
		}
	}

	return PinType_e::HighZ;
}

static PinState_e convertPinState(JsonValue v)
{
	const char* value = v ? const_cast<const char*>(v.toString()) : "";

	for (auto pair : pinStateMap)
	{
		if (strcmp(pair.first, value) == 0)
		{
			return pair.second;
		}
	}

	return PinState_e::NoChange;
}

static Timer_e convertTimer(JsonValue v)
{
	const char* value = v ? const_cast<const char*>(v.toString()) : "";

	for (auto pair : timerMap)
	{
		if (strcmp(pair.first, value) == 0)
		{
			return pair.second;
		}
	}

	return Timer_e::None;
}

static uint16_t convertChannel(JsonValue v)
{
	const char* value = v ? const_cast<const char*>(v.toString()) : "";

	for (auto pair : channelMap)
	{
		if (strcmp(pair.first, value) == 0)
		{
			return pair.second;
		}
	}

	return TIM_CHANNEL_ALL;
}

bool MessageHandlerThread::isEndOfMessage(char* check)
{
	return strcmp(END_OF_MESSAGE, check) == 0;
}

void MessageHandlerThread::main()
{
	Message message;

	uart.startReceiving();

	PWM::printHelpMessage(uart);

	while(true)
	{
		bool resetBuffer{false};
		bool sendHelpMessage{false};
		messages.pop(message);

		size_t messageLength = strlen(message);

		if (stringLength + messageLength < sizeof(stringBuffer))
		{
			strcat(stringBuffer, message);
			stringLength += messageLength;

			if (isEndOfMessage(stringBuffer + stringLength - 4))
			{
				resetBuffer = true;
				sendHelpMessage = !parseJson(stringBuffer);
			}
		}
		else
		{
			resetBuffer = true;
		}

		if (resetBuffer)
		{
			stringLength = 0;
			memset(stringBuffer, 0, sizeof(stringBuffer));
		}

		if (sendHelpMessage)
		{
			uart.transmit(HELP_MESSAGE);
		}
	}
}

bool MessageHandlerThread::parseJson(char* buffer) {
	static char tempBuffer[4096];

	size_t length = strlen(buffer);
	if (length > sizeof(tempBuffer))
		return false;

	strncpy(tempBuffer, buffer, length);

	char* endptr;
	JsonValue jsonValue;
	JsonAllocator allocator;
	auto parseResult = jsonParse(tempBuffer, &endptr, &jsonValue, allocator);
	// We expect an array of Message type
	if (JSON_PARSE_OK == parseResult && jsonValue.getTag() == JsonTag::JSON_ARRAY)
	{
		bool result = true;
		for (auto i : jsonValue)
		{
			PinConfiguration config;

			JsonValue& pinConfigurationJson = i->value;
			config.port = convertPort(pinConfigurationJson(PORT_NAME));
			config.pinState = convertPinState(pinConfigurationJson(PIN_STATE_NAME));
			config.pinType = convertPinType(pinConfigurationJson(PIN_TYPE_NAME));
			config.pinNumber = 1 << static_cast<uint32_t>(pinConfigurationJson(PIN_NUMBER_NAME).toNumber());
			switch(config.pinType)
			{
			case PinType_e::PWM:
			{
				config.timer = convertTimer(pinConfigurationJson(PWM_TIMER_NAME));
				config.channel = convertChannel(pinConfigurationJson(PWM_CHANNEL_NAME));
				config.frequency = pinConfigurationJson(PWM_FREQUENCY_NAME).toNumber();
				config.duty = pinConfigurationJson(PWM_DUTY_CYCLE_NAME).toNumber();
				auto pinDesc = PWM::findPinDef(config.port, config.pinNumber, config.timer, config.channel);
				if (pinDesc)
					PWM::startPWM(pinDesc, config.frequency, config.duty);
				break;
			}
			case PinType_e::OpenDrain:
			case PinType_e::PushPull:
			case PinType_e::HighZ: {
				Pin p;
				p.handleConfiguration(config);
				break;
			}
			}
		}

		return result;
	}

	return false;
}

void MessageHandlerThread::uartRXReceived(char* rx)
{
	Message m;
	strcpy(m, rx);
	messages.push(m);
}
