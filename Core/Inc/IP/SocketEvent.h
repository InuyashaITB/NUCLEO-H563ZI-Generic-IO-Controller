#ifndef USER_CORE_INC_IP_SOCKETEVENT_H_
#define USER_CORE_INC_IP_SOCKETEVENT_H_

#include "Event.h"

enum SocketEvents : ULONG {
	Connected		= 0b10,
	Disconnected	= 0b100,
	DataReceived	= 0b1000,
};

class SocketEvent : public Event<SocketEvents>
{
public:
	SocketEvent() : Event() { }
protected:
private:
};



#endif /* USER_CORE_INC_IP_SOCKETEVENT_H_ */
