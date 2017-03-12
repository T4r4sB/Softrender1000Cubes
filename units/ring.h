#pragma once
#include "tbarr.h"

namespace tblib
{
	template <typename T, int N>
	class Ring
	{
		tblib::CArray<T, N> elements;
		unsigned int b,e;
	public:
		Ring() : b(0), e(0) {}

		bool Empty () {return b==e;}
		
		void Push(const T& t)
		{
			elements[b] = t;
			b = (b+1)%N;
			assert (!Empty());
		}

		T Pop()
		{
			assert (!Empty());
			int oe = e;
			e = (e+1)%N;
			return elements[oe];
		}
	};
};