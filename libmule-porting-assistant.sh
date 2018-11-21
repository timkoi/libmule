#!/bin/bash
# libMule Porting Assistant

openinpreferrededitor() {
	PREFERREDEDITOR=
	if test "$EDITOR" != ""; then
		PREFERREDEDITOR=$EDITOR
	else
		if command -v nano > /dev/null 2>&1; then
			PREFERREDEDITOR=nano
		elif command -v xemacs > /dev/null 2>&1; then
			PREFERREDEDITOR=xemacs
		elif command -v emacs > /dev/null 2>&1; then
			PREFERREDEDITOR=emacs
		elif command -v vim > /dev/null 2>&1; then
			PREFERREDEDITOR=vim
		else
			PREFERREDEDITOR=vi
		fi
	fi
	echo "Your preferred editor appears to be $PREFERREDEDITOR, if not, then set the EDITOR variable properly."
	sleep 1
	$PREFERREDEDITOR $1
}

changepinsstrings() {
	printf "Which C++ type can handle values recieved from microcontroller's pins? (unsigned or int) {default: unsigned} "
	read userinput_pin
	printf "Which string type is supported by $PLATFORMNAME (stdstring) {default: stdstring} "
	read userinput_str
	if test "$userinput_pin" != "unsigned" && test "$userinput_pin" != "int"; then
		userinput_pin=unsigned
	fi
	userinput_str=stdstring
	PLATFORMSTRING=$userinput_str
	PLATFORMHWPINTYPE=$userinput_pin
}

changestopcode() {
	echo "[WARNING] It is not recommended that you change this setting, because you might break servo motor stopping mechanism used by libMule. Modify with caution!"
	sleep 2
	printf "Which stop code? {default: -789} "
	read userinput_sc
	if test "$userinput_sc" = ""; then
		userinput_sc=-789
	fi
	PLATFORMSTOPCODE=$userinput_sc
}

changeos() {
	printf "Which OS? (linux, qnx, bsd, unix or posix) {default: $PLATFORMOS} "
	read userinput_os
	if test "$userinput_os" = "linux" || test "$userinput_os" = "Linux"; then
		userinput_os=Linux
	elif test "$userinput_os" = "qnx" || test "$userinput_os" = "QNX"; then
		userinput_os=QNX
	elif test "$userinput_os" = "bsd" || test "$userinput_os" = "BSD" || test "$userinput_os" = "unix" || test "$userinput_os" = "UNIX"; then
		userinput_os=UNIX
	else
		userinput_os=POSIX
	fi
	PLATFORMOS=$userinput_os
}

changeadditionaldefines() {
	echo "Here is the list of defines:"
	echo "========================================================"
	echo " ID  | Name and (possibly) value                        "
	echo "========================================================"
	DEFCOUNT=0
	for DefineNum in $PLATFORMADDITIONALDEFINES; do
		DEFCOUNT=$(( $DEFCOUNT + 1 ))
		echo "$DEFCOUNT  | $DefineNum"
	done
	echo "========================================================"
	echo ""
	echo "What would you like to do?"
	echo "[1] Add a define"
	echo "[2] Remove a specific define"
	echo "[3] Clear all these defines"
	echo "[4] Abort"
	echo "ALL YOUR CHANGES ARE AUTOMATICALLY SAVED!!!"
	printf "Your choice (1-4): "
	read userinput_nc
	if test "$userinput_nc" != "1" && test "$userinput_nc" != "2" && test "$userinput_nc" != "3" && test "$userinput_nc" != "4"; then
		userinput_nc=4
	fi
	if test "$userinput_nc" = "1"; then
		printf "What to add? (NAME=VALUE) "
		read userinput_dn
		if test "$userinput_dn" = ""; then
			echo "Aborted"
			return 1
		else
			PLATFORMADDITIONALDEFINES="$userinput_dn $PLATFORMADDITIONALDEFINES"
			echo "Added successfully!"
			return 0
		fi
	elif test "$userinput_nc" = "2"; then
		printf "Which one to remove (enter the ID)? "
		read userinput_dr
		if test "$userinput_dr" = "" || test "$userinput_dr" = "0"; then
			echo "Aborted"
			return 2
		else
			NEWDEFINES=""
			DEFINERINDEX=0
			for DefineNR in $PLATFORMADDITIONALDEFINES; do
				DEFINERINDEX=$(( $DEFINERINDEX + 1 ))
				if test "$DEFINERINDEX" != "$userinput_dr"; then
					NEWDEFINES="$NEWDEFINES $DefineNR"
				fi
			done
			PLATFORMADDITIONALDEFINES="$NEWDEFINES"
			echo "Removed successfully!"
			return 0
		fi
	elif test "$userinput_nc" = "3"; then
		printf "Really? (yes/no) {default: no} "
		read userinput_dc
		if test "$userinput_dc" = ""; then
			userinput_dc=NO
		fi
		if test "$userinput_dc" = "Y" || test "$userinput_dc" = "YES" || test "$userinput_dc" = "yes" || test "$userinput_dc" = "y" || test "$userinput_dc" = "true"; then
			userinput_dc=yes
		else
			userinput_dc=no
		fi
		if test "$userinput_dc" = "yes"; then
			PLATFORMADDITIONALDEFINES=""
			echo "Removed successfully!"
			return 0
		else
			echo "Aborted"
			return 3
		fi
	else
		echo "Aborted"
		return 4
	fi
}

