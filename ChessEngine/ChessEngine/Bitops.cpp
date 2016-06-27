#include <assert.h>
#include <intrin.h>
#include <cstdlib>
#include <iostream>
#include "Bitops.h"

// Reverses the order of the bits in a binary integer representation.
void reverseBits64(uint64_t &mask)
{
	mask = (mask & 0x5555555555555555ui64) << 1  | (mask & 0xAAAAAAAAAAAAAAAAui64) >> 1;
	mask = (mask & 0x3333333333333333ui64) << 2  | (mask & 0xCCCCCCCCCCCCCCCCui64) >> 2;
	mask = (mask & 0x0F0F0F0F0F0F0F0Fui64) << 4  | (mask & 0xF0F0F0F0F0F0F0F0ui64) >> 4;
	mask = (mask & 0x00FF00FF00FF00FFui64) << 8  | (mask & 0xFF00FF00FF00FF00ui64) >> 8;
	mask = (mask & 0x0000FFFF0000FFFFui64) << 16 | (mask & 0xFFFF0000FFFF0000ui64) >> 16;
	mask = mask << 32 | mask >> 32;
}

void reverseBits8(uint8_t &mask)
{
	mask = (mask & 0x55) << 1 | (mask & 0xAA) >> 1;
	mask = (mask & 0x33) << 2 | (mask & 0xCC) >> 2;
	mask = mask << 4 | mask >> 4;
}

// Rotates a 64-bit sequence to the right.
void rotr64(uint64_t &mask, const unsigned int &places)
{
	int p = places;
	mask = (mask >> p) | (mask << (-p & 63));
}

// Rotates a 64-bit sequence to the left.
void rotl64(uint64_t &mask, const unsigned int &places)
{
	int p = places;
	mask = (mask << p) | (mask >> (-p & 63));
}

// Returns a pointer to a table containing the indices of all set bits in a 64-bit mask.
uint8_t *bit_i(uint64_t mask, unsigned int &count)
{
	unsigned long long popcnt = bitcount(mask);
	uint8_t *outPtr = new uint8_t[popcnt];
	unsigned long index = 0;
	unsigned int n = 0;

	while (mask)
	{
		_BitScanForward64(&index, mask);
		outPtr[n++] = (uint8_t)index;
		mask &= mask - 1;
	}

	count = (unsigned int)popcnt;
	return outPtr;
}

// Returns a vector containing the indices of all set bits in a 64-bit mask.
std::vector<unsigned long> bit_i(uint64_t mask)
{
	std::vector<unsigned long> vec;
	vec.resize(bitcount(mask));

	unsigned long index;
	unsigned int n = 0;
	while (mask)
	{
		_BitScanForward64(&index, mask);
		vec[n++] = index;
		mask &= mask - 1;
	}

	return vec;
}

// Returns the number of set bits in a 64-bit mask.
unsigned long bitcount(uint64_t mask)
{
	/*unsigned long long popcnt = __popcnt64(mask);
	if (popcnt > 64)
	{
		std::cout << "FATAL ERROR. POPCNT instruction returned unexpected result. __popcnt64(0x" << std::hex << mask << ") = " << popcnt << ". Maybe the instruction is not supported by your processor." << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}*/
	return (unsigned long)__popcnt64(mask);
}