#include <errno.h>
#include <sys/stat.h>
#include <3ds.h>


#define FILE_CHUNK_SIZE 4096


typedef struct{
	u32 argc;
	char args[0x300 - 0x4];
}ciaParam;


static int isCiaInstalled(u64 titleId){
	u32 titlesToRetrieve
	u32 titlesRetrieved;
	u64* titleIds;
	Result failed;
	
	failed = AM_GetTitleCount(MEDIATYPE_SD, &titlesToRetrieve);
	if(R_FAILED(failed))
		return -1;
	
	titleIds = malloc(titlesToRetrieve * sizeof(uint64_t));
	if(titleIds == NULL)
		return -1;
	
	failed = AM_GetTitleList(&titlesRetrieved, MEDIATYPE_SD, titlesToRetrieve, titleIds);
	if(R_FAILED(failed) == NULL)
		return -1;
	
	for(u32 titlesToCheck = 0; titlesToCheck < titlesRetrieved; titlesToCheck++){
		if(titleIds[titlesToCheck] == titleID){
			free(titleIds);
			return 1;
		}	
	}
	
	free(titleIds);
	return 0;
}

static int installCia(Handle ciaFile){
	Result failed;
	Handle outputHandle;
	u64 fileSize;
	u64 fileOffset = 0;
	u32 bytesRead;
	u32 bytesWritten;
	u8 transferBuffer[FILE_CHUNK_SIZE];
	
	failed = AM_StartCiaInstall(MEDIATYPE_SD, &outputHandle);
	if(R_FAILED(failed))
		return -1;
	
	failed = FSFILE_GetSize(ciaFile, &fileSize);
	if(R_FAILED(failed))
		return -1;
	
	while(fileOffset < fileSize){
		u64 bytesRemaining = fileSize - fileOffset;
		failed = FSFILE_Read(ciaFile, &bytesRead, fileOffset, transferBuffer, bytesRemaining < FILE_CHUNK_SIZE ? bytesRemaining : FILE_CHUNK_SIZE);
		if(R_FAILED(failed)){
			AM_CancelCIAInstall(outputHandle);
			return -1;
		}
		
		failed = FSFILE_Write(outputHandle, &bytesWritten, fileOffset, transferBuffer, bytesRead, 0);
		if(R_FAILED(failed)){
			AM_CancelCIAInstall(outputHandle);
			return -1;
		}
		
		if(bytesWritten != bytesRead){
			AM_CancelCIAInstall(outputHandle);
			return -1;
		}
		
		fileOffset += bytesWritten;
	}
	
	failed = AM_FinishCiaInstall(outputHandle);
	if(R_FAILED(failed))
		return -1;
	
	return 1;
}

static u64 getCiaTitleId(Handle ciaFile){
	Result failed;
	AM_TitleEntry ciaInfo;
	
	failed = AM_GetCiaFileInfo(MEDIATYPE_SD, &ciaInfo, ciaFile);
	if(R_FAILED(failed))
		return 0x0000000000000000;
	
	return ciaInfo.titleId;
}

static void errorAndQuit(const char* errorStr){
	errorConf error;

	errorInit(&error, ERROR_TEXT, CFG_LANGUAGE_EN);
	errorText(&error, errorStr);
	errorDisp(&error);
	exit(0);
}

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
	
	inited = R_SUCCEEDED(amInit()) && R_SUCCEEDED(fsInit());
	if(inited){
		Result res;
		FS_Archive ciaArchive;
		Handle ciaFile;
		u64 titleId;
		int ciaInstalled; = isCiaInstalled(path);
		ciaParam param;
		int argsLength;
		extern char __argv_hmac[0x20];
		
		//open cia file
		res = FSUSER_OpenArchive(&ciaArchive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
		if(R_FAILED(res))
			errorAndQuit("Cant open SD FS archive.");
		
		res = FSUSER_OpenFile(&ciaFile, ciaArchive, fsMakePath(PATH_ASCII, path + 5/*skip "sdmc:"*/), FS_OPEN_READ, 0);
		if(R_FAILED(res))
			errorAndQuit("Cant open CIA file.");
		
		titleId = getCiaTitleId(ciaFile);
		if(titleId == 0x0000000000000000)
			errorAndQuit("Cant get CIA file title id.");
		
		ciaInstalled = isCiaInstalled(titleId);
		if(ciaInstalled == -1){
			//error
			errorAndQuit("Could not read title id list.");
		}
		else if(ciaInstalled == 0){
			//not installed
			int error = installCia(ciaFile);
			
			if(error == -1)
				errorAndQuit("Cant install CIA.");
		}
		
		FSFILE_Close(ciaFile);
		FSUSER_CloseArchive(ciaArchive);
		
		if(args == NULL || args[0] == '\0'){
			param.argc = 0;
			argsLength = 0;
		}
		else{
			bool inSingleQuotes = false;
			bool inDoubleQuotes = false;
			int argStringLength = strlen(args);
			int argPtr;
			param.argc = 0;
			argsLength = 0;
			
			//build argument list like a terminal command on linux
			for(unsigned int argPtr = 0; argPtr < argStringLength; argPtr++){
				if(args[argPtr] == '\'')
					inSingleQuotes = !inSingleQuotes;
				else if(args[argPtr] == '\"')
					inDoubleQuotes = !inDoubleQuotes;
				else{
					if(!inSingleQuotes && !inDoubleQuotes && args[argPtr] == ' '){
						param.argc++;
						param.args[argsLength] = '\0';
					}
					else{
						param.args[argsLength] = args[argPtr];
					}
					
					argsLength++;
				}
			}
		}
		
		res = APT_PrepareToDoApplicationJump(0, titleId, 0x1);
		if(R_SUCCEEDED(res))
			res = APT_DoApplicationJump(&param, sizeof(param.argc) + argsLength, __argv_hmac);
	}
	
	//should never be reached
	amExit();
	fsExit();
	errno = ENOTSUP;
	return -1;
}
