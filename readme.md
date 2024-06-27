### Introduction ###
This is a simple POC project for generically controlling an STM32 microcontroller via a UART (or, in this case, the ST-LINK USB to UART bridge)<br>
The current example is designed around the NUCLEO-H563ZI (STM32H563ZI microcontroller)<br>
To port this to another STM microcontroller, use the datasheet to rebuild the _pin_defenitions table with appropriate mappings of GPIO to Timer Handle / Channel / Alternate Function Value

## Messaging ##
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