changedependencies() {
	printf "Does $PLATFORMNAME need any third-party libraries to work? (yes or no) {default: no} "
	read userinput_tl
	if test "$userinput_tl" = "yes" || test "$userinput_tl" = "Y" || test "$userinput_tl" = "YES" || test "$userinput_tl" = "y"; then
		userinput_tl=yes
	else
		userinput_tl=no
	fi
	if test "$userinput_tl" = "no"; then
		PLATFORMDOWNLOADS=no
		echo "Aborted"
		return 1
	fi
	PLATFORMDOWNLOADS=yes
	printf "Would you like to edit src/3rdparty/download-3rdparty-components.sh? (yes or no) {default: yes} "
	read userinput_editsh
	if test "$userinput_editsh" = "N" || test "$userinput_editsh" = "NO" || test "$userinput_editsh" = "n" || test "$userinput_editsh" = "no"; then
		userinput_editsh=no
	else
		userinput_editsh=yes
	fi
	if test "$userinput_editsh" = "yes"; then
		openinpreferrededitor ./src/3rdparty/download-3rdparty-components.sh
	else
		echo "Aborted"
		return 2
	fi
}

changesourcecode() {
	FCOUNT=0
	printf "Here is a list of headers that you can modify: "
	for HeaderFile in $PLATFORMHEADERS; do
		FCOUNT=$(( $FCOUNT + 1 ))
		printf "($FCOUNT) $HeaderFile "
	done
	echo ""
	printf "Here is a list of sources that you can modify: "
	for SourceFile in $PLATFORMSOURCES; do
		FCOUNT=$(( $FCOUNT + 1 ))
		printf "($FCOUNT) $SourceFile "
	done
	echo ""
	echo "What would you like to do?"
	echo "[1] Add an existing file or directory"
	echo "[2] Remove one of the listed files"
	echo "[3] Edit one of the listed files"
	echo "[4] Abort"
	printf "Your choice (1-3): "
	read userchoice_tmp
	if test "$userchoice_tmp" != "1" && test "$userchoice_tmp" != "2" && test "$userchoice_tmp" != "3"; then
		userchoice_tmp=4
	fi
	if test "$userchoice_tmp" = "1"; then
		echo TODO
	elif test "$userchoice_tmp" = "2"; then
		echo TODO
	elif test "$userchoice_tmp" = "3"; then
		echo TODO
	else
		echo "Aborted"
		return 4
	fi
}

