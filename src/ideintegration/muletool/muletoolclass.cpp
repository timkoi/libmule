//
// This file is a part of libMule - Microcontroller-Universal 
// Library (that is extendable)
// Copyright (C) 2018-2019 Tim K <timprogrammer@rambler.ru>
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

#include "muletoolclass.h"

MuleToolClass::MuleToolClass(int argc, char** argv) {
#ifdef _WIN32
	dirsepchar = '\\';
	try {
		userHomeDir = std::string(std::getenv("USERPROFILE"));
	}
	catch (...) {
		userHomeDir = "C:\\";
	}
#else
	dirsepchar = '/';
	try {
		userHomeDir = "/home/" + std::string(std::getenv("USER"));
#  if defined(__APPLE__) && defined(__MACH__)
		userHomeDir = "/Users/" + std::string(std::getenv("USER"));
#  elif defined(__BEOS__)
		userHomeDir = "/boot/home";
#  endif
	}
	catch (...) {
		userHomeDir = "/";
	}
#endif
	libMuleFilenamePrefix = "";
	successfullyInitialized = init(argc, argv);
}

MuleToolClass::~MuleToolClass() {
}

bool MuleToolClass::init(int argc, char** argv) {
	// convert char** to vector
	for (int i = 1; i < argc; i++)
		cliArgs.push_back(std::string(argv[i]));
	currentProgName = std::string(argv[0]);
	actionToRun = "nmtd";
	arduinoPort = "NONE";
	if (stringVectorContains(cliArgs, "-quiet") == false && stringVectorContains(cliArgs, "-q") == false) {
		viewHeaderMessage();
		beQuiet = false;
	}
	else
		beQuiet = true;
	
	if (stringVectorContains(cliArgs, "-nolibmule") == false)
		doNotIncludeLibMule = false;
	else
		doNotIncludeLibMule = true;
		
		
	if (stringVectorContains(cliArgs, "-config") == true || stringVectorContains(cliArgs, "-c") == true)
		haveToDetectConfig = false;
	else
		haveToDetectConfig = true;
	
	if (stringVectorContains(cliArgs, "-internal-printconfigfile") == true || stringVectorContains(cliArgs, "-pf") == true) {
		actionToRun = "printconfigfilename";
		needToDumpConfigFileName = true;
	}
	else
		needToDumpConfigFileName = false;

	if (stringVectorContains(cliArgs, "-internal-printplatform") == true || stringVectorContains(cliArgs, "-pt") == true)
		actionToRun = "printplatform";

		
	if (stringVectorContains(cliArgs, "-internal-nocmdprint") == true)
		printCommandsOnScreen = false;
	else
		printCommandsOnScreen = true;
	
	if ((stringVectorContains(cliArgs, "--help") == true) || (stringVectorContains(cliArgs, "-help") == true) || (stringVectorContains(cliArgs, "-h") == true)) {
		actionToRun = "printhelp";
		return true;
	}
	else
		parseArguments();

	if (detectAndLoadConfig() == false)
		return false;
	
	return true;
}

bool MuleToolClass::detectAndLoadConfig() {
	if (haveToDetectConfig == true) {
		std::vector<std::string> possibleLocations = fillPossibleLocationsVector();
		configFilePath = FILENOTFOUND;
		for (int i = 0; i < possibleLocations.size(); i++) {
			if (fileExists(possibleLocations[i] + dirsepchar + "MuleTool.mcfg") == true) {
				configFilePath = possibleLocations[i] + dirsepchar + "MuleTool.mcfg";
				break;
			}
		}
		if (configFilePath == FILENOTFOUND) {
			verboseInfo("Configuration file not found, exiting with a critical error");
			return false;
		}
	}
	verboseInfo("Using configuration file located at " + configFilePath);
	return loadConfig(configFilePath);
}

