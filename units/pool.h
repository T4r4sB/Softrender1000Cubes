#pragma once

#include "tbarr.h"

namespace tblib
{
	template <typename T, int N> 
	class Pool : _BaseArray <T,N>
	{
		tblib::CArray <int, N> NextFree;
		int m_size;
		int m_free;
		int m_count;

		int ReserveIndex ()
		{
			assert (!Full());

			int index;
			if (m_free>=0) // есть свободные элементы
			{
				index = m_free;
				m_free = NextFree[index]; // следующий элемент списка
			} else
			{
				index = m_size++; // новый элемент, а свободных всё ещё нет
			}

			NextFree[index] = -2; // он занят
			++m_count;
			return index;
		}		

		Pool (const Pool&);
		Pool& operator=(const Pool&);

	public :
		Pool () : m_size(0), m_free(-1), m_count(0) {}

		int Size() { return m_size; }
		int Count () { return m_count;}
		int Capacity () { return N;}

		bool Full ()
		{
			return m_free<0 && m_size>=N;
		}

		bool Valid (int index) { return index>=0 && index<m_size && NextFree[index]<-1;}
		
		int Push ()
		{ 
			int index = ReserveIndex();
			new (MemBeg()+index) T;
			return index;
		}

		int Push (const T& t)
		{ 
			int index = ReserveIndex();
			new (MemBeg()+index) T(t);
			return index;
		}

		int Emplace ()	
			{	
			int index = ReserveIndex();
			new (MemBeg()+index) T();
			return index; }
			
		template<typename A1>
		int Emplace (const A1&a1)	
			{	
			int index = ReserveIndex();
			new (MemBeg()+index) T(a1);
			return index;	}
			
		template<typename A1,typename A2>
		int Emplace (const A1&a1, const A2&a2)	
			{	
			int index = ReserveIndex();
			new (MemBeg()+index) T(a1,a2);
			return index;}
						
		template<typename A1,typename A2,typename A3>
		int Emplace (const A1&a1, const A2&a2, const A3&a3)	
			{	
			int index = ReserveIndex();
			new (MemBeg()+index) T(a1,a2,a3);
			return index;}
						
		template<typename A1,typename A2,typename A3,typename A4>
		int Emplace (const A1&a1, const A2&a2, const A3&a3, const A4&a4)	
			{	
			int index = ReserveIndex();
			new (MemBeg()+index) T(a1,a2,a3,a4);
			return index;}
						
		template<typename A1,typename A2,typename A3,typename A4,typename A5>
		int Emplace (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5)	
			{	
			int index = ReserveIndex();
			new (MemBeg()+index) T(a1,a2,a3,a4,a5);
			return index;}

		void FreeIndex (int index)
		{
			assert (Valid(index));
			NextFree[index] = m_free; // -1 для последнего инвалида, -2 для свободного
			m_free = index;
			MemBeg()[index].~T();
			--m_count;
		}

		T& operator [] (int index)
		{
			assert (Valid(index));
			return MemBeg()[index];
		}

		const T& operator [] (int index) const
		{
			assert (Valid(index));
			return MemBeg()[index];
		}

		~Pool ()
		{
			for (int i=0; i<Size(); ++i)
				if (Valid(i))
				{
					MemBeg()[i].~T();
				}
		}
	};
};