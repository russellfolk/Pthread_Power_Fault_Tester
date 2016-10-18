typedef struct _thread_statistics
{
	int num_records;
	int num_successful;
	int num_partial;
	// time stamp record was written, recorded in nanoseconds since the epoch
	long record_write_time;
	// was the last record successful
	bool last_success;
} thread_statistics;