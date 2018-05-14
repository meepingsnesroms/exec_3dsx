#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "mini-hb-menu/common.h"


extern const loaderFuncs_s loader_Ninjhax1;
extern const loaderFuncs_s loader_Ninjhax2;
extern const loaderFuncs_s loader_Rosalina;

/*
static int parseArgsAsTerminal(const char* argString, argData_s* argsOut){
	bool inSingleQuotes = false;
	bool inDoubleQuotes = false;
	int argStringLength = strlen(argString);
	int argsLength = 0;
	unsigned int argPtr;
	//int totalArgs = 0;
	char* argArray = (char*)argsOut->buf;
	
	memset(argsOut->buf, '\0', sizeof(argsOut->buf));
	
	//build argument list like a terminal command on linux
	for(argPtr = 0; argPtr < argStringLength; argPtr++){
		if(argString[argPtr] == '\'')
			inSingleQuotes = !inSingleQuotes;
		else if(argString[argPtr] == '\"')
			inDoubleQuotes = !inDoubleQuotes;
		else{
			if(!inSingleQuotes && !inDoubleQuotes && argString[argPtr] == ' '){
				//totalArgs++;
				argArray[argsLength] = '\0';
			}
			else{
				argArray[argsLength] = argString[argPtr];
			}
			
			argsLength++;
		}
	}
	
	return argPtr;
}
*/

int exec_3dsx(const char* path, const char* args){
	struct stat sBuff; 
	argData_s newProgramArgs;
	bool fileExists;
	bool inited;
	
	if(path == NULL || path[0] == '\0'){
		errno = EINVAL;
		return -1;
	}
	
	fileExists = stat(path, &sBuff) == 0;
	if(!fileExists){
		errno = ENOENT;
		return -1;
	}
	else if(S_ISDIR(sBuff.st_mode)){
		errno = EINVAL;
		return -1;
	}

	//args
	memset(newProgramArgs.buf, '\0', ENTRY_ARGBUFSIZE);
	newProgramArgs.dst = (char*)newProgramArgs.buf;
	//launchAddArg(&newProgramArgs, path);
	if(args != NULL || args[0] != '\0'){
		launchAddArgsFromString(&newProgramArgs, args);
	}
	
	//launch as if its the first program run
	osSetSpeedupEnable(false);
	
	inited = loader_Rosalina.init();
	if(inited){
		loader_Rosalina.launchFile(path, &newProgramArgs, NULL);
		exit(0);
	}
	
	inited = loader_Ninjhax2.init();
	if(inited){
		loader_Ninjhax2.launchFile(path, &newProgramArgs, NULL);
		exit(0);
	}
	
	inited = loader_Ninjhax1.init();
	if(inited){
		loader_Ninjhax1.launchFile(path, &newProgramArgs, NULL);
		exit(0);
	}
	
	//should never be reached
	errno = ENOTSUP;
	return -1;
}