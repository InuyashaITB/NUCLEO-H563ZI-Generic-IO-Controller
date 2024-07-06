#ifndef INC_USB_CDC_H_
#define INC_USB_CDC_H_

#include <functional>

#include "ux_api.h"
#include "ux_device_cdc_acm.h"

#include "BinaryLock.h"
#include "CDCEvent.h"
#include "QueueT.h"
#include "Thread.h"

class CDC : public Thread
{
public:
	static CDC& getInstance() { return *instance; }
	CDC() : Thread("USB-CDC", TX_MID_PRIORITY, 1024 * 4) { instance = this; }

	void setRxCallback(std::function<void(uint8_t*,size_t)> func) { rxReceivedCallback = func; }
	void transmit(const char* msg);

	static void setHandle(UX_SLAVE_CLASS_CDC_ACM* h) { getInstance().handle = h; }
	static void sendEvent(CDCEvents e) { getInstance().events.post(e); }
protected:
	virtual void main() override;
private:
	static CDC* instance;
	UX_SLAVE_CLASS_CDC_ACM* handle;

	std::function<void(uint8_t*,size_t)> rxReceivedCallback{};

	CDCEvent events;

	class Reader : public Thread
	{
	public:
		Reader(UX_SLAVE_CLASS_CDC_ACM* handle, CDCEvent& eventHandler) :
			Thread("USB-CDC-RX", TX_MID_PRIORITY, 512),
			handle{handle},
			eventHandler{eventHandler}
			{
			}

		void retreiveData(uint8_t* pOutput, size_t maxLength, size_t& outputLength);
	protected:
		virtual void main() override;
	private:
		UX_SLAVE_CLASS_CDC_ACM* handle;
		CDCEvent& eventHandler;
		char buffer[1024];
		size_t currentDataLength{0};
		void handleReceivedData(char* data, size_t& readLength);
		BinaryLock dataLock { };
	};
};




#endif /* INC_USB_CDC_H_ */
