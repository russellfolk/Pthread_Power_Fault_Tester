#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __APPLE__
	#include <fcntl.h>
	#define FILE_FLAGS (O_RDWR | O_SYNC)
#elif __linux__
	// needed for O_DIRECT
	#define _GNU_SOURCE
	#include <fcntl.h>
	#define FILE_FLAGS (O_RDWR | O_DIRECT)
#endif

size_t get_file_size(int);
int open_file(const char *);