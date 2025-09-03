#pragma once

#include "detail/type_vec1.hpp"
#include "detail/type_vec2.hpp"
#include "detail/type_vec3.hpp"
#include "detail/type_vec4.hpp"
#include <cmath>

namespace glm
{
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> pow(vec<L, T, Q> const& base, vec<L, T, Q> const& exponent);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> exp(vec<L, T, Q> const& v);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> log(vec<L, T, Q> const& v);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> exp2(vec<L, T, Q> const& v);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> log2(vec<L, T, Q> const& v);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> sqrt(vec<L, T, Q> const& v);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> inversesqrt(vec<L, T, Q> const& v);
}

#include "detail/func_exponential.inl"