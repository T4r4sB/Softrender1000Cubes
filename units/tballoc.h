#pragma once
#include "stdint.h"
#include "stddef.h"
#include "algorithm.h"

namespace tblib
{
	class Allocator
	{		
		static const int MAX_LN = 32;

		struct BlockHeader
		{
			bool free;			
			uint8_t ln2sz;
      BlockHeader *prev, *next;
      BlockHeader *prevFree, *nextFree;
		};
		
		char *memory, *endAlloc, *end, *maxEnd;

		BlockHeader* last;

		int allocated;

		BlockHeader* firstFree [MAX_LN];

		void Align16 (size_t& s);
		int BhAlignedSize ();
		int NeedLn (int asize);
		BlockHeader* PtrToBh (void* ptr);
		void* BhToPtr (BlockHeader* bh);
		BlockHeader* EndBH ();
		ptrdiff_t BlockDist (BlockHeader* b1, BlockHeader* b2) ;
		int BlockSize (int ln2sz);
		bool NoAfterEnd ();
		bool CheckLastBlock1Time ();
		void CheckLastBlock ();		
		void CheckInitialization ();
		bool FirstFreeCorrect ();

	public :

		void* Malloc (int size);
		void Free (void* ptr);
		bool ReallocInplace (void* ptr, int size);
		bool Empty ();
		int AllocatedMemory ();
		int MaxAlloc ();
		int UsedMemory ();
	};
}

extern tblib::Allocator tbAlloc;


