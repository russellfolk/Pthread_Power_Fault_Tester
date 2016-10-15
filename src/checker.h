#include <iostream>
#include <unistd.h>
#include <map>
#include <iomanip>
#include <cstring>

#include "record_indexes.h"
#include "file_lib.h"
#include "Fletcher64.h"
#include "checker_thread_statistics.h"

const int RECORD_SIZE = 8 * 1024;

void check_file(int, int);
long * get_record(int, long);
long print_record(long *);
void parse_record(long *);
bool checksum_record(long *);
void print_summary(void);

std::map<long, thread_statistics> stats;
std::map<long, thread_statistics>::iterator stats_it;
int total_threads = 0;