#include "checker.h"

/**
 * device-file record checker
 *
 * This is a device-file checker that goes through and reads all the records written to the device-file and validates
 * them. Ideally, all records will be complete and valid, but as this is used to test for the effect of power loss, it
 * is expected to see partial writes and other failures.
 *
 * @param  argc the number of flags and arguments provided
 * @param  argv the flags and arguments provided
 * @return      result code for the program running
 */
int main(int argc, char **argv)
{
	// required for argument parsing
	bool eflag = false; // exit flag, display help was requested
	bool fflag = false;  // default filename flag
	int c;
	char * filename;
	extern char *optarg;
	extern int optind;
	std::string usage = "To run: ./bin/checker -f <device file> -d (to enable debug)\n"
	                    "If no arguments are supplied: device file = bin/device-file, debug = off";

	// default file name
	const char * def_filename = "bin/device-file";

	while ((c = getopt(argc, argv, "f:d?")) != -1)
	{
		switch (c)
		{
			case 'f':
				filename = optarg;
				fflag = true;
				break;
			case 'd':
				dflag = true;
				break;
			case '?':
				std::cout << usage << std::endl;
				eflag = true;
				break;
			default:
				// implement other options, such as help, etc.
				break;
		}
	}

	// Need to exit program if '?' argument is given
	if (eflag)
		exit(0);

	// Need to implement default filename
	if (fflag)
		filename = strdup(def_filename);

	// open the file and set the descriptor
	int fd = open_file(filename);

	// figure out how many records can exist in the device-file
	int file_size = (int) get_file_size(fd);
	int num_records = file_size / RECORD_SIZE;

	// parse and check all records in the device-file
	check_file(fd, num_records);

	// if not in debug mode, print out statistics
	if (!dflag)
		print_summary();

	// clean up and exit
	close(fd);
	return 0;
}

void check_file(int fd, int num_records)
{
	// process all existing records in the device-file
	for (long r_a = 0; r_a < num_records; r_a++)
	{
		// get the current record
		long * record = get_record(fd, r_a);
		// parse the record
		parse_record(record, r_a);
	}
}

long * get_record(int fd, long record_address)
{
	long * record = (long *) malloc(RECORD_SIZE);

	// read record at a given address
	pread(fd, record, RECORD_SIZE, (record_address * RECORD_SIZE));

	return record;
}

void parse_record(long * record, long record_address)
{
	// if the record is not blank, print what has been written to it
	if (!is_record_blank(record))
	{
		// do not print if wanting only debug information
		if (!dflag)
			print_record(record);
	}
	else
		return; // no need to go further because it's completely blank

	// check for the thread that wrote the record
	long thread_id = record[IND_THREAD_ID];
	// initialize stats for the thread if it is newly discovered
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
		// track total number of threads existing
		total_threads++;
	}

	// compare the checksum of the record read with the head and tail checksums
	// that are contained within the record
	bool complete_record = checksum_record(record);

	stats[thread_id].num_records++;

	// check if it's the latest record
	if (record[IND_TIMESTAMP] > stats[thread_id].record_write_time)
	{
		stats[thread_id].last_success = complete_record;
		stats[thread_id].record_write_time = record[IND_TIMESTAMP];
	}

	if (!complete_record)
	{
		// debug output
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
		// not a complete record, increment partial write records, calculate the
		// percentage successfully written
		stats[thread_id].num_partial++;
		stats[thread_id].percentage_written +=
		                 percent_written(record, record_address);
	}
	else
	{
		// complete record, increment complete record counter and add 100% as
		// percentage successfully written
		stats[thread_id].num_complete++;
		stats[thread_id].percentage_written += 1.0;
	}

	// check for a missed write, this should never happen, but just in case...
	if (!address_valid(record, record_address))
		stats[thread_id].num_missed_write++;
}

double percent_written(long * record, long record_address)
{
	double percent = 0.0;

	// figure out what the checksum is and check against the head and tail checksums
	long checksum = Fletcher64(record);
	if (record[IND_HEAD_CHECKSUM] == checksum)
		percent += 1;
	if (record[IND_TAIL_CHECKSUM] == checksum)
		percent += 1;

	// check if this is a valid thread id
	if (thread_id_valid(record))
		percent += 1;

	// check if this is a valid record id
	if (record_num_valid(record))
		percent += 1;

	// check if this is a valid address
	if (address_valid(record, record_address))
		percent += 1;

	// check if this is a valid time stamp
	if (timestamp_valid(record))
		percent += 1;

	// recreate and test the "random data" that is included in a record
	for (int i = IND_FIRST_RANDOM_RECORD; i < IND_TAIL_CHECKSUM - 1; i++)
	{
		long random_fill = record[i-1] + record[0] * record[1] * i + record[2];
		if (record[i] == random_fill)
			percent += 1;
	}

	// normalize by the number of pieces to obtain a value of [0,1]
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

	// debug output
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

std::time_t estimated_power_loss(void)
{
	std::time_t failure_time;
	long last_creation_time = 0;
	// find the greatest time stamp (occurred at the latest time)
	for (stats_it = stats.begin(); stats_it != stats.end(); stats_it++)
	{
		thread_statistics these_stats = stats_it->second;
		if (these_stats.record_write_time > last_creation_time)
			last_creation_time = these_stats.record_write_time;
	}

	// get the current time
	auto now = std::chrono::system_clock::now();
	auto since_epoch = now.time_since_epoch();

	// convert to milliseconds since the UNIX epoch
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch).count();

	// calculate time of failure as subtracting the number of milliseconds between now and last creation time
	failure_time = std::chrono::system_clock::to_time_t(now - std::chrono::milliseconds(millis - last_creation_time));
	return failure_time;
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
	std::time_t failure_time = estimated_power_loss();
	std::cout << std::endl << "Estimated time of powerloss" << std::endl
	          << std::put_time(std::localtime(&failure_time), "%F %T") << std::endl;
	std::cout << std::endl << "Statistics Summary -- " << total_threads
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