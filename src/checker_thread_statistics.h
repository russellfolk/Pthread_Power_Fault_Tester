/**
  * Thread Statistics stores the data about each writer thread discovered in the
  * device-file
  *
  * With this structure we store the number of records written, the number of
  * complete records written, and the number of partial (should sum to total)
  * records written.
  * Furthermore, we can track the percentage of tried writes vs successful
  * writes.
  * Another, nicety is tracking if there are write misses, where a record is to
  * be written to one location but instead is written to another. This number is
  * expected to always be 0.
  * Finally, we record the record write time so that we can estimate the time of
  * power failure.
  */
typedef struct _thread_statistics
{
	// total number of records written
	int num_records;
	// number of records completely written
	int num_complete;
	// number of records partially written
	int num_partial;
	// this is the percentage of possible writes written [0, 1]
	double percentage_written;
	// complete records written to the wrong location, should always be 0...
	int num_missed_write;
	// time stamp record was written, recorded in milliseconds since the epoch
	long record_write_time;
	// was the last record successful
	bool last_success;
} thread_statistics;