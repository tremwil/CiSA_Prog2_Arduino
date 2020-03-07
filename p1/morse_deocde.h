#pragma once
#include "codes.h"

// Find largest X such that sum |D % X| < eps for fixed eps?
// 



// Tries to match dot length to fit the timings as best as possible
// Try to find largest X such that X | (t_{n+1} - t_n) for all n
// this n must then be the dot length
// From the value of n one assumes the international standard and finds
// dash = 3 * dot, code pause = 1 * dot, char pause = 3 * dot, word pause = 7 * dot
bool readMorseCodeBlock(const int *timings, int tSz, char *outBuffer, int bSz)
{
	
}