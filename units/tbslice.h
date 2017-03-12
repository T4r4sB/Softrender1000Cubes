#pragma once

#include "assert.h"
#include "stdint.h"

namespace tblib
{
	template <typename T>
	struct DefaultComparator
	{
		bool operator () (const T& left, const T& right)
		{
			return left<right;
		}
	};

	/*
	template <typename T>
	struct RemoveConst;// { typedef T type; };
	template <typename T>
	struct RemoveConst <const T> { typedef T type; };
*/

	template <typename T>
	class Slice
	// отрезок чужого массива, за целостность не отвечает
	{
		template <typename Comp>
		void ShiftDown (Comp comp, int i, int h) const
		{
			int left = 2*i+1-m_low, right=left+1;

			for (int maxChild=left; maxChild<h; maxChild=left)
			{
				if (right<h && comp(m_begin[left],m_begin[right]))
					maxChild=right;

				if (comp(m_begin[i], m_begin[maxChild]))
					Swap(m_begin[i], m_begin[maxChild]);
				else
					break;

				i     = maxChild;
				left  = 2*i+1-m_low;
				right = left+1;
			}
		}
	protected :
		T* m_begin;
		int m_low,m_high;
	public :
		typedef T value_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef T* iterator;
		typedef T* reverse_iterator;
		typedef const T* const_iterator;
		typedef const T* const_reverse_iterator;

		Slice () : m_begin(NULL),m_low(0),m_high(0) {}
		Slice (T* first_elem, int low, int high) : m_begin(first_elem-low),m_low(low),m_high(high) {}

		template <typename U>
		Slice (const Slice <U> &sl) : m_begin (sl.begin()-sl.Low()), m_low(sl.Low()), m_high(sl.High()) {}
    //Slice <typename RemoveConst<t>::type > студия не жрёт

		Slice (const Slice& sl) : m_begin(sl.m_begin), m_low(sl.m_low), m_high(sl.m_high) {}

		reference operator [] (int index) const
		{
			assert(index>=m_low && index<m_high);
			return m_begin[index];
		}

		iterator begin () const
		{
			return iterator(m_begin + m_low);
		}

		iterator end () const
		{
			return iterator(m_begin + m_high);
		}		

		reverse_iterator rbegin () const
		{
			return reverse_iterator(m_begin + m_high-1);
		}

		reverse_iterator rend () const
		{
			return reverse_iterator(m_begin + m_low-1);
		}		

		reference front () const
		{
			return m_begin[m_low];
		}
		
		reference back () const
		{
			return m_begin[m_high-1];
		}    

		int Low () const
		{
			return m_low;
		}

		int High () const
		{
			return m_high;
		}	

		int Size () const
		{
			return m_high-m_low;
		}

		Slice<T> GetSlice(int low, int high) const
		{
			assert (low>=m_low && high<=m_high);
			return tblib::Slice<T>(m_begin+low, low, high);
		}


	public :
		template <typename Comp>
		void Sort (Comp comp) const
		{
			for (int i = (m_low+m_high)/2-1; i>=m_low; --i)
			{
				ShiftDown (comp, i, m_high);
			}
			
			for (int i = m_high-1; i>m_low; --i)
			{
				Swap (m_begin[m_low], m_begin[i]); 
				ShiftDown(comp, m_low, i);
			}
		}

		void Sort () const
		{
			Sort (DefaultComparator<T>());
		}

	};
}