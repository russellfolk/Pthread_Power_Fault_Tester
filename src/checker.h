#include <iostream>
#include <unistd.h>
#include <map>
#include <iomanip>
#include <cstring>
#include <chrono>

#include "record_indexes.h"
#include "file_lib.h"
#include "Fletcher64.h"
#include "checker_thread_statistics.h"

const int RECORD_SIZE = 8 * 1024;
const int NUM_PIECES = 1024;

void check_file(int, int);
long * get_record(int, long);
void parse_record(long *, long);
double percent_written(long *, long);
bool thread_id_valid(long *);
bool record_num_valid(long *);
bool address_valid(long *, long);
bool timestamp_valid(long *);
bool checksum_record(long *);
bool is_record_blank(long *);
std::time_t estimated_power_loss(void);
void print_record(long *);
void print_summary(void);

std::map<long, thread_statistics> stats;
std::map<long, thread_statistics>::iterator stats_it;

int total_threads = 0;
bool dflag = false;
