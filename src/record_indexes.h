/**
  * This file contains all firm indexes for known values in a given record.
  * Needed by both writer and checker in order to allow for proper checking and
  * statistic recording. The writer must create a valid record using these
  * locations.
  */

// To attempt to be able to track when the file is broken, I am writing a
// checksum to the front and back of the file
#define IND_HEAD_CHECKSUM 0
// This location stores the id for the thread that writes the record
#define IND_THREAD_ID 1
// This location stores the number of records this thread has written so far
#define IND_RECORD_NUM 2
// This location stores the address that this record is supposed to be written
// to in the file
#define IND_RECORD_ADDRESS 3
// This location stores the time in nanoseconds from UNIX epoch that the record
// was generated to be written
#define IND_TIMESTAMP 4
// This location stores the first "random data" entry in the record. "Random
// data" is generated using a known mathematical formula which can be reverse-
// engineered for checking how accurately a record is written.
#define IND_FIRST_RANDOM_RECORD 5
// To attempt to be able to track when the file is broken, I am writing a
// checksum to the front and back of the file
// 1023 is due to the size of long being 8 bytes, a record being 8KB, and thus
// 8 * 1024 / 8 = 1024. Account for 0 indexed and the last entry is 1023.
#define IND_TAIL_CHECKSUM 1023