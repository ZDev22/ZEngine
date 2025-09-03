#pragma once

#include "detail/type_vec3.hpp"

namespace glm
{	
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL T length(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL T distance(vec<L, T, Q> const& p0, vec<L, T, Q> const& p1);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR T dot(vec<L, T, Q> const& x, vec<L, T, Q> const& y);
	template<typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<3, T, Q> cross(vec<3, T, Q> const& x, vec<3, T, Q> const& y);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> normalize(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> faceforward(
		vec<L, T, Q> const& N,
		vec<L, T, Q> const& I,
		vec<L, T, Q> const& Nref);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> reflect(
		vec<L, T, Q> const& I,
		vec<L, T, Q> const& N);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> refract(
		vec<L, T, Q> const& I,
		vec<L, T, Q> const& N,
		T eta);
}

#include "detail/func_geometric.inl"