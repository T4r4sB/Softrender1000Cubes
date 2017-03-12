#include "tbstring.h"

char pat_int   [] = "%d";
char pat_uint  [] = "%u";
char pat_hex   [] = "%x";
char pat_lint  [] = "%Ld";
char pat_luint [] = "%Lu";
char pat_lhex  [] = "%Lx";
char pat_flt   [] = "%0.*f";
char pat_lflt  [] = "%0.*lf";
char pat_dig   [] = "%g";
char pat_ldig  [] = "%lg";
char pat_exp   [] = "%e";
char pat_lexp  [] = "%le";
char pat_ptr   [] = "%p";

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace tblib
{			
	const int STR_SZ = 4;
	
	void Format::PassToNext ()
	{
		bool slash=false;
		++position; // процент перебегаем
		// теперь если не хватит, то это пиздец косяк

		for (; position<input.Size(); ++position)
		{
			if (slash)
			{
				slash = false;
				buf.EmplaceBack(input[position]);
			} else if (input[position]=='\\')
			{
				slash = true;
			}	else if (input[position]=='%')
			{
				position=position;
				break;
			} else
			{						
				buf.EmplaceBack(input[position]);
			}
		}

		if (Complete())
		{				
			buf.EmplaceBack(char(0));
		}
	}

	template <const char* pat, typename T>
	Format& Format::Arg(T x, int width0, int width)
	{
		assert (!Complete());
		tblib::Shrink(width, 256);
		tblib::Shrink(width0, 256);
		tblib::Enlarge(width, width0);


		if (width<=1)
		{
			int s = buf.Size();
			int l = 0;

			for (int addsz = STR_SZ; s+addsz<0x100000; addsz*=2)
			{
				int new_size = s + addsz;
				assert (new_size>s);	
				buf.Resize(new_size);			
				char* ptr = &buf[s];
				l = snprintf(ptr, addsz, pat, x);
				if (l>0 && l<addsz)
					break;
			}
			buf.Resize(s + l);

		} else
		{
			Vector<char> tmp;
			int s = buf.Size();
			int l = 0;

			for (int addsz = STR_SZ; s+addsz<0x100000; addsz*=2)
			{
				tmp.Resize(addsz);
				l = sprintf(&tmp[0], pat, x);
				if (l>0 && l<addsz)
					break;
			}

			int w = Max(l, width), w0 = Max(l, width0);

			int new_size = s + w;
			assert (new_size>s);
			buf.Resize(new_size);

			char* ptr = &buf[s];
			memset(ptr,      ' ', w-w0);
			memset(ptr+w-w0, '0', w0-l);
			memcpy(ptr+w-l,  &tmp[0], l);
		}

		PassToNext ();		
		return *this;
	}
	
	template <const char* pat, typename T>
	Format& Format::ArgDig(T x, int width0, int digits, int width)
	{
		assert (!Complete());
		tblib::Shrink(width, 256);
		tblib::Shrink(width0, 256);
		tblib::Enlarge(width, width0);

		if (width<=1)
		{
			int s = buf.Size();
			int l = 0;

			for (int addsz = STR_SZ; s+addsz<0x100000; addsz*=2)
			{
				int new_size = s + addsz;
				assert (new_size>s);	
				buf.Resize(new_size);			
				char* ptr = &buf[s];
				l = snprintf(ptr, addsz, pat, digits, x);
				if (l>0 && l<addsz)
					break;
			}
			buf.Resize(s + l);			

		} else
		{
			Vector<char> tmp;
			int s = buf.Size();
			int l = 0;
			for (int addsz = STR_SZ; s+addsz<0x100000; addsz*=2)
			{
				tmp.Resize(addsz);
				l = sprintf(&tmp[0], pat, digits, x);
				if (l>0 && l<addsz)
					break;
			}

			int w = Max(l, width), w0 = Max(l, width0);

			int new_size = s + w;
			assert (new_size>s);
			buf.Resize(new_size);

			char* ptr = &buf[s];
			memset(ptr,      ' ', w-w0);
			memset(ptr+w-w0, '0', w0-l);
			memcpy(ptr+w-l,  &tmp[0], l);
		}

		PassToNext ();		
		return *this;
	}
	
	Format& Format::s(StringRef c, int width)
	{
		assert (!Complete());
		if (c.Size()>0)
		{
			int l = c.Size();
			int s = buf.Size();
			int w = Max(l, width);
			buf.Resize(buf.Size()+w);
			char* ptr = &buf[s];
			memset(ptr, ' ', w-l);
			if (c.Size()>0)
				memcpy(ptr+w-l, c.begin(), l);
		}
		PassToNext ();		
		return *this;
	}

	Format& Format::i(int x, int width)
	{
		return Arg<pat_int> (x, 0, width);
	}
	
	Format& Format::u(unsigned int x, int width)
	{
		return Arg<pat_uint> (x, 0, width);
	}
	
	Format& Format::h(unsigned int x, int width)
	{
		return Arg<pat_hex> (x, 0, width);
	}
		/*
	Format& Format::li(long long x, int width)		
	{
		return Arg<pat_lint> (x, 0, width);
	}

	Format& Format::lu(unsigned long long x, int width)	
	{
		return Arg<pat_luint> (x, 0, width);
	}

	Format& Format::lh(unsigned long long x, int width)
	{
		return Arg<pat_lhex> (x, 0, width);
	}	*/
	
	Format& Format::f(float x, int digits, int width)		
	{
		return ArgDig<pat_flt> (x, 0, digits, width);
	}			
	
	Format& Format::f(double x, int digits, int width)		
	{
		return ArgDig<pat_lflt> (x, 0, digits, width);
	}			
	
	Format& Format::g(float x, int width)		
	{
		return Arg<pat_dig> (x, 0, width);
	}			
	
	Format& Format::g(double x, int width)		
	{
		return Arg<pat_ldig> (x, 0, width);
	}			
	
	Format& Format::p(void* x, int width)
	{
		return Arg<pat_ptr> (x, 0, width);
	}	



	Format& Format::i0(int x, int width0, int width)
	{
		return Arg<pat_int> (x, width0, width);
	}
	
	Format& Format::u0(unsigned int x, int width0, int width)
	{
		return Arg<pat_uint> (x, width0, width);
	}
	
	Format& Format::h0(unsigned int x, int width0, int width)
	{
		return Arg<pat_hex> (x, width0, width);
	}	
		/*
	Format& Format::li0(long long x, int width0, int width)		
	{
		return Arg<pat_lint> (x, width0, width);
	}

	Format& Format::lu0(unsigned long long x, int width0, int width)	
	{
		return Arg<pat_luint> (x, width0, width);
	}

	Format& Format::lh0(unsigned long long x, int width0, int width)
	{
		return Arg<pat_lhex> (x, width0, width);
	}	*/
	
	Format& Format::f0(float x, int width0, int digits, int width)		
	{
		return ArgDig<pat_flt> (x, width0, digits, width);
	}	
	
	Format& Format::f0(double x, int width0, int digits, int width)		
	{
		return ArgDig<pat_lflt> (x, width0, digits, width);
	}						
	
	Format& Format::g0(float x, int width0, int width)		
	{
		return Arg<pat_dig> (x, width0, width);
	}				
	
	Format& Format::g0(double x, int width0, int width)		
	{
		return Arg<pat_ldig> (x, width0, width);
	}			
	
	Format& Format::p0(void* x, int width0, int width)
	{
		return Arg<pat_ptr> (x, width0, width);
	}
	
	template <const char* pat, typename T>
	bool StrTo (StringRef c, T& t)
	{			
		if (c.Size()>0)
		{
			return sscanf(StrBuf(c), pat, &t)==1; 
		} else
		{
			return false;
		}
	}

	bool StrToI (StringRef c, int& x)
	{
		return StrTo <pat_int> (c, x);
	}	

	bool StrToU (StringRef c, unsigned int& x)
	{
		return StrTo <pat_uint> (c, x);
	}	

	bool StrToF (StringRef c, float& x)
	{
		return StrTo <pat_flt> (c, x);
	}

	bool StrToF (StringRef c, double& x)
	{
		return StrTo <pat_lflt> (c, x);
	}
};