#include <jni.h>
#include <fcntl.h>
#include <asm-generic/fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "mr_helper.h"
#include "emulator.h"


int getFileType(const char *name)
{
	struct stat s1;
	int ret;

	//返回 0 成功
	ret = stat(name, &s1);
	if(ret != 0) {
		LOGE("getFileType errno=%d", errno);
		return MR_IS_INVALID;
	}

	if (s1.st_mode & S_IFDIR)
		return MR_IS_DIR;
	else if (s1.st_mode & S_IFREG)
		return MR_IS_FILE;
	else
		return MR_IS_INVALID;
}

int getFileSize(const char *path)
{
	struct stat s1;
	int ret;

	ret = stat(path, &s1);
	if (ret != 0) {
		LOGE("getFileSize errno=%d", errno);
		return -1;
	}

	return s1.st_size;
}
