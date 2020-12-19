#pragma once

#include "Antialising.h"

namespace Engine
{
	class FXAA : public Antialising
	{
	public:
		void Filter() override;
	};
}
