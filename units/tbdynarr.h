#pragma once 

#include "tbarr.h"
#include "tbptr.h"

namespace tblib
{                 
  template <typename U>
  class Vector;

	template <typename T>
	class HeapCArray 
	{		
		T* m_begin;
		int m_size;

		void PrepareSize (int a_size)
		{
			m_begin = a_size ? reinterpret_cast<T*>(tbAlloc.Malloc(a_size*sizeof(T))) : NULL;
			m_size = m_begin ? a_size : 0;
		}				
		
	public :
		typedef HeapCArray<T> container_type;
		typedef T value_type;

		typedef value_type & reference;
		typedef const value_type & const_reference;
		typedef ForwardPointer_<container_type, value_type> iterator;
		typedef ForwardPointer_<const container_type, const value_type> const_iterator;
		typedef ReversePointer_<container_type, value_type> reverse_iterator;
		typedef ReversePointer_<const container_type, const value_type> const_reverse_iterator;
		
		iterator begin () {
			return iterator(this, m_begin);
		}
		
		const_iterator begin () const {
			return const_iterator(this, m_begin);
		}

		reverse_iterator rbegin () {
			return reverse_iterator(this, m_begin + m_size - 1);
		}
		
		const_reverse_iterator rbegin () const {
			return const_reverse_iterator(this, m_begin + m_size - 1);
		}

		iterator end () {
			return iterator(this, m_begin + m_size);
		}
		
		const_iterator end () const {
			return const_iterator(this, m_begin + m_size);
		}

		reverse_iterator rend () {
			return reverse_iterator(this, m_begin - 1);
		}
		
		const_reverse_iterator rend () const {
			return const_reverse_iterator(this, m_begin - 1);
		}

		int Size () const
		{
			return m_size;
		}

		int Capacity () const
		{
			return m_size;
		}		

		reference front () {
			return *m_begin;
		}

		const_reference front () const {
			return *m_begin;
		}
		
		reference back () {
			return operator [] (m_size-1);
		}

		const_reference back () const {
			return operator [] (m_size-1);
		}

		reference operator [] (int index) {
			assert (index>=0 && index<m_size);			
			return m_begin[index];
		}

		const_reference operator [] (int index) const {
			assert (index>=0 && index<m_size);			
			return m_begin[index];
		}		
		
		Slice<T> GetSlice(int low, int high)	{
			assert (low>=0 && high<=m_size);
			return tblib::Slice<T>(m_begin+low, low, high);
		}				

		Slice<T> GetSlice ()
		{
			return GetSlice(0, this->Size());
		}
		
		HeapCArray () : m_begin (NULL), m_size(0)  
		{
		}

		template <typename U>
		HeapCArray (const U& u)
		{
			PrepareSize (u.Size());

			iterator dst = begin();
			typename U::const_iterator src = u.begin();

			while (src != u.end())
			{
				new ((T*)(dst)) T(*src);
				++dst;
				++src;
			}
		}
		
		HeapCArray (const HeapCArray& u)
		// продублируем, потому что эта падла сама не допирает
		{
			PrepareSize (u.Size());

			iterator dst = begin();
			HeapCArray::const_iterator src = u.begin();

			while (src != u.end())
			{
				new ((T*)(dst)) T(*src);
				++dst;
				++src;
			}
		}
		
		template <int N>
		HeapCArray (const T (&e)[N])
		{
			PrepareSize (N);

			iterator dst = begin();
			const T* src = e;

			while (src != e+N)
			{
				new ((T*)(dst)) T(*src);
				++dst;
				++src;
			}
		}
		
		template <typename U>
		HeapCArray& operator = (const U& u)
		{			
			Recreate(*this, u);
			return *this;
		}
		
		HeapCArray& operator = (const HeapCArray& u)
		{
			if (this!=&u)
				Recreate(*this, u);
			return *this;
		}

		void Resize (int a_size)
		{
			assert (m_size==0);
			PrepareSize(a_size);
			for (int i=0; i<m_size; ++i)
				new (m_begin+i) T;
		}

