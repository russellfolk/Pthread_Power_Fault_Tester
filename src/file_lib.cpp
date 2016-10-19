#include "file_lib.h"

int open_file(const char * filename)
{
	int fd;

	fd = open(filename, FILE_FLAGS);
	if (fd == -1)
	{
		if (dflag)
			std::cout << "File is null." << std::endl;
	}

	return fd;
}

size_t get_file_size(int fd)
{
	size_t file_size;
	char *buffer;
	struct stat stbuf;

	if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
	{
		if (dflag)
			std::cout << "Cannot interpret information about the file."
	}

	file_size = stbuf.st_size;

	buffer = (char*)malloc(file_size);
	if (buffer == NULL) {
		if (dflag)
			std::cout << "Size of the file is null." << std::endl;
	}

	return file_size;
}