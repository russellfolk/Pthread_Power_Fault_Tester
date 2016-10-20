/**
  * Thread Info saves information required by each thread in the writer program
  *
  * When the writer is running, each thread is responsible for creating a record
  * containing its thread id, the number of records created thus far, and
  * writing them to a file.
  *
  * @member thread_id  the local id for the thread working
  * @member record_num the number of records written thus far
  * @member fd         the file descriptor of the file to be written to
  */
typedef struct saved_thread_info
{
	long thread_id;
	long record_num;
	int fd;
} thread_info;