#pragma once

namespace glm{
namespace detail
{
	template<typename T, int N>
	struct _swizzle_base0
	{
	protected:
		GLM_FUNC_QUALIFIER T& elem(int i){ return (reinterpret_cast<T*>(_buffer))[i]; }
		GLM_FUNC_QUALIFIER T const& elem(int i) const{ return (reinterpret_cast<const T*>(_buffer))[i]; }
		char    _buffer[1];
	};

	template<int N, typename T, qualifier Q, int E0, int E1, int E2, int E3, bool Aligned>
	struct _swizzle_base1 : public _swizzle_base0<T, N>
	{
	};

	template<typename T, qualifier Q, int E0, int E1, bool Aligned>
	struct _swizzle_base1<2, T, Q, E0,E1,-1,-2, Aligned> : public _swizzle_base0<T, 2>
	{
		GLM_FUNC_QUALIFIER vec<2, T, Q> operator ()()  const { return vec<2, T, Q>(this->elem(E0), this->elem(E1)); }
	};

	template<typename T, qualifier Q, int E0, int E1, int E2, bool Aligned>
	struct _swizzle_base1<3, T, Q, E0,E1,E2,-1, Aligned> : public _swizzle_base0<T, 3>
	{
		GLM_FUNC_QUALIFIER vec<3, T, Q> operator ()()  const { return vec<3, T, Q>(this->elem(E0), this->elem(E1), this->elem(E2)); }
	};

	template<typename T, qualifier Q, int E0, int E1, int E2, int E3, bool Aligned>
	struct _swizzle_base1<4, T, Q, E0,E1,E2,E3, Aligned> : public _swizzle_base0<T, 4>
	{
		GLM_FUNC_QUALIFIER vec<4, T, Q> operator ()()  const { return vec<4, T, Q>(this->elem(E0), this->elem(E1), this->elem(E2), this->elem(E3)); }
	};
	template<int N, typename T, qualifier Q, int E0, int E1, int E2, int E3, int DUPLICATE_ELEMENTS>
	struct _swizzle_base2 : public _swizzle_base1<N, T, Q, E0,E1,E2,E3, detail::is_aligned<Q>::value>
	{
		struct op_equal
		{
			GLM_FUNC_QUALIFIER void operator() (T& e, T& t) const{ e = t; }
		};

		struct op_minus
		{
			GLM_FUNC_QUALIFIER void operator() (T& e, T& t) const{ e -= t; }
		};

		struct op_plus
		{
			GLM_FUNC_QUALIFIER void operator() (T& e, T& t) const{ e += t; }
		};

		struct op_mul
		{
			GLM_FUNC_QUALIFIER void operator() (T& e, T& t) const{ e *= t; }
		};

		struct op_div
		{
			GLM_FUNC_QUALIFIER void operator() (T& e, T& t) const{ e /= t; }
		};

	public:
		GLM_FUNC_QUALIFIER _swizzle_base2& operator= (const T& t)
		{
			for (int i = 0; i < N; ++i)
				(*this)[i] = t;
			return *this;
		}

		GLM_FUNC_QUALIFIER _swizzle_base2& operator= (vec<N, T, Q> const& that)
		{
			_apply_op(that, op_equal());
			return *this;
		}

		GLM_FUNC_QUALIFIER void operator -= (vec<N, T, Q> const& that)
		{
			_apply_op(that, op_minus());
		}

		GLM_FUNC_QUALIFIER void operator += (vec<N, T, Q> const& that)
		{
			_apply_op(that, op_plus());
		}

		GLM_FUNC_QUALIFIER void operator *= (vec<N, T, Q> const& that)
		{
			_apply_op(that, op_mul());
		}

		GLM_FUNC_QUALIFIER void operator /= (vec<N, T, Q> const& that)
		{
			_apply_op(that, op_div());
		}

		GLM_FUNC_QUALIFIER T& operator[](int i)
		{
			const int offset_dst[4] = { E0, E1, E2, E3 };
			return this->elem(offset_dst[i]);
		}
		GLM_FUNC_QUALIFIER T operator[](int i) const
		{
			const int offset_dst[4] = { E0, E1, E2, E3 };
			return this->elem(offset_dst[i]);
		}

