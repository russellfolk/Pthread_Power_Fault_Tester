#include <iostream>
#include <unistd.h>

#include "record_indexes.h"
#include "file_lib.h"
#include "Fletcher64.h"

const int RECORD_SIZE = 8 * 1024;

void check_file(int, int);
long * get_record(int, long);
long print_record(long *);

