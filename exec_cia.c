#include <errno.h>
#include <sys/stat.h>
#include <3ds.h>


static inline int isCiaInstalled(u64 titleId){
	u32 titlesToRetrieve
	u32 titlesRetrieved;
	u64* titleIds;
	Result failed;
	
	failed = AM_GetTitleCount(MEDIATYPE_SD, &titlesToRetrieve);
	if(failed)
		return -1;
	
	titleIds = malloc(titlesToRetrieve * sizeof(uint64_t));
	if(titleIds == NULL)
		return -1;
	
	failed = AM_GetTitleList(&titlesRetrieved, MEDIATYPE_SD, titlesToRetrieve, titleIds);
	if(failed == NULL)
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

static inline u64 getCiaTitleId(const char* path){
	
}

int exec_cia(const char* path, const char* args){
	struct stat sBuff; 
	bool fileExists;
	bool inited = !amInit();
	
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
	
	if(inited){
		u64 titleId = getCiaTitleId(path);
		int ciaInstalled = isCiaInstalled(path);
		
		if(ciaInstalled == -1){
			//error
		}
		else if(ciaInstalled == 0){
			//not installed
		}
		else{
			//installed
			
		}
		Handle ciaFile;
		AM_TitleEntry ciaInfo;
		
		Result AM_GetCiaFileInfo(FS_MediaType mediatype, AM_TitleEntry *titleEntry, Handle fileHandle);
		Result AM_GetTitleList(u32* titlesRead, FS_MediaType mediatype, u32 titleCount, u64 *titleIds);
		//Result AM_GetTitleInfo(FS_MediaType mediatype, u32 titleCount, u64 *titleIds, AM_TitleEntry *titleInfo);
		
	}
	
	//should never be reached
	errno = ENOTSUP;
	return -1;
}
