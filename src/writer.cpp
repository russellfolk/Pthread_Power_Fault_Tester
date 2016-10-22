#include "writer.h"

/**
 * device-file record writer
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
	int num_threads = 0;
	std::string usage = "To run: ./bin/writer -f <device file> -w <# threads to use> -d (to enable debug)\n"
	                    "If no arguments are supplied: device file = bin/device-file, # threads = 4, debug = off";

	// permanent values
	int num_records;
	int fd;
	const char * def_filename = "bin/device-file";
	const int def_num_threads = 4;

	while ((c = getopt(argc, argv, "w:f:d?")) != -1)
	{
		switch (c)
		{
			case 'f':
				filename = optarg;
				fflag = true;
				break;
			case 'w':
				num_threads = atoi(optarg);
				break;
			case 'd':
				dflag = true;
				break;
			case '?':
				std::cout << usage << std::endl;
				eflag = true;
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

	// Need to have a sane number of threads
	if (num_threads < 1 || num_threads > 100)
		num_threads = def_num_threads;

	// open the file and set the descriptor
	fd = open_file(filename);

	// figure out how many records can exist in the device-file
	file_size = (int) get_file_size(fd);
	num_records = file_size / RECORD_SIZE;
	first_record = 0;
	last_record = num_records;

	// debug output
	if (dflag)
	{
		std::cout << "File range is 0 to " << file_size << "." << std::endl;
		std::cout << "Number of possible records: " << num_records << std::endl;
	}

	int thread_error; // pthread_create error code
	pthread_t threads[num_threads]; // array of threads that will be working
	thread_info thread_data[num_threads]; // array of information needed by each thread

	// create threads and start writing records
	for (int t = 0; t < num_threads; t++)
	{
		// initialize thread-centric data
		thread_data[t].thread_id = t;
		thread_data[t].record_num = 1;
		thread_data[t].fd = fd;
		// create threads, start writing records
		thread_error = pthread_create(&threads[t], NULL, worker_thread_init, (void *) &thread_data[t]);
		if (thread_error)
		{
			printf("ERROR; return code from pthread_create() is %d\n", thread_error);
			exit(-1);
		}
	}

	// wait for all threads to complete their task
	for (int t = 0; t < num_threads; t++)
		pthread_join(threads[t], NULL);

	// clean up and exit
	close(fd);
	return 0;
}

void * worker_thread_init(void * thread_arg)
{
	// parse the thread-centric data
	thread_info * thread_data;
	thread_data = (thread_info *) thread_arg;
	// if in debug mode, print 10 records, else run indefinitely
	if (dflag)
		for (int i = 0; i < 10; i++)
			create_record(thread_data);
	else
		while (true)
			create_record(thread_data);
	pthread_exit(NULL);
}

void create_record(thread_info * record_data)
{
	// record the time this record is started
	auto since_epoch = std::chrono::high_resolution_clock::now().time_since_epoch();
	auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch).count();

	// create an array of longs that comprise a complete record
	long * record = (long *) malloc(RECORD_SIZE);

	// collect the address for the record
	long address = generate_address();

	// store the checksum of the generated record for validation
	long checksum;

	// fill known / constant record data
	record[IND_THREAD_ID] = record_data->thread_id;
	record[IND_RECORD_NUM] = record_data->record_num;
	record[IND_RECORD_ADDRESS] = address;
	record[IND_TIMESTAMP] = (long) nanos;

	// generate "random data"
	for (int i = IND_FIRST_RANDOM_RECORD; i < NUM_PIECES - 1; i++)
	{
		record[i] = record[i-1] + record[0] * record[1] * i + record[2];
	}

	// checksum the record and fill in the blanks where required
	checksum = Fletcher64(record);
	record[IND_TAIL_CHECKSUM] = checksum;
	record[IND_HEAD_CHECKSUM] = checksum;

	// debug output
	if (dflag)
		std::cout << "Record: " << record[IND_THREAD_ID] << " " << record[IND_RECORD_NUM] << " "
		          << record[IND_RECORD_ADDRESS] << " " << record[IND_TIMESTAMP] << " " << checksum << std::endl;

	// write the created record to the device-file
	size_t msg = write_record(record_data, record, RECORD_SIZE);

	// debug output
	if (dflag)
		std::cout << "msg " << msg << std::endl;

	// increment the number of records created
	record_data->record_num += 1;

	// clean up memory
	free(record);
}

size_t write_record(thread_info * record_data, long * record, long size)
{
	// parse the required arguments to write a record to the device-file
	size_t nbyte = size / NUM_PIECES;
	off_t offset = record[IND_RECORD_ADDRESS] * RECORD_SIZE;
	int fd = record_data->fd;

	// debug output
	if (dflag)
		std::cout << "fd " << fd << " nbyte " << nbyte << " offset " << offset << std::endl;

	size_t msg = 0; // number of bytes successfully written

	// write all the pieces of the record, sequentially, to the device-file
	for (int piece = 0; piece < NUM_PIECES; piece++)
	{
		// debug output
		if (dflag)
			if (piece < IND_FIRST_RANDOM_RECORD && piece != IND_TAIL_CHECKSUM)
				std::cout << record[piece] << " " << "offset: " << offset << " ";
		// write single long and increment number of bytes written
		msg += pwrite(fd, &record[piece], nbyte, offset);
		// increase offset so that it's written to the next spot
		offset += (off_t) nbyte;
	}

	// debug output
	if (dflag)
		std::cout << std::endl << "wrote: " << msg << std::endl;

	return msg; // should be 8192 (8KB)
}

int generate_address(void)
{
	// declare static generator so it can be used by all threads
	static thread_local std::mt19937* generator;

	// seed with current time
	if (!generator)
		generator = new std::mt19937(clock());

	// utilize a uniform distribution
	std::uniform_int_distribution<int> distribution(first_record, last_record);

	// return the generated number
	return distribution(*generator);
}