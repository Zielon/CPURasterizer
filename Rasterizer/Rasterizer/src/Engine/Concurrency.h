#pragma once

#include <functional>
#include <execution>
#include <algorithm>
#include <utility>

#ifdef PARALLEL
constexpr std::execution::parallel_policy policy = std::execution::par;
#else
constexpr std::execution::sequenced_policy policy = std::execution::seq;
#endif

#include "../Config.h"

namespace Engine
{
	class Concurrency final
	{
	public:
		template <class It, class Fn>
		CFG_FORCE_INLINE static void ForEach(It first, It last, Fn func)
		{
			std::for_each(policy, first, last, func);
		}

		CFG_FORCE_INLINE static void ForEach(uint32_t first, uint32_t last, std::function<void(int)> func)
		{
			std::vector<int> v(last - first);
			std::iota(std::begin(v), std::end(v), first);
			std::for_each(policy, v.begin(), v.end(), std::move(func));
		}

		template <class It, class Ty>
		CFG_FORCE_INLINE static void Fill(It first, It last, const Ty& val)
		{
			std::fill(policy, first, last, val);
		};
	};
}
