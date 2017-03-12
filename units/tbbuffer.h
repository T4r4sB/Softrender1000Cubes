#pragma once
#include "container.h"
#include "tbdynarr.h"
#include "tballoc.h"

namespace tblib
{
	template <typename T>
	class TriangleBuffer_ 
	{
		typedef HeapArray_<T> Block;

		static const int 
			LN_MIN_BLOCK = 6,
			MIN_BLOCK = 1<<LN_MIN_BLOCK;

		Array <Block, 28> m_data;
		// блоки только управляют памятью, а не самими объектами
		// в первом блоке MIN_BLOCK-1 элемент, во втором MIN_BLOCK, 
		// в каждом следующем вдвое больше, чем в предыдущем
		int m_size;
		
		template <typename HOST, typename U, typename Block>
		class TriangleIterator_
		{
			HOST* host;
			Block* cur;
			U* u;
			TriangleIterator_ (HOST* host, Block* cur, U* u) : host(host), cur(cur), u(u) {}


			int GetIndex ()
			{
				int bi = cur - host->m_data.begin();
				assert (bi>=0);
				if (bi==0) 
					return u-cur->begin();
				else
					return (MIN_BLOCK << (bi-1)) + (u-cur->begin() - 1);
			}

			void Incr (int n)
			{
				assert (n>=0);
				u+=n;
				if (u<cur->begin()+cur->Capacity)	return;	// остались в текущем блоке

				int index = GetIndex ();
				if (index>=host->Size())
				{
          *this = host->end();
					return;
				}

				int nblock, inblock;
				host->GetBlock(index, nblock, inblock);
				cur = host->m_data.begin() + nblock;
				u = cur.begin() + inblock;
			}

			void Decr (int n)
			{
				assert (n>=0);
				u-=n;
				if (u>=cur->begin()) return; // остались в текущем блоке

				int index = GetIndex ();
				if (index<0)
				{
					*this = host->rend();
					return;
				}

				int nblock, inblock;
				host->GetBlock(index, nblock, inblock);
				cur = host->m_data.begin() + nblock;
				u = cur.begin() + inblock;
			}

			void MoveUp ()
			{
				assert (*this != host->end());
				assert (*this != host->begin());

				int nblock, inblock;
				host->GetBlock(index, nblock, inblock);

				if (nblock==0)
				{
					inblock = (inblock-1)/2;
				} else if (nblock==1)
				{
					inblock = (inblock+MINBLOCK-2)/2;
					--nblock;
				} else
				{
					inblock = inblock/2;
					--nblock;
				}
				
				cur = host->m_data.begin() + nblock;
				u = cur.begin() + inblock;        
			}

			void MoveDown (int branch)
			{
				assert (*this != host->end());
				assert (*this != host->begin());

				int nblock, inblock;
				host->GetBlock(index, nblock, inblock);

				if (nblock>0)
				{
					inblock = inblock*2+branch;
					++nblock;
				} else if (inblock<MINBLOCK/2-1)
				{
					inblock = inblock*2+branch+1;
				} else 
				{
					inblock = inblock*2-MINBLOCK+2+branch;
					++inblock;
				}
				
				cur = host->m_data.begin() + nblock;
				u = cur.begin() + inblock;        
			}
/*
			void SwapUp ()
			{
			}

			void SwapDown (TriangleIterator_ last)
			{
			}*/

			bool Valid () const
			{
        return cur>=host->m_data.begin() && cur<host->m_data.end() && u>=cur->begin() && u<cur->end();
			}
			
#ifdef _MSC_VER
			friend typename TriangleBuffer_; 
			friend typename TriangleBuffer_::iterator;
			friend typename TriangleBuffer_::const_iterator;
			//friend class ReverseTriangleIterator_;
#else
			friend class TriangleBuffer_; 
			friend class TriangleBuffer_::iterator;
			friend class TriangleBuffer_::const_iterator;
			friend class ReverseTriangleIterator_;
#endif

		public :
			TriangleIterator_ () {}

