### Introduction ###
This is a simple POC project for generically controlling an STM32 microcontroller via a UART (or, in this case, the ST-LINK USB to UART bridge)<br>
Input strings are also routable via TCP socket 42069<br>
The current example is designed around the NUCLEO-H563ZI (STM32H563ZI microcontroller)<br>
To port this to another STM microcontroller, use the datasheet to rebuild the _pin_defenitions table with appropriate mappings of GPIO to Timer Handle / Channel / Alternate Function Value

## Messaging ##
Upon startup, the unit will send out a list of PWM-able pins, as well as its IP address (acquired via DHCP) assuming it is connected to a DHCP capable router<br>
MessageHandlerThread determines if the input is "complete" when it receives 4 repeated CR ('\r') characters at the end of a message

## Example Terminal Input (PWM) ##
[
    {
        "Port": "B",
        "PinNumber": 0,
        "PinType": "PWM",
        "Frequency": 0.5,
        "DutyCycle": 0.5
    }
]

## Example Terminal Input (General Purpose Output) ##
[
    {
        "Port": "G",
        "PinState": "Low",
        "PinType": "PushPull",
        "PinNumber": 4
    }
]

## To store the current configuration to non-volatile memory ##
[
	{
		"Command": "StoreConfig"
	}
]

## To reset the configuration of non-volatile memory ##
[
	{
		"Command": "ResetStorage"
	}
]