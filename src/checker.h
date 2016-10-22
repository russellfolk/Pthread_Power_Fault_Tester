#include <iostream>
#include <unistd.h>
#include <map>
#include <iomanip>
#include <cstring>
#include <chrono>

// local libraries needed to run the checker
#include "record_indexes.h"
#include "file_lib.h"
#include "Fletcher64.h"
#include "checker_thread_statistics.h"

// size of the record, specified by the assignment
const int RECORD_SIZE = 8 * 1024;

// number of pieces that are inside a record as long = 8, 8KB / 8 = 1024
const int NUM_PIECES = 1024;

// map containing the statistics of each thread hashed by each thread's id
std::map<long, thread_statistics> stats;
// iterator for the map, used to go through the map
std::map<long, thread_statistics>::iterator stats_it;

// global value that will contain the total number of threads found writing to
// the supplied device-file
int total_threads = 0;

// flag that specifies whether or not to display debug information
bool dflag = false;

/**
  * Check device-file
  *
  * This loops through all of the records that could exist in the supplied
  * device-file. It begins by obtaining the actual record and then starting the
  * parsing process.
  *
  * @param fd          file descriptor that points to the device-file
  * @param num_records the total number of records contained in the device-file
  */
void check_file(int, int);

/**
  * Get a record from the device-file
  *
  * Given a specific address in the device-file, return the record written at
  * that address in the form of a long * array.
  *
  * @param  fd             file descriptor that points to the device-file
  * @param  record_address specific address for a record in the device-file
  * @return                pointer to array with record information
  */
long * get_record(int, long);

/**
  * Parse a given record
  *
  * First, if a record has been written to, print what has been written. If no
  * information has been written, cease parsing further.
  * Next, check if it's a complete record by checksuming what is read and
  * comparing with the head and tail checksum values.
  * Finally, if complete increment the completed stats, else figure out what is
  * valid, what is not, and figure out the percentage written.
  *
  * @param record         a pointer to a single record from the device-file
  * @param record_address the address the record was located in the device-file
  */
void parse_record(long *, long);

/**
  * Calculate what percentage of the record was successfully written
  *
  * Validates all pieces of the record and calculates the percentage of the
  * record that was successfully written. Directly tied to the formula used by
  * the writer in order to accurately recreate what should be included in a
  * record.
  *
  * @param  record         a pointer to a single record from the device-file
  * @param  record_address the address the record was located in the device-file
  * @return                percentage of record successfully written [0,1]
  */
double percent_written(long *, long);

/**
  * Check if the thread id is valid
  *
  * Currently, checks if this thread id has been seen before and returns... Not
  * necessarily a good check.
  *
  * @param  record a pointer to a single record from the device-file
  * @return        if the thread id is valid or not
  */
bool thread_id_valid(long *);

/**
  * Check if the record number is valid
  *
  * Currently, checks if this record number is greater than the number of
  * successfully written records. As only the last record should be a partial
  * record, this should be a sane check.
  *
  * @param  record a pointer to a single record from the device-file
  * @return        if the record number is valid or not
  */
bool record_num_valid(long *);

/**
  * Check if the address is valid
  *
  * Currently, checks if this record contains the same address within as it was
  * actually located at. This is used also to validate that this is not a
  * miswrite. This should never be false unless the record address was just not
  * written to the record.
  *
  * @param  record         a pointer to a single record from the device-file
  * @param  record_address the address the record was located in the device-file
  * @return                if the address is valid or not
  */
bool address_valid(long *, long);

/**
  * Check if the time stamp is valid
  *
  * Currently, checks if the time stamp is greater than 0. As a time stamp is a
  * count of the number of milliseconds since the UNIX epoch, this is valid for
  * a very long time to come...
  *
  * @param  record a pointer to a single record from the device-file
  * @return        if the time stamp is valid or not
  */
bool timestamp_valid(long *);

/**
  * Check if the checksum is valid
  *
  * Reruns a Fletcher64 checksum on the record as it exists now then returns if
  * it matches the checksums stored in the head and tail of the record.
  *
  * @param  record a pointer to a single record from the device-file
  * @return        if the head and tail checksums are valid or not
  */
bool checksum_record(long *);

/**
  * Check if a record has been written to
  *
  * As a device-file initialially contains all zeros, this checks for any
  * non-zero value at a given record. If any non-zero is detected, the record is
  * known to be altered.
  *
  * @param  record a pointer to a single record from the device-file
  * @return        if the record has been written to
  */
bool is_record_blank(long *);

/**
  * Estimate the time that a power loss was simulated
  *
  * This function grabs the current time and uses the current milliseconds since
  * the UNIX epoch against the latest time stamp from all threads that wrote to
  * the device-file and uses that to calculate what time the power failure may
  * have occurred.
  *
  * @return  the estimated time that the power loss was simulated
  */
std::time_t estimated_power_loss(void);

/**
  * Print out written records
  *
  * Any record that has been written to will be written to the screen. The
  * required information for this assignment is thread id, record number,
  * address written to, time written, and checksum of the record. Basically, all
  * data not "randomly generated".
  *
  * thread id, record number, address, time stamp, checksum
  */
void print_record(long *);

/**
  * Print out the statistics of all the records written
  *
  * Print the estimated time of power failure simulation.
  *
  * For each thread that wrote data to the device-file print the following:
  * thread id, number of records written, number of successful writes,
  * number of partial writes, percentage of data successfully written,
  * number of writes written to the wrong location,
  * if the last record was a successful (complete) write
  */
void print_summary(void);