bool MuleToolClass::loadConfig(const std::string& cfgname) {
	if (fileExists(cfgname) == false) {
		verboseInfo("MuleToolClass::loadConfig thinks that " + cfgname + " does not exist, exiting with a critical error");
		return false;
	}
	std::string filectnt = readFromFileToString(cfgname);
	std::vector<std::string> splitfilectnt = split(filectnt, std::string("\n").c_str());
	for (int i = 0; i < splitfilectnt.size(); i++) {
		if (splitfilectnt[i][0] != '#') {
			if (lastStringChar(splitfilectnt[i]) == ':')
				splitfilectnt[i] = splitfilectnt[i] + "  ";
			std::vector<std::string> splitline = split(splitfilectnt[i], std::string(":").c_str());
			std::string hone = splitline[0];
			std::string htwo = splitline[1];
			htwo = replaceContextAlternatives(hone, htwo);
			if (hone == "CC")
				compilerCC = htwo;
			else if (hone == "CXX")
				compilerCXX = htwo;
			else if (hone == "CFLAGS")
				compilerFlags_CC = htwo;
			else if (hone == "CXXFLAGS")
				compilerFlags_CXX = htwo;
			else if (hone == "LD")
				linkerLD = htwo;
			else if (hone == "LDFLAGS")
				linkerFlags_LD = htwo;
			else if (hone == "TARGET")
				libMuleTarget = htwo;
			else if (hone == "LIBMULEMULTIPREFIX" || hone == "MULTIPREFIX" || hone == "INFIX" || hone == "LIBMULEINFIX")
				libMuleFilenamePrefix = htwo;
			else if (hone == "LIBMULE") {
				libMuleLib = htwo + dirsepchar + "lib" + dirsepchar + "libMule" + libMuleFilenamePrefix + ".a";
				libMuleInclude = htwo + dirsepchar + "include" + dirsepchar + "libmule";
			}
			else if (hone == "DEPLOY")
				deploy_copyFile = htwo;
			else if (hone == "ARDUINOPORT") {
				if (arduinoPort != "NONE")
					arduinoPort = htwo;
			}
			else if (hone == "RUN")
				deploy_runFile = htwo;
		}
	}
	return true;
}

std::vector<std::string> MuleToolClass::split(const std::string& origstr, const char* delim) {
	std::vector<std::string> result;
	char* strToSplit = new char[stringLength(origstr) + 1];
	std::copy(origstr.begin(), origstr.end(), strToSplit);
	strToSplit[stringLength(origstr)] = '\0';

	char* strtokedstring = strtok(strToSplit, delim);
	result.push_back(std::string(strtokedstring));

	while (true) {
		strtokedstring = strtok(NULL, delim);
		if (!strtokedstring)
			break;
		result.push_back(std::string(strtokedstring));
	}

	return result;
}

bool MuleToolClass::fileExists(const std::string& fname) {
	std::ifstream istream(fname.c_str());
	return istream.good();
}

std::string MuleToolClass::readFromFileToString(const std::string& fname) {
	std::ifstream t(fname.c_str());
#ifndef __sun
	return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
#else
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
#endif
}

std::vector<std::string> MuleToolClass::fillPossibleLocationsVector() {
	std::vector<std::string> possibleLocations;
	possibleLocations.push_back(std::string(TOOLPREFIX) + dirsepchar + "etc");
	possibleLocations.push_back(std::string(TOOLPREFIX));
	possibleLocations.push_back(getCurrentDirectory() + dirsepchar + "etc");
	possibleLocations.push_back(getCurrentDirectory());
	possibleLocations.push_back(userHomeDir + dirsepchar + ".timkoisoft" + dirsepchar + "mule");
	possibleLocations.push_back(userHomeDir + dirsepchar + ".timkoisoft" + dirsepchar + "libmule");
	possibleLocations.push_back(userHomeDir + dirsepchar + ".config" + dirsepchar + "timkoisoft" + dirsepchar + "mule");
	possibleLocations.push_back(userHomeDir + dirsepchar + ".config" + dirsepchar + "timkoisoft" + dirsepchar + "libmule");
#ifndef _WIN32
	possibleLocations.push_back("/etc");
	possibleLocations.push_back("/etc/timkoisoft/mule");
	possibleLocations.push_back("/etc/timkoisoft/libmule");
	possibleLocations.push_back("/opt/timkoisoft/mule/etc");
	possibleLocations.push_back("/opt/timkoisoft/libmule/etc");
	possibleLocations.push_back("/usr/share/timkoisoft/libmule");
	possibleLocations.push_back("/usr/share/timkoisoft/mule");
#else
	possibleLocations.push_back(std::string(userHomeDir + "\\timkoisoft\\libmule"));
	possibleLocations.push_back(std::string(std::getenv("APPDATA")) + "\\Timkoisoft\\Mule\\etc");
	possibleLocations.push_back(std::string(std::getenv("APPDATA")) + "\\Timkoisoft\\Mule");
	possibleLocations.push_back(std::string(std::getenv("APPDATA")) + "\\Timkoisoft\\libMule\\etc");
	possibleLocations.push_back(std::string(std::getenv("APPDATA")) + "\\Timkoisoft\\libMule");
	possibleLocations.push_back(std::string(std::getenv("ALLUSERSPROFILE")) + "\\Timkoisoft\\Mule\\etc");
	possibleLocations.push_back(std::string(std::getenv("ALLUSERSPROFILE")) + "\\Timkoisoft\\Mule");
	possibleLocations.push_back(std::string(std::getenv("ALLUSERSPROFILE")) + "\\Timkoisoft\\libMule\\etc");
	possibleLocations.push_back(std::string(std::getenv("ALLUSERSPROFILE")) + "\\Timkoisoft\libMule");
	possibleLocations.push_back(std::string(std::getenv("ProgramFiles")) + "\\Timkoisoft\\Mule");
	possibleLocations.push_back(std::string(std::getenv("ProgramFiles")) + "\\Timkoisoft\\Mule");
#endif
	return possibleLocations;
}