changecflags() {
	echo "# Edit these variables accordingly and save this file" > /tmp/muleport.cflags
	echo "AdditionalCompilerFlags:$PLATFORMCFLAGS" >> /tmp/muleport.cflags
	echo "AdditionalLinkerFlags:$PLATFORMLDFLAGS" >> /tmp/muleport.cflags
	echo "" >> /tmp/muleport.cflags
	openinpreferrededitor /tmp/muleport.cflags
	while read FileLine; do
		FIRSTHALF=`echo "$FileLine" | cut -d ':' -f1`
		SECONDHALF=`echo "$FileLine" | cut -d ':' -f2`
		if test "$FIRSTHALF" = "AdditionalCompilerFlags"; then
			PLATFORMCFLAGS="$SECONDHALF"
		elif test "$FIRSTHALF" = "AdditionalLinkerFlags"; then
			PLATFORMLDFLAGS="$SECONDHALF"
		fi
	done < /tmp/muleport.cflags
	rm -r -f /tmp/muleport.cflags
}

editormainmenu() {
	echo "Which setting you would like to modify?"
	echo "[1] Change $PLATFORMNAME's firmware operating system type (current: $PLATFORMOS)"
	echo "[2] Change $PLATFORMNAME C++ pin type and string type (current: $PLATFORMHWPINTYPE pins and $PLATFORMSTRING strings)"
	echo "[3] Change $PLATFORMNAME's stop code (used only for motors and servos, current: $PLATFORMSTOPCODE)"
	echo "[4] Change $PLATFORMNAME compile-time defines (current: $PLATFORMADDITIONALDEFINES)"
	echo "[5] Change $PLATFORMNAME's third-party dependencies (depends on third-party libraries: $PLATFORMDOWNLOADS)"
	echo "[6] Add/remove additional compiler/linker flags used when building libMule for $PLATFORMNAME"
	echo "[7] Edit $PLATFORMNAME source code and header files"
	echo "[8] Exit libMule Porting Assistant"
	printf "Your choice (1-8): "
	read userchoice_tmp;
	if test "$userchoice_tmp" = ""; then
		userchoice_tmp=8
	fi
	if test "$userchoice_tmp" = "1"; then
		changeos
	elif test "$userchoice_tmp" = "2"; then
		changepinsstrings
	elif test "$userchoice_tmp" = "3"; then
		changestopcode
	elif test "$userchoice_tmp" = "4"; then
		changeadditionaldefines
	elif test "$userchoice_tmp" = "5"; then
		changedependencies
	elif test "$userchoice_tmp" = "6"; then
		changecflags
	elif test "$userchoice_tmp" = "7"; then
		changesourcecode
	elif test "$userchoice_tmp" = "8"; then
		printf "Save changes? Type \"yes\" or \"no\". {default: no} "
		read useranswer_toq
		if test "$useranswer_toq" = "true" || test "$useranswer_toq" = "yes" || test "$useranswer_toq" = "YES" || test "$useranswer_toq" = "y" || test "$useranswer_toq" = "Y"; then
			useranswer_toq=yes
		else
			useranswer_toq=no
		fi
		if test "$useranswer_toq" = "yes"; then
			echo "not finished yet"
		fi
		echo "Aborted"
		exit 0
	fi
	editormainmenu
}

TOOLVERSION=0.4.0
PLATFORMNAME=
PLATFORMDEFINE=
PLATFORMOS=
PLATFORMHWPINTYPE=
PLATFORMSTRING=
PLATFORMSTOPCODE=
PLATFORMADDITIONALDEFINES=
PLATFORMCOUT=no
PLATFORMDOWNLOADS=no
PLATFORMCFLAGS=
PLATFORMLDFLAGS=
PLATFORMSOURCES=
PLATFORMHEADERS=

echo "Welcome to libMule Porting Assistant $TOOLVERSION! What would you like to do?"
echo "[1] Create a new platform from scratch"
echo "[2] Create a new platform basing on an existing one"
echo "[3] Load and modify an existing platform"
echo "[4] Exit"
printf "Enter your choice (1-4): "
read userinput_tmp
if test "$userinput_tmp" != "1" && test "$userinput_tmp" != "2" && test "$userinput_tmp" != "3" && test "$userinput_tmp" != "4"; then
	userinput_tmp=4
