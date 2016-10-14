#include "main.h"

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
	char * num_thr;

	// permanent values
	int num_records;
	int fd;

	while ((c = getopt(argc, argv, "w:f:")) != -1)
	{
		switch (c)
		{
			case 'f':
				filename = optarg;
				fflag = 1;
				break;
			case 'w':
				num_threads = atoi(optarg);//std::strtol(optarg, &num_thr, 10);
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

	pthread_mutex_t f_lock;// = PTHREAD_MUTEX_INITIALIZER;
	std::cout << "mutex?";
	pthread_mutex_init(&f_lock, NULL);
	std::cout << "init?";

	int rc;
	pthread_t threads[num_threads];
	thread_info thread_data[num_threads];
	for (int t = 0; t < num_threads; t++)
	{
		std::cout << "Starting thread " << t;
		thread_data[t].thread_id = t;
		thread_data[t].record_num = 1;
		thread_data[t].fd = fd;
		thread_data[t].file_lock = &f_lock;
		rc = pthread_create(&threads[t], NULL, worker_thread_init, (void *) &thread_data[t]);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	close(fd);
	pthread_mutex_destroy(&f_lock);
	pthread_exit(NULL);
	return 0;
}

void * worker_thread_init(void * thread_arg)
{
	thread_info * thread_data;
	thread_data = (thread_info *) thread_arg;
	//std::cout << "Thread " << thread_id << " rec " << record_num << " fd " << fd << std::endl;
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
	for (int i = FIRST_RANDOM_RECORD; i < record_size - 1; i++)
	{
		record[i] = record[i-1] + record[0] * record[1] * i + record[2];
	}

	checksum = Fletcher64(record, record_size - 1);
	record[record_size - 1] = Fletcher64(record, record_size - 1);

	//std::cout << "Record: " << record[IND_THREAD_ID] << " " << record[IND_RECORD_NUM] << " " << record[2] << " " << checksum << " " << record[record_size-1] << std::endl;
	size_t msg = write_record(record_data, record, (record_size * sizeof(record)));
	//std::cout << "msg " << msg << std::endl;
	record_data->record_num += 1;
	free(record);
}

size_t write_record(thread_info * record_data, long * record, long size)
{
	size_t nbyte = size;
	off_t offset = record[IND_RECORD_ADDRESS] * RECORD_SIZE;

	//std::cout << "fd " << fd << " nbyte " << nbyte << " offset " << offset << std::endl;
	//std::cout << (offset < file_size) << " " << (offset + nbyte < file_size) << std::endl;
	pthread_mutex_lock(record_data->file_lock);
	size_t msg = pwrite(record_data->fd, record, nbyte, offset);
	pthread_mutex_unlock(record_data->file_lock);
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