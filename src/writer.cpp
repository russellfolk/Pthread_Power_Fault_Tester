#include "writer.h"

int main(int argc, char **argv)
{
	// required for argument parsing
	int fflag = 0;
	int c;
	const char * def_filename = "device-file";
	char * filename;
	extern char *optarg;
	extern int optind;
	//static char usage[] = "usage: %s -f filename\n";
	const int def_num_threads = 2;
	int num_threads;

	// permanent values
	int num_records;
	int fd;

	while ((c = getopt(argc, argv, "w:f:d")) != -1)
	{
		switch (c)
		{
			case 'f':
				filename = optarg;
				fflag = 1;
				break;
			case 'w':
				num_threads = atoi(optarg);
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

	if (!num_threads)
		num_threads = def_num_threads;

	fd = open_file(filename);

	file_size = (int) get_file_size(fd);
	num_records = file_size / RECORD_SIZE;
	first_record = 0;
	last_record = num_records;

	std::cout << "File range is 0 to " << file_size << "." << std::endl;

	std::cout << "Number of possible records: " << num_records << std::endl;

	int rc;
	pthread_t threads[num_threads];
	thread_info thread_data[num_threads];
	for (int t = 0; t < num_threads; t++)
	{
		thread_data[t].thread_id = t;
		thread_data[t].record_num = 1;
		thread_data[t].fd = fd;
		rc = pthread_create(&threads[t], NULL, worker_thread_init, (void *) &thread_data[t]);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for (int t = 0; t < num_threads; t++)
		pthread_join(threads[t], NULL);

	pthread_exit(NULL);
	close(fd);
	return 0;
}

void * worker_thread_init(void * thread_arg)
{
	thread_info * thread_data;
	thread_data = (thread_info *) thread_arg;
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
	long record_size = RECORD_SIZE / sizeof(long);
	long size = record_size * sizeof(long);
	long * record = (long *) malloc(size);
	long address = intRand();
	long checksum;

	record[IND_THREAD_ID] = record_data->thread_id;
	record[IND_RECORD_NUM] = record_data->record_num;
	record[IND_RECORD_ADDRESS] = address;
	record[IND_TIMESTAMP] = (long) nanos;
	for (int i = IND_FIRST_RANDOM_RECORD; i < record_size - 1; i++)
	{
		record[i] = record[i-1] + record[0] * record[1] * i + record[2];
	}

	checksum = Fletcher64(record);
	record[IND_TAIL_CHECKSUM] = checksum;
	record[IND_HEAD_CHECKSUM] = checksum;

	if (dflag)
		std::cout << "Record: " << record[IND_THREAD_ID] << " " << record[IND_RECORD_NUM] << " " << record[IND_RECORD_ADDRESS] << " " << record[IND_TIMESTAMP] << " " << checksum << std::endl;
	size_t msg = write_record(record_data, record, size);
	if (dflag)
		std::cout << "msg " << msg << std::endl;
	record_data->record_num += 1;
	free(record);
}

size_t write_record(thread_info * record_data, long * record, long size)
{
	size_t nbyte = size / NUM_PIECES;
	off_t offset = record[IND_RECORD_ADDRESS] * RECORD_SIZE;
	int fd = record_data->fd;

	if (dflag)
		std::cout << "fd " << fd << " nbyte " << nbyte << " offset " << offset << std::endl;

	size_t msg = 0;
	for (int piece = 0; piece < NUM_PIECES; piece++)
	{
		if (dflag)
			if (piece < IND_FIRST_RANDOM_RECORD && piece != IND_TAIL_CHECKSUM)
				std::cout << record[piece] << " " << "offset: " << offset << " ";
		msg += pwrite(fd, &record[piece], nbyte, offset);
		offset += (off_t) nbyte;
	}
	if (dflag)
		std::cout << std::endl << "wrote: " << msg << std::endl;

	return msg;
}

int intRand(void)
{
	static thread_local std::mt19937* generator;
	if (!generator) generator = new std::mt19937(clock());
	std::uniform_int_distribution<int> distribution(first_record, last_record);
	return distribution(*generator);
}