std::string MuleToolClass::getCurrentDirectory() {
	char cwd[1400];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		return std::string(cwd);
	return userHomeDir;
}

bool MuleToolClass::stringVectorContains(const std::vector<std::string>& invec, const std::string& initem) {
	int vecsize = invec.size();
	for (int i = 0; i < vecsize; i++) {
		if (invec[i] == initem)
			return true;
	}
	return false;
}

int MuleToolClass::run() {
	if (successfullyInitialized == false)
		criticalError(-1, "Application failed to initialize because the configuration could not be loaded properly");
	
	//verboseInfo("MuleToolClass::run -> " + actionToRun);
	if (actionToRun == "printhelp") {
		viewHelp();
		return 0;
	}
	else if (actionToRun == "compile")
		return compileFiles();
	else if (actionToRun == "link")
		return linkProgram();
	else if (actionToRun == "completebuild")
		return compileAndLinkProgram();
	else if (actionToRun == "deploy")
		return deployProgram();
	else if (actionToRun == "printconfigfilename") {
		std::cout << configFilePath << std::endl;
		return 0;
	}
	else if (actionToRun == "printplatform") {
		std::cout << libMuleTarget << std::endl;
		return 0;
	}
	return -1;
}

void MuleToolClass::criticalError(const int& ercode, const std::string& ermessage) {
	std::cout << "[ERROR №" << ercode << "] " << ermessage.c_str() << std::endl;
	std::exit(ercode);
}

void MuleToolClass::verboseInfo(const std::string& vinfo) {
	if (beQuiet == false)
		std::cout << "[INFO] " << vinfo << std::endl;
}

void MuleToolClass::viewHeaderMessage() {
	std::cout << "muletool " << TOOLVERSION << " - a libMule application deployment and building utility" << std::endl;
	std::cout << "Copyright (c) 2018-2019 Tim K <timprogrammer@rambler.ru>. Licensed under GNU LGPLv2.1." << std::endl;
	std::cout << "Original target platform is " << TOOLTARGET << std::endl;
	std::cout << "" << std::endl;
}

void MuleToolClass::viewHelp() {
	std::cout << "Usage: " << currentProgName << " [some arguments here]" << std::endl;
	std::cout << "     -deploy [binary name]           Deploy file [binary name] to target" << std::endl;
	std::cout << "                                     device and run it." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "     -compile [optionally filename]  If a filename is specified after   " << std::endl;
	std::cout << "                                     -compile, then it will be compiled " << std::endl;
	std::cout << "                                     into an object file."  << std::endl;
	std::cout << "" << std::endl;
	std::cout << "     -link [object files]            Link all object files specified on" << std::endl;
	std::cout << "                                     the command-line (if none are spe-" << std::endl;
	std::cout << "                                     cified, then all object files in  " << std::endl;
	std::cout << "                                     $PWD will be automatically selected)" << std::endl;
	std::cout << "                                     into a binary file that could be ran" << std::endl;
	std::cout << "                                     on the target device." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "    -build [object files]            Compile [object files] and link them" << std::endl;
	std::cout << "                                     into a binary.";
	std::cout << "" << std::endl;
	std::cout << "     -o [filename]                   A useful option that specifies out-" << std::endl;
	std::cout << "                                     put filename for -compile and -link" << std::endl;
	std::cout << "                                     options. Please specify it before all" << std::endl;
	std::cout << "                                     the other options." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "    -config [filename]              Read muletool configuration from " << std::endl;
	std::cout << "                                    [filename]." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "    -arduinoport [port]              Specify the port where your Arduino board" << std::endl;
	std::cout << "                                     is connected." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "     -quiet                          Be as quiet as possible." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "     -help                           Print this help message and exit." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "All bugs should be reported to " << TOOLBUGURL << std::endl;
	
}

