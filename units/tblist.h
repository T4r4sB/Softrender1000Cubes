#pragma once
#include "container.h"
#include "tballoc.h"

namespace tblib
{
	template <typename T>
	class List_ 
	{
		struct BlockHeader
		{
      BlockHeader* prev;
			BlockHeader* next;
			BlockHeader (BlockHeader* prev,	BlockHeader* next) 
				: prev(prev), next(next) {}
		};

		struct TBox
		{
			T t;
		};

		struct Block : BlockHeader, TBox
		{	
			Block (BlockHeader* prev,	BlockHeader* next) : BlockHeader(prev,next) {}
		};

		int m_size;
    BlockHeader m_head;

		template <typename HOST, typename U, typename UBox, typename BH, typename B> 
		class _listiterator
		{
			HOST* host;
			BH* ptr;
			_listiterator (HOST* host, BH* ptr) : host(host), ptr(ptr) {}
			
			bool valid () const
			{
				return ptr != &host->m_head;
			}

			template <typename V>
			friend class List_; 
			
			template <typename V>
			friend class List; 
			
			template <typename H, typename V, typename W, typename X>
			friend class _reverse_listiterator; 


		public :
			_listiterator () {}
			
			operator U* ()    const {	return &static_cast<UBox*>(static_cast<B*>(ptr))->t; }
			U& operator * ()  const { assert(valid()); return  static_cast<UBox*>(static_cast<B*>(ptr))->t; }
			U* operator -> () const { assert(valid()); return &static_cast<UBox*>(static_cast<B*>(ptr))->t; }

			_listiterator& operator ++ ()
			{
				if (valid())
					ptr = ptr->next; 
				return *this;
			}
			
			_listiterator& operator -- ()
			{
				if (valid())
					ptr = ptr->prev; 
				return *this;
			}
			
			_listiterator operator ++ (int)
			{
				_listiterator result = *this;
				++(*this);
				return result;
			}			
			
			_listiterator operator -- (int)
			{
				_listiterator result = *this;
				--(*this);
				return result;
			}
			
			_listiterator& operator += (int n)
			{
				for (int i=0; i<n; ++i)
					++(*this);
				return *this;
			}			
				
			_listiterator& operator -= (int n)
			{
				for (int i=0; i<n; ++i)
					--(*this);
				return *this;
			}
			
			_listiterator operator + (int n) const
			{
				_listiterator result = *this;
				result += n;
				return result;
			}						
			
			_listiterator operator - (int n) const
			{
				_listiterator result = *this;
				result -= n;
				return result;
			}			
		};
		
		template <typename HOST, typename U, typename UBox, typename BH, typename B>
		class _reverse_listiterator : public _listiterator<HOST,U,UBox,BH,B>
		{
			_reverse_listiterator (HOST* host, BH* ptr)	: _listiterator<HOST,U,UBox,BH,B> (host,ptr) {}
				
			template <typename V>
			friend class List_; 
			
			template <typename V>
			friend class List; 

		public :
			
			_reverse_listiterator& operator += (int n)
			{
				return static_cast<_reverse_listiterator&> (_listiterator<HOST,U,UBox,BH,B>::operator -= (n));
			}			
				
			_reverse_listiterator& operator -= (int n)
			{
				return static_cast<_reverse_listiterator&> (_listiterator<HOST,U,UBox,BH,B>::operator += (n));
			}
			
			_reverse_listiterator& operator ++ ()
			{
				return static_cast<_reverse_listiterator&> (_listiterator<HOST,U,UBox,BH,B>::operator -- ());
			}
			
			_reverse_listiterator& operator -- ()
			{
				return static_cast<_reverse_listiterator&> (_listiterator<HOST,U,UBox,BH,B>::operator ++ ());
			}			
			
			_reverse_listiterator operator + (int n) const
			{
				_reverse_listiterator result = *this;
				result += n;
				return result;
			}						
			
			_reverse_listiterator operator - (int n) const
			{
				_reverse_listiterator result = *this;
				result -= n;
				return result;
			}					
			
