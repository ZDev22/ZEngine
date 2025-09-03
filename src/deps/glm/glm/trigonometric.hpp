#pragma once

#include "detail/setup.hpp"
#include "detail/qualifier.hpp"

namespace glm
{
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> radians(vec<L, T, Q> const& degrees);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> degrees(vec<L, T, Q> const& radians);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> sin(vec<L, T, Q> const& angle);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> cos(vec<L, T, Q> const& angle);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> tan(vec<L, T, Q> const& angle);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> asin(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> acos(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> atan(vec<L, T, Q> const& y, vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> atan(vec<L, T, Q> const& y_over_x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> sinh(vec<L, T, Q> const& angle);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> cosh(vec<L, T, Q> const& angle);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> tanh(vec<L, T, Q> const& angle);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> asinh(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> acosh(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> atanh(vec<L, T, Q> const& x);
}

#include "detail/func_trigonometric.inl"