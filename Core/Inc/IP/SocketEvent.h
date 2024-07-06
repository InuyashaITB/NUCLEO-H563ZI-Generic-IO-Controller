#ifndef USER_CORE_INC_IP_SOCKETEVENT_H_
#define USER_CORE_INC_IP_SOCKETEVENT_H_

#include "Event.h"

enum class SocketEvents : ULONG {
	Connected		= 0b1,
	Disconnected	= 0b10,
	DataReceived	= 0b100
};

inline SocketEvents operator|(SocketEvents lhs, SocketEvents rhs) {
	return static_cast<SocketEvents>(
			static_cast<std::underlying_type<SocketEvents>::type>(lhs) |
			static_cast<std::underlying_type<SocketEvents>::type>(rhs)
	);
}

inline SocketEvents operator&(SocketEvents lhs, SocketEvents rhs) {
	return static_cast<SocketEvents>(
			static_cast<std::underlying_type<SocketEvents>::type>(lhs) &
			static_cast<std::underlying_type<SocketEvents>::type>(rhs)
	);
}

inline SocketEvents operator^(SocketEvents lhs, SocketEvents rhs) {
	return static_cast<SocketEvents>(
			static_cast<std::underlying_type<SocketEvents>::type>(lhs) ^
			static_cast<std::underlying_type<SocketEvents>::type>(rhs)
	);
}

inline SocketEvents operator~(SocketEvents e) {
	return static_cast<SocketEvents>(
			~static_cast<std::underlying_type<SocketEvents>::type>(e)
	);
}

// Define bitwise OR assignment operator
inline SocketEvents& operator|=(SocketEvents& lhs, SocketEvents rhs) {
    lhs = lhs | rhs;
    return lhs;
}

// Define bitwise AND assignment operator
inline SocketEvents& operator&=(SocketEvents& lhs, SocketEvents rhs) {
    lhs = lhs & rhs;
    return lhs;
}

// Define bitwise XOR assignment operator
inline SocketEvents& operator^=(SocketEvents& lhs, SocketEvents rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

inline bool operator==(SocketEvents& lhs, SocketEvents& rhs) {
	return static_cast<std::underlying_type<SocketEvents>::type>(lhs) ==
			static_cast<std::underlying_type<SocketEvents>::type>(rhs);
}

inline bool operator!=(SocketEvents& lhs, SocketEvents& rhs) {
	return static_cast<std::underlying_type<SocketEvents>::type>(lhs) !=
			static_cast<std::underlying_type<SocketEvents>::type>(rhs);
}

class SocketEvent : public Event<SocketEvents>
{
public:
	SocketEvent() : Event() { }
protected:
private:
};

#endif /* USER_CORE_INC_IP_SOCKETEVENT_H_ */