	protected:
		template<typename U>
		GLM_FUNC_QUALIFIER void _apply_op(vec<N, T, Q> const& that, const U& op)
		{
			T t[N];
			for (int i = 0; i < N; ++i)
				t[i] = that[i];
			for (int i = 0; i < N; ++i)
				op( (*this)[i], t[i] );
		}
	};

	template<int N, typename T, qualifier Q, int E0, int E1, int E2, int E3>
	struct _swizzle_base2<N, T, Q, E0,E1,E2,E3, 1> : public _swizzle_base1<N, T, Q, E0,E1,E2,E3, detail::is_aligned<Q>::value>
	{
		struct Stub {};

		GLM_FUNC_QUALIFIER _swizzle_base2& operator= (Stub const&) { return *this; }

		GLM_FUNC_QUALIFIER T operator[]  (int i) const
		{
			const int offset_dst[4] = { E0, E1, E2, E3 };
			return this->elem(offset_dst[i]);
		}
	};

	template<int N, typename T, qualifier Q, int E0, int E1, int E2, int E3>
	struct _swizzle : public _swizzle_base2<N, T, Q, E0, E1, E2, E3, (E0 == E1 || E0 == E2 || E0 == E3 || E1 == E2 || E1 == E3 || E2 == E3)>
	{
		typedef _swizzle_base2<N, T, Q, E0, E1, E2, E3, (E0 == E1 || E0 == E2 || E0 == E3 || E1 == E2 || E1 == E3 || E2 == E3)> base_type;

		using base_type::operator=;

		GLM_FUNC_QUALIFIER operator vec<N, T, Q> () const { return (*this)(); }
	};

#define GLM_SWIZZLE_TEMPLATE1   template<int N, typename T, qualifier Q, int E0, int E1, int E2, int E3>
#define GLM_SWIZZLE_TEMPLATE2   template<int N, typename T, qualifier Q, int E0, int E1, int E2, int E3, int F0, int F1, int F2, int F3>
#define GLM_SWIZZLE_TYPE1       _swizzle<N, T, Q, E0, E1, E2, E3>
#define GLM_SWIZZLE_TYPE2       _swizzle<N, T, Q, F0, F1, F2, F3>
#define GLM_SWIZZLE_VECTOR_BINARY_OPERATOR_IMPLEMENTATION(OPERAND)                 \
	GLM_SWIZZLE_TEMPLATE2                                                          \
	GLM_FUNC_QUALIFIER vec<N, T, Q> operator OPERAND ( const GLM_SWIZZLE_TYPE1& a, const GLM_SWIZZLE_TYPE2& b)  \
	{                                                                               \
		return a() OPERAND b();                                                     \
	}                                                                               \
	GLM_SWIZZLE_TEMPLATE1                                                          \
	GLM_FUNC_QUALIFIER vec<N, T, Q> operator OPERAND ( const GLM_SWIZZLE_TYPE1& a, const vec<N, T, Q>& b)                   \
	{                                                                               \
		return a() OPERAND b;                                                       \
	}                                                                               \
	GLM_SWIZZLE_TEMPLATE1                                                          \
	GLM_FUNC_QUALIFIER vec<N, T, Q> operator OPERAND ( const vec<N, T, Q>& a, const GLM_SWIZZLE_TYPE1& b)                   \
	{                                                                               \
		return a OPERAND b();                                                       \
	}

#define GLM_SWIZZLE_SCALAR_BINARY_OPERATOR_IMPLEMENTATION(OPERAND)								\
	GLM_SWIZZLE_TEMPLATE1																		\
	GLM_FUNC_QUALIFIER vec<N, T, Q> operator OPERAND ( const GLM_SWIZZLE_TYPE1& a, const T& b)	\
	{																							\
		return a() OPERAND b;																	\
	}																							\
	GLM_SWIZZLE_TEMPLATE1																		\
	GLM_FUNC_QUALIFIER vec<N, T, Q> operator OPERAND ( const T& a, const GLM_SWIZZLE_TYPE1& b)	\
	{																							\
		return a OPERAND b();																	\
	}
#define GLM_SWIZZLE_FUNCTION_1_ARGS(RETURN_TYPE,FUNCTION)												\
	GLM_SWIZZLE_TEMPLATE1																				\
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const GLM_SWIZZLE_TYPE1& a)		\
	{																									\
		return FUNCTION(a());																			\
	}

#define GLM_SWIZZLE_FUNCTION_2_ARGS(RETURN_TYPE,FUNCTION)                                                       \
	GLM_SWIZZLE_TEMPLATE2                                                                                       \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const GLM_SWIZZLE_TYPE1& a, const GLM_SWIZZLE_TYPE2& b) \
	{                                                                                                           \
		return FUNCTION(a(), b());                                                                              \
	}                                                                                                           \
	GLM_SWIZZLE_TEMPLATE1                                                                                       \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const GLM_SWIZZLE_TYPE1& a, const GLM_SWIZZLE_TYPE1& b) \
	{                                                                                                           \
		return FUNCTION(a(), b());                                                                              \
	}                                                                                                           \
	GLM_SWIZZLE_TEMPLATE1                                                                                       \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const GLM_SWIZZLE_TYPE1& a, const typename V& b)         \
	{                                                                                                           \
		return FUNCTION(a(), b);                                                                                \
	}                                                                                                           \
	GLM_SWIZZLE_TEMPLATE1                                                                                       \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const V& a, const GLM_SWIZZLE_TYPE1& b)                  \
	{                                                                                                           \
		return FUNCTION(a, b());                                                                                \
	}

