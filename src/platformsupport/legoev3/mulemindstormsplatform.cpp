#include "mulemindstormsplatform.h"

MuleMindstormsPlatform::MuleMindstormsPlatform()
{
    if (initialize() == false)
        platformInitializationException(1, "MuleMindstormsPlatform::initialize() returned false, not true");
}

bool MuleMindstormsPlatform::initialize() {
	InitEV3();
	return true;
}

MuleMindstormsPlatform::~MuleMindstormsPlatform() {
	FreeEV3();
	return;
}

#ifdef MULE_FEATURES_SENSORS
std::vector<MuleDevice*> MuleMindstormsPlatform::getDevices() {
    devlist.clear();
    for (int i = 0; i < 5; i++)
	devlist.push_back(new MuleDevice(i + 1));
    return devlist;
}

MULE_OTHER_HWPINTYPE MuleMindstormsPlatform::getPinMode(MULE_OTHER_HWPINTYPE pin) {
    muledebug("pin = " + muleinttostr((int)(pin)));
    platformInitializationException(9, "Not finished yet");
    return 0;
}

bool MuleMindstormsPlatform::setPinMode(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE mode) {
    muledebug("pin = " + muleinttostr((int)(pin)));
    muledebug("mode = " + muleinttostr((int)(mode)));
    if (pin == 5)
	return writeToPin(pin, mode);
    else
	return false;
}

MULE_OTHER_HWPINTYPE MuleMindstormsPlatform::readFromPin(MULE_OTHER_HWPINTYPE pin) {
    muledebug("pin = " + muleinttostr((int)(pin)));
    if (pin == 1)
	return readSensor(IN_1);
    else if (pin == 2)
	return readSensor(IN_2);
    else if (pin == 3)
	return readSensor(IN_3);
    else if (pin == 4)
	return readSensor(IN_4);
    else if (pin == 5)
	return (MotorPower(OUT_ALL) - '0');
    else {
	platformInitializationException(3, "No such port on the EV3: " + muleinttostr(pin));
	return -1;
    }
}

bool MuleMindstormsPlatform::writeToPin(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE ct) {
    muledebug("pin = " + muleinttostr((int)(pin)));
    muledebug("ct = " + muleinttostr((int)(ct)));
    if (pin == 5) {
	int motrot = ct;
	if (motrot == MULE_MINDSTORMS_MOTOROFF)
		Off(OUT_ALL);
	else if (motrot < 0) {
		motrot = abs(motrot);
		OnRevReg(OUT_ALL, motrot);
	}
	else
		OnFwdReg(OUT_ALL, motrot);
	return true;
    }
    else
    	return false;
}
#endif

#ifdef MULE_FEATURES_FILEIO
MULE_OTHER_STRINGTYPE MuleMindstormsPlatform::readFromFile(MULE_OTHER_STRINGTYPE file) {
    muledebug("file = " + file);
    std::ifstream t("/tmp/" + file);
    return MULE_OTHER_STRINGTYPE((std::istreambuf_iterator<char>(t)),
                                    std::istreambuf_iterator<char>());
}

bool MuleMindstormsPlatform::writeToFile(MULE_OTHER_STRINGTYPE file, MULE_OTHER_STRINGTYPE ct) {
    if (fileExists(file) == false)
        return false;
    std::ofstream stream("/tmp/" + file);
    stream << ct;
    stream.close();
    return true;
}

bool MuleMindstormsPlatform::fileExists(MULE_OTHER_STRINGTYPE file) {
    MULE_OTHER_STRINGTYPE fnamereal = "/tmp/" + file;
    struct stat buffer;
    return (stat (fnamereal.c_str(), &buffer) == 0);
}
#endif