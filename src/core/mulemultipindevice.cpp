#include "core/mulemultipindevice.h"

MuleMultipinDevice::MuleMultipinDevice(MULE_OTHER_HWPINTYPE pin1, MULE_OTHER_HWPINTYPE pin2, MULE_OTHER_HWPINTYPE pin3, MULE_OTHER_HWPINTYPE pin4, MULE_OTHER_HWPINTYPE pin5, MULE_OTHER_HWPINTYPE pin6, MULE_OTHER_HWPINTYPE pin7, MULE_OTHER_HWPINTYPE pin8) {
	if (internalInit(pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8) != 0)
		muleexception(84, "MuleMultipinDevice could not initialize", false);
}

MuleMultipinDevice::MuleMultipinDevice(MuleDevice dev1, MuleDevice dev2, MuleDevice dev3, MuleDevice dev4, MuleDevice dev5, MuleDevice dev6, MuleDevice dev7, MuleDevice dev8) {
	if (internalInit(dev1, dev2, dev3, dev4, dev5, dev6, dev7, dev8) != 0)
		muleexception(84, "MuleMultipinDevice could not initialize", false);
}

MuleMultipinDevice::MuleMultipinDevice(std::vector<MuleDevice> devs) {
	if (internalInit(devs) != 0)
		muleexception(84, "MuleMultipinDevice could not initialize", false);
}

MuleMultipinDevice::MuleMultipinDevice(std::vector<MuleDevice*> devs) {
	if (internalInit(devs) != 0)
		muleexception(84, "MuleMultipinDevice could not initialize", false);
}

MuleMultipinDevice::MuleMultipinDevice(int pins[MULE_MULTIPIN_LIMIT]) {
	if (internalInit(pins) != 0)
		muleexception(84, "MuleMultipinDevice could not initialize", false);
}

MuleMultipinDevice::MuleMultipinDevice(MuleDevice* dev1, MuleDevice* dev2, MuleDevice* dev3, MuleDevice* dev4, MuleDevice* dev5, MuleDevice* dev6, MuleDevice* dev7, MuleDevice* dev8) {
	if (internalInit(dev1, dev2, dev3, dev4, dev5, dev6, dev7, dev8) != 0)
		muleexception(84, "MuleMultipinDevice could not initialize", false);
}

MuleMultipinDevice::MuleMultipinDevice(std::vector<MULE_OTHER_HWPINTYPE> pinsvec) {
	if (internalInit(pinsvec) != 0)
		muleexception(84, "MuleMultipinDevice could not initialize", false);
}

int MuleMultipinDevice::internalInit(MULE_OTHER_HWPINTYPE pin1, MULE_OTHER_HWPINTYPE pin2, MULE_OTHER_HWPINTYPE pin3, MULE_OTHER_HWPINTYPE pin4, MULE_OTHER_HWPINTYPE pin5, MULE_OTHER_HWPINTYPE pin6, MULE_OTHER_HWPINTYPE pin7, MULE_OTHER_HWPINTYPE pin8) {
	std::vector<MuleDevice*> devices;
	devices.push_back(new MuleDevice(pin1));
	if (pin2 != -1)
		devices.push_back(new MuleDevice(pin2));
	if (pin3 != -1)
		devices.push_back(new MuleDevice(pin3));
	if (pin4 != -1)
		devices.push_back(new MuleDevice(pin4));
	if (pin5 != -1)
		devices.push_back(new MuleDevice(pin5));
	if (pin6 != -1)
		devices.push_back(new MuleDevice(pin6));
	if (pin7 != -1)
		devices.push_back(new MuleDevice(pin7));
	if (pin8 != -1)
		devices.push_back(new MuleDevice(pin8));
	mDevices = devices;
	return 0;
}

int MuleMultipinDevice::internalInit(MuleDevice* dev1, MuleDevice* dev2, MuleDevice* dev3, MuleDevice* dev4, MuleDevice* dev5, MuleDevice* dev6, MuleDevice* dev7, MuleDevice* dev8) {
	std::vector<MuleDevice*> devices;
	devices.push_back(dev1);
	if (dev2->getPin() != -1)
		devices.push_back(dev2);
	if (dev3->getPin() != -1)
		devices.push_back(dev3);
	if (dev4->getPin() != -1)
		devices.push_back(dev4);
	if (dev5->getPin() != -1)
		devices.push_back(dev5);
	if (dev6->getPin() != -1)
		devices.push_back(dev6);
	if (dev7->getPin() != -1)
		devices.push_back(dev7);
	if (dev8->getPin() != -1)
		devices.push_back(dev8);
	return 0;
}

int MuleMultipinDevice::internalInit(MuleDevice dev1, MuleDevice dev2, MuleDevice dev3, MuleDevice dev4, MuleDevice dev5, MuleDevice dev6, MuleDevice dev7, MuleDevice dev8) {
	std::vector<MuleDevice*> devices;
	devices.push_back(&dev1);
	if (dev2.getPin() != -1)
		devices.push_back(&dev2);
	if (dev3.getPin() != -1)
		devices.push_back(&dev3);
	if (dev4.getPin() != -1)
		devices.push_back(&dev4);
	if (dev5.getPin() != -1)
		devices.push_back(&dev5);
	if (dev6.getPin() != -1)
		devices.push_back(&dev6);
	if (dev7.getPin() != -1)
		devices.push_back(&dev7);
	if (dev8.getPin() != -1)
		devices.push_back(&dev8);
	mDevices = devices;
	return 0;
}

