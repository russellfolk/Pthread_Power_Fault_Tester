#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

size_t get_file_size(int);
int open_file(const char *);