		HeapCArray (int a_size)  
		{
			PrepareSize(a_size);
			for (int i=0; i<m_size; ++i)
				new (m_begin+i) T;
		}

		~HeapCArray ()
		{	
			if (m_size)
			{
				assert (m_begin);
				for (int i=0; i<m_size; ++i)
					m_begin[i].~T();
				tbAlloc.Free(m_begin);
			} else
			{
				assert (m_begin==NULL);
			}
		}
		
		template <typename Comp>
		void Sort (Comp comp) 
		{ GetSlice().Sort<Comp>(comp); }
		
		void Sort () 
		{ GetSlice().Sort(DefaultComparator<T>()); }
		
		void MoveFrom_ (HeapCArray& from)
		{
			m_begin = from.m_begin;
			m_size = from.m_size;
		}
	};

	template <typename T>
	class HeapArray_ 
	{
		// эту хуйню и пишу потому, что Єбана€ студи€ оп€ть кукарекает не в тему
		HeapArray_& operator = (const HeapArray_&);
		T* m_begin;
		int m_capacity;
		int m_size;

	public :
		typedef HeapArray_<T> container_type;
		typedef T value_type;

		typedef value_type & reference;
		typedef const value_type & const_reference;
		typedef ForwardPointer_<HeapArray_, T > iterator;
		typedef ForwardPointer_<const HeapArray_, const T> const_iterator;
		typedef ReversePointer_<HeapArray_, T > reverse_iterator;
		typedef ReversePointer_<const HeapArray_, const T> const_reverse_iterator;
		
		iterator begin () {
			return iterator(this, m_begin);
		}
		
		const_iterator begin () const {
			return const_iterator(this, m_begin);
		}

		reverse_iterator rbegin () {
			return reverse_iterator(this, m_begin + m_size - 1);
		}
		
		const_reverse_iterator rbegin () const {
			return const_reverse_iterator(this, m_begin + m_size - 1);
		}

		iterator end () {
			return iterator(this, m_begin + m_size);
		}
		
		const_iterator end () const {
			return const_iterator(this, m_begin + m_size);
		}

		reverse_iterator rend () {
			return reverse_iterator(this, m_begin - 1);
		}
		
		const_reverse_iterator rend () const {
			return const_reverse_iterator(this, m_begin - 1);
		}

		reference front () {
			return *m_begin;
		}

		const_reference front () const {
			return *m_begin;
		}
		
		reference back () {
			return operator [] (m_size-1);
		}

		const_reference back () const {
			return operator [] (m_size-1);
		}

		reference operator [] (int index) {
			assert (index>=0 && index<m_size);			
			return m_begin[index];
		}

		const_reference operator [] (int index) const {
			assert (index>=0 && index<m_size);			
			return m_begin[index];
		}

		int Size() const {
			return m_size; 
		}

		int Capacity () const {
			return m_capacity;
		}

		HeapArray_ () : m_size(0)
		{
			m_capacity = 0;
			m_begin = NULL;
		}
		
		~HeapArray_ ()
		{
			if (m_begin)
				tbAlloc.Free(m_begin);
		}	

		template <typename U>
		friend class TriangleBuffer_; 		
		
		template <typename U>
		friend class PtrVector_;
		
		template <typename U>
		friend class HeapArray;
		
		template <typename U>
		friend class Vector_;
		
		template <typename U>
		friend class Vector;
		
		template <typename U>
		friend class PtrVector_;
		
		template <typename U>
		friend class PtrVector;
		
		template <typename U>
		friend class Container_;			

		void MoveFrom_ (HeapArray_& from)
		{
			m_size     = from.m_size;
			m_begin    = from.m_begin;
			m_capacity = from.m_capacity;
		}		
		
		Slice<T> GetSlice(int low, int high)	{
			assert (low>=0 && high<=m_size);
			return tblib::Slice<T>(this->m_begin+low, low, high);
		}					

		Slice<T> GetSlice ()
		{
			return GetSlice(0, this->Size());
		}
				
		template <typename Comp>
		void Sort (Comp comp) 
		{ GetSlice ().Sort<Comp>(comp); }
		