int MuleMultipinDevice::internalInit(std::vector<MuleDevice> devs) {
	if (devs.size() > MULE_MULTIPIN_LIMIT) {
		muleexception(-24, "MuleMultipinDevice can maximum handle " + muleinttostr(MULE_MULTIPIN_LIMIT) + " devices", false);
		return -1;
	}
	std::vector<MuleDevice*> devices;
	for (int i = 0; i < devs.size(); i++) {
		if (devs[i].getPin() != -1)
			devices.push_back(&(devs[i]));
	}
	mDevices = devices;
	return 0;
}

int MuleMultipinDevice::internalInit(std::vector<MuleDevice*> devs) {
	if (devs.size() > MULE_MULTIPIN_LIMIT) {
		muleexception(-24, "MuleMultipinDevice can maximum handle " + muleinttostr(MULE_MULTIPIN_LIMIT) + " devices", false);
		return -1;
	}
	mDevices = devs;
	return 0;
}

int MuleMultipinDevice::internalInit(int pins[MULE_MULTIPIN_LIMIT]) {
	return internalInit(pins[0], pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7]);
}

int MuleMultipinDevice::internalInit(std::vector<MULE_OTHER_HWPINTYPE> pinsvec) {
	if (pinsvec.size() > MULE_MULTIPIN_LIMIT) {
		muleexception(-24, "MuleMultipinDevice can maximum handle " + muleinttostr(MULE_MULTIPIN_LIMIT) + " devices", false);
		return -1;
	}
	std::vector<MuleDevice*> devvec;
	for (int i = 0; i < pinsvec.size(); i++) {
		if (pinsvec[i] != -1)
			devvec.push_back(new MuleDevice(pinsvec[i]));
	}
	mDevices = devvec;
	return 0;
}

MuleMultipinDevice::~MuleMultipinDevice() {
	for (int i = 0; i < mDevices.size(); i++)
		delete mDevices[i];
}

MuleDevice* MuleMultipinDevice::getPin(int nPin) {
	muledebug("MuleMultipinDevice::getPin(" + muleinttostr(nPin) + ")");
	for (int i = 0; i < mDevices.size(); i++) {
		muledebug("i = " + muleinttostr(i));
		muledebug("mDevices[i]->getPin() = " + muleinttostr(mDevices[i]->getPin()));
		if (mDevices[i]->getPin() == nPin) {
			muledebug("found the correct pin");
			return mDevices[i];
		}
		else
			muledebug("this isn't the correct pin");
	}
	muleexception(-25, "No device with pin " + muleinttostr(nPin) + " found");
	return new MuleDevice(-1);
}

MuleDevice* MuleMultipinDevice::getIndex(int nIndex) {
	if (nIndex > (mDevices.size() - 1))
		return mDevices[nIndex];
	muleexception(-26, "No device with index " + muleinttostr(nIndex) + " found");
	return new MuleDevice(-1);
}

bool MuleMultipinDevice::removePin(int nPin) {
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->getPin() == nPin) {
			mDevices.erase(mDevices.begin() + i);
			return true;
		}
	}
	muleexception(-25, "No device with pin " + muleinttostr(nPin) + " found");
	return false;
}

bool MuleMultipinDevice::removeIndex(int nIndex) {
	if (nIndex < mDevices.size()) {
		mDevices.erase(mDevices.begin() + nIndex);
		return true;
	}
	muleexception(-26, "No device with index " + muleinttostr(nIndex) + " found");
	return false;
}

bool MuleMultipinDevice::addPin(MuleDevice* dev) {
	if ((mDevices.size() + 1) > MULE_MULTIPIN_LIMIT) {
		muleexception(-24, "MuleMultipinDevice can maximum handle " + muleinttostr(MULE_MULTIPIN_LIMIT) + " devices");
		return false;
	}
	mDevices.push_back(dev);
	return true;
}

bool MuleMultipinDevice::writeToAll(MULE_OTHER_HWPINTYPE val) {
	 for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->write(val) == false) {
			muleexception(-28, "Cannot write to device");
			return false;
		}
	 }
	 return false;
}

std::vector<MULE_OTHER_HWPINTYPE> MuleMultipinDevice::readFromAll() {
	std::vector<MULE_OTHER_HWPINTYPE> resultvec;
	for (int i = 0; i < mDevices.size(); i++)
		resultvec.push_back(mDevices[i]->read());
	return resultvec;
}

std::vector<MULE_OTHER_HWPINTYPE> MuleMultipinDevice::toPinVector() {
	std::vector<MULE_OTHER_HWPINTYPE> resultvec;
	for (int i = 0; i < mDevices.size(); i++)
		resultvec.push_back(mDevices[i]->getPin());
	return resultvec;
}
