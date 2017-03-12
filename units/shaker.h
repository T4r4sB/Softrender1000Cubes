#pragma once
#include "assert.h"

namespace std
{
	template <int size> 
	class shaker
	{
		struct Element
		{
			int  index;
			bool good;
		};

		Element elements [size];
		int     indexes  [size];
		int     count;

	public:
		shaker()
		{
			for (int i=0; i<size; ++i)
				elements [i].good = false;
			count = 0;
		}

		void add(int n)
		{
			assert (n>=0 && n<size);
			if (!elements[n].good)
			{
				elements[n].good  = true;
				elements[n].index = count;
				indexes[count] = n;
				++count;
			}
		}

		bool has (int n)
		{
			assert (n>=0 && n<size);
			return elements[n].good;
		}

		void del(int n)
		{
			assert (n>=0 && n<size);
			if (elements[n].good)
			{
				elements[n].good = false;
				int i  = elements[n].index;
				int ln = indexes[count-1];
				elements[ln].index = i;
				indexes[i] = ln;
				--count;
			}
		}

		void change(int n, bool newCond)
		{
			newCond ? add(n) : del(n);
		}

		int getRand()
		{
			return count ? indexes[rand()%count] : -1;
		}
	};
};