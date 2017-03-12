#pragma once
#include "tbstream.h"

namespace tblib
{	
	template <typename T>
	class Container_ : public T 
	{
		typedef typename T::value_type vt; // для жосиси
		typedef typename T::iterator titer;
		typedef typename T::const_iterator tciter;
		typedef typename T::reverse_iterator triter;

		typename T::iterator ReserveOne () 
		{
			int o_size = this->Size();
			titer result = this->PrepareSize(o_size+1);
			assert (Size()==o_size+1);
			return result;
		}

	public :
		void  EmplaceBack ()	
			{	new (ReserveOne()) vt;	}
		template<typename A1> void  EmplaceBack (const A1&a1) 
			{	new (ReserveOne()) vt(a1);		}
		template<typename A1, typename A2> void  EmplaceBack (const A1&a1, const A2&a2) 
			{	new (ReserveOne()) vt(a1,a2);		}
		template<typename A1, typename A2, typename A3> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3) 
			{	new (ReserveOne()) vt(a1,a2,a3);		}
		template<typename A1, typename A2, typename A3, typename A4> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4) 
			{	new (ReserveOne()) vt(a1,a2,a3,a4);		}
		template<typename A1, typename A2, typename A3, typename A4, typename A5> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5) 
			{	new (ReserveOne()) vt(a1,a2,a3,a4,a5);		}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6) 
			{	new (ReserveOne()) vt(a1,a2,a3,a4,a5,a6);		}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6, const A7& a7) 
			{	new (ReserveOne()) vt(a1,a2,a3,a4,a5,a6,a7);		}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6, const A7&a7, const A8&a8) 
			{	new (ReserveOne()) vt(a1,a2,a3,a4,a5,a6,a7,a8);		}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6, const A7&a7, const A8&a8, const A9& a9) 
			{	new (ReserveOne()) vt(a1,a2,a3,a4,a5,a6,a7,a8,a9);		}
			
		Container_ (const IFStream& s)
		{
			int new_size = tblib::To<int>::From(s);
			this->PrepareSize(new_size);
			for (titer it = this->begin(); it != this->end(); ++it)
				new (it) vt(tblib::To<vt>::From(s));
		}

		friend const OFStream& operator << (const OFStream& s, const Container_<T>& t)
		{
      s << t.Size();
			for (tciter it = t.begin(); it!=t.end(); ++it)
				s << *it;
			return s;
		}		

		template <typename U>
		Container_(const U& u)
		{	
			this->PrepareSize (u.Size());

			titer dst = this->begin();
			typename U::const_iterator src = u.begin();

			while (src != u.end())
			{
				new ((vt*)(dst)) vt(*src);
				++dst;
				++src;
			}
		}

		template <typename U>
		Container_& operator = (const U& u)
		{

			if (u.Size()<this->Size())
				Shrink(u.Size());
			
			titer dst = this->begin();
			typename U::const_iterator src = u.begin();

			while (dst != this->end())
			{
				*dst = *src;
				++dst;
				++src;
			}

			dst = this->PrepareSize (u.Size());

			while (src != u.end())
			{
				new ((vt*)(dst)) vt(*src);
				++dst;
				++src;
			}		
			return *this;
		}

		template <int N>
		Container_ (const typename T::value_type (&e)[N])
		{
			this->PrepareSize (N);

			titer dst = this->begin();
			const vt* src = e;

			while (src != e+N)
			{
				new ((vt*)(dst)) vt(*src);
				++dst;
				++src;
			}
		}
		
		template <int N>
		Container_& operator = (const typename T::value_type (&e) [N])
		{
			if (N < this->Size())
				this->Shrink(N);
			
			titer dst = this->begin();
			const vt* src = e;

			while (dst != this->end())
			{
				*dst = *src;
				++dst;
				++src;
			}

			this->PrepareSize (N);

			while (src != e+N)
			{
				new ((vt*)(dst)) vt(*src);
				++dst;
				++src;
			}		
			return *this;
		}

		void Shrink (int a_size)
		{
			assert (a_size <= this->Size() && a_size>=0);
			triter r = this->rbegin();
			for (int i = this->Size(); i > a_size; --i)
			{
				// этот посредник нужен из-за тупости сраной студии
				((vt*)(r))->~vt();
				++r;
			}
			this->PrepareSize(a_size);
		}

		void Resize (int a_size)
		{
			if (a_size > this->Size())
			{
				for (titer it = this->PrepareSize(a_size); it!=this->end(); ++it)
					new (it) vt;
			} else
			{
				this->Shrink (a_size);
			}
		}

		void Erase (titer it)
		{
			assert (this->Size()>0);
			if (*it==this->back())
			{
				Shrink(this->Size()-1);
			} else
			{
				((vt*)(it))->~vt();
			  Move_(*it, this->back());
				this->PrepareSize(this->Size()-1);
			}
		}

		void EraseByValue (const typename T::value_type& t)
		{
			for (titer it = this->begin(); it != this->end(); ++it)
			{
				if (*it == t)
				{
					Erase(it);
					break;
				}
			}
		}
		
		void EraseDoubles ()
		{
			if (this->Size()<2)
				return;

			int sz=0;
			titer gb = this->begin();

			for (titer it = this->begin(); it != this->end(); ++it)
			{
				bool good = true;
				for (titer it2 = this->begin(); it2 !=gb; ++it2)
					if (*it == *it2)
						{ good =	false; break; }
				if (good)
				{
					Swap(*gb, *it);
					++gb;
					++sz;
				}
			}

			Shrink(sz);
		}

		~Container_ ()
		{			
			Shrink(0);
		}
		
		Container_ () : T() {}
	};
	

#define CONSTRUCTORS(NAME,BASE)\
	NAME () {}\
	\
	template <typename A1>\
	NAME (const A1& a1) : BASE(a1) {}\
	\
	NAME (const NAME& a1) : BASE(a1) {}\
	\
	template <typename A1, typename A2>\
	NAME (const A1& a1, const A2& a2) : BASE(a1, a2) {}\
	\
	template <typename A1>\
	NAME& operator = (const A1& a1) {\
		return static_cast<NAME&> (BASE::operator =(a1));\
	}\
	\
	NAME& operator = (const NAME& a1) {\
		if (this==&a1)\
			return *this;\
		else\
			return static_cast<NAME&> (BASE::operator =(a1));\
	}
	
#define CONTAINER_CONSTRUCTORS(NAME,BASE) CONSTRUCTORS(NAME,Container_<BASE >)

	template <typename T>
	class _Range 
	{
		T m_begin, m_end;
	public :
		typedef T const_iterator;
		_Range (T m_begin, T m_end) : m_begin(m_begin), m_end(m_end) {}
		T begin () const { return m_begin; }
		T end   () const { return m_end; }
		int Size () const { return int(m_end-m_begin); }
	};
	
	template <typename T>
	_Range<T> Range(T m_begin, T m_end) 
	{
		return _Range<T> (m_begin, m_end);
	}
}
