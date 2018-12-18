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

#include <mule.h>

#define SENSORPIN 0

int main() {
	MuleApplication mApp;
	muleprintf("MuleSensor test on %s", mApp.getPlatformName().c_str());
	muleprintf("Step 1. Create a new instance of MuleSensor... ");
	MuleSensor lsensor(SENSORPIN);
	muleprintf("success\n");
	muleprintf("Step 2. Wait till triggered... ");
	if (lsensor.waitUntilTriggered() == false) {
		muleprintf("failed\n");
		mApp.exit(1);
	}
	else
		muleprintf("success\n");
	muleprintf("That's it, goodbye\n");
	mApp.exit(0);
	return 0;
}