		void Sort () 
		{ GetSlice ().Sort(DefaultComparator<T>()); }
		
	private :			
			
		void InitMem (int a_capacity) 
		{
			m_begin     = reinterpret_cast<T*> (tbAlloc.Malloc(a_capacity*sizeof(T)));
			m_capacity  = m_begin ? a_capacity : 0;	
		}

		iterator PrepareSize (int a_size) 
		{		
			if (Capacity()==0) // выделени€ пам€ти - только пустым массивам или в спец методе
			{			
				assert (m_begin==NULL);
				InitMem (a_size);
			}
			assert (a_size>=0 && a_size<=Capacity());
			iterator result = iterator(this, m_begin+m_size);
			m_size = a_size;
			return result;
		}
	};	

	template <typename T>
	class HeapArray : public Container_ <HeapArray_<T> >
	{
	public :
		CONTAINER_CONSTRUCTORS(HeapArray, HeapArray_<T>)		
			
		HeapArray (int a_capacity) 
		{
			this->InitMem (a_capacity);
		}	
		
		void MoveFrom_ (HeapArray& from)
		{
			this->m_begin    = from.m_begin;
			this->m_size     = from.m_size;
			this->m_capacity = from.m_capacity;
		}		
	};

	template <typename T>
	class Vector_ : public HeapArray_<T>
	{
		typedef typename HeapArray_<T>::iterator titer;

		void Realloc (int a_capacity, int valid_size)
		{
			assert (valid_size>=0);
			assert (a_capacity>=0);

			if (a_capacity==0)
			{				
				if (this->m_begin)
					tbAlloc.Free(this->m_begin);
				this->m_capacity = 0;
				this->m_begin    = NULL;
				return;
			}
			
			int memsize = a_capacity*sizeof(T);

			if (!this->m_begin)
			{
				assert (this->m_size==0);
				this->m_begin    = reinterpret_cast<T*> (tbAlloc.Malloc(memsize));
				this->m_capacity = this->m_begin ? a_capacity : 0;
				return;
			}			

			if (tbAlloc.ReallocInplace(this->m_begin, memsize))
			{				
				this->m_capacity = a_capacity;				
				return;
			}

			T* new_begin = reinterpret_cast<T*> (tbAlloc.Malloc(memsize));

			if (!new_begin)
				return;

			// а вот тут делаем опасный мув
			for (int i=0; i<valid_size; ++i)
				Move_(new_begin[i], this->m_begin[i]);

			tbAlloc.Free(this->m_begin);

			this->m_begin = new_begin;
			this->m_capacity = a_capacity;
		}
		
		titer PrepareSize (int a_size) 
		{		
			Reserve(a_size, Min(a_size, m_size));
			titer result = this->end();
			this->m_size = a_size;
			return result;
		}

		template <typename U>
		friend class Container_;

	public :
		CONSTRUCTORS(Vector_, HeapArray_<T>)
			
		void Reserve (int a_size, int valid_size)
		{
			assert (a_size>=0);
			if (a_size > this->m_capacity)
				Realloc(Max(a_size, this->m_capacity + this->m_capacity/4 + 16), valid_size);
			else if (a_size < (this->m_capacity-16)/2 || a_size==0)
				Realloc(a_size, valid_size);
		}

	};
	
	template <typename T>
	class Vector : public Container_ <Vector_<T> >
	{
	public :
		CONTAINER_CONSTRUCTORS(Vector, Vector_<T>)		
			
		Vector (int a_capacity) 
		{
			this->InitMem (a_capacity);
		}	
		
		Slice<T> GetSlice(int low, int high)	{
			assert (low>=0 && high<=m_size);
			return tblib::Slice<T>(this->m_begin+low, low, high);
		}					

		Slice<T> GetSlice ()
		{
			return GetSlice(0, this->Size());
		}
		
		void MoveFrom_ (Vector& from)
		{
			this->m_begin    = from.m_begin;
			this->m_size     = from.m_size;
			this->m_capacity = from.m_capacity;
		}		
		