			operator U* ()    const { return  u; }
			U& operator * ()  const { assert(Valid()); return *u; }
			U* operator -> () const { assert(Valid()); return  u; }

			TriangleIterator_& operator += (int n)
			{
				if (n>0) Incr(n); else Decr(-n);
				return *this;
			}			
				
			TriangleIterator_& operator -= (int n)
			{
				if (n>0) Decr(n); else Incr(-n);
				return *this;
			}
			
			TriangleIterator_ operator + (int n) const
			{
				TriangleIterator_ result = *this;
				result += n;
				return result;
			}						
			
			TriangleIterator_ operator - (int n) const
			{
				TriangleIterator_ result = *this;
				result -= n;
				return result;
			}			
			
			TriangleIterator_& operator ++ ()
			{
				if (cur>=host->m_data.end()) return *this;
				++u;
				if (u<cur->begin()+cur->Capacity()) return *this;
				++cur;
				if (cur>=host->m_data.end()) u=NULL;
				else u=cur->begin();
				return *this;
			}
			
			TriangleIterator_& operator -- ()
			{
				if (cur<=host->m_data.rend()) return *this;
				--u;
				if (u>=cur->begin()) return *this;
				--cur;
				if (cur<=host->m_data.rend()) u=NULL;
				else u=cur->rbegin();
				return *this;
			}
			
			TriangleIterator_ operator ++ (int)
			{
				TriangleIterator_ result = *this;
				++(*this);
				return result;
			}			
			
			TriangleIterator_ operator -- (int)
			{
				TriangleIterator_ result = *this;
				--(*this);
				return result;
			}

			bool operator == (const typename       TriangleBuffer_::iterator& other) const { return cur==other.cur && u==other.u; }
			bool operator == (const typename TriangleBuffer_::const_iterator& other) const { return cur==other.cur && u==other.u; }
			bool operator != (const typename       TriangleBuffer_::iterator& other) const { return cur!=other.cur || u!=other.u; }
			bool operator != (const typename TriangleBuffer_::const_iterator& other) const { return cur!=other.cur || u!=other.u; }
		};		
		
		template <typename HOST, typename U, typename Block>
		class ReverseTriangleIterator_ : public TriangleIterator_<HOST,U,Block>
		{
			ReverseTriangleIterator_ (HOST* host, Block* cur, U* u) 
				: TriangleIterator_<HOST,U,Block> (host, cur, u) {}
				
#ifdef _MSC_VER
			friend typename TriangleBuffer_; 
			friend typename TriangleBuffer_::iterator;
			friend typename TriangleBuffer_::const_iterator;
#else
			friend class TriangleBuffer_; 
			friend class TriangleBuffer_::iterator;
			friend class TriangleBuffer_::const_iterator;
#endif			

		public :
      			
			ReverseTriangleIterator_& operator += (int n)
			{
				assert(this->Valid());
				if (n>0) this->Decr(n); else this->Incr(-n);
			}			
				
			ReverseTriangleIterator_& operator -= (int n)
			{
				assert(this->Valid());
				if (n>0) this->Incr(n); else this->Decr(-n);
			}
			
			ReverseTriangleIterator_& operator ++ ()
			{
				if (this->cur <= this->host->m_data.rend()) return *this;			
				--this->u;
				if (this->u >= this->cur->begin()) return *this;
				--this->cur;
				if (this->cur <= this->host->m_data.rend()) this->u=NULL;
				else this->u = this->cur->rbegin();
				return *this;
			}
			
			ReverseTriangleIterator_& operator -- ()
			{
				if (this->cur >= this->host->m_data.end()) return *this;			
				++this->u;
				if (this->u < this->cur->begin() + this->cur->Capacity) return *this;
				++this->cur;
				if (this->cur >= this->host->m_data.end()) this->u=NULL;
				else this->u = this->cur->begin();
				return *this;
			}		

			ReverseTriangleIterator_ operator + (int n) const
			{
				ReverseTriangleIterator_ result = *this;
				result += n;
				return result;
			}						
			
