#include "Fletcher64.h"

uint64_t Fletcher64 (long * record)
{
	uint64_t sum1 = 0;
	uint64_t sum2 = 0;

	// mod_value is the max value of 2^32-1 so that I avoid 0 checksums
	uint64_t mod_value = 4294967295;

	for(int i = IND_THREAD_ID; i < IND_TAIL_CHECKSUM; i++)
	{
		sum1 = (sum1 + record[i]) % mod_value;
		sum2 = (sum2 + sum1) % mod_value;
	}

	return (sum2 << 32) | sum1;
}