std::string MuleToolClass::replaceContextAlternatives(const std::string& vname, const std::string& vval) {
	std::string calt = "";
	if (vname == "CC" || vname == "LD")
		calt = compilerCXX;
	else if (vname == "CXX")
		calt = compilerCC;
	else if (vname == "LDFLAGS" || vname == "CFLAGS")
		calt = compilerFlags_CXX;
	else if (vname == "CXXFLAGS")
		calt = compilerFlags_CC;
	else if (vname == "DEPLOY" || vname == "RUN")
		calt = deploy_binName;
	
	std::string result = "";
	for (int i = 0; i < stringLength(vval); i++) {
		if (vval[i] != '%')
			result = result + vval[i];
		else {
			if ((i < (stringLength(vval) - 1)) && (vval[i + 1] == 'A')) {
				i = i + 1;
				result = result + calt;
			}
			else if ((i < (stringLength(vval) - 1)) && (vval[i + 1] == 'P')) {
				i = i + 1;
				result = result + TOOLPREFIX;
			}
			else if ((i < (stringLength(vval) - 1)) && (vval[i + 1] == 'V')) {
				i = i + 1;
				result = result + arduinoPort;
			}
			else
				result = result + vval[i];
		}
	}
	return result;
}

void MuleToolClass::parseArguments() {
	actionArgs.clear();
	for (int i = 0; i < cliArgs.size(); i++) {
		if (cliArgs[i].rfind("-internal-", 0) != 0) {
			if (cliArgs[i] == "-compile" || cliArgs[i] == "-bc") {
				actionToRun = "compile";
				if ((i + 1) < (cliArgs.size() - 1)) {
					actionArgs.push_back(cliArgs[i + 1]);
					i = i + 1;
				}
			}
			else if (cliArgs[i] == "-compile-and-link" || cliArgs[i] == "-compile_and_link" || cliArgs[i] == "-clink" || cliArgs[i] == "-cl" || cliArgs[i] == "-be" || cliArgs[i] == "-build") {
				actionToRun = "completebuild";
				if ((i + 1) < (cliArgs.size() - 1)) {
					actionArgs.push_back(cliArgs[i + 1]);
					i = i + 1;
				}
			}
			else if (cliArgs[i] == "-link" || cliArgs[i] == "-bl") {
				actionToRun = "link";
				if ((i + 1) < (cliArgs.size() - 1)) {
					actionArgs.push_back(cliArgs[i + 1]);
					i = i + 1;
				}
			}
			else if (cliArgs[i] == "-deploy" || cliArgs[i] == "-br") {
				actionToRun = "deploy";
				deploy_binName = cliArgs[i + 1];
				i = i + 1;
			}
			else if (cliArgs[i] == "-config" || cliArgs[i] == "-c") {
				if ((i + 1) < (cliArgs.size() - 1) && (fileExists(cliArgs[i + 1]))) {
					haveToDetectConfig = false;
					configFilePath = cliArgs[i + 1];
				}
				else
					haveToDetectConfig = true;
				i = i + 1;
				
			}
			else if (cliArgs[i] == "-o") {
				if ((i + 1) < (cliArgs.size() - 1)) {
					actionArgs.push_back(cliArgs[i]);
					actionArgs.push_back(cliArgs[i + 1]);
					i = i + 1;
				}
			}
			else if ((cliArgs[i] == "-aport") || (cliArgs[i] == "-arduinoport") || (cliArgs[i] == "-port") || (cliArgs[i] == "-ba")) {
				if ((i + 1) < (cliArgs.size() - 1)) {
					arduinoPort = cliArgs[i + 1];
					i = i + 1;
				}
			}
			else if (cliArgs[i] == "-quiet")
				beQuiet = true;
			else if (cliArgs[i] == "-nolibmule")
				doNotIncludeLibMule = true;
			else if (cliArgs[i] == "-lazy")
				actionToRun = "lazymode";
			else
				actionArgs.push_back(cliArgs[i]);
		}
	}
	
	if (actionToRun == "nmtd")
		criticalError(6, "Nothing to do, exiting");
	
}

