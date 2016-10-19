/**
  * file_lib is a File Library that allows for the needed file operations
  *
  * This project requires low level system calls that will allow for the file to
  * opened using a specific set of flags. For macOS I am using read/write and
  * sync rather than dsync in an effort to "break the file". For linux I am also
  * including the direct flag to bypass the operating system and write directly
  * to disk. These methods were found to be ineffective as the POSIX directive
  * for pwrite demands that it is an "atomic operation" in that the OS will not
  * interrupt pwrite / pread until the operation is finished or before it begins
  * a sequential operation.
  *
  * This library is capable of opening a binary file for read write operations.
  * Furthermore it is capable of analyzing the size of a binary file, which is
  * required to determine how many records are capable of existing inside the
  * file.
  */
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __APPLE__
	#include <fcntl.h>
	#define FILE_FLAGS (O_RDWR | O_SYNC)
#elif __linux__
	// needed for O_DIRECT
	#include <fcntl.h>
	#define FILE_FLAGS (O_RDWR | O_DIRECT | O_SYNC)
#endif

/**
  * Open file returns a file descriptor
  *
  * This method takes in a filename (const char *) and opens the file using the
  * flags defined according to the operating system it is being run upon. This
  * file descriptor is allowed to be used throughout an application for file
  * read/write operations until the file is closed later.
  *
  * @param  const char *  the filename of a file to be opened
  * @return               the file descriptor for the file opened
  */
int open_file(const char *);

/**
 * Get file size returns the size of a binary file in bytes
 *
 * The normal method of fseek will not work reliably with binary files as shown
 * by much research. Instead, I found a POSIX-compliant way to determine the
 * size of a binary file and have reimplemented it from the following source:
 * https://www.securecoding.cert.org/confluence/display/c/FIO19-C.+Do+not+ \\
 * use+fseek()+and+ftell()+to+compute+the+size+of+a+regular+file
 *
 * @param  int the file descriptor for an already opened binary file
 * @return     the size of the binary file in bytes
 */
size_t get_file_size(int);