			ReverseTriangleIterator_ operator - (int n) const
			{
				ReverseTriangleIterator_ result = *this;
				result -= n;
				return result;
			}					
			
			ReverseTriangleIterator_ operator ++ (int)
			{
				ReverseTriangleIterator_ result = *this;
				++(*this);
				return result;
			}			
			
			ReverseTriangleIterator_ operator -- (int)
			{
				ReverseTriangleIterator_ result = *this;
				--(*this);
				return result;
			}
			
			bool operator == (const typename       TriangleBuffer_::reverse_iterator& other) const { return this->cur==other.cur && this->u==other.u; }
			bool operator == (const typename TriangleBuffer_::const_reverse_iterator& other) const { return this->cur==other.cur && this->u==other.u; }
			bool operator != (const typename       TriangleBuffer_::reverse_iterator& other) const { return this->cur!=other.cur || this->u!=other.u; }
			bool operator != (const typename TriangleBuffer_::const_reverse_iterator& other) const { return this->cur!=other.cur || this->u!=other.u; }
		};

		void GetBlock (int index, int& nblock, int& inblock)
		{
			assert (index>=0 && index<m_size);
			if (index+1<MIN_BLOCK)
			{
				nblock = 0;
				inblock = index;
			} else
			{
				nblock = IntLn2(index+1)-LN_MIN_BLOCK+1;
				inblock = index+1 - (MIN_BLOCK<<(nblock-1));
			}
			assert (nblock>=0 && nblock<=m_data.Size());
			assert (inblock>=0 && inblock<=m_data[nblock].Size());
		}

		int count_in_firstnblocks (int n)
		{
			return n==0 ? 0 : (1<<(LN_MIN_BLOCK+n-1))-1;
		}

	public :

		typedef T value_type;

		typedef T& reference;
		typedef const T& const_reference;
		typedef TriangleIterator_<TriangleBuffer_, T, Block> iterator;
		typedef TriangleIterator_<const TriangleBuffer_, const T, const Block> const_iterator;
		typedef ReverseTriangleIterator_<TriangleBuffer_, T, Block> reverse_iterator;
		typedef ReverseTriangleIterator_<const TriangleBuffer_, const T, const Block> const_reverse_iterator;

		iterator begin () 
		{ 
			if (m_data.Size()==0) 
				return iterator(this, m_data.begin(), NULL);
			else
				return iterator (this, &m_data.front(), m_data.front().begin()); 
		}

		const_iterator begin () const 
		{ 
			if (m_data.Size()==0) 
				return const_iterator(this, m_data.begin(), NULL);
			else
				return const_iterator (this, &m_data.front(), m_data.front().begin()); 
		}

		iterator end () 
		{
			if (m_data.Size()==0) 
				return iterator(this, m_data.end(), NULL);
			else if (m_data.back().Size()==m_data.back().Capacity())
				return iterator(this, m_data.end(), NULL);
			else 
				return iterator(this, &m_data.back(), m_data.back().end());		
		}

		const_iterator end () const 
		{
			if (m_data.Size()==0) 
				return const_iterator(this, m_data.end(), NULL);
			else if (m_data.back().Size()==m_data.back().Capacity())
				return const_iterator(this, m_data.end(), NULL);
			else 
				return const_iterator(this, &m_data.back(), m_data.back().end());		
		}

		reverse_iterator rbegin ()
		{
			if (m_data.Size()==0)
				return reverse_iterator(this, m_data.rbegin(), NULL);
			else 
				return reverse_iterator(this, m_data.rbegin(), m_data.rbegin()->rbegin());
		}		

		const_reverse_iterator rbegin () const
		{
			if (m_data.Size()==0)
				return const_reverse_iterator(this, m_data.rbegin(), NULL);
			else 
				return const_reverse_iterator(this, m_data.rbegin(), m_data.rbegin()->rbegin());
		}
		
