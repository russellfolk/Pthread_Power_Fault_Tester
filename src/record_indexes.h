/**
  * This file contains all firm indexes for known values in a given record.
  * Needed by both writer and checker in order to allow for proper checking and
  * statistic recording. The writer must create a valid record using these
  * locations.
  */

#define IND_HEAD_CHECKSUM 0
#define IND_THREAD_ID 1
#define IND_RECORD_NUM 2
#define IND_RECORD_ADDRESS 3
#define IND_TIMESTAMP 4
#define IND_FIRST_RANDOM_RECORD 5
#define IND_TAIL_CHECKSUM 1023 // <= 8KB / 8 = 1024: array 0 indexed