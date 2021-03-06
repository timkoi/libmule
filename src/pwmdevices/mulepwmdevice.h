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

#ifndef MULEPWMDEVICE_H
#define MULEPWMDEVICE_H

#include "core/muledevice.h"
#include "core/muleglobalfunctions.h"
#include "platformsupport/common/mulecommonplatform.h"
#include MULE_OTHER_NATIVEPLATFORMHEADER

class MulePWMDevice : public MuleDevice {
	public:
	  MulePWMDevice(int devpin);
	  ~MulePWMDevice();
	  MULE_OTHER_HWPINTYPE read() { return getRange(); }
	  bool write(MULE_OTHER_HWPINTYPE val) { return start(val); }
	  bool start(MULE_OTHER_HWPINTYPE dutycycle);
	  bool on() { return start(getRange()); }
	  bool stop() { return start(0); }
	  bool off() { return stop(); }
	  int getRange();
	  int range() { return getRange(); }
	  int getFrequency();
	  int frequency() { return getFrequency(); }
	  bool isOn();
	  bool isActive() { return isOn(); };
	  bool setRange(MULE_OTHER_HWPINTYPE range);
	  bool setFrequency(MULE_OTHER_HWPINTYPE frequency);
	  
	private:
	  bool deviceIsTurnedOn;
};

#endif
