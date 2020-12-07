#pragma once

#include <execution>

namespace Engine
{
	class Concurrency final
	{
	public:
		template <class It, class Fn>
		__forceinline static void ForEach(It first, It last, Fn func)
		{
			std::for_each(std::execution::par, first, last, func);
		}

		template <class It, class Ty>
		__forceinline static void Fill(It first, It last, const Ty& val)
		{
			std::fill(std::execution::par, first, last, val);
		};
	};
}
