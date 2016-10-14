#include <iostream>
#include <random>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <pthread.h>
#include <chrono>

#include "thread_info.h"
#include "thread_local.h"
#include "record_indexes.h"

#define O_BINARY 0
const int RECORD_SIZE = 8 * 1024;

int first_record;
int last_record;
int file_size;

size_t get_file_size(int);
void create_record(thread_info *);
size_t write_record(thread_info *, long *, long);
void * worker_thread_init(void *);
int intRand(void);
int open_file(const char * filename);
uint64_t Fletcher64 (long *, int);