#pragma once
#include "setup.hpp"
#include <limits>

namespace glm{
namespace detail
{
	template <typename T, bool isFloat>
	struct compute_equal
	{
		GLM_FUNC_QUALIFIER GLM_CONSTEXPR static bool call(T a, T b)
		{
			return a == b;
		}
	};
}
}