bool MuleToolClass::directoryExists(const std::string& dname) {
	struct stat dirinfo;
	if (stat(dname.c_str(), &dirinfo) != 0)
		return false;
	if (dirinfo.st_mode & S_IFDIR)
		return true;
	return false;
}

int MuleToolClass::compileFiles() {
	std::vector<std::string> filesToCompile;
	std::string cflags = compilerFlags_CC;
	std::string cxxflags = compilerFlags_CXX;
	for (int i = 0; i < actionArgs.size(); i++) {
		if (fileExists(actionArgs[i]))
			filesToCompile.push_back(actionArgs[i]);
		else if (directoryExists(actionArgs[i]))
			criticalError(11, "You can only compile specific files, not a whole directory.");
		else {
			cflags = cflags + " " + actionArgs[i];
			cxxflags = cxxflags + " " + actionArgs[i];
		}
	}
	if (doNotIncludeLibMule == false) {
		cflags = cflags + " -I" + libMuleInclude;
		cxxflags = cxxflags + " -I" + libMuleInclude;
	}
	bool oneFileNoNeedToIgnoreOutput = false;
	if ((filesToCompile.size() < 2) && (stringVectorContains(actionArgs, "-o") == true))
		oneFileNoNeedToIgnoreOutput = true;
	for (int j = 0; j < filesToCompile.size(); j++) {
		std::string extension = "";
		if (filesToCompile.at(j).find_last_of(".") != std::string::npos)
			extension = (std::string(filesToCompile[j]).substr(filesToCompile.at(j).find_last_of(".") + 1));
		std::string buildcmd;
		if (extension == "cxx" || extension == "cpp" || extension == "c++" || extension == "cc" || extension == "C")
			buildcmd = compilerCXX + " " + cxxflags;
		else if (extension == "c")
			buildcmd = compilerCC + " " + cflags;
		else
			buildcmd = "echo \"skip " + filesToCompile[j] + "\"";
		std::string outobjectname = filesToCompile[j];
		outobjectname = replaceSubstring(outobjectname, "..", "{U}");
		// create a proper object filename
		std::vector<std::string> veconame = split(outobjectname, ".");
		outobjectname = veconame[0];
		for (int i = 1; i < (veconame.size() - 1); i++)
			outobjectname = outobjectname + "." + veconame[i];
		outobjectname = outobjectname + ".o";
		outobjectname = replaceSubstring(outobjectname, "{U}", "..");
		if (oneFileNoNeedToIgnoreOutput == false)
			buildcmd = buildcmd + " -o \"" + outobjectname + "\" \"" + filesToCompile[j] + "\"";
		else
			buildcmd = buildcmd + " \"" + filesToCompile[j] + "\"";
		if ((fileExists(outobjectname)) && (oneFileNoNeedToIgnoreOutput == false)) {
			if (printCommandsOnScreen == true)
#ifdef _WIN32
				std::cout << "del /q " << outobjectname << std::endl;
			std::system(std::string("del /q \"" + outobjectname + "\"").c_str());
#else
				std::cout << "rm -r -f " << outobjectname << std::endl;
			std::system(std::string("rm -r -f \"" + outobjectname + "\"").c_str());
#endif
		}
		if (printCommandsOnScreen == true)
			std::cout << buildcmd << std::endl;
		if (std::system(buildcmd.c_str()) != 0)
			criticalError(10, "Failed to compile \"" + filesToCompile[j] + "\"");
	}
	std::cout << "Everything was compiled successfully" << std::endl;
	return 0;
}

int MuleToolClass::linkProgram() {
	std::vector<std::string> filesToCompile;
	std::string ldflags = linkerFlags_LD;
	bool haveOutput = false;
	for (int i = 0; i < actionArgs.size(); i++) {
		if (fileExists(actionArgs[i]))
			filesToCompile.push_back(actionArgs[i]);
		else if (directoryExists(actionArgs[i]))
			criticalError(11, "You can only compile specific files, not a whole directory.");
		else {
			if (actionArgs[i] == "-o")
				haveOutput = true;
			ldflags = ldflags + " " + actionArgs[i];
		}
	}
	if (haveOutput == false)
		ldflags = ldflags + " -o a.out";
	std::string buildcmd = linkerLD + " " + ldflags;
	for (int j = 0; j < filesToCompile.size(); j++)
		buildcmd = buildcmd + " " + filesToCompile[j];
	if (doNotIncludeLibMule == false)
		buildcmd = buildcmd + " " + libMuleLib;
	if (printCommandsOnScreen == true)
		std::cout << buildcmd << std::endl;
	if (std::system(buildcmd.c_str()) != 0)
		return 12;
	std::cout << "Everything linked successfully" << std::endl;
	return 0;
}

