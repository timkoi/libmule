//
// This file is a part of libMule - Microcontroller-Universal 
// Library (that is extendable)
// Copyright (C) 2018 Tim K <timprogrammer@rambler.ru>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "mulepigpioplatform.h"

MulePigpioPlatform::MulePigpioPlatform() {
    if (initialize() == false)
        platformInitializationException(1, "MulePigpioPlatform::initialize() returned false, not true");
}

void MulePigpioPlatform::internal_cleanDevList() {
    gpioTerminate();
    for (int i = 0; i < devlist.size(); i++) {
	delete devlist.at(i);
	devlist.at(i) = NULL;
    }
    devlist.clear();
}

bool MulePigpioPlatform::initialize() {
	if (gpioInitialise() < 0)
		return false;
	devlist.clear();
	for (int i = 0; i < 54; i++)
		devlist.push_back(new MuleDevice(i));
	return true;
}

#ifdef MULE_FEATURES_CORE
MULE_OTHER_HWPINTYPE MulePigpioPlatform::getPinMode(MULE_OTHER_HWPINTYPE pin) {
    muledebug("pin = " + muleinttostr((int)(pin)));
    return gpioGetMode(pin);
}

bool MulePigpioPlatform::setPinMode(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE mode) {
    muledebug("pin = " + muleinttostr((int)(mode)));
    muledebug("mode = " + muleinttostr((int)(mode)));
    if (gpioSetMode(pin, mode) == 0)
	return true;
    return false;
}

MULE_OTHER_HWPINTYPE MulePigpioPlatform::readFromPin(MULE_OTHER_HWPINTYPE pin) {
    muledebug("pin = " + muleinttostr((int)(pin)));
    return gpioRead(pin);
}

bool MulePigpioPlatform::writeToPin(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE ct) {
    // this function is based on gpioWrite() function from tiny_gpio.c
    muledebug("pin = " + muleinttostr((int)(pin)));
    muledebug("ct = " + muleinttostr((int)(ct)));
    if (gpioWrite(pin, ct) == 0)
	return true;
    return false;
}

bool MulePigpioPlatform::setPullUpDown(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE val) {
    // this function is based on gpioSetPullUpDown() function from tiny_gpio.c
    muledebug("pin = " + muleinttostr((int)(pin)));
    muledebug("val = " + muleinttostr((int)(val)));
    if (gpioSetPullUpDown(pin, val) == 0)
	return true;
    return false;
}
#endif

MULE_OTHER_STRINGTYPE MulePigpioPlatform::readFromFile(MULE_OTHER_STRINGTYPE file) {
    muledebug("file = " + file);
    std::ifstream t(file.c_str());
    return MULE_OTHER_STRINGTYPE((std::istreambuf_iterator<char>(t)),
                                    std::istreambuf_iterator<char>());
}

bool MulePigpioPlatform::writeToFile(MULE_OTHER_STRINGTYPE file, MULE_OTHER_STRINGTYPE ct) {
    muledebug("MulePigpioPlatform::writeToFile(" + file + "," + ct + ") called");
    try {
	    std::ofstream stream(file.c_str());
	    stream << ct;
	    stream.close();
	    return true;
    }
    catch (...) {
	    muledebug("try catch failed");
    }
    return false;
}

bool MulePigpioPlatform::fileExists(MULE_OTHER_STRINGTYPE file) {
    struct stat buffer;
    return (stat (file.c_str(), &buffer) == 0);
}

bool MulePigpioPlatform::deleteFile(MULE_OTHER_STRINGTYPE file) {
    return (unlink(file.c_str()) == 0);
}


#ifdef MULE_FEATURES_SOUND
void MulePigpioPlatform::doBeep() {
	std::system("aplay -d 1 /dev/urandom");
}

bool MulePigpioPlatform::playWaveFile(MULE_OTHER_STRINGTYPE filename) {
	if (fileExists(filename) == false)
		return false;
	if (std::system(MULE_OTHER_STRINGTYPE("aplay " + filename).c_str()) == 0)
		return true;
	else
		return false;
	return false;
}

bool MulePigpioPlatform::stopAllSounds() {
	if (std::system("killall aplay") == 0)
		return true;
	else
		return false;
}
#endif

#ifdef MULE_FEATURES_PWMDEVICES
bool MulePigpioPlatform::startPWM(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE dutycycle) {
	if (gpioPWM(pin, dutycycle) == 0)
		return true;
	return false;
}

MULE_OTHER_HWPINTYPE MulePigpioPlatform::getPWMDutyCycle(MULE_OTHER_HWPINTYPE pin) {
	int result = gpioGetPWMdutycycle(pin);
	if (result != PI_BAD_USER_GPIO && result != PI_NOT_PWM_GPIO)
		return result;
	return -1;
}

MULE_OTHER_HWPINTYPE MulePigpioPlatform::getPWMRange(MULE_OTHER_HWPINTYPE pin) {
	int range = gpioGetPWMrange(pin);
	if (range != PI_BAD_USER_GPIO)
		return range;
	return -1;
}

bool MulePigpioPlatform::setPWMRange(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE range) {
	int result = gpioSetPWMrange(pin, range);
	if (result != PI_BAD_USER_GPIO && result != PI_BAD_DUTYRANGE)
		return true;
	return false;
}

MULE_OTHER_HWPINTYPE MulePigpioPlatform::getPWMFrequency(MULE_OTHER_HWPINTYPE pin) {
	int result = gpioGetPWMfrequency(pin);
	if (result != PI_BAD_USER_GPIO)
		return result;
	return -1;
}

bool MulePigpioPlatform::setPWMFrequency(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE freq) {
	int result = gpioSetPWMfrequency(pin, freq);
	if (result != PI_BAD_USER_GPIO)
		return true;
	return false;
}
#endif

#ifdef MULE_FEATURES_SENSORS
bool MulePigpioPlatform::sensorWaitUntilTriggered(MULE_OTHER_HWPINTYPE pin) {
	if (setPullUpDown(pin, MULE_PUD_DOWN) == false)
		return false;
	mulesleep(0.1);
	
	int numberofchecks = 0;
	while (readFromPin(pin) == MULE_LOW)
		numberofchecks = numberofchecks + 1;
	
	return true;
}
#endif

