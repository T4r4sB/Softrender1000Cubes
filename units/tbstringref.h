#pragma once

#include "tbslice.h"
#include "string.h"
		
namespace tblib
{	
	typedef Slice<const char> BaseStringRef;

	class StringRef : public BaseStringRef
	{
	public :
		StringRef ()  {}
		StringRef (const BaseStringRef& bs) 
			: BaseStringRef(bs) {}
		StringRef (const char* m_begin, int m_low, int m_high) 
			: BaseStringRef(m_begin, m_low, m_high) {} 
		StringRef (const Slice<char>& bs) 
			: BaseStringRef(bs.begin(), bs.Low(), bs.High()) {}
		StringRef (const char* c) 
			: BaseStringRef(c, 0, c ? int(strlen(c)) : 0 ) {}
	};	
		
	inline bool operator == (StringRef sl, const char* c)
	{
		for (int i=sl.Low(); i<sl.High(); ++i)
			if (sl[i] != c[i-sl.Low()])
				return false;

		return c[sl.High()-sl.Low()] == 0;	
	}
}
