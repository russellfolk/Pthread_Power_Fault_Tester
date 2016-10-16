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
	bool dflag = false;

	while ((c = getopt(argc, argv, "f:d")) != -1)
	{
		switch (c)
		{
			case 'f':
				filename = optarg;
				fflag = 1;
				break;
			case 'd':
				dflag = true;
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
	print_summary();

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

	if (!record_blank(record))
	{
		std::cout << thread_id << "\t" << record_num << "\t" << address << "\t" << timestamp << "\t" << checksum << std::endl;
		parse_record(record);
	}
	return checksum;
}

void parse_record(long * record)
{
	long thread_id = record[IND_THREAD_ID];
	// record does not exists
	if (stats.find(thread_id) == stats.end())
	{
		thread_statistics these_stats = {
			0, // no records added yet
			true, // no true last record
		};
		stats.insert(std::make_pair(thread_id, these_stats));
		total_threads++;
	}

	bool valid_record = checksum_record(record);

	stats[thread_id].num_records++;
	if (!valid_record)
	{
		if (dflag)
		{
			std::cout
				  << "Bad Record:" << std::endl
		          << "Head checksum: " << record[IND_HEAD_CHECKSUM] << std::endl
		          << "Tail checksum: " << record[IND_TAIL_CHECKSUM] << std::endl
		          << "Thread ID: " << record[IND_THREAD_ID] << "\t"
		          << "Record ID: " << record[IND_RECORD_NUM] << std::endl
		          << "Address: " << record[IND_RECORD_ADDRESS] << std::endl
		          << "Timestamp: " << record[IND_TIMESTAMP] << std::endl;
		}
		stats[thread_id].last_success = valid_record;
	}
}

bool checksum_record(long * record)
{
	long checksum = Fletcher64(record);
	return (checksum == record[IND_HEAD_CHECKSUM]) &&
	       (checksum == record[IND_TAIL_CHECKSUM]);
}

void print_summary(void)
{
	std::cout << "\nStatistics Summary -- " << total_threads << " total threads\n" << std::endl;
	std::cout << "Thread ID" << "\t" << "Number of Records" << "\t" << "Last record intact" << std::endl;
	for (stats_it = stats.begin(); stats_it != stats.end(); stats_it++)
	{
		long thread_id = stats_it->first;
		thread_statistics these_stats = stats_it->second;
		std::cout << std::setw(9) << thread_id << "\t" << std::setw(17) << these_stats.num_records << "\t";
		if (these_stats.last_success)
			std::cout << std::setw(18) << "Yes" << std::endl;
		else
			std::cout << std::setw(18) << "No" << std::endl;
	}
}

bool record_blank(long * record)
{
	bool blank = true;
	for (int i = 0; i < (RECORD_SIZE/sizeof(long)) - 1; i++)
		if (record[i] != 0)
			blank = false;
	return blank;
}