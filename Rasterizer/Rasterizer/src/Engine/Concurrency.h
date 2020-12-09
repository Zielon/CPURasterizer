#pragma once

#include <execution>

#define PARALLEL

#ifdef PARALLEL
constexpr std::execution::parallel_policy policy = std::execution::par;
#else
constexpr std::execution::sequenced_policy policy = std::execution::seq;
#endif

namespace Engine
{
	class Concurrency final
	{
	public:
		template <class It, class Fn>
		__forceinline static void ForEach(It first, It last, Fn func)
		{
			std::for_each(policy, first, last, func);
		}

		template <class It, class Ty>
		__forceinline static void Fill(It first, It last, const Ty& val)
		{
			std::fill(policy, first, last, val);
		};
	};
}