fi
SELECTEDACTIONID=$userinput_tmp
if test "$SELECTEDACTIONID" = "1" || test "$SELECTEDACTIONID" = "2"; then
	printf "How will your platform be called? "
	read PLATFORMNAME
	printf "How will libMule internally identify your platform? (aka enter the name of the platform in upper-case letters, e.g. ARDUINO) "
	read PLATFORMDEFINE
	PLATFORMDEFINE=`echo "$PLATFORMDEFINE" | sed 's/MULE_PLATFORM_//g'`
	if test "$PLATFORMNAME" = "" || test "$PLATFORMDEFINE" = ""; then
		echo "Nothing was specified, exiting"
		exit 2
	fi
	printf "Performing several tests, this might take a while..."
	for ConfigFileCreateMule in `find ./src -type f -name "vars.mcfg"`; do
		if cat "$ConfigFileCreateMule" | grep -q "PlatformID:$PLATFORMDEFINE"; then
			echo ". failed!"
			echo "[ERROR] A platform with identifier $PLATFORMDEFINE already exists."
			exit 3
		else
			printf '.'
		fi
	done
	if test ! -e "./src/platformsupport/$PLATFORMNAME/vars.mcfg"; then
		echo ". done!"
	else
		echo ". failed!"
		echo "[ERROR] A platform with the name \"$PLATFORMNAME\" already exists."
		exit 4
	fi
	if test "$SELECTEDACTIONID" = "2"; then
		printf "Basing on which platform you would like to create your own (legoev3, pigpio_rpi or dummy)? {default: dummy} "
		read FORKEDPLATFORM
		if test "$FORKEDPLATFORM" = ""; then
			FORKEDPLATFORM=dummy
		fi
		if test ! -e "./src/platformsupport/$FORKEDPLATFORM/vars.mcfg"; then
			echo "[ERROR] Platform $FORKEDPLATFORM is not available in this release of libMule."
			exit 5
		fi
		printf "Importing changes, this may take a while... "
		mkdir -p ./src/platformsupport/$PLATFORMNAME
		cp ./src/platformsupport/$FORKEDPLATFORM/vars.mcfg ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		mv ./src/platformsupport/$PLATFORMNAME/vars.mcfg ./src/platformsupport/$PLATFORMNAME/vars.mcfg.old
		sed 's/PlatformID:/BeforeImportPlatformID:/g' ./src/platformsupport/$PLATFORMNAME/vars.mcfg.old | sed 's/Sources:/BeforeImportSources:/g' | sed 's/Headers:/BeforeImportHeaders:/g' | sed 's/AdditionalCompilerFlags:/BeforeImportCFlags:/g' > ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		rm -r -f ./src/platformsupport/$PLATFORMNAME/vars.mcfg.old
		echo "PlatformID:$PLATFORMDEFINE" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "done!"
	else
		mkdir -p ./src/platformsupport/$PLATFORMNAME
		printf "Which OS does your target device use (linux, qnx, bsd, unix or posix)? {default: posix} "
		read tmp_deviceos
		if test "$tmp_deviceos" = "linux" || test "$tmp_deviceos" = "Linux"; then
			tmp_deviceos=Linux
		elif test "$tmp_deviceos" = "qnx" || test "$tmp_deviceos" = "QNX"; then
			tmp_deviceos=QNX
		elif test "$tmp_deviceos" = "bsd" || test "$tmp_deviceos" = "unix" || test "$tmp_deviceos" = "BSD" || test "$tmp_deviceos" = "UNIX"; then
			tmp_deviceos=UNIX
		else
			tmp_deviceos=POSIX
		fi
		printf "If your target platform has any GPIO pins or device pins, what is their type in C++? Is it \"unsigned\" or \"int\"? {default: unsigned} "
		read tmp_devicepin
		if test "$tmp_devicepin" = "" || test "tmp_devicepin" != "int"; then
			tmp_devicepin=unsigned
		fi
		tmp_devicestring=stdstring
		tmp_devicestopcode=-789
		printf "Does this device have any terminal output? Type \"yes\" or \"no\". {default: no} "
		read tmp_devicehascout
		if test "$tmp_devicehascout" = "yes" || test "$tmp_devicehascout" = "Y" || test "$tmp_devicehascout" = "y" || test "$tmp_devicehascout" = "YES"; then
			tmp_devicehascout=yes
		else
			tmp_devicehascout=no
		fi
		printf "Does this platform depend on any third-party libraries that have to be downloaded from the internet? Type \"yes\" or \"no\". {default: no} "
		read tmp_devicethirdpartydl
		if test "$tmp_devicethirdpartydl" = "yes" || test "$tmp_thirdpartydl" = "Y" || test "$tmp_thirdpartydl" = "y" || test "$tmp_thirdpartydl" = "YES"; then
			tmp_devicehascout=yes
		else
			tmp_devicehascout=no
		fi
		printf "Saving all changes... "
		echo "# Automatically generated by libMule Porting Assistant on `date +"%d.%m.%Y %H:%M:%S"`" > ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "OS:$tmp_deviceos" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "PlatformID:$PLATFORMDEFINE" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "HardwarePinType:$tmp_devicepin" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "StringType:$tmp_devicestring" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "MotorStopCode:$tmp_devicestopcode" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "HasCout:$tmp_devicehascout" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "Required3rdPartyDownloads:$tmp_devicethirdpartydl" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
		echo "done!"
	fi
	printf "Any additional compiler flags needed? If yes, type them here. "
	read tmp_deviceflags
	echo "AdditionalCompilerFlags:" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
	tmp_deviceclassname="Mule $PLATFORMNAME Platform"
	tmp_deviceclassname=`echo "$tmp_deviceclassname" | sed 's/ //g'`
	printf "Are you okay with the class name $tmp_deviceclassname? {default: yes} "
	read tmp_userinput_onq
	if test "$tmp_userinput_onq" = "yes" || test "$tmp_userinput_onq" = "Y" || test "$tmp_userinput_onq" = "y" || test "$tmp_userinput_onq" = "YES" || test "$tmp_userinput_onq" = ""; then
		tmp_userinput_onq=yes
	else
		tmp_userinput_onq=no
	fi
	if test "$tmp_userinput_onq" = "no"; then
		printf "What's the new name, then? "
		read tmp_deviceclassname
		if test "$tmp_deviceclassname" = ""; then
			tmp_deviceclassname=Mule$PLATFORMNAMEPlatform
			echo "Nothing was entered, using Mule$PLATFORMNAMEPlatform name"
		fi
	fi
	echo "#ifndef PORTINGASSISTANT_CLASSHEADER_$PLATFORMDEFINE" > ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#define PORTINGASSISTANT_CLASSHEADER_$PLATFORMDEFINE" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#include \"platformsupport/common/mulecommonplatform.h\"" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#include \"core/muleconfig.h\"" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#include <iostream>" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#include <fstream>" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#include <cstdlib>" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#include <sstream>" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#define MULE_HOMEDIRECTORY \"if your device has a directory where user data can be stored, specify it here\"" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#define MULE_DOCUMENTSDIRECTORY \"if your device has a directory where user data can be stored, specify it here\"" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#define MULE_TEMPDIRECTORY \"/tmp\"" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#define MULE_INPUT 0" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#define MULE_OUTPUT 1" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "class $tmp_deviceclassname : public MuleCommonPlatform {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "    public:" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      $tmp_deviceclassname();" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      MULE_OTHER_STRINGTYPE getPlatformName() { return "$PLATFORMNAME"; }" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "# ifdef MULE_FEATURES_SENSORS" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      std::vector<MuleDevice*> getDevices();" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      MULE_OTHER_HWPINTYPE getPinMode(MULE_OTHER_HWPINTYPE pin);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool setPinMode(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE mode);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      MULE_OTHER_HWPINTYPE readFromPin(MULE_OTHER_HWPINTYPE pin);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool writeToPin(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE ct);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool setPullUpDown(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE val);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "# endif" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "# ifdef MULE_FEATURES_FILEIO" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      MULE_OTHER_STRINGTYPE readFromFile(MULE_OTHER_STRINGTYPE file);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool writeToFile(MULE_OTHER_STRINGTYPE file, MULE_OTHER_STRINGTYPE contents);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool fileExists(MULE_OTHER_STRINGTYPE file);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool deleteFile(MULE_OTHER_STRINGTYPE file);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "# endif" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "# ifdef MULE_FEATURES_SOUND" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      void doBeep();" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool playWaveFile(MULE_OTHER_STRINGTYPE filename);" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      MULE_OTHER_STRINGTYPE getSoundBackend() { return \"dynamic\"; }" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "      bool stopAllSounds();" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "# endif" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "};" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#endif"  >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h
	echo "#include \"platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h\"" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo ""  >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "$tmp_deviceclassname::$tmp_deviceclassname() {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "      // TODO: implement device initialization code here" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "std::vector<MuleDevice*> $tmp_deviceclassname::getDevices() {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     std::vector<MuleDevice*> list_of_connected_devices;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     list_of_connected_devices.clear();" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     int maxpincount = 54; // replace this with the total amount of GPIO/device pins available on your device" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     for (int i = 0; i < maxpincount; i++)" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "          list_of_connected_devices.push_back(new MuleDevice(i));" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     return list_of_connected_devices;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "MULE_OTHER_HWPINTYPE $tmp_deviceclassname::getPinMode(MULE_OTHER_HWPINTYPE pin) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     // TODO: implement this function." >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     // the only thing it does is checking if the specified pin is an input or an output pin and returning either MULE_INPUT or MULE_OUTPUT" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::setPinMode(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE mode) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     // TODO: implement this function."  >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     // the only thing it does is initializing the specified pin either as an input or an output pin (if everything is okay, then true is returned, if not, then false is returned)" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     return false;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "MULE_OTHER_HWPINTYPE $tmp_deviceclassname::readFromPin(MULE_OTHER_HWPINTYPE pin) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // TODO: implement this function." >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // it gets a value from the sensor connected to the specified pin and returns that value" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return -1;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::writeToPin(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE ct) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // TODO: implement this function." >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // it tries to write \"ct\" to the specified pin" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return false;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::setPullUpDown(MULE_OTHER_HWPINTYPE pin, MULE_OTHER_HWPINTYPE val) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // TODO: implement this function IF your target device actually supports pull-up-down resistors." >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "#ifdef MULE_FEATURES_FILEIO" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "MULE_OTHER_STRINGTYPE $tmp_deviceclassname::readFromFile(MULE_OTHER_STRINGTYPE file) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    std::ifstream filereader(file.c_str());" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    if (filereader.is_open() == false)" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "         return MULE_OTHER_STRINGTYPE(\"\");" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    MULE_OTHER_STRINGTYPE result = \"\";" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    MULE_OTHER_STRINGTYPE line;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    while (getline(filereader, line)) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "         if (result == \"\")" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "                result = line;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "         else" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "                result = result + \"\\n\" + line;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    }" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    filereader.close();" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return result;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::writeToFile(MULE_OTHER_STRINGTYPE file, MULE_OTHER_STRINGTYPE contents) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    std::ofstream outwriter(file.c_str());" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    if (outwriter.is_open() == false)" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "        return false;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    else" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "        outwriter << contents;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    outwriter.close();" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return true;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::fileExists(MULE_OTHER_STRINGTYPE file) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "     return (bool)(std::ifstream(file.c_str()));" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::deleteFile(MULE_OTHER_STRINGTYPE file) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "#  ifdef MULE_OS_UNIX" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    MULE_OTHER_STRINGTYPE cmd = \"rm -r -f \" + file;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    if (std::system(cmd.c_str()))" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "       return true;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return false;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "#  else" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return false;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "#  endif"  >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "#endif" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "#ifdef MULE_FEATURES_SOUND" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "void $tmp_deviceclassname::doBeep() {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // TODO: implement this function." >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // all it does is just beeping though the built-in PC speaker" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::playWaveFile(MULE_OTHER_STRINGTYPE filename) {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // TODO: implement this function." >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // all it does it just playing a PCM sample though the built-in PC speaker" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return false;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "bool $tmp_deviceclassname::stopAllSounds() {" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // TODO: implement this function." >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    // it just turns off the built-in PC speaker and that's it" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "    return false;" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "}" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "#endif" >> ./src/platformsupport/$PLATFORMNAME/$tmp_deviceclassname.cpp
	echo "Sources:$tmp_deviceclassname.cpp" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
	echo "Headers:$tmp_deviceclassname.h" >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
	mv ./src/platformsupport/common/mulecurrentplatform.h ./src/platformsupport/common/mulecurrentplatform.h.old
	cat ./src/platformsupport/common/mulecurrentplatform.h.old | sed "13i\#define MULE_INTERNAL_CURRENTPLATFORMNAME MULE_OTHER_STRINGTYPE(\"$PLATFORMNAME\")" | sed "13i\#define MULE_INTERNAL_CURRENTPLATFORMCLASS $tmp_deviceclassname" | sed "13i\#include \"platformsupport/$PLATFORMNAME/$tmp_deviceclassname.h\"" | sed "13i\#elif defined(MULE_PLATFORM_$PLATFORMDEFINE)" > ./src/platformsupport/common/mulecurrentplatform.h
	rm -r -f ./src/platformsupport/common/mulecurrentplatform.h.old
	echo "Platform creation finished, now you can reconfigure it to your needs"
	echo ""
