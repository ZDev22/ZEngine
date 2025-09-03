#pragma once

#include "detail/qualifier.hpp"
#include "detail/setup.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat2x2.hpp"
#include "mat2x3.hpp"
#include "mat2x4.hpp"
#include "mat3x2.hpp"
#include "mat3x3.hpp"
#include "mat3x4.hpp"
#include "mat4x2.hpp"
#include "mat4x3.hpp"
#include "mat4x4.hpp"

namespace glm {
namespace detail
{
	template<length_t C, length_t R, typename T, qualifier Q>
	struct outerProduct_trait{};

	template<typename T, qualifier Q>
	struct outerProduct_trait<2, 2, T, Q>
	{
		typedef mat<2, 2, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<2, 3, T, Q>
	{
		typedef mat<3, 2, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<2, 4, T, Q>
	{
		typedef mat<4, 2, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<3, 2, T, Q>
	{
		typedef mat<2, 3, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<3, 3, T, Q>
	{
		typedef mat<3, 3, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<3, 4, T, Q>
	{
		typedef mat<4, 3, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<4, 2, T, Q>
	{
		typedef mat<2, 4, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<4, 3, T, Q>
	{
		typedef mat<3, 4, T, Q> type;
	};

	template<typename T, qualifier Q>
	struct outerProduct_trait<4, 4, T, Q>
	{
		typedef mat<4, 4, T, Q> type;
	};
}
	template<length_t C, length_t R, typename T, qualifier Q>
	GLM_FUNC_DECL mat<C, R, T, Q> matrixCompMult(mat<C, R, T, Q> const& x, mat<C, R, T, Q> const& y);
	template<length_t C, length_t R, typename T, qualifier Q>
	GLM_FUNC_DECL typename detail::outerProduct_trait<C, R, T, Q>::type outerProduct(vec<C, T, Q> const& c, vec<R, T, Q> const& r);
	template<length_t C, length_t R, typename T, qualifier Q>
	GLM_FUNC_DECL typename mat<C, R, T, Q>::transpose_type transpose(mat<C, R, T, Q> const& x);
	template<length_t C, length_t R, typename T, qualifier Q>
	GLM_FUNC_DECL T determinant(mat<C, R, T, Q> const& m);
	template<length_t C, length_t R, typename T, qualifier Q>
	GLM_FUNC_DECL mat<C, R, T, Q> inverse(mat<C, R, T, Q> const& m);
}

#include "detail/func_matrix.inl"