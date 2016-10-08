#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define O_BINARY 0
const int RECORD_SIZE = 8 * 1024;

size_t get_file_size(const char *);

int main()
{
	int num_records, file_size;
    std::cout << "Hello, World!" << std::endl;

    file_size = (int) get_file_size("device-file");
    num_records = file_size / RECORD_SIZE;

    std::cout << "File range is 0 to " << file_size << "." << std::endl;

    std::cout << "Number of possible records: " << num_records << std::endl;

    return 0;
}


// https://www.securecoding.cert.org/confluence/display/c/FIO19-C.+Do+not+use+fseek()+and+ftell()+to+compute+the+size+of+a+regular+file

size_t get_file_size(const char * filename)
{
	size_t file_size;
	char *buffer;
	struct stat stbuf;
	int fd;

	fd = open(filename, O_BINARY);
	if (fd == -1)
	{
  		/* Handle error */
	}

	if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
	{
		/* Handle error */
	}

	file_size = stbuf.st_size;

	buffer = (char*)malloc(file_size);
	if (buffer == NULL) {
		/* Handle error */
	}

 	return file_size;
}