elif test "$SELECTEDACTIONID" = "3"; then
	printf "Which platform? "
	read PLATFORMNAME
	PLATFORMNAME=`echo "$PLATFORMNAME" | sed 's/ //g'`
	if test "$PLATFORMNAME" = ""; then
		echo "Nothing was specified, aborted"
		exit 10
	fi
	if test ! -e "./src/platformsupport/$PLATFORMNAME/vars.mcfg"; then
		echo "[ERROR] Platform $PLATFORMNAME is not supported by this version of libMule or does not exist"
		exit 11
	fi
elif test "$SELECTEDACTIONID" = "4"; then
	echo "Aborted"
	exit 0
fi

echo " " >> ./src/platformsupport/$PLATFORMNAME/vars.mcfg
printf "Loading src/platformsupport/$PLATFORMNAME/vars.mcfg."
while read linefrommcfg; do
	printf '.'
	HALFONE=`echo "$linefrommcfg" | cut -d ':' -f1`
	HALFTWO=`echo "$linefrommcfg" | cut -d ':' -f2`
	if test "$HALFONE" = "OS"; then
		PLATFORMOS="$HALFTWO"
	elif test "$HALFONE" = "PlatformID"; then
		PLATFORMDEFINE=`echo "$HALFTWO" | sed 's/MULE_PLATFORM_//g'`
	elif test "$HALFONE" = "HardwarePinType"; then
		PLATFORMHWPINTYPE=$HALFTWO
	elif test "$HALFONE" = "StringType"; then
		PLATFORMSTRING=$HALFTWO
	elif test "$HALFONE" = "MotorStopCode"; then
		PLATFORMSTOPCODE=$HALFTWO
	elif test "$HALFONE" = "Defines"; then
		PLATFORMADDITIONALDEFINES="$HALFTWO"
	elif test "$HALFONE" = "HasCout"; then
		if test "$HALFTWO" = "true" || test "$HALFTWO" = "Y" || test "$HALFTWO" = "y" || test "$HALFTWO" = "yes" || test "$HALFTWO" = "YES"; then
			HALFTWO=yes
		else
			HALFTWO=no
		fi
		PLATFORMCOUT=$HALFTWO
	elif test "$HALFONE" = "Requires3rdPartyDownloads"; then
		if test "$HALFTWO" = "true" || test "$HALFTWO" = "Y" || test "$HALFTWO" = "y" || test "$HALFTWO" = "yes" || test "$HALFTWO" = "YES"; then
			HALFTWO=yes
		else
			HALFTWO=no
		fi
		PLATFORMDOWNLOADS=$HALFTWO
	elif test "$HALFONE" = "Sources"; then
		PLATFORMSOURCES="$HALFTWO"
	elif test "$HALFONE" = "Headers"; then
		PLATFORMHEADERS="$HALFTWO"
	elif test "$HALFONE" = "AdditionalCompilerFlags"; then
		PLATFORMCFLAGS="$HALFTWO"
	elif test "$HALFONE" = "AdditionalLinkerFlags"; then
		PLATFORMLDFLAGS="$HALFTWO"
	fi
done < ./src/platformsupport/$PLATFORMNAME/vars.mcfg
echo ". done!"
editormainmenu
