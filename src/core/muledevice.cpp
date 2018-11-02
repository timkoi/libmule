#include "core/muledevice.h"
#include "core/muleapplication.h"

MuleDevice::MuleDevice(const MULE_OTHER_HWPINTYPE& pin)
{
    if (dvinit(pin))
        muledebug("MuleDevice with pin " + muleinttostr(pin) + " successfully initialized");
    else
        muleexception(8, "MuleDevice(" + muleinttostr(pin) + ") could not be initialized", true);
}

bool MuleDevice::dvinit(const MULE_OTHER_HWPINTYPE& pin) {
    pinNum = pin;
    if (!(MuleApplicationWideData::appWideFirstInstance))
        return false;
    muledebug("I'm going to access the only running MuleApplication instance to get a pointer to MuleCurrentPlatform class");
    MuleApplication* maInstance = (MuleApplication*)(MuleApplicationWideData::appWideFirstInstance);
    muledebug("Now run getPlatformClass()");
    if (maInstance->areNecessaryPartsReady == true) {
	muledebug("maInstance->areNecessaryPartsReady == true");
    	mcpInstance = (void*)(maInstance->getPlatformClass());
    	MuleCurrentPlatform* convertedMcpInstance = (MuleCurrentPlatform*)(mcpInstance);
    	convertedMcpInstance->getPinMode(pin);
    }
    else
	muledebug("maInstance->areNecessaryPartsReady != true");
    return true;
}

MULE_OTHER_HWPINTYPE MuleDevice::read() {
    MuleCurrentPlatform* convertedMcpInstance = (MuleCurrentPlatform*)(mcpInstance);
    return convertedMcpInstance->readFromPin(pinNum);
}

bool MuleDevice::write(const MULE_OTHER_HWPINTYPE& val) {
    MuleCurrentPlatform* convertedMcpInstance = (MuleCurrentPlatform*)(mcpInstance);
    return convertedMcpInstance->writeToPin(pinNum, val);
}

MULE_OTHER_HWPINTYPE MuleDevice::mode() {
    MuleCurrentPlatform* convertedMcpInstance = (MuleCurrentPlatform*)(mcpInstance);
    return convertedMcpInstance->getPinMode(pinNum);
}

bool MuleDevice::setMode(const MULE_OTHER_HWPINTYPE& mode) {
    MuleCurrentPlatform* convertedMcpInstance = (MuleCurrentPlatform*)(mcpInstance);
    return convertedMcpInstance->setPinMode(pinNum, mode);
}

bool MuleDevice::trigger(const MULE_OTHER_HWPINTYPE& pulselen, const MULE_OTHER_HWPINTYPE& level) {
	muledebug("Software implementation of GPIO trigger is active");
	try {
		this->write(level);
		mulemicrosecsleep(pulselen);
		if (level != 0)
			this->write(0);
		else
			this->write(1);
		return true;
	}
	catch (...) {
		muledebug("ASSERT or SEGFAULT was caused, will return false");
	}
	return false;
}
