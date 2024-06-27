/*
 * BinaryLock.h
 *
 *  Created on: Jun 25, 2024
 *      Author: mpeterson
 */

#ifndef INC_UTILITY_BINARYLOCK_H_
#define INC_UTILITY_BINARYLOCK_H_

#include "Lock.h"

class BinaryLock : public Lock
{
public:
	BinaryLock(const char* name) : Lock(name, 1) { }
protected:
private:
};



#endif /* INC_UTILITY_BINARYLOCK_H_ */
