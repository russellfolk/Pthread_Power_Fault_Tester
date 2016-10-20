#include <unistd.h>
#include <cstdint>

// location of first element in the array
// [IND_THREAD_ID]
#include "record_indexes.h"

/**
 * Provides a Fletcher64 checksum for a given record entry. This checksum will
 * start with the IND_THREAD_ID, the first true value, and go to the value pre-
 * ceeding IND_TAIL_CHECKSUM, the last value of the record. The computed check-
 * sum is then returned to the requesting interface and used for operations.
 *
 * Implementation derived from:
 * https://en.wikipedia.org/wiki/Fletcher%27s_checksum
 *
 * @param  record   This should be an array of 1024 elements
 * @return          The checksum for elements 1-1022 (not including head/tail
 *                  checksums).
 */
uint64_t Fletcher64 (long *);