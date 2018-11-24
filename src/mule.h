// mule.h - global include header for libMule
// Part of libMule
// Copyright (c) Tim K (timprogrammer@rambler.ru) 2018. Licensed under GNU GPLv3.

#include "core/muleapplication.h"
#include "core/muledevice.h"
#include "core/muleglobalfunctions.h"
#include "core/muleconfig.h"
#include "platformsupport/common/mulecommonplatform.h"
#include "platformsupport/common/mulecurrentplatform.h"
#ifdef MULE_PLATFORM_DUMMY
#include "platformsupport/dummy/muledummyplatform.h"
#elif defined(MULE_PLATFORM_LEGOEV3)
#include "platformsupport/legoev3/mulemindstormsplatform.h"
#elif defined(MULE_PLATFORM_PIGPIO)
#include "platformsupport/pigpio_rpi/mulepigpioplatform.h"
#elif defined(MULE_PLATFORM_ARDUINOAVR)
#include "platformsupport/arduinoavr/MuleArduinoAVRPlatform.h"
#endif
#ifdef MULE_FEATURES_FILEIO
#include "fileio/mulefile.h"
#include "fileio/muledirectoryfunctions.h"
#endif
#ifdef MULE_FEATURES_SOUND
#include "sound/mulesoundplayer.h"
#endif
#ifdef MULE_FEATURES_PWMDEVICES
#include "pwmdevices/mulepwmdevice.h"
#include "pwmdevices/muleservodevice.h"
#endif
#ifdef MULE_FEATURES_SENSORS
#include "sensors/mulesensor.h"
#endif
