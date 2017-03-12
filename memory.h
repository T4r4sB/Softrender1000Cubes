#pragma once
#include "tbslice.h"
#include "tbarr.h"
#include "stdint.h"
#include "assert.h"
#include "pasrand.h"

bool finish = false;

class StackPool
{
	int size;
	char *data;
	char *beg;
	char *maxmem;
	bool global;
	
	StackPool(const StackPool&);
	StackPool& operator =(const StackPool&);
public :
	StackPool (char* mem, int size, bool global) : data(mem), beg(data), maxmem(data), size(size), global(global) {}

	template <typename T>
	T* Alloc (int count)
	{
			
		int bytecount = count*sizeof(T);
		if (bytecount>0)
		{	
			beg = (char*)(((size_t)beg+15) & (~15)); // выравнивание
			assert(beg+bytecount-1 < data+size);
			T* result = reinterpret_cast<T*>(beg);
			beg += bytecount;
			if (beg>maxmem) 
				maxmem=beg;


			return result;
		} else
			return NULL;
	}

	typedef char *MemoryState;

	bool ValidEnd (char* ptr) { return ptr<=beg; } // валиден либо на границе
 	void SaveState (MemoryState &ms) { ms=beg; }
	void RestoreState (MemoryState ms) 
	{ 
#ifndef NDEBUG
		assert(!global || finish);			
		if (!finish || !global) { 
			assert(beg>=ms); 
			beg=ms; 
		} 
#else
		beg=ms;
#endif
	} // порядок освобождения определить нельзя(
};

#define DefineStackPool(Name,Size,Global)\
	char __##Name[Size];\
	StackPool Name(__##Name,Size,Global)

DefineStackPool(globalStackPool,0x2000000,true);
DefineStackPool(temporaryStackPool,0x800000,false);

Random markerrnd(0);

enum PoolKind { PK_GLOBAL, PK_TEMP };

template <typename T>
class BasicMArray : public tblib::Slice<T>
{
	int marker;
#ifndef NDEBUG
	int *pmarker;
#endif
	StackPool* pool;
	StackPool::MemoryState ms;

	BasicMArray (const BasicMArray&);
	BasicMArray& operator= (const BasicMArray&);

public :
	BasicMArray () 
	{
		pool = NULL;
		new (this) tblib::Slice<T>(NULL, 0, 0);
	}
  
	BasicMArray ( int low, int high, PoolKind poolKind ) 
	{		
		pool = poolKind==PK_GLOBAL ? &globalStackPool : &temporaryStackPool;
		pool->SaveState (ms);
#ifndef NDEBUG
		pmarker = pool->Alloc<int>(1);
		markerrnd.NextSeed();
		*pmarker = markerrnd.randseed;
		marker   = *pmarker;
#endif
		T* ptr = pool->Alloc<T>(high-low+1);
		new(this)	tblib::Slice<T>(ptr, low, high);
	}
	
	void Init (int low, int high, PoolKind poolKind ) 
	{
		this->~BasicMArray();
		new (this) BasicMArray(low, high, poolKind);
	}

	~BasicMArray ()
	{
		if (pool)
		{
			pool->RestoreState (ms);
		}
	}

	T& operator [] (int index)
	{
		assert (pmarker);
		assert (marker == *pmarker);
		assert (pool->ValidEnd((char*)(end())));
		return tblib::Slice<T>::operator [] (index);
	}
	
	T& operator [] (int index) const
	{
		assert (pmarker);
		assert (marker == *pmarker);
		assert (pool->ValidEnd((char*)(end())));
		return tblib::Slice<T>::operator [] (index);
	}
};

template <typename T>
class MSlice : public BasicMArray<T>
{
	MSlice (const MSlice&);
	MSlice& operator= (const MSlice&);
public :
	MSlice () : BasicMArray<T> () {}

	MSlice ( int low, int high, PoolKind poolKind ) : BasicMArray<T> (low, high, poolKind)
	{		
		for (int i=low; i<high; ++i)
			new(&(*this)[i]) T;
	}
	
	void Init (int low, int high, PoolKind poolKind ) 
	{
		tblib::Recreate(*this, low, high, poolKind);
	}

	~MSlice () {
		for (int i=m_high-1; i>=m_low; --i)
			(&(*this)[i])->~T();
	}
};

template <typename T>
class MArray : public BasicMArray<T>
{
	MArray (const MArray&);
	MArray& operator= (const MArray&);
	int m_size;
public :
	int      High()   { return m_size; }
	iterator end ()   {	return begin() + (High()-Low()); }	
	MArray       () : BasicMArray<T> (), m_size(Low()) {}
	MArray       ( int low, int high, PoolKind poolKind ) 
		: BasicMArray<T> (low, high, poolKind), m_size(low) {}
	
	void Init (int low, int high, PoolKind poolKind ) 
	{
		tblib::Recreate(*this, low, high, poolKind);
	}

	int Capacity () { return m_high-m_low; }
	~MArray () {		
		for (int i=m_size-1; i>=m_low; --i)
			(&(*this)[i])->~T();
	}

	void  EmplaceBack ()	
		{	assert (m_size<m_high);	new (end()) T;	++m_size;	}
	template<typename A1> void  EmplaceBack (const A1&a1) 
		{	assert (m_size<m_high);	new (end()) T(a1);	++m_size;	}
	template<typename A1, typename A2> void  EmplaceBack (const A1&a1, const A2&a2) 
		{	assert (m_size<m_high);	new (end()) T(a1,a2);	++m_size;	}
	template<typename A1, typename A2, typename A3> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3) 
		{	assert (m_size<m_high);	new (end()) T(a1,a2,a3);	++m_size;	}
	template<typename A1, typename A2, typename A3, typename A4> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4) 
		{	assert (m_size<m_high);	new (end()) T(a1,a2,a3,a4);	++m_size;	}
	template<typename A1, typename A2, typename A3, typename A4, typename A5> void  EmplaceBack (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5) 
		{	assert (m_size<m_high);	new (end()) T(a1,a2,a3,a4,a5);	++m_size;	}


	void PushBack (const T& t)
	{
		assert (m_size<m_high);	
		new (&(*this)[m_size]) T(t);
		++m_size;
	}

	T& operator [] (int index)
	{
		assert (index<m_size);
		return BasicMArray<T>::operator [] (index);
	}
	
	T& operator [] (int index) const
	{
		assert (index<m_size);
		return BasicMArray<T>::operator [] (index);
	}
};

template <typename T, int N>
struct Pool
{
	struct PoolItem : T
	{
		bool free;
		PoolItem* next;
	};

	int poolCount;
	PoolItem items[N];
	PoolItem* freePool;

	T* NewPtr()
	{
		assert(poolCount<N);
		assert(freePool);
		assert(freePool->free);
		T* result = freePool;
		freePool->free = false;
		freePool = freePool->next;
		++poolCount;
		return result;
	}

	void FreePtr (T* p)
	{
		PoolItem* pi = (PoolItem*)p;
		assert (!pi->free);
		pi->free = true;
		pi->next = freePool;
		freePool = pi;
		--poolCount;
	}

	Pool()
	{
		poolCount=0;
		freePool = &items[0];
		for (int i=0; i<N-1; ++i)
		{
			items[i].next = &items[i+1];
			items[i].free = true;
		}
		items[N-1].next = NULL;
		items[N-1].free = true;
	}

	~Pool()
	{
		assert(poolCount==0);
	}
};