		reverse_iterator rend ()
		{
			return reverse_iterator(this, m_data.rbegin(), NULL);
		}		
		
		const_reverse_iterator rend () const
		{
			return const_reverse_iterator(this, m_data.rbegin(), NULL);
		}		

		reference front () 
		{
			return *begin();
		}

		const_reference front () const
		{
			return *begin();
		}		

		reference back () 
		{
			return *rbegin();
		}

		const_reference back () const
		{
			return *rbegin();
		}

		int Size () const { return m_size; }

		TriangleBuffer_ () : m_size(0) {}		

		T& operator [] (int index)
		{
			int nblock, inblock;
			GetBlock (index, nblock, inblock);
			return m_data[nblock][inblock];
		}		
			
		const T& operator [] (int i) const
		{			
			int nblock, inblock;
			GetBlock (index, nblock, inblock);
			return m_data[nblock][inblock];
		}
		
		template <typename U>
		friend class Container_;

		template <typename U>
		friend class TriangleBuffer;
		
	private : 
		
		iterator PrepareSize (int a_size) 
		{
			iterator result;
			// мудянко
			tblib::Enlarge(a_size,0);

			if ((a_size>0 && m_size>0 && (a_size|m_size)<MIN_BLOCK )
				|| (a_size^m_size) < tblib::Min(a_size, m_size))
			{
        Block& bl = m_data.back ();
				result = iterator (this, &bl, bl.end());
				bl.PrepareSize(bl.Size() + a_size-m_size);
			} else if (a_size>m_size) 
			{
        if (m_data.Size()==0 && a_size<MIN_BLOCK)
				{
					// первый блок только
					assert(m_size==0);
					m_data.Resize(1);
					Block& bl = m_data.back();
					bl.PrepareSize(MIN_BLOCK-1);
					bl.PrepareSize(a_size);
					result = iterator (this, &bl, bl.begin());
				} else
				{
					int bc = m_data.Size();
					int nbc = IntLn2(a_size)-LN_MIN_BLOCK+2;
					assert (nbc>bc);
					int innbCount = a_size+1 - (MIN_BLOCK<<(nbc-2));
					assert(innbCount>0);
					
					bool full_block = (m_data.Size()==0 || m_data.back().Size()==m_data.back().Capacity());

					if (!full_block) result = iterator(this, &m_data.back(), m_data.back().end());				
					m_data.Resize(nbc);

					// если bc было не ноль, то старый последний блок тоже надо раздвинуть до положенного размера
					for (int i=tblib::Max(0,bc-1); i<nbc; ++i)
					{
						int elc = i==0 ? MIN_BLOCK-1 : (MIN_BLOCK << (i-1));
						assert(i<bc || m_data[i].Capacity()==0);
						m_data[i].PrepareSize(elc); // установить вместимость	
						assert(m_data[i].Capacity()==elc); // переполнение памяти!
						if (i==nbc-1)
							m_data[i].PrepareSize(innbCount);
					}
					if (full_block) result = iterator(this, &m_data[bc], m_data[bc].begin());	
				}
			} else
			{
				if (a_size==0)
				{
					m_data.Resize(0);
				} else
				{
					int nbc = a_size<MIN_BLOCK ? 1 : IntLn2(a_size)-LN_MIN_BLOCK+2;
					int innbCount = nbc==1 ? a_size : a_size+1 - (MIN_BLOCK<<(nbc-2));
					assert (innbCount>=0);
					result = end();
					m_data.Resize(nbc);
					if (nbc)
						m_data[nbc-1].PrepareSize(innbCount);
				}
			}
			m_size = a_size;
			return result;
		}
	};
	
	template <typename T>
	class TriangleBuffer : public Container_ <TriangleBuffer_<T> >
	{
	public :
		CONTAINER_CONSTRUCTORS(TriangleBuffer, TriangleBuffer_<T>)	

		void MoveFrom_ (TriangleBuffer& from)
		{
			this->m_size = from.m_size;
			Move(this->m_data, from.m_data);
		}
	};
}