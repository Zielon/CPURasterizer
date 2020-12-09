#pragma once

#include <immintrin.h>

class BoolAVX
{
public:
	enum { size = 8 };

	union
	{
		__m256 m256;
		float v[8];
		int i[8];
	};
};