#define GLM_SWIZZLE_FUNCTION_2_ARGS_SCALAR(RETURN_TYPE,FUNCTION)                                                             \
	GLM_SWIZZLE_TEMPLATE2                                                                                                    \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const GLM_SWIZZLE_TYPE1& a, const GLM_SWIZZLE_TYPE2& b, const T& c)   \
	{                                                                                                                         \
		return FUNCTION(a(), b(), c);                                                                                         \
	}                                                                                                                         \
	GLM_SWIZZLE_TEMPLATE1                                                                                                    \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const GLM_SWIZZLE_TYPE1& a, const GLM_SWIZZLE_TYPE1& b, const T& c)   \
	{                                                                                                                         \
		return FUNCTION(a(), b(), c);                                                                                         \
	}                                                                                                                         \
	GLM_SWIZZLE_TEMPLATE1                                                                                                    \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const GLM_SWIZZLE_TYPE1& a, const typename S0::vec_type& b, const T& c)\
	{                                                                                                                         \
		return FUNCTION(a(), b, c);                                                                                           \
	}                                                                                                                         \
	GLM_SWIZZLE_TEMPLATE1                                                                                                    \
	GLM_FUNC_QUALIFIER typename GLM_SWIZZLE_TYPE1::RETURN_TYPE FUNCTION(const typename V& a, const GLM_SWIZZLE_TYPE1& b, const T& c)           \
	{                                                                                                                         \
		return FUNCTION(a, b(), c);                                                                                           \
	}

}
}

namespace glm
{
	namespace detail
	{
		GLM_SWIZZLE_SCALAR_BINARY_OPERATOR_IMPLEMENTATION(-)
		GLM_SWIZZLE_SCALAR_BINARY_OPERATOR_IMPLEMENTATION(*)
		GLM_SWIZZLE_VECTOR_BINARY_OPERATOR_IMPLEMENTATION(+)
		GLM_SWIZZLE_VECTOR_BINARY_OPERATOR_IMPLEMENTATION(-)
		GLM_SWIZZLE_VECTOR_BINARY_OPERATOR_IMPLEMENTATION(*)
		GLM_SWIZZLE_VECTOR_BINARY_OPERATOR_IMPLEMENTATION(/)
	}
}