			_reverse_listiterator operator ++ (int)
			{
				_reverse_listiterator result = *this;
				++(*this);
				return result;
			}			
			
			_reverse_listiterator operator -- (int)
			{
				_reverse_listiterator result = *this;
				--(*this);
				return result;
			}
		};

	public :
		
		typedef T value_type;

		typedef T& reference;
		typedef const T& const_reference;
		typedef _listiterator<List_, T, TBox, BlockHeader, Block> iterator;
		typedef _listiterator<const List_, const T, const TBox, const BlockHeader, const Block> const_iterator;
		typedef _reverse_listiterator<List_, T, TBox, BlockHeader, Block> reverse_iterator;
		typedef _reverse_listiterator<const List_, const T, const TBox, const BlockHeader, const Block> const_reverse_iterator;

    iterator begin ()
		{
			return iterator(this, m_head.next);
		}
		
    const_iterator begin () const
		{
			return const_iterator(this, m_head.next);
		}
		
    reverse_iterator rbegin ()
		{
			return reverse_iterator(this, m_head.prev);
		}
		
    const_reverse_iterator rbegin () const
		{
			return const_reverse_iterator(this, m_head.prev);
		}

		iterator end () 
		{
			return iterator(this, &m_head);
		}
		
		const_iterator end () const
		{
			return const_iterator(this, &m_head);
		}
		
		reverse_iterator rend () 
		{
			return reverse_iterator(this, &m_head);
		}
		
		const_reverse_iterator rend () const
		{
			return const_reverse_iterator(this, &m_head);
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

		int Size ()
		{
			return m_size;
		}

		List_ () : m_size(0), m_head(&m_head, &m_head) {}
		
		template <typename U>
		friend class Container_;
	
		template <typename V>
		friend class List; 
		
	private : 
		
		iterator PrepareSize (int a_size)
		{
			if (a_size>m_size)
			{
				BlockHeader* cur_end = m_head.prev;
				Block* b=NULL;
				for (int i=m_size; i<a_size; ++i)
				{
					b = reinterpret_cast<Block*>(tbAlloc.Malloc(sizeof(Block)));
          if (b)
					{
						new (b) BlockHeader (cur_end, cur_end->next);
						cur_end->next->prev = b;
						cur_end->next = b;
						++m_size;
					} 
				}
				return b ? iterator(this, b) : end();
			} else if (a_size<m_size)
			{
				BlockHeader* cur_end = m_head.prev;
				for (int i=a_size; i<m_size; ++i)
				{
          BlockHeader* tmp = cur_end;
					cur_end = cur_end->prev;
					tbAlloc.Free(tmp);
				}
				cur_end->next = &m_head;
				m_head.prev = cur_end;
				m_size = a_size;
				return end();
			} else
			{
				return end();
			}
		}
	};
	
	template <typename T>
	class List : public Container_ <List_<T> >
	{
	public :
		CONTAINER_CONSTRUCTORS(List, List_<T>)	

		void Rotate (typename List::iterator beg, typename List::iterator mid, typename List::iterator en)
		{
			assert (beg!=mid);
			assert (beg!=en);
			assert (mid!=en);

			List::BlockHeader
				*bpr = beg.ptr->prev, 
				*mpr = mid.ptr->prev, 
				*epr = en .ptr->prev; 

      bpr->next = mid.ptr;
			mid.ptr->prev = bpr;

			epr->next = beg.ptr;
			beg.ptr->prev = epr;

			mpr->next = en .ptr;
      en .ptr->prev = mpr;
		}

		void Erase (typename List::iterator it)
		{
			if (it+1!=this->end())
				Rotate(it, it+1, this->end());
			this->Shrink(this->Size()-1);
		}

		bool InList (T* t) const
		{
			for (List::const_iterator it = this->begin(); it != this->end(); ++it)
				if (it==t)
					return true;
			return false;
		}
		
		void Erase (T* t)
		{
			assert (InList(t));
			TBox* box = reinterpret_cast<TBox*>(t);
			Erase (List::iterator(this, static_cast<Block*>(box)));
		}
	};
}