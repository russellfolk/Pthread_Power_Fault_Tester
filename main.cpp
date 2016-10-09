#include <iostream>
#include <random>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>

#include "thread_info.h"
#include "thread_local.h"

#define O_BINARY 0
const int RECORD_SIZE = 8 * 1024;
const int ADDRESS_LOCATION = 2;

int first_record;
int last_record;
int file_size;

size_t get_file_size(int);
void create_record(int, thread_info *);
size_t write_record(int, long *, long);
void worker_thread_init(int);
int intRand(void);
int open_file(const char * filename);
uint64_t Fletcher64 (long *, int);

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
	int num_records;
	int fd;

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

	fd = open_file(filename);

    file_size = (int) get_file_size(fd);
    num_records = file_size / RECORD_SIZE;
    first_record = 0;
    last_record = num_records;

    std::cout << "File range is 0 to " << file_size << "." << std::endl;

    std::cout << "Number of possible records: " << num_records << std::endl;

    worker_thread_init(fd);

    close(fd);
    return 0;
}

void worker_thread_init(int fd)
{
	// set up running record...
	thread_info * record_data = (thread_info *) malloc(sizeof(record_data));
	// replace with thread id
	record_data->thread_id = 0;
	record_data->record_num = 1;

	for (int i = 0; i < 10; i++)
		create_record(fd, record_data);
}

void create_record(int fd, thread_info * record_data)
{
	long record_size = RECORD_SIZE / sizeof(long);
	long size = record_size * sizeof(long);
	long * record = (long *) malloc(size);
	long address = intRand();
	long checksum = 1;

	record[0] = record_data->thread_id;
	record[1] = record_data->record_num;
	record[ADDRESS_LOCATION] = address;
	for (int i = ADDRESS_LOCATION+1; i < record_size - 1; i++)
	{
		record[i] = record[i-1] + record[0] * record[1] * i + record[2];
	}

	checksum = Fletcher64(record, record_size - 1);
	record[record_size - 1] = Fletcher64(record, record_size - 1);

	std::cout << "Record: " << record[0] << " " << record[1] << " " << record[2] << " " << checksum << " " << record[record_size-1] << std::endl;
	size_t msg = write_record(fd, record, (record_size * sizeof(record)));
	std::cout << "msg " << msg << std::endl;
	record_data->record_num += 1;
	free(record);
}

size_t write_record(int fd, long * record, long size)
{
	size_t nbyte = size;
	off_t offset = record[ADDRESS_LOCATION] * RECORD_SIZE;

	std::cout << "fd " << fd << " nbyte " << nbyte << " offset " << offset << std::endl;
	std::cout << (offset < file_size) << " " << (offset + nbyte < file_size) << std::endl;
	size_t msg = pwrite(fd, record, nbyte, offset);
	return msg;
}

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

	fd = open(filename, O_RDWR | O_DSYNC);
	if (fd == -1)
	{
		std::cout << "file is null?" << std::endl;
  		/* Handle error */
	}

	return fd;
}

int intRand(void)
{
    static thread_local std::mt19937* generator;
    if (!generator) generator = new std::mt19937(clock());
    std::uniform_int_distribution<int> distribution(first_record, last_record);
    return distribution(*generator);
}

uint64_t Fletcher64 (long * data, int number_to_checksum)
{
	uint64_t sum1 = 0;
	uint64_t sum2 = 0;
	uint64_t mod_value = 4294967296;

	for(int i = 0; i < number_to_checksum; i++)
	{
		sum1 = (sum1 + data[i]) % mod_value;
		sum2 = (sum2 + sum1) % mod_value;
	}

	return (sum2 << 32) | sum1;
}