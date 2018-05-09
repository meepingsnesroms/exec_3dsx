#include <errno.h>
#include <sys/stat.h>
#include <3ds.h>


int exec_cia(const char* path, const char* args){
	struct stat sBuff; 
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
		errno = EINVAL;;
		return -1;
	}
	
	
	
	//should never be reached
	errno = ENOTSUP;
	return -1;
}
