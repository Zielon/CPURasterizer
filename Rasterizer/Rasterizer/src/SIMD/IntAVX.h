#pragma once

#include <immintrin.h>

class IntAVX
{
public:
	enum { size = 8 };

	union
	{
		__m256i m256;
		int i[8];
	};
};
