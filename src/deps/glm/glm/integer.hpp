#pragma once

#include "detail/qualifier.hpp"
#include "common.hpp"
#include "vector_relational.hpp"

namespace glm
{
	template<length_t L, qualifier Q>
	GLM_FUNC_DECL vec<L, uint, Q> uaddCarry(
		vec<L, uint, Q> const& x,
		vec<L, uint, Q> const& y,
		vec<L, uint, Q> & carry);
	template<length_t L, qualifier Q>
	GLM_FUNC_DECL vec<L, uint, Q> usubBorrow(
		vec<L, uint, Q> const& x,
		vec<L, uint, Q> const& y,
		vec<L, uint, Q> & borrow);
	template<length_t L, qualifier Q>
	GLM_FUNC_DISCARD_DECL void umulExtended(
		vec<L, uint, Q> const& x,
		vec<L, uint, Q> const& y,
		vec<L, uint, Q> & msb,
		vec<L, uint, Q> & lsb);
	template<length_t L, qualifier Q>
	GLM_FUNC_DISCARD_DECL void imulExtended(
		vec<L, int, Q> const& x,
		vec<L, int, Q> const& y,
		vec<L, int, Q> & msb,
		vec<L, int, Q> & lsb);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> bitfieldExtract(
		vec<L, T, Q> const& Value,
		int Offset,
		int Bits);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> bitfieldInsert(
		vec<L, T, Q> const& Base,
		vec<L, T, Q> const& Insert,
		int Offset,
		int Bits);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> bitfieldReverse(vec<L, T, Q> const& v);
	template<typename genType>
	GLM_FUNC_DECL int bitCount(genType v);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, int, Q> bitCount(vec<L, T, Q> const& v);
	template<typename genIUType>
	GLM_FUNC_DECL int findLSB(genIUType x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, int, Q> findLSB(vec<L, T, Q> const& v);
	template<typename genIUType>
	GLM_FUNC_DECL int findMSB(genIUType x);
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, int, Q> findMSB(vec<L, T, Q> const& v);
}

#include "detail/func_integer.inl"