#pragma once

#include "./ext/vector_uint2.hpp"
#include "./ext/vector_float2.hpp"
#include "./ext/vector_float4.hpp"

namespace glm
{
	GLM_FUNC_DECL uint packUnorm2x16(vec2 const& v);
	GLM_FUNC_DECL uint packSnorm2x16(vec2 const& v);
	GLM_FUNC_DECL uint packUnorm4x8(vec4 const& v);
	GLM_FUNC_DECL uint packSnorm4x8(vec4 const& v);
	GLM_FUNC_DECL vec2 unpackUnorm2x16(uint p);
	GLM_FUNC_DECL vec2 unpackSnorm2x16(uint p);
	GLM_FUNC_DECL vec4 unpackUnorm4x8(uint p);
	GLM_FUNC_DECL vec4 unpackSnorm4x8(uint p);
	GLM_FUNC_DECL double packDouble2x32(uvec2 const& v);
	GLM_FUNC_DECL uvec2 unpackDouble2x32(double v);
	GLM_FUNC_DECL uint packHalf2x16(vec2 const& v);
	GLM_FUNC_DECL vec2 unpackHalf2x16(uint v);
}

#include "detail/func_packing.inl"