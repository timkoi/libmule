//
// This file is a part of libMule - Microcontroller-Universal 
// Library (that is extendable)
// Copyright (C) 2019 Tim K <timprogrammer@rambler.ru>
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

#include "core/muleplatformdependantfunctions.h"
#include <unistd.h>

void muleplatformprintf(MULE_OTHER_STRINGTYPE in, ...) {
	va_list otherargs;
	va_start(otherargs, in);
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
	va_end(otherargs);
}
void muleplatformsleep(double seconds) {
	usleep(seconds * 1000 * 1000);
}
