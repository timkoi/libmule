#!/bin/sh
MULEBOOTSTRAP_CXX=
MULEBOOTSTRAP_CXXFLAGS=
MULEBOOTSTRAP_LD=
MULEBOOTSTRAP_LDFLAGS=
MULEBOOTSTRAP_COMMONFLAGS="-I$PWD/src/ideintegration/muletool"

if test -e "$PWD/muletool.bootstrapstuff"; then
	rm -r -f "$PWD/muletool.bootstrapstuff"
fi
mkdir -p "$PWD/muletool.bootstrapstuff"

printf "Probing host compilers... "
if which g++ > /dev/null 2>&1; then
	echo "GNU C++ detected"
	MULEBOOTSTRAP_CXX=g++
	MULEBOOTSTRAP_CXXFLAGS="-c -Os -g "
	MULEBOOTSTRAP_LD=g++
	MULEBOOTSTRAP_LDFLAGS="-Os"
elif which clang++ > /dev/null 2>&1; then
	echo "Apple LLVM detected"
	MULEBOOTSTRAP_CXX=clang++
	MULEBOOTSTRAP_CXXFLAGS="-c -Os -g "
	MULEBOOTSTRAP_LD=clang++
	MULEBOOTSTRAP_LDFLAGS="-Os"
elif which CC > /dev/null 2>&1; then
	echo "Sun Studio detected"
	MULEBOOTSTRAP_CXX=CC
	MULEBOOTSTRAP_CXXFLAGS="-xO3 -fast -c"
	MULEBOOTSTRAP_LD=CC
	MULEBOOTSTRAP_LDFLAGS="-xO3 -fast"
else
	echo "nothing detected"
	exit 1
fi

OBJSTOLINK=""
printf "Building muletool"
for SourceFile in $PWD/src/ideintegration/muletool/*.cpp; do
	if $MULEBOOTSTRAP_CXX $MULEBOOTSTRAP_COMMONFLAGS $MULEBOOTSTRAP_CXXFLAGS $SourceFile -o $SourceFile.mbootstrap.o; then
		OBJSTOLINK="$SourceFile.mbootstrap.o $OBJSTOLINK"
		printf "."
	else
		echo ". failed!"
		echo "Failed to compile $SourceFile"
		exit 2
	fi
done
printf "."
if $MULEBOOTSTRAP_LD $MULEBOOTSTRAP_LDFLAGS $OBJSTOLINK -o "$PWD/muletool.bootstrapstuff/muletool"; then
	echo ". done!"
else
	echo ". failed!"
	echo "Failed to link muletool"
	exit 3
fi

printf "Creating $PWD/muletool.bootstrapstuff/MuleTool.mcfg... "
echo "# Automatically generated by bootstrap-muletool.sh" > $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "LIBMULE: " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "TARGET:$MULETARGET " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "CC:$CC " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "CXX:$CXX " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "CFLAGS:$CFLAGS " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "CXXFLAGS:$CXXFLAGS " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "LD:$LD " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "LDFLAGS:$LDFLAGS " >> $PWD/muletool.bootstrapstuff/MuleTool.mcfg
echo "done!"
exit 0
