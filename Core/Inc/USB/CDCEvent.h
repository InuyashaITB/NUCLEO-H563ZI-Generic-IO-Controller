#ifndef SRC_USB_CDCEVENT_H_
#define SRC_USB_CDCEVENT_H_

#include "Event.h"

enum CDCEvents {
	Connected = 0b1,
	Disconnected = 0b10,
	DataReceived = 0b100,
};

inline CDCEvents operator|(CDCEvents lhs, CDCEvents rhs) {
	return static_cast<CDCEvents>(
			static_cast<std::underlying_type<CDCEvents>::type>(lhs) |
			static_cast<std::underlying_type<CDCEvents>::type>(rhs)
	);
}

inline CDCEvents operator&(CDCEvents lhs, CDCEvents rhs) {
	return static_cast<CDCEvents>(
			static_cast<std::underlying_type<CDCEvents>::type>(lhs) &
			static_cast<std::underlying_type<CDCEvents>::type>(rhs)
	);
}

inline CDCEvents operator^(CDCEvents lhs, CDCEvents rhs) {
	return static_cast<CDCEvents>(
			static_cast<std::underlying_type<CDCEvents>::type>(lhs) ^
			static_cast<std::underlying_type<CDCEvents>::type>(rhs)
	);
}

inline CDCEvents operator~(CDCEvents e) {
	return static_cast<CDCEvents>(
			~static_cast<std::underlying_type<CDCEvents>::type>(e)
	);
}

// Define bitwise OR assignment operator
inline CDCEvents& operator|=(CDCEvents& lhs, CDCEvents rhs) {
    lhs = lhs | rhs;
    return lhs;
}

// Define bitwise AND assignment operator
inline CDCEvents& operator&=(CDCEvents& lhs, CDCEvents rhs) {
    lhs = lhs & rhs;
    return lhs;
}

// Define bitwise XOR assignment operator
inline CDCEvents& operator^=(CDCEvents& lhs, CDCEvents rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

inline bool operator==(CDCEvents& lhs, CDCEvents& rhs) {
	return static_cast<std::underlying_type<CDCEvents>::type>(lhs) ==
			static_cast<std::underlying_type<CDCEvents>::type>(rhs);
}

inline bool operator!=(CDCEvents& lhs, CDCEvents& rhs) {
	return static_cast<std::underlying_type<CDCEvents>::type>(lhs) !=
			static_cast<std::underlying_type<CDCEvents>::type>(rhs);
}

class CDCEvent : public Event<CDCEvents>
{
public:
	CDCEvent() : Event {"CDC-Events"} {	}
};



#endif /* SRC_USB_CDCEVENT_H_ */
