#include "file_lib.h"

// https://www.securecoding.cert.org/confluence/display/c/FIO19-C.+Do+not+use+fseek()+and+ftell()+to+compute+the+size+of+a+regular+file

size_t get_file_size(int fd)
{
	size_t file_size;
	char *buffer;
	struct stat stbuf;

	if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
	{
		/* Handle error */
	}

	file_size = stbuf.st_size;

	buffer = (char*)malloc(file_size);
	if (buffer == NULL) {
		/* Handle error */
		std::cout << "buffer is null" << std::endl;
	}

	return file_size;
}

int open_file(const char * filename)
{
	int fd;

	fd = open(filename, FILE_FLAGS);
	if (fd == -1)
	{
		std::cout << "file is null?" << std::endl;
		/* Handle error */
	}

	return fd;
}