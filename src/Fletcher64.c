#include "Fletcher64.h"

uint64_t Fletcher64 (long * data, int number_to_checksum)
{
	uint64_t sum1 = 0;
	uint64_t sum2 = 0;
	uint64_t mod_value = 4294967296;

	for(int i = 0; i < number_to_checksum; i++)
	{
		sum1 = (sum1 + data[i]) % mod_value;
		sum2 = (sum2 + sum1) % mod_value;
	}

	return (sum2 << 32) | sum1;
}