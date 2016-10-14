

typedef struct saved_thread_info
{
	long thread_id;
	long record_num;
	int fd;
	pthread_mutex_t * file_lock;
} thread_info;