int MuleToolClass::deployProgram() {
	std::vector<std::string> filesToCompile;
	verboseInfo("Deploying program \"" + deploy_binName + "\"");
	if (printCommandsOnScreen == true)
		std::cout << deploy_copyFile << std::endl;
	if (std::system(deploy_copyFile.c_str()) != 0)
		return 14;
	if (printCommandsOnScreen == true)
		std::cout << deploy_runFile << std::endl;
	if (std::system(deploy_runFile.c_str()) != 0)
		return 15;
	std::cout << "Your program was successfully deployed" << std::endl;
	return 0;
}

std::vector<std::string> MuleToolClass::getDirectoryContents(const std::string& dname) {
	if (directoryExists(dname) == false)
		return std::vector<std::string>();
	
	std::vector<std::string> result;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (dname.c_str())) != NULL) {
		while ((ent = readdir (dir)) != NULL)
			result.push_back(std::string(ent->d_name));
		closedir (dir);
	}
	return std::vector<std::string>();
}

std::string MuleToolClass::getVariableValue(const std::string& vname) {
	if (vname == "CC")
		return compilerCC;
	else if (vname == "CXX" || vname == "C++")
		return compilerCXX;
	else if (vname == "CFLAGS" || vname == "CCFLAGS")
		return compilerFlags_CC;
	else if (vname == "CXXFLAGS" || vname == "C++FLAGS")
		return compilerFlags_CXX;
	else if (vname == "LD" || vname == "LINK")
		return linkerLD;
	else if (vname == "LDFLAGS" || vname == "LINKFLAGS")
		return linkerFlags_LD;
	else if (vname == "LIBMULE")
		return std::string(libMuleInclude + dirsepchar + ".." + dirsepchar + "..");
	else if (vname == "TARGET")
		return libMuleTarget;
	else if (vname == "DEPLOY")
		return deploy_copyFile;
	else if (vname == "RUN")
		return deploy_runFile;
	else if (vname == "MULTIPREFIX")
		return libMuleFilenamePrefix;
	else if (vname == "INTERNAL_CONFNAME")
		return configFilePath;
	else if (vname == "INTERNAL_HOMEDIR")
		return userHomeDir;
	return "";
}

char MuleToolClass::lastStringChar(const std::string& stritself) {
	int strlength = 0;
	while (stritself[strlength] != '\0')
		strlength = strlength + 1;
	return stritself[strlength - 1];
}

std::string MuleToolClass::replaceSubstring(const std::string& origstring, const std::string& tofind, const std::string& toreplace) {
	size_t theindex = 0;
	std::string result = origstring;
	while (true) {
		theindex = result.find(tofind, theindex);
		if (theindex == std::string::npos)
			break;
		
		result.replace(theindex, stringLength(tofind), toreplace);
		
		theindex = theindex + stringLength(tofind);
	}
	return result;
}

int MuleToolClass::stringLength(const std::string& origstring) {
	return strlen(origstring.c_str());
}

int MuleToolClass::compileAndLinkProgram() {
	std::vector<std::string> actionArgs2;
	for (int i = 0; i < actionArgs.size(); i++) {
		int prev = i - 1;
		if (prev < 0)
			prev = 0;
		if (actionArgs.at(i) != "-o" && actionArgs.at(prev) != "-o")
			actionArgs2.push_back(actionArgs.at(i));
	}
	std::vector<std::string> actionArgs3 = actionArgs;
	actionArgs = actionArgs2;
	int toret = compileFiles();
	if (toret != 0)
		return toret;
	for (int j = 0; j < actionArgs3.size(); j++) {
		std::string extension = "";
		std::string fn = actionArgs3.at(j);
		std::size_t pos = fn.find_last_of(".");
		if (pos != std::string::npos)
			extension = fn.substr(pos + 1);
		if (extension == "cxx" || extension == "cpp" || extension == "c++" || extension == "cc" || extension == "C" || extension == "c") {
			fn = fn.substr(0, pos);
			fn = fn + ".o";
		}
		actionArgs3[j] = fn;
	}
	actionArgs = actionArgs3;
	toret = linkProgram();
	return toret;
}
