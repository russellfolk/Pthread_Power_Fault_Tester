#include "checker.h"

int main(int argc, char **argv)
{
	// required for argument parsing
	int fflag = 0;
	int c;
	const char * def_filename = "device-file";
	char * filename;
	extern char *optarg;
	extern int optind;

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

	int fd = open_file(filename);
	int file_size = (int) get_file_size(fd);
	int num_records = file_size / RECORD_SIZE;

	check_file(fd, num_records);

	close(fd);

	return 0;
}

void check_file(int fd, int num_records)
{
	for (int r_a = 0; r_a < num_records; r_a++)
	{
		long * record = get_record(fd, r_a);
		print_record(record);
	}
}

long * get_record(int fd, long record_address)
{
	long * record = (long *) malloc(RECORD_SIZE);

	pread(fd, record, RECORD_SIZE, (record_address * RECORD_SIZE));

	return record;
}

long print_record(long * record)
{
	long thread_id = record[IND_THREAD_ID];
	long record_num = record[IND_RECORD_NUM];
	long address = record[IND_RECORD_ADDRESS];
	long timestamp = record[IND_TIMESTAMP];
	long checksum = record[((RECORD_SIZE/sizeof(long)) - 1)];

	if (checksum != 0)
	{
		std::cout << thread_id << "\t" << record_num << "\t" << address << "\t" << timestamp << "\t" << checksum << std::endl;
	}
	return checksum;
}
