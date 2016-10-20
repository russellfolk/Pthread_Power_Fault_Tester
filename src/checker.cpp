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

	while ((c = getopt(argc, argv, "f:d")) != -1)
	{
		switch (c)
		{
			case 'f':
				filename = optarg;
				fflag = 1;
				break;
			case 'd':
				std::cout << "DFLAG SET!!!!!!" << std::endl;
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
	if (!dflag)
		print_summary();

	close(fd);

	return 0;
}

void check_file(int fd, int num_records)
{
	for (long r_a = 0; r_a < num_records; r_a++)
	{
		long * record = get_record(fd, r_a);
		parse_record(record, r_a);
	}
}

long * get_record(int fd, long record_address)
{
	long * record = (long *) malloc(RECORD_SIZE);

	pread(fd, record, RECORD_SIZE, (record_address * RECORD_SIZE));

	return record;
}

void parse_record(long * record, long record_address)
{
	if (!is_record_blank(record))
	{
		if (!dflag)
			print_record(record);
	}
	else
		return; // no need to go further because it's completely blank

	long thread_id = record[IND_THREAD_ID];
	// record does not exists
	if (stats.find(thread_id) == stats.end())
	{
		thread_statistics these_stats = {
			0, // no records added yet
			0, // no good records
			0, // no bad records
			0.0, // percentage of writes
			0, // no records written to the wrong spot
			0, // no last timestamp
			true, // no true last record
		};
		stats.insert(std::make_pair(thread_id, these_stats));
		total_threads++;
	}

	bool complete_record = checksum_record(record);

	stats[thread_id].num_records++;
	if (record[IND_TIMESTAMP] > stats[thread_id].record_write_time)
	{
		stats[thread_id].last_success = complete_record;
		stats[thread_id].record_write_time = record[IND_TIMESTAMP];
	}
	if (!complete_record)
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
		stats[thread_id].num_partial++;
		stats[thread_id].percentage_written = percent_written(record, record_address);
	}
	else
	{
		stats[thread_id].num_complete++;
		stats[thread_id].percentage_written += 1.0;
		if (!address_valid(record, record_address))
			stats[thread_id].num_missed_write++;
	}
}

double percent_written(long * record, long record_address)
{
	double percent = 0.0;
	long checksum = Fletcher64(record);
	if (record[IND_HEAD_CHECKSUM] == checksum)
		percent += 1;
	if (record[IND_TAIL_CHECKSUM] == checksum)
		percent += 1;
	if (thread_id_valid(record))
		percent += 1;
	if (record_num_valid(record))
		percent += 1;
	if (address_valid(record, record_address))
		percent += 1;
	if (timestamp_valid(record))
		percent += 1;

	for (int i = IND_FIRST_RANDOM_RECORD; i < IND_TAIL_CHECKSUM - 1; i++)
	{
		long random_fill = record[i-1] + record[0] * record[1] * i + record[2];
		if (record[i] == random_fill)
			percent += 1;
	}

	percent /= NUM_PIECES;
	return percent;
}

bool thread_id_valid(long * record)
{
	return stats.find(record[IND_THREAD_ID]) != stats.end();
}

bool record_num_valid(long * record)
{
	return (thread_id_valid(record) &&
	        record[IND_RECORD_NUM] > stats[record[IND_THREAD_ID]].num_records);
}

bool address_valid(long * record, long record_address)
{
	long assumed_r_a = record[IND_RECORD_ADDRESS];
	long actual_r_a = record_address;
	if (dflag)
	{
		std::cout << "assumed_r_a " << assumed_r_a << " actual_r_a "
		          << actual_r_a;
	}
	return assumed_r_a == actual_r_a;
}

bool timestamp_valid(long * record)
{
	return record[IND_TIMESTAMP] > 0;
}

bool checksum_record(long * record)
{
	long checksum = Fletcher64(record);
	return (checksum == record[IND_HEAD_CHECKSUM]) &&
	       (checksum == record[IND_TAIL_CHECKSUM]);
}

bool is_record_blank(long * record)
{
	bool blank = true;
	for (int i = 0; i < (RECORD_SIZE/sizeof(long)) - 1; i++)
		if (record[i] != 0)
			blank = false;
	return blank;
}

void print_record(long * record)
{
	long thread_id = record[IND_THREAD_ID];
	long record_num = record[IND_RECORD_NUM];
	long address = record[IND_RECORD_ADDRESS];
	long timestamp = record[IND_TIMESTAMP];
	long checksum = record[((RECORD_SIZE/sizeof(long)) - 1)];

	std::cout << thread_id << "\t" << record_num << "\t" << address << "\t"
	          << timestamp << "\t" << checksum << std::endl;
}

void print_summary(void)
{
	std::cout << "\nStatistics Summary -- " << total_threads
	          << " total threads\n" << std::endl;
	std::cout << "Thread ID" << "\t" << "Number of Records" << "\t"
	          << "Successful Writes" << "\t" << "Partial Writes" << "\t"
	          << "% Written" << "\t" << "Missed Writes" << "\t"
	          << "Last record intact" << std::endl;
	for (stats_it = stats.begin(); stats_it != stats.end(); stats_it++)
	{
		long thread_id = stats_it->first;
		thread_statistics these_stats = stats_it->second;
		double percentage_written = these_stats.percentage_written;
		percentage_written /= these_stats.num_records;
		std::cout << std::setw(9) << thread_id << "\t" << std::setw(17)
		          << these_stats.num_records << "\t"
		          << std::setw(17) << these_stats.num_complete << "\t"
		          << std::setw(14) << these_stats.num_partial << "\t"
		          << std::setw(9) << percentage_written << "\t"
		          << std::setw(13) << these_stats.num_missed_write << "\t";
		if (these_stats.last_success)
			std::cout << std::setw(18) << "Yes" << std::endl;
		else
			std::cout << std::setw(18) << "No" << std::endl;
	}
}