#define GLM_SWIZZLE2_2_MEMBERS(T, Q, E0,E1) \
	struct { detail::_swizzle<2, T, Q, 0,0,-1,-2> E0 ## E0; }; \
	struct { detail::_swizzle<2, T, Q, 0,1,-1,-2> E0 ## E1; }; \
	struct { detail::_swizzle<2, T, Q, 1,0,-1,-2> E1 ## E0; }; \
	struct { detail::_swizzle<2, T, Q, 1,1,-1,-2> E1 ## E1; };

#define GLM_SWIZZLE2_3_MEMBERS(T, Q, E0,E1) \
	struct { detail::_swizzle<3,T, Q, 0,0,0,-1> E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<3,T, Q, 0,0,1,-1> E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<3,T, Q, 0,1,0,-1> E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<3,T, Q, 0,1,1,-1> E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<3,T, Q, 1,0,0,-1> E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<3,T, Q, 1,0,1,-1> E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<3,T, Q, 1,1,0,-1> E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<3,T, Q, 1,1,1,-1> E1 ## E1 ## E1; };

#define GLM_SWIZZLE2_4_MEMBERS(T, Q, E0,E1) \
	struct { detail::_swizzle<4,T, Q, 0,0,0,0> E0 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,0,1> E0 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,1,0> E0 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,1,1> E0 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,0,0> E0 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,0,1> E0 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,1,0> E0 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,1,1> E0 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,0,0> E1 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,0,1> E1 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,1,0> E1 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,1,1> E1 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,0,0> E1 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,0,1> E1 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,1,0> E1 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,1,1> E1 ## E1 ## E1 ## E1; };

#define GLM_SWIZZLE3_2_MEMBERS(T, Q, E0,E1,E2) \
	struct { detail::_swizzle<2,T, Q, 0,0,-1,-2> E0 ## E0; }; \
	struct { detail::_swizzle<2,T, Q, 0,1,-1,-2> E0 ## E1; }; \
	struct { detail::_swizzle<2,T, Q, 0,2,-1,-2> E0 ## E2; }; \
	struct { detail::_swizzle<2,T, Q, 1,0,-1,-2> E1 ## E0; }; \
	struct { detail::_swizzle<2,T, Q, 1,1,-1,-2> E1 ## E1; }; \
	struct { detail::_swizzle<2,T, Q, 1,2,-1,-2> E1 ## E2; }; \
	struct { detail::_swizzle<2,T, Q, 2,0,-1,-2> E2 ## E0; }; \
	struct { detail::_swizzle<2,T, Q, 2,1,-1,-2> E2 ## E1; }; \
	struct { detail::_swizzle<2,T, Q, 2,2,-1,-2> E2 ## E2; };

#define GLM_SWIZZLE3_3_MEMBERS(T, Q ,E0,E1,E2) \
	struct { detail::_swizzle<3, T, Q, 0,0,0,-1> E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 0,0,1,-1> E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 0,0,2,-1> E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 0,1,0,-1> E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 0,1,1,-1> E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 0,1,2,-1> E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 0,2,0,-1> E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 0,2,1,-1> E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 0,2,2,-1> E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 1,0,0,-1> E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 1,0,1,-1> E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 1,0,2,-1> E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 1,1,0,-1> E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 1,1,1,-1> E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 1,1,2,-1> E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 1,2,0,-1> E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 1,2,1,-1> E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 1,2,2,-1> E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 2,0,0,-1> E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 2,0,1,-1> E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 2,0,2,-1> E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 2,1,0,-1> E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 2,1,1,-1> E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 2,1,2,-1> E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 2,2,0,-1> E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 2,2,1,-1> E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 2,2,2,-1> E2 ## E2 ## E2; };

#define GLM_SWIZZLE3_4_MEMBERS(T, Q, E0,E1,E2) \
	struct { detail::_swizzle<4,T, Q, 0,0,0,0> E0 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,0,1> E0 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,0,2> E0 ## E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,1,0> E0 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,1,1> E0 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,1,2> E0 ## E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,2,0> E0 ## E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,2,1> E0 ## E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,0,2,2> E0 ## E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,0,0> E0 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,0,1> E0 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,0,2> E0 ## E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,1,0> E0 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,1,1> E0 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,1,2> E0 ## E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,2,0> E0 ## E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,2,1> E0 ## E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,1,2,2> E0 ## E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,0,0> E0 ## E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,0,1> E0 ## E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,0,2> E0 ## E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,1,0> E0 ## E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,1,1> E0 ## E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,1,2> E0 ## E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,2,0> E0 ## E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,2,1> E0 ## E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 0,2,2,2> E0 ## E2 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,0,0> E1 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,0,1> E1 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,0,2> E1 ## E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,1,0> E1 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,1,1> E1 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,1,2> E1 ## E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,2,0> E1 ## E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,2,1> E1 ## E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,0,2,2> E1 ## E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,0,0> E1 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,0,1> E1 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,0,2> E1 ## E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,1,0> E1 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,1,1> E1 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,1,2> E1 ## E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,2,0> E1 ## E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,2,1> E1 ## E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,1,2,2> E1 ## E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,0,0> E1 ## E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,0,1> E1 ## E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,0,2> E1 ## E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,1,0> E1 ## E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,1,1> E1 ## E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,1,2> E1 ## E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,2,0> E1 ## E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,2,1> E1 ## E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 1,2,2,2> E1 ## E2 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,0,0> E2 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,0,1> E2 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,0,2> E2 ## E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,1,0> E2 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,1,1> E2 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,1,2> E2 ## E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,2,0> E2 ## E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,2,1> E2 ## E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,0,2,2> E2 ## E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,0,0> E2 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,0,1> E2 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,0,2> E2 ## E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,1,0> E2 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,1,1> E2 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,1,2> E2 ## E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,2,0> E2 ## E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,2,1> E2 ## E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,1,2,2> E2 ## E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,0,0> E2 ## E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,0,1> E2 ## E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,0,2> E2 ## E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,1,0> E2 ## E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,1,1> E2 ## E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,1,2> E2 ## E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,2,0> E2 ## E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,2,1> E2 ## E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<4,T, Q, 2,2,2,2> E2 ## E2 ## E2 ## E2; };

#define GLM_SWIZZLE4_2_MEMBERS(T, Q, E0,E1,E2,E3) \
	struct { detail::_swizzle<2,T, Q, 0,0,-1,-2> E0 ## E0; }; \
	struct { detail::_swizzle<2,T, Q, 0,1,-1,-2> E0 ## E1; }; \
	struct { detail::_swizzle<2,T, Q, 0,2,-1,-2> E0 ## E2; }; \
	struct { detail::_swizzle<2,T, Q, 0,3,-1,-2> E0 ## E3; }; \
	struct { detail::_swizzle<2,T, Q, 1,0,-1,-2> E1 ## E0; }; \
	struct { detail::_swizzle<2,T, Q, 1,1,-1,-2> E1 ## E1; }; \
	struct { detail::_swizzle<2,T, Q, 1,2,-1,-2> E1 ## E2; }; \
	struct { detail::_swizzle<2,T, Q, 1,3,-1,-2> E1 ## E3; }; \
	struct { detail::_swizzle<2,T, Q, 2,0,-1,-2> E2 ## E0; }; \
	struct { detail::_swizzle<2,T, Q, 2,1,-1,-2> E2 ## E1; }; \
	struct { detail::_swizzle<2,T, Q, 2,2,-1,-2> E2 ## E2; }; \
	struct { detail::_swizzle<2,T, Q, 2,3,-1,-2> E2 ## E3; }; \
	struct { detail::_swizzle<2,T, Q, 3,0,-1,-2> E3 ## E0; }; \
	struct { detail::_swizzle<2,T, Q, 3,1,-1,-2> E3 ## E1; }; \
	struct { detail::_swizzle<2,T, Q, 3,2,-1,-2> E3 ## E2; }; \
	struct { detail::_swizzle<2,T, Q, 3,3,-1,-2> E3 ## E3; };

#define GLM_SWIZZLE4_3_MEMBERS(T, Q, E0,E1,E2,E3) \
	struct { detail::_swizzle<3, T, Q, 0,0,0,-1> E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 0,0,1,-1> E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 0,0,2,-1> E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 0,0,3,-1> E0 ## E0 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 0,1,0,-1> E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 0,1,1,-1> E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 0,1,2,-1> E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 0,1,3,-1> E0 ## E1 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 0,2,0,-1> E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 0,2,1,-1> E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 0,2,2,-1> E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 0,2,3,-1> E0 ## E2 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 0,3,0,-1> E0 ## E3 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 0,3,1,-1> E0 ## E3 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 0,3,2,-1> E0 ## E3 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 0,3,3,-1> E0 ## E3 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 1,0,0,-1> E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 1,0,1,-1> E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 1,0,2,-1> E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 1,0,3,-1> E1 ## E0 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 1,1,0,-1> E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 1,1,1,-1> E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 1,1,2,-1> E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 1,1,3,-1> E1 ## E1 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 1,2,0,-1> E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 1,2,1,-1> E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 1,2,2,-1> E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 1,2,3,-1> E1 ## E2 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 1,3,0,-1> E1 ## E3 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 1,3,1,-1> E1 ## E3 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 1,3,2,-1> E1 ## E3 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 1,3,3,-1> E1 ## E3 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 2,0,0,-1> E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 2,0,1,-1> E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 2,0,2,-1> E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 2,0,3,-1> E2 ## E0 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 2,1,0,-1> E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 2,1,1,-1> E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 2,1,2,-1> E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 2,1,3,-1> E2 ## E1 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 2,2,0,-1> E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 2,2,1,-1> E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 2,2,2,-1> E2 ## E2 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 2,2,3,-1> E2 ## E2 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 2,3,0,-1> E2 ## E3 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 2,3,1,-1> E2 ## E3 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 2,3,2,-1> E2 ## E3 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 2,3,3,-1> E2 ## E3 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 3,0,0,-1> E3 ## E0 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 3,0,1,-1> E3 ## E0 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 3,0,2,-1> E3 ## E0 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 3,0,3,-1> E3 ## E0 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 3,1,0,-1> E3 ## E1 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 3,1,1,-1> E3 ## E1 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 3,1,2,-1> E3 ## E1 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 3,1,3,-1> E3 ## E1 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 3,2,0,-1> E3 ## E2 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 3,2,1,-1> E3 ## E2 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 3,2,2,-1> E3 ## E2 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 3,2,3,-1> E3 ## E2 ## E3; }; \
	struct { detail::_swizzle<3, T, Q, 3,3,0,-1> E3 ## E3 ## E0; }; \
	struct { detail::_swizzle<3, T, Q, 3,3,1,-1> E3 ## E3 ## E1; }; \
	struct { detail::_swizzle<3, T, Q, 3,3,2,-1> E3 ## E3 ## E2; }; \
	struct { detail::_swizzle<3, T, Q, 3,3,3,-1> E3 ## E3 ## E3; };

#define GLM_SWIZZLE4_4_MEMBERS(T, Q, E0,E1,E2,E3) \
	struct { detail::_swizzle<4, T, Q, 0,0,0,0> E0 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,0,1> E0 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,0,2> E0 ## E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,0,3> E0 ## E0 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,1,0> E0 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,1,1> E0 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,1,2> E0 ## E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,1,3> E0 ## E0 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,2,0> E0 ## E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,2,1> E0 ## E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,2,2> E0 ## E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,2,3> E0 ## E0 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,3,0> E0 ## E0 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,3,1> E0 ## E0 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,3,2> E0 ## E0 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,0,3,3> E0 ## E0 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,0,0> E0 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,0,1> E0 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,0,2> E0 ## E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,0,3> E0 ## E1 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,1,0> E0 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,1,1> E0 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,1,2> E0 ## E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,1,3> E0 ## E1 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,2,0> E0 ## E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,2,1> E0 ## E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,2,2> E0 ## E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,2,3> E0 ## E1 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,3,0> E0 ## E1 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,3,1> E0 ## E1 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,3,2> E0 ## E1 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,1,3,3> E0 ## E1 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,0,0> E0 ## E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,0,1> E0 ## E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,0,2> E0 ## E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,0,3> E0 ## E2 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,1,0> E0 ## E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,1,1> E0 ## E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,1,2> E0 ## E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,1,3> E0 ## E2 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,2,0> E0 ## E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,2,1> E0 ## E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,2,2> E0 ## E2 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,2,3> E0 ## E2 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,3,0> E0 ## E2 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,3,1> E0 ## E2 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,3,2> E0 ## E2 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,2,3,3> E0 ## E2 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,0,0> E0 ## E3 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,0,1> E0 ## E3 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,0,2> E0 ## E3 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,0,3> E0 ## E3 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,1,0> E0 ## E3 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,1,1> E0 ## E3 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,1,2> E0 ## E3 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,1,3> E0 ## E3 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,2,0> E0 ## E3 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,2,1> E0 ## E3 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,2,2> E0 ## E3 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,2,3> E0 ## E3 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,3,0> E0 ## E3 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,3,1> E0 ## E3 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,3,2> E0 ## E3 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 0,3,3,3> E0 ## E3 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,0,0> E1 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,0,1> E1 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,0,2> E1 ## E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,0,3> E1 ## E0 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,1,0> E1 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,1,1> E1 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,1,2> E1 ## E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,1,3> E1 ## E0 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,2,0> E1 ## E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,2,1> E1 ## E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,2,2> E1 ## E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,2,3> E1 ## E0 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,3,0> E1 ## E0 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,3,1> E1 ## E0 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,3,2> E1 ## E0 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,0,3,3> E1 ## E0 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,0,0> E1 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,0,1> E1 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,0,2> E1 ## E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,0,3> E1 ## E1 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,1,0> E1 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,1,1> E1 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,1,2> E1 ## E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,1,3> E1 ## E1 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,2,0> E1 ## E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,2,1> E1 ## E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,2,2> E1 ## E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,2,3> E1 ## E1 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,3,0> E1 ## E1 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,3,1> E1 ## E1 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,3,2> E1 ## E1 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,1,3,3> E1 ## E1 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,0,0> E1 ## E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,0,1> E1 ## E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,0,2> E1 ## E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,0,3> E1 ## E2 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,1,0> E1 ## E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,1,1> E1 ## E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,1,2> E1 ## E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,1,3> E1 ## E2 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,2,0> E1 ## E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,2,1> E1 ## E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,2,2> E1 ## E2 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,2,3> E1 ## E2 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,3,0> E1 ## E2 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,3,1> E1 ## E2 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,3,2> E1 ## E2 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,2,3,3> E1 ## E2 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,0,0> E1 ## E3 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,0,1> E1 ## E3 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,0,2> E1 ## E3 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,0,3> E1 ## E3 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,1,0> E1 ## E3 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,1,1> E1 ## E3 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,1,2> E1 ## E3 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,1,3> E1 ## E3 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,2,0> E1 ## E3 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,2,1> E1 ## E3 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,2,2> E1 ## E3 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,2,3> E1 ## E3 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,3,0> E1 ## E3 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,3,1> E1 ## E3 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,3,2> E1 ## E3 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 1,3,3,3> E1 ## E3 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,0,0> E2 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,0,1> E2 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,0,2> E2 ## E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,0,3> E2 ## E0 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,1,0> E2 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,1,1> E2 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,1,2> E2 ## E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,1,3> E2 ## E0 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,2,0> E2 ## E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,2,1> E2 ## E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,2,2> E2 ## E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,2,3> E2 ## E0 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,3,0> E2 ## E0 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,3,1> E2 ## E0 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,3,2> E2 ## E0 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,0,3,3> E2 ## E0 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,0,0> E2 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,0,1> E2 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,0,2> E2 ## E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,0,3> E2 ## E1 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,1,0> E2 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,1,1> E2 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,1,2> E2 ## E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,1,3> E2 ## E1 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,2,0> E2 ## E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,2,1> E2 ## E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,2,2> E2 ## E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,2,3> E2 ## E1 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,3,0> E2 ## E1 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,3,1> E2 ## E1 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,3,2> E2 ## E1 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,1,3,3> E2 ## E1 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,0,0> E2 ## E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,0,1> E2 ## E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,0,2> E2 ## E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,0,3> E2 ## E2 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,1,0> E2 ## E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,1,1> E2 ## E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,1,2> E2 ## E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,1,3> E2 ## E2 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,2,0> E2 ## E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,2,1> E2 ## E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,2,2> E2 ## E2 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,2,3> E2 ## E2 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,3,0> E2 ## E2 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,3,1> E2 ## E2 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,3,2> E2 ## E2 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,2,3,3> E2 ## E2 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,0,0> E2 ## E3 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,0,1> E2 ## E3 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,0,2> E2 ## E3 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,0,3> E2 ## E3 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,1,0> E2 ## E3 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,1,1> E2 ## E3 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,1,2> E2 ## E3 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,1,3> E2 ## E3 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,2,0> E2 ## E3 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,2,1> E2 ## E3 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,2,2> E2 ## E3 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,2,3> E2 ## E3 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,3,0> E2 ## E3 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,3,1> E2 ## E3 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,3,2> E2 ## E3 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 2,3,3,3> E2 ## E3 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,0,0> E3 ## E0 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,0,1> E3 ## E0 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,0,2> E3 ## E0 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,0,3> E3 ## E0 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,1,0> E3 ## E0 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,1,1> E3 ## E0 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,1,2> E3 ## E0 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,1,3> E3 ## E0 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,2,0> E3 ## E0 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,2,1> E3 ## E0 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,2,2> E3 ## E0 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,2,3> E3 ## E0 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,3,0> E3 ## E0 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,3,1> E3 ## E0 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,3,2> E3 ## E0 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,0,3,3> E3 ## E0 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,0,0> E3 ## E1 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,0,1> E3 ## E1 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,0,2> E3 ## E1 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,0,3> E3 ## E1 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,1,0> E3 ## E1 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,1,1> E3 ## E1 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,1,2> E3 ## E1 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,1,3> E3 ## E1 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,2,0> E3 ## E1 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,2,1> E3 ## E1 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,2,2> E3 ## E1 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,2,3> E3 ## E1 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,3,0> E3 ## E1 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,3,1> E3 ## E1 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,3,2> E3 ## E1 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,1,3,3> E3 ## E1 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,0,0> E3 ## E2 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,0,1> E3 ## E2 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,0,2> E3 ## E2 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,0,3> E3 ## E2 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,1,0> E3 ## E2 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,1,1> E3 ## E2 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,1,2> E3 ## E2 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,1,3> E3 ## E2 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,2,0> E3 ## E2 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,2,1> E3 ## E2 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,2,2> E3 ## E2 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,2,3> E3 ## E2 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,3,0> E3 ## E2 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,3,1> E3 ## E2 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,3,2> E3 ## E2 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,2,3,3> E3 ## E2 ## E3 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,0,0> E3 ## E3 ## E0 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,0,1> E3 ## E3 ## E0 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,0,2> E3 ## E3 ## E0 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,0,3> E3 ## E3 ## E0 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,1,0> E3 ## E3 ## E1 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,1,1> E3 ## E3 ## E1 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,1,2> E3 ## E3 ## E1 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,1,3> E3 ## E3 ## E1 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,2,0> E3 ## E3 ## E2 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,2,1> E3 ## E3 ## E2 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,2,2> E3 ## E3 ## E2 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,2,3> E3 ## E3 ## E2 ## E3; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,3,0> E3 ## E3 ## E3 ## E0; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,3,1> E3 ## E3 ## E3 ## E1; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,3,2> E3 ## E3 ## E3 ## E2; }; \
	struct { detail::_swizzle<4, T, Q, 3,3,3,3> E3 ## E3 ## E3 ## E3; };