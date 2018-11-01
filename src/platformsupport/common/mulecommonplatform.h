
#ifndef MULECOMMONPLATFORM_H
#define MULECOMMONPLATFORM_H

#include "core/muledevice.h"
#include "core/muleconfig.h"
#include <vector>
#include <string>
#include <cstdlib>
#include "core/muleglobalfunctions.h"

class MuleCommonPlatform
{
public:
    MuleCommonPlatform() {}

    virtual bool initialize() {}
#ifdef MULE_FEATURES_SENSORS
    virtual std::vector<MuleDevice*> getDevices() {}
    virtual MULE_OTHER_HWPINTYPE getPinMode(MULE_OTHER_HWPINTYPE pin) {}
    virtual bool setPinMode(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE mode) {}
    virtual MULE_OTHER_HWPINTYPE readFromPin(MULE_OTHER_HWPINTYPE pin) {}
    virtual bool writeToPin(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE ct) {}
#endif
#ifdef MULE_FEATURES_FILEIO
    virtual MULE_OTHER_STRINGTYPE readFromFile(MULE_OTHER_STRINGTYPE file) {}
    virtual bool writeToFile(MULE_OTHER_STRINGTYPE file, MULE_OTHER_STRINGTYPE ct) {}
    virtual bool fileExists(MULE_OTHER_STRINGTYPE file) {}
    virtual bool deleteFile(MULE_OTHER_STRINGTYPE file) {}
#endif
#ifdef MULE_FEATURES_SOUND
    virtual void doBeep() {}
    virtual bool playWaveFile(MULE_OTHER_STRINGTYPE filename) {}
    virtual MULE_OTHER_STRINGTYPE getSoundBackend() {}
#endif
protected:
    void platformInitializationException(const int& erc, const MULE_OTHER_STRINGTYPE& message) {
        muleprintf("libMule " + muleinttostr(MULE_VERSION_MAJOR) + "." + muleinttostr(MULE_VERSION_MINOR) + "." + muleinttostr(MULE_VERSION_UPDATE) + " Platform Initialization Error\n");
        muleprintf( "A critical error occured during the initialization of the device platform.\n");
        muleprintf("\n");
        muleprintf("Error №" + muleinttostr(erc) + ": " + message);
        muleprintf("\n\n");
        muleprintf("The application will now exit now.\n");
        int exitcode = 400 + erc;
	std::exit(exitcode);
    }


};

#endif // MULECOMMONPLATFORM_H
