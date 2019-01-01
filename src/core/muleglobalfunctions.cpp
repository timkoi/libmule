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

#include "core/muleglobalfunctions.h"

void mulestrmodreplace(MULE_OTHER_STRINGTYPE& subject, MULE_OTHER_STRINGTYPE search,
                          MULE_OTHER_STRINGTYPE replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != MULE_OTHER_STRINGTYPE::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}
MULE_OTHER_STRINGTYPE muleyesno(bool in) {
    if (in == true)
        return "yes";
    else
        return "no";
}

void muleprintf(MULE_OTHER_STRINGTYPE in, ...) {
    va_list otherargs;
    va_start(otherargs, in);
#ifdef MULE_FEATURES_SUPPORTSCOUT
    vprintf(in.c_str(), otherargs);
#endif
#ifdef MULE_PLATFORM_LEGOEV3
    LcdPrintf('0', in.c_str(), otherargs);
    Wait(1000);
#elif defined(MULE_PLATFORM_MICROCONTROLLERSIM)
    FILE* myf;
#  ifndef _WIN32
    char* name = "/tmp/mulemicrosim/SERIAL";
#  else
    const char* name = MULE_OTHER_STRINGTYPE(MULE_OTHER_STRINGTYPE(getenv("TEMP")) + "\\mulemicrosim\\SERIAL").c_str();
#  endif
    myf = fopen(name, "a");
    if (myf != NULL) {
	fprintf(myf, in.c_str(), otherargs);
	fclose(myf);
    }
    else
	delete myf;
#endif
    va_end(otherargs);
    return;
}

MULE_OTHER_STRINGTYPE muleinttostr(int in) {
	char buf[70];
	itoa(in, buf, 10);
	return MULE_OTHER_STRINGTYPE(buf);
}

void muledebug(MULE_OTHER_STRINGTYPE debugout) {
#ifdef MULE_FEATURES_DEBUG
    muleprintf("[DEBUG] " + debugout + "\n");
#endif
    return;
}

MULE_OTHER_STRINGTYPE mulegetcwd() {
#ifdef MULE_FEATURES_FILEIO
    char wtw[1600];
    getcwd(wtw, 1600);
    return MULE_OTHER_STRINGTYPE(wtw);
#else
    return MULE_OTHER_STRINGTYPE("/undefined");
#endif
}


void muleexception(int errcode, MULE_OTHER_STRINGTYPE errtext, bool cancatch) {
    muleprintf("libMule Application Exception\n");
    muleprintf("An error occured while running the code of current application.\n");
    muleprintf("\n");
    muleprintf("Error code: %d\n", errcode);
    muleprintf("Message: %s\n", errtext.c_str());
    muleprintf("Critical: %s\n", muleyesno(cancatch).c_str());
    muleprintf("\n");
    if (cancatch == true) {
        muleprintf("This program will continue to run if the algorithm catches this exception.\n");
#ifndef MULE_INTERNAL_NOEXCEPTIONS
        std::exception();
#else
	muleprintf("Your microcontroller does not support exceptions, so the app will continue to work, but bugs and glitches might occur.");
#endif
    }
    else {
        muleprintf("This program will now close because this exception is critical.\n");
        exit(errcode + 500);
    }
    return;
}


void mulesleep(double seconds) {
#ifdef MULE_PLATFORM_LEGOEV3
	Wait((int)(seconds * 1000));
#elif defined(MULE_PLATFORM_MICROCONTROLLERSIM)
	usleep((int)(1000 * 1000 * seconds));
#elif defined(MULE_PLATFORM_ARDUINOAVR)
	delay((long)(seconds * 1000));
#elif defined(MULE_OS_UNIX)
	std::system(MULE_OTHER_STRINGTYPE("sleep " + muleinttostr((int)(ceil(seconds)))).c_str());
#else
	muleprintf("[libMule/WARNING] mulesleep is not supported on this platform");
	return;
#endif
}

void mulemicrosecsleep(int microseconds) {
#ifdef MULE_OS_LINUX
	usleep(microseconds);
#else
	mulesleep(((1/100000) * microseconds));
#endif
}

