namespace glm{
namespace detail
{
	GLM_FUNC_QUALIFIER float overflow()
	{
		volatile float f = 1e10;

		for(int i = 0; i < 10; ++i)
			f = f * f;
		return f;
	}

	union uif32
	{
		GLM_FUNC_QUALIFIER uif32() :
			i(0)
		{}

		GLM_FUNC_QUALIFIER uif32(float f_) :
			f(f_)
		{}

		GLM_FUNC_QUALIFIER uif32(unsigned int i_) :
			i(i_)
		{}

		float f;
		unsigned int i;
	};

	GLM_FUNC_QUALIFIER float toFloat32(hdata value)
	{
		int s = (value >> 15) & 0x00000001;
		int e = (value >> 10) & 0x0000001f;
		int m =  value        & 0x000003ff;

		if(e == 0)
		{
			if(m == 0)
			{

				detail::uif32 result;
				result.i = static_cast<unsigned int>(s << 31);
				return result.f;
			}
			else
			{

				while(!(m & 0x00000400))
				{
					m <<= 1;
					e -=  1;
				}

				e += 1;
				m &= ~0x00000400;
			}
		}
		else if(e == 31)
		{
			if(m == 0)
			{

				uif32 result;
				result.i = static_cast<unsigned int>((s << 31) | 0x7f800000);
				return result.f;
			}
			else
			{

				uif32 result;
				result.i = static_cast<unsigned int>((s << 31) | 0x7f800000 | (m << 13));
				return result.f;
			}
		}

		e = e + (127 - 15);
		m = m << 13;

		uif32 Result;
		Result.i = static_cast<unsigned int>((s << 31) | (e << 23) | m);
		return Result.f;
	}

	GLM_FUNC_QUALIFIER hdata toFloat16(float const& f)
	{
		uif32 Entry;
		Entry.f = f;
		int i = static_cast<int>(Entry.i);

		int s =  (i >> 16) & 0x00008000;
		int e = ((i >> 23) & 0x000000ff) - (127 - 15);
		int m =   i        & 0x007fffff;

		if(e <= 0)
		{
			if(e < -10)
			{
				return hdata(s);
			}


			m = (m | 0x00800000) >> (1 - e);

			if(m & 0x00001000)
				m += 0x00002000;

			return hdata(s | (m >> 13));
		}
		else if(e == 0xff - (127 - 15))
		{
			if(m == 0)
			{
				return hdata(s | 0x7c00);
			}
			else
			{
				m >>= 13;

				return hdata(s | 0x7c00 | m | (m == 0));
			}
		}
		else
		{

			if(m &  0x00001000)
			{
				m += 0x00002000;

				if(m & 0x00800000)
				{
					m =  0;
					e += 1;
				}
			}

			if (e > 30)
			{
				overflow();

				return hdata(s | 0x7c00);
			}

			return hdata(s | (e << 10) | (m >> 13));
		}
	}

}
}