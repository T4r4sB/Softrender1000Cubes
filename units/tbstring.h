#pragma once

#include "tbstream.h"		
#include "tbdynarr.h"

namespace tblib
{	
	inline const OFStream& operator << (const OFStream& s, StringRef c)
	{
		//s << c.Size();
		for (int i=c.Low(); i<c.High(); ++i)
			s << c[i];
		return s;
	}	
		
	class StrBuf
	{
		inline int BufLen (const char* c)
		{
			if (c==NULL)
				return 0;
			int l = int(strlen(c));
			if (l==0)
				return 0;
			return l+1;
		}
	protected :
		tblib::Vector<char> buf;
		inline StrBuf (int length) : buf(length) {}

	public :
		StrBuf () {}

		enum BinaryMarker 
		{
			BINARY
		};

		template <typename T>
		inline StrBuf (BinaryMarker bm, const T* t, int count) : buf (int(sizeof(T)*count>0 ? sizeof(T)*count+1 : 0))
		{
			(void)bm;
			if (buf.Capacity())
			{
				buf.Resize(buf.Capacity());
				memcpy(buf.begin(), t, sizeof(T)*count);
				buf.back() = 0;
			}
		}

		inline StrBuf (StringRef c) : buf(c.Size()>0 ? c.Size()+1 : 0) 
		{
			if (buf.Capacity())
			{
				buf.Resize(buf.Capacity());
				memcpy(buf.begin(), c.begin(), c.Size());
				buf.back() = 0;
			}
		}
		
		inline StrBuf (const char* c) : buf (BufLen(c))
		{
			buf.Resize(buf.Capacity());
			if (buf.Size())
				memcpy(buf.begin(), c, buf.Size());
		}

		inline void Append (StringRef c)
		{
			if (c.Size()<=0)
				return;
			int s = tblib::Max(0,buf.Size()-1);
			buf.Resize(s+c.Size()+1);
			memcpy(&buf[s], c.begin(), c.Size());
			buf.back()=0;
		}

		inline int Length ()
		{
			return buf.Size() ? buf.Size()-1 : 0; 
		}

		StringRef str() const
		{
			if (buf.Size()>0)
        return StringRef (buf.begin(), 0, buf.Size()-1);
			else
				return StringRef (NULL, 0, 0);
		}

		inline operator const char* () const
		{			
			if (buf.Size()>0)
        return buf.begin();
			else
				return NULL;
		}

		inline void MoveFrom_ (StrBuf& from)
		{
			buf.MoveFrom_(from.buf);
		}

		inline StringRef GetSlice (int low, int high)
		{
			return buf.GetSlice (low, high);
		}
		
		inline StringRef GetSlice ()
		{
			return buf.GetSlice (0, Length());
		}

		inline StrBuf (const IFStream& f)
		{
			int Size = tblib::To<int>::From(f);
			if (Size)
			{
				buf.Resize(Size+1);
				for (int i=0; i<Size; ++i)
					buf[i] = tblib::To<char>::From(f);
				buf.back() = 0;
			}
		}
		
		inline friend const OFStream& operator << (const OFStream& s, const StrBuf& b)
		{
			if (b.buf.Size())
			{
				s << b.buf.Size()-1;
				for (int i=0; i<b.buf.Size()-1; ++i)
					s << b.buf[i];
			} else
			{
				s << 0;
			}
			return s;
		}
	};

	class Format : public StrBuf
	{
		HeapCArray<char> input;
		int position;

		inline bool Complete () const
		{
			return position == input.Size();
		}

		void PassToNext ();

		template <const char* pat, typename T>
		Format& Arg(T x, int width0, int width);
		
		template <const char* pat, typename T>
		Format& ArgDig(T x, int width0, int digits, int width);

	public :
		inline Format (StringRef str) : StrBuf(256), input(str), position(-1)
		{
			PassToNext ();			
		}

		Format& s(StringRef c, int width=0);
		
		Format& i(int x                  , int width=0);		
		Format& u(unsigned int x         , int width=0);		
		Format& h(unsigned int x         , int width=0);		
		/*
		Format& li(long long x           , int width=0);		
		Format& lu(unsigned long long  x , int width=0);		
		Format& lh(unsigned long long  x , int width=0);	
		*/
		Format& f(float x                , int digits, int width=0);		
		Format& g(float x                , int width=0);		
		Format& f(double x               , int digits, int width=0);		
		Format& g(double x               , int width=0);		
		Format& p(void* x                , int width=0);
		
		Format& i0(int x                  , int width0, int width=0);		
		Format& u0(unsigned int x         , int width0, int width=0);		
		Format& h0(unsigned int x         , int width0, int width=0);		
		/*
		Format& li0(long long x           , int width0, int width=0);		
		Format& lu0(unsigned long long  x , int width0, int width=0);		
		Format& lh0(unsigned long long  x , int width0, int width=0);
		*/
		Format& f0(float x                , int width0, int digits, int width=0);		
		Format& g0(float x                , int width0, int width=0);		
		Format& f0(double x               , int width0, int digits, int width=0);		
		Format& g0(double x               , int width0, int width=0);		
		Format& p0(void* x                , int width0, int width=0);

		inline operator const char* () const
		{
			assert (Complete());
			return position ? &buf[0] : NULL;
		}
		
		StringRef str() const
		{
			assert (Complete());
			return StringRef(buf.begin(), 0, tblib::Max(buf.Size()-1, 0));
		}

		inline friend const OFStream& operator << (const OFStream& s, const Format& f)
		{
			assert (f.Complete());
			if (f.buf.Size())
			{
				//s << f.buf.Size()-1;
				for (int i=0; i<f.buf.Size()-1; ++i)
					s << f.buf[i];
			} else
			{
				s << 0;
			}
			return s;
		}
	};	

	bool StrToI (StringRef c, int& x);
	bool StrToU (StringRef c, unsigned int& x);
	bool StrToF (StringRef c, float& x);
	bool StrToF (StringRef c, double& x);
	bool StrToLI (StringRef c, long long int& x);
	bool StrToLU (StringRef c, unsigned long long& x);
};