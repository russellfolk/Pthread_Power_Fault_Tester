typedef struct _thread_statistics
{
	int num_records;
	int num_successful;
	int num_partial;
	// this is the percentage of possible writes written [0, 1]
	double percentage_written;
	// complete records written to the wrong location, should always be 0...
	int num_missed_write;
	// time stamp record was written, recorded in nanoseconds since the epoch
	long record_write_time;
	// was the last record successful
	bool last_success;
} thread_statistics;