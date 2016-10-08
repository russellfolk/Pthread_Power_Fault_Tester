#include <iostream>
#include <random>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define O_BINARY 0
const int RECORD_SIZE = 8 * 1024;

int first_record;
int last_record;

size_t get_file_size(const char *);
void write_records(void);

int main(int argc, char **argv)
{
	// required for argument parsing
	int fflag = 0;
	int c;
	const char * def_filename = "device-file";
	char * filename;
	extern char *optarg;
	extern int optind;
	static char usage[] = "usage: %s -f filename\n";

	// permanent values
	int num_records, file_size;

	while ((c = getopt(argc, argv, "f:")) != -1)
	{
		switch (c)
		{
			case 'f':
				filename = optarg;
				fflag = 1;
				break;
			default:
				// implement other options, such as help, etc.
				break;
		}
	}

	if (fflag == 0)
	{
		// need to implement default filename
		filename = strdup(def_filename);
	}

    file_size = (int) get_file_size(filename);
    num_records = file_size / RECORD_SIZE;
    first_record = 0;
    last_record = num_records;

    std::cout << "File range is 0 to " << file_size << "." << std::endl;

    std::cout << "Number of possible records: " << num_records << std::endl;

    write_records();

    return 0;
}

void write_records(void)
{
	long thread_id = 0;
	long record_id = 1;
	long address;
	long checksum = 1;
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distr(first_record, last_record);

	for (int i = 0; i < 10; i++)
	{
		address = distr(generator);
		std::cout << "Record: " << thread_id << " " << record_id << " " << address << " " << checksum << std::endl;
		record_id++;
	}
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