		template <typename Comp>
		void Sort (Comp comp) 
		{ GetSlice().Sort<Comp>(comp); }
		
		void Sort () 
		{ GetSlice().Sort(DefaultComparator<T>()); }
	};
		
	template <typename T>
	class PtrVector_ : public Vector_<PtrVInfo_<T>*>
	{
		typedef Vector_<PtrVInfo_<T>*> Base;
	public :
		CONSTRUCTORS(PtrVector_, Base)	

		typedef PtrVector_<T> container_type;
		typedef T value_type;
		typedef value_type & reference;
		typedef const value_type & const_reference;
		typedef ForwardPointerUP_<container_type, value_type> iterator;
		typedef ForwardPointerUP_<const container_type, const value_type> const_iterator;
		typedef ReversePointerUP_<container_type, value_type> reverse_iterator;
		typedef ReversePointerUP_<const container_type, const value_type> const_reverse_iterator;

		void MoveFrom_ (PtrVector_& from)
		{
			Base::MoveFrom_(from);			
		}		
		
		iterator begin () {
			return iterator(this, m_begin);
		}
		
		const_iterator begin () const {
			return const_iterator(this, m_begin);
		}

		reverse_iterator rbegin () {
			return reverse_iterator(this, m_begin + m_size - 1);
		}
		
		const_reverse_iterator rbegin () const {
			return const_reverse_iterator(this, m_begin + m_size - 1);
		}

		iterator end () {
			return iterator(this, m_begin + m_size);
		}
		
		const_iterator end () const {
			return const_iterator(this, m_begin + m_size);
		}

		reverse_iterator rend () {
			return reverse_iterator(this, m_begin - 1);
		}
		
		const_reverse_iterator rend () const {
			return const_reverse_iterator(this, m_begin - 1);
		}
		
		reference front () {
			return **m_begin;
		}

		const_reference front () const {
			return **m_begin;
		}
		
		reference back () {
			return operator [] (m_size-1);
		}

		const_reference back () const {
			return operator [] (m_size-1);
		}

		reference operator [] (int index) {
			assert (index>=0 && index<m_size);	
			return *(m_begin[index]);
		}

		const_reference operator [] (int index) const {
			assert (index>=0 && index<m_size);			
			return *(m_begin[index]);
		}		
		
		template <typename U>
		friend class Container_;

		template <typename U>
		friend class PtrVector;
	private :		

		iterator PrepareSize (int a_size) 
		{					
			if (a_size<m_size)
				for (int i=a_size; i<m_size; ++i)
					tbAlloc.Free(m_begin[i]);

			this->Reserve(a_size, Min(a_size, m_size));
			iterator result = this->end();
			
			if (a_size>m_size)
			{
				for (int i=m_size; i<a_size; ++i)
				{
					m_begin[i] = static_cast<PtrVInfo_<T>*> (tbAlloc.Malloc(sizeof(PtrVInfo_<T>)));
					m_begin[i]->index = i;
				}
			}

			this->m_size = a_size;
			return result;
		}				
	};
	
	template <typename T>
	class PtrVector : public Container_ <PtrVector_<T> >
	{
	public :
		CONTAINER_CONSTRUCTORS(PtrVector, PtrVector_<T>)		

		void MoveFrom_ (PtrVector& from)
		{
			Container_ <PtrVector_<T> >::MoveFrom_(from);			
		}				
		
		void Erase(typename PtrVector_<T>::iterator it)
		{
			Erase(it->u->index);
		}
		
		void Erase(int index)
		{
			assert (index>=0 && index<m_size);	

			tblib::Swap(this->m_begin[this->m_size-1], this->m_begin[index]);
			this->m_begin[index]->index = index;
			this->Shrink(this->m_size-1);
		}

		int GetIndex(typename PtrVector_<T>::value_type* t) const
		{
			PtrVInfo_<T>* info = static_cast <PtrVInfo_<T>* > (t);
			assert (this->m_begin[info->index] == info);
			return info->index;
		}

		void Erase(typename PtrVector_<T>::value_type* t)
		{
			Erase(GetIndex(t));
		}		
	};

}