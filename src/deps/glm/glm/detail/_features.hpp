#pragma once
#if(GLM_COMPILER & GLM_COMPILER_GCC)

#	define GLM_CXX11_STATIC_ASSERT

#elif(GLM_COMPILER & GLM_COMPILER_CLANG)
#	if(__has_feature(cxx_exceptions))
#		define GLM_CXX98_EXCEPTIONS
#	endif

#	if(__has_feature(cxx_rtti))
#		define GLM_CXX98_RTTI
#	endif

#	if(__has_feature(cxx_access_control_sfinae))
#		define GLM_CXX11_ACCESS_CONTROL_SFINAE
#	endif

#	if(__has_feature(cxx_alias_templates))
#		define GLM_CXX11_ALIAS_TEMPLATE
#	endif

#	if(__has_feature(cxx_alignas))
#		define GLM_CXX11_ALIGNAS
#	endif

#	if(__has_feature(cxx_attributes))
#		define GLM_CXX11_ATTRIBUTES
#	endif

#	if(__has_feature(cxx_constexpr))
#		define GLM_CXX11_CONSTEXPR
#	endif

#	if(__has_feature(cxx_decltype))
#		define GLM_CXX11_DECLTYPE
#	endif

#	if(__has_feature(cxx_default_function_template_args))
#		define GLM_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS
#	endif

#	if(__has_feature(cxx_defaulted_functions))
#		define GLM_CXX11_DEFAULTED_FUNCTIONS
#	endif

#	if(__has_feature(cxx_delegating_constructors))
#		define GLM_CXX11_DELEGATING_CONSTRUCTORS
#	endif

#	if(__has_feature(cxx_deleted_functions))
#		define GLM_CXX11_DELETED_FUNCTIONS
#	endif

#	if(__has_feature(cxx_explicit_conversions))
#		define GLM_CXX11_EXPLICIT_CONVERSIONS
#	endif

#	if(__has_feature(cxx_generalized_initializers))
#		define GLM_CXX11_GENERALIZED_INITIALIZERS
#	endif

#	if(__has_feature(cxx_implicit_moves))
#		define GLM_CXX11_IMPLICIT_MOVES
#	endif

#	if(__has_feature(cxx_inheriting_constructors))
#		define GLM_CXX11_INHERITING_CONSTRUCTORS
#	endif

#	if(__has_feature(cxx_inline_namespaces))
#		define GLM_CXX11_INLINE_NAMESPACES
#	endif

#	if(__has_feature(cxx_lambdas))
#		define GLM_CXX11_LAMBDAS
#	endif

#	if(__has_feature(cxx_local_type_template_args))
#		define GLM_CXX11_LOCAL_TYPE_TEMPLATE_ARGS
#	endif

#	if(__has_feature(cxx_noexcept))
#		define GLM_CXX11_NOEXCEPT
#	endif

#	if(__has_feature(cxx_nonstatic_member_init))
#		define GLM_CXX11_NONSTATIC_MEMBER_INIT
#	endif

#	if(__has_feature(cxx_nullptr))
#		define GLM_CXX11_NULLPTR
#	endif

#	if(__has_feature(cxx_override_control))
#		define GLM_CXX11_OVERRIDE_CONTROL
#	endif

#	if(__has_feature(cxx_reference_qualified_functions))
#		define GLM_CXX11_REFERENCE_QUALIFIED_FUNCTIONS
#	endif

#	if(__has_feature(cxx_range_for))
#		define GLM_CXX11_RANGE_FOR
#	endif

#	if(__has_feature(cxx_raw_string_literals))
#		define GLM_CXX11_RAW_STRING_LITERALS
#	endif

#	if(__has_feature(cxx_rvalue_references))
#		define GLM_CXX11_RVALUE_REFERENCES
#	endif

#	if(__has_feature(cxx_static_assert))
#		define GLM_CXX11_STATIC_ASSERT
#	endif

#	if(__has_feature(cxx_auto_type))
#		define GLM_CXX11_AUTO_TYPE
#	endif

#	if(__has_feature(cxx_strong_enums))
#		define GLM_CXX11_STRONG_ENUMS
#	endif

#	if(__has_feature(cxx_trailing_return))
#		define GLM_CXX11_TRAILING_RETURN
#	endif

#	if(__has_feature(cxx_unicode_literals))
#		define GLM_CXX11_UNICODE_LITERALS
#	endif

#	if(__has_feature(cxx_unrestricted_unions))
#		define GLM_CXX11_UNRESTRICTED_UNIONS
#	endif

#	if(__has_feature(cxx_user_literals))
#		define GLM_CXX11_USER_LITERALS
#	endif

#	if(__has_feature(cxx_variadic_templates))
#		define GLM_CXX11_VARIADIC_TEMPLATES
#	endif

#endif