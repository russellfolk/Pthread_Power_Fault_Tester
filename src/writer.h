#include <iostream>
#include <random>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <pthread.h>
#include <chrono>

// local libraries needed to run the writer
#include "thread_info.h"
#include "thread_local_random.h"
#include "record_indexes.h"
#include "Fletcher64.h"
#include "file_lib.h"

// number of pieces that are inside a record as long = 8, 8KB / 8 = 1024
const int NUM_PIECES = 1024;
// size of the record, specified by the assignment
const int RECORD_SIZE = 8 * 1024;

// global variables that contain the range for the random number generator for
// an address to be made
int first_record;
int last_record;

// the overall size of the device-file being operated upon
int file_size;

// flag that specifies whether or not to display debug information
bool dflag = false;

/**
  * Worker Thread Initialization
  *
  * worker_thread_init does one of two things, if debug mode is initialized, it
  * will generate 10 records and write them to the supplied device-file. This
  * allows for the device-file to be checked using the checker and verifying
  * that the information written is generated correctly. If debug mode is not
  * activated, then this will infinitely loop through creating and writing
  * records to the supplied device-file until the program is killed.
  *
  * @param thread_arg is a thread_info that contains the recurring data about a
  *                   record that must be included / incremented with every
  *                   record created
  */
void * worker_thread_init(void *);

/**
  * Create a record entry for the device file
  *
  * create_record generates a record that is setup using the record_indexes.h
  * fields to place the required information. A record includes a checksum, the
  * id of the thread that creates the record, the id of the record (incremented
  * with every record starting with 1), the address the record is to be written
  * to, the number of milliseconds since the epoch when the record was created,
  * and a set of computer generated data that is a set formula that can be later
  * verified to see where a breakdown occurs.
  *
  * @param record_data contains the recurring data about a record that must be
  *                    included / incremented with every record created
  */
void create_record(thread_info *);

/**
  * Write a Record to the supplied device-file
  *
  * Attempts to write a complete record to the supplied device-file. After much
  * research it was found that pwrite() is an "atomic function" in that
  * according to POSIX standards the command cannot be interrupted in the middle
  * of a write. As such, a record is now split up into 1024 writes, 1 write per
  * long contained in the record, and the writes are sequentially written to the
  * device-file. At the end of the write, the total number of bytes written is
  * returned. This SHOULD be 8192 bytes, but it is not guaranteed.
  *
  * @param record_data contains the recurring data about a record that must be
  *                    included / incremented with every record created; needed
  *                    for the file descriptor of the device-file
  * @param record      a complete record to be written to the device-file
  * @param size        the total size, in bytes, of the record to be written
  * @return            the number of bytes that is written to the device-file
  */
size_t write_record(thread_info *, long *, long);

/**
  * Generate random record Address for device-file
  *
  * This is a thread safe implementation of the mt19937 standard library random
  * number generator using a uniform distribution. The reason for a global rng
  * is so that each thread will not recreate the same pattern of records to
  * write to the device-file.
  *
  * @return  return an address location between the global first_record and
  * last_record values of the device-file
  */
int generate_address(void);