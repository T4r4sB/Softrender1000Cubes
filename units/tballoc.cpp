#include "stdint.h"
#include "tballoc.h"
#include "stdint.h"
#include "stddef.h"
#include "algorithm.h"
#include "assert.h"
#include "malloc.h"
#include <cstring>

namespace tblib
{
	void Allocator::Align16 (size_t& s)
	{
		s += 15;
		s &= (~15);
	}

	int Allocator::BhAlignedSize ()
	{
		size_t s = sizeof(BlockHeader);
		Align16(s);
		return int(s);
	}

	int Allocator::NeedLn (int aSize)
	{
		if (aSize<=0) 
			return -1;

		int size = aSize-1;			
		int ln2sz = size==0 ? 0 : IntLn2(size)+1;

		if (ln2sz>=MAX_LN)
			return -1;

		return ln2sz;
	}

	Allocator::BlockHeader* Allocator::PtrToBh (void* ptr)
	{			
		BlockHeader* bh = reinterpret_cast<BlockHeader*> 
			(reinterpret_cast<char*>(ptr)-BhAlignedSize());
		assert (bh->ln2sz >= 0 && bh->ln2sz < MAX_LN); 
		return bh;
	}

	void* Allocator::BhToPtr (BlockHeader* bh)
	{			
		return static_cast<void*>
			(reinterpret_cast<char*>(bh)+BhAlignedSize());
	}

	Allocator::BlockHeader* Allocator::EndBH ()
	{
		return reinterpret_cast<BlockHeader*>(endAlloc);
	}

  ptrdiff_t Allocator::BlockDist (BlockHeader* b1, BlockHeader* b2) 
	{
		return int(reinterpret_cast<char*>(b2) - reinterpret_cast<char*>(b1));
	}

	int Allocator::BlockSize (int ln2sz)
	{
		return (1<<ln2sz) + BhAlignedSize();
	}

	bool Allocator::NoAfterEnd ()
	{
		if (reinterpret_cast<char*>(last) >= endAlloc)
			return false;
		if (last==NULL)
		{
			for (int i=0; i<MAX_LN; ++i)
				if (firstFree[i])
					return false;
		} else
		{
			for (int i=0; i<MAX_LN; ++i)
				if (firstFree && firstFree[i]>last)
					return false;
		}
		return true;
	}

	bool Allocator::CheckLastBlock1Time ()
	{
		assert (last==NULL || last->next==NULL);
		assert (NoAfterEnd());
		if (last && last->free)
		{				
			assert (last->prev == NULL || BlockSize (last->prev->ln2sz) == BlockDist(last->prev, last));

			BlockHeader* lp = last->prevFree;
			BlockHeader* ln = last->nextFree;
			assert (lp==NULL || lp->ln2sz == last->ln2sz);
			assert (ln==NULL || ln->ln2sz == last->ln2sz);
			if (lp) lp->nextFree = ln;
			if (ln) ln->prevFree = lp;

			if (firstFree[last->ln2sz] == last)
				firstFree[last->ln2sz] = ln;

			endAlloc -= BlockSize(last->ln2sz);
			last = last->prev;
			if (last)
				last->next=NULL;
			
			assert (last==NULL || last->next==NULL);

			return true;
		} else
			return false;
	}		

	void Allocator::CheckLastBlock ()
	{
		if (!CheckLastBlock1Time ()) return;
		if (!CheckLastBlock1Time ()) return;
		if (!CheckLastBlock1Time ()) return;
		if (!CheckLastBlock1Time ()) return;
		if (!CheckLastBlock1Time ()) return;
		if (!CheckLastBlock1Time ()) return;
		if (!CheckLastBlock1Time ()) return;
		if (!CheckLastBlock1Time ()) return;
	}

	bool Allocator::FirstFreeCorrect ()
	{		
		static int check=0;
		++check;

		for (int i=0; i<MAX_LN; ++i)
			if (firstFree[i] && firstFree[i]->ln2sz!=i)
				{ assert (false); return false; }
		return true;
	}
	
	void Allocator::CheckInitialization ()
	{
		if (!memory)
		{
			const int full_size = 64*0x100000;
			memory   = reinterpret_cast<char*>(::malloc(full_size)); // надо как-то уйти от хардкода
			end      = memory+full_size;
			endAlloc = memory;
			maxEnd   = memory;
			Align16 (reinterpret_cast<size_t&>(endAlloc));
		}

		assert (FirstFreeCorrect());
	}

