#include "CDC.h"
#include "Debug.h"

CDC* CDC::instance {nullptr};

extern bool usb_connected;

void CDC::main()
{
	while (handle == nullptr)
		tx_thread_sleep(100);

	CDC::Reader reader { handle, events };
	reader.start();

	while (true)
	{
		CDCEvents e;
		events.waitForEventFlagsAny(static_cast<CDCEvents>(0xFFFF'FFFF), e, TX_WAIT_FOREVER);

		if (e & CDCEvents::Disconnected)
		{
			Debug::send("USB-CDC Disconnected");
			connected = usb_connected;
		}

		if (e & CDCEvents::Connected)
		{
			Debug::send("USB-CDC Connected");
			connected = usb_connected;
		}

		if (e & CDCEvents::DataReceived)
		{
			uint8_t buffer[512];
			size_t outputLength;
			reader.retreiveData(buffer, sizeof(buffer) - 1, outputLength);
			buffer[outputLength] = 0;
			rxReceivedCallback(buffer, outputLength);
		}
	}
}

void CDC::transmit(const char* msg)
{
	const char* pMsg = msg;
	ULONG len = strlen(msg);

	while (len > 0 && connected)
	{
		ULONG sentLength = 0;
		ux_device_class_cdc_acm_write(handle, (UCHAR*)pMsg, len, &sentLength);

		if (sentLength == 0)
			break;

		pMsg += sentLength;
		len -= sentLength;
	}
}

void CDC::Reader::main()
{
	size_t readLength = 0;
	dataLock.give();
	while (true)
	{
		ux_device_class_cdc_acm_read(handle, (UCHAR *)buffer, sizeof(buffer), (ULONG*)&readLength);

		if (readLength > 0)
		{
			handleReceivedData(buffer, readLength);

			// If there is still data remaining, throw an error
			if (readLength > 0)
				Debug::send("USB-CDC-RX Dropped %u bytes", readLength);
		}

		tx_thread_sleep(MS_TO_TICK(10));
	}
}

void CDC::Reader::handleReceivedData(char* data, size_t& readLength)
{
	dataLock.take();
	char* pMyData = &buffer[currentDataLength];
	size_t copyLength = std::min(readLength, sizeof(buffer) - currentDataLength);
	std::copy(data, data + copyLength, pMyData);
	dataLock.give();

	readLength -= copyLength;
	eventHandler.post(CDCEvents::DataReceived);
}

void CDC::Reader::retreiveData(uint8_t* pOutput, size_t maxLength, size_t& outputLength)
{
	dataLock.take();
	outputLength = std::min(maxLength, this->currentDataLength);
	std::copy(std::begin(this->buffer), std::end(this->buffer),pOutput);
	this->currentDataLength -= outputLength;
	dataLock.give();
}
