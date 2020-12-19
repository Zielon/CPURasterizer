#pragma once

namespace Engine
{
	class Antialising
	{
	public:
		virtual ~Antialising() = default;
		virtual void Filter() = 0;
	};
}
