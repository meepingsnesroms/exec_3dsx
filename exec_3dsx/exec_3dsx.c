#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "mini-hb-menu/common.h"


extern const loaderFuncs_s loader_Ninjhax1;
extern const loaderFuncs_s loader_Ninjhax2;
extern const loaderFuncs_s loader_Rosalina;

static void (*launch_3dsx)(const char* path, argData_s* args, executableMetadata_s* em);


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
	/*
	//memset(newProgramArgs.buf, '\0', sizeof(newProgramArgs.buf));
	newProgramArgs.dst = (char*)&newProgramArgs.buf[0];
	//launchAddArg(&newProgramArgs, path);
	if(args != NULL && args[0] != '\0'){
		launchAddArgsFromString(&newProgramArgs, args);
	}
	*/
	
	inited = loader_Rosalina.init();
	launch_3dsx = loader_Rosalina.launchFile;
	
	if(!inited){
		inited = loader_Ninjhax2.init();
		launch_3dsx = loader_Ninjhax2.launchFile;
	}

	if(!inited){
		inited = loader_Ninjhax1.init();
		launch_3dsx = loader_Ninjhax1.launchFile;
	}
	
	if(inited){
		osSetSpeedupEnable(false);
		launch_3dsx(path, &newProgramArgs, NULL);
		exit(0);
	}
	
	//should never be reached
	errno = ENOTSUP;
	return -1;
}