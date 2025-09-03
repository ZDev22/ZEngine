#pragma once

#include "detail/qualifier.hpp"
#include "detail/_fixes.hpp"

namespace glm
{	template<typename genType>
	GLM_FUNC_DECL GLM_CONSTEXPR genType abs(genType x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> abs(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> sign(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> floor(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> trunc(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> round(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> roundEven(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> ceil(vec<L, T, Q> const& x);
	template<typename genType>
	GLM_FUNC_DECL genType fract(genType x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> fract(vec<L, T, Q> const& x);
	template<typename genType>
	GLM_FUNC_DECL genType mod(genType x, genType y);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> mod(vec<L, T, Q> const& x, T y);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> mod(vec<L, T, Q> const& x, vec<L, T, Q> const& y);
	template<typename genType>
	GLM_FUNC_DECL genType modf(genType x, genType& i);
	template<typename genType>
	GLM_FUNC_DECL GLM_CONSTEXPR genType min(genType x, genType y);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> min(vec<L, T, Q> const& x, T y);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> min(vec<L, T, Q> const& x, vec<L, T, Q> const& y);
	template<typename genType>
	GLM_FUNC_DECL GLM_CONSTEXPR genType max(genType x, genType y);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> max(vec<L, T, Q> const& x, T y);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> max(vec<L, T, Q> const& x, vec<L, T, Q> const& y);
	template<typename genType>
	GLM_FUNC_DECL GLM_CONSTEXPR genType clamp(genType x, genType minVal, genType maxVal);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> clamp(vec<L, T, Q> const& x, T minVal, T maxVal);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> clamp(vec<L, T, Q> const& x, vec<L, T, Q> const& minVal, vec<L, T, Q> const& maxVal);
	template<typename genTypeT, typename genTypeU>
	GLM_FUNC_DECL GLM_CONSTEXPR genTypeT mix(genTypeT x, genTypeT y, genTypeU a);
	template<length_t L, typename T, typename U, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, U, Q> const& a);
	template<length_t L, typename T, typename U, qualifier Q>
	GLM_FUNC_DECL GLM_CONSTEXPR vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, U a);
	template<typename genType>
	GLM_FUNC_DECL genType step(genType edge, genType x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> step(T edge, vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> step(vec<L, T, Q> const& edge, vec<L, T, Q> const& x);
	template<typename genType>
	GLM_FUNC_DECL genType smoothstep(genType edge0, genType edge1, genType x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> smoothstep(T edge0, T edge1, vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> smoothstep(vec<L, T, Q> const& edge0, vec<L, T, Q> const& edge1, vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, bool, Q> isnan(vec<L, T, Q> const& x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, bool, Q> isinf(vec<L, T, Q> const& x);
	GLM_FUNC_DECL int floatBitsToInt(float v);
	template<length_t L, qualifier Q>
	GLM_FUNC_DECL vec<L, int, Q> floatBitsToInt(vec<L, float, Q> const& v);
	GLM_FUNC_DECL uint floatBitsToUint(float v);
	template<length_t L, qualifier Q>
	GLM_FUNC_DECL vec<L, uint, Q> floatBitsToUint(vec<L, float, Q> const& v);
	GLM_FUNC_DECL float intBitsToFloat(int v);
	template<length_t L, qualifier Q>
	GLM_FUNC_DECL vec<L, float, Q> intBitsToFloat(vec<L, int, Q> const& v);
	GLM_FUNC_DECL float uintBitsToFloat(uint v);
	template<length_t L, qualifier Q>
	GLM_FUNC_DECL vec<L, float, Q> uintBitsToFloat(vec<L, uint, Q> const& v);
	template<typename genType>
	GLM_FUNC_DECL genType fma(genType const& a, genType const& b, genType const& c);
	template<typename genType>
	GLM_FUNC_DECL genType frexp(genType x, int& exp);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> frexp(vec<L, T, Q> const& v, vec<L, int, Q>& exp);
	template<typename genType>
	GLM_FUNC_DECL genType ldexp(genType const& x, int const& exp);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> ldexp(vec<L, T, Q> const& v, vec<L, int, Q> const& exp);
}

#include "detail/func_common.inl"