	void* Allocator::Malloc (int size)
	{
		CheckInitialization ();
		CheckLastBlock ();
		void* result = NULL;
		BlockHeader* newBlock=NULL;
		int ln2sz = NeedLn(size);

		
		if (size>0x100000)
		{
			result = ::malloc(size);
		}	else
		{
			if (ln2sz<0)
				return NULL;

			if (firstFree[ln2sz])
			{
				newBlock = firstFree[ln2sz];		
				assert ( newBlock==last || 
					BlockSize(newBlock->ln2sz) == BlockDist(newBlock, newBlock->next));
				firstFree[ln2sz] = firstFree[ln2sz]->nextFree;
				if (firstFree[ln2sz])
					firstFree[ln2sz]->prevFree = NULL;
				assert (newBlock->free);
			} else
			{
				if (end - endAlloc < BlockSize(ln2sz))
				{
					//ой неудача, придётся у оси выклянчивать(
					result = ::malloc(size);
					//result = NULL;					
				} else
				{
					newBlock = EndBH ();
					newBlock->next = NULL;
					newBlock->prev = last;
					if (last)
						last->next = newBlock;

					last = newBlock;
					endAlloc += BlockSize(ln2sz);
					Enlarge(maxEnd, endAlloc);
				}
			}
		}
		if (newBlock)
		{
			newBlock->ln2sz = uint8_t(ln2sz);
			newBlock->free = false;
			result = BhToPtr(newBlock);
			allocated += BlockSize(ln2sz);
		}
#ifndef NDEBUG
		if (result)
			memset(result, 0xcc, size);
#endif
		return result;
	}

	void Allocator::Free (void* ptr)
	{			
		CheckInitialization ();
		CheckLastBlock ();
		if (ptr==NULL)
			return;

		if (ptr>=memory && ptr < endAlloc)
		{
			// мы в аллокаторе
			BlockHeader* curBlock = PtrToBh(ptr);
			assert (!curBlock->free);
			allocated -= BlockSize(curBlock->ln2sz);

			if (curBlock == last)
			{
				assert (last->prev == NULL || 
					BlockSize(last->prev->ln2sz) == BlockDist(last->prev, last));
				endAlloc -= BlockSize(curBlock->ln2sz);
				last = last->prev;
				if (last)
					last->next=NULL;
			} else
			{
				BlockHeader* tmp = firstFree [curBlock->ln2sz];
				firstFree [curBlock->ln2sz] = curBlock;
				if (tmp)
					tmp->prevFree = curBlock;
				assert (tmp == NULL || tmp->ln2sz == curBlock->ln2sz);
				curBlock->nextFree = tmp;
				curBlock->prevFree = NULL;
				curBlock->free = true;
			}
		} else
		{
			// блок выделен осью
			::free(ptr);
		}
		
		assert (last==NULL || last->next==NULL);
	}

	bool Allocator::ReallocInplace (void* ptr, int size)
	{
		CheckInitialization ();
		CheckLastBlock ();
		
		if (ptr>=memory && ptr < endAlloc)
		{
			// мы в аллокаторе
			if (size==0)
			{
				Free(ptr);
				return true;
			}

			BlockHeader* curBlock = PtrToBh(ptr);
			assert (!curBlock->free);
			int ln2sz = NeedLn(size);

			if (ln2sz<0)
				return false;

			if (curBlock==last)
			{
				int diff = BlockSize(ln2sz) - BlockSize(curBlock->ln2sz);
				if (end - endAlloc < diff)
					return false;
				curBlock->ln2sz = uint8_t(ln2sz);
				allocated += diff;
				endAlloc += diff;
				Enlarge(maxEnd, endAlloc);
				return true;
			} else 
			{
				return (ln2sz <= curBlock->ln2sz); // реальный размер не трогаем даже
			}
		} else
		{
			// блок выделен осью, хуй знает, можно ли его реаллоцировать
			return false;
		}
	}

	bool Allocator::Empty ()
	{
		CheckInitialization ();
		return allocated==0;
	}		

	int Allocator::AllocatedMemory ()
	{
		CheckInitialization ();
		return allocated;
	}

	int Allocator::UsedMemory ()
	{
		CheckInitialization ();
		return int(endAlloc - memory);
	}

	int Allocator::MaxAlloc ()
	{
		return int(maxEnd - memory);
	}
}

extern tblib::Allocator tbAlloc;
tblib::Allocator tbAlloc;