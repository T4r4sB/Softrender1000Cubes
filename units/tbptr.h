#pragma once

#include "tbAlloc.h"
#include "tbstream.h"
#include "tbdynarr.h"

namespace tblib
{	
	template <typename T>
	class UniquePtr
	{
		typedef char check[sizeof(T)]; // ololo

		T* ptr;

		UniquePtr (const UniquePtr&);
		UniquePtr& operator = (const UniquePtr&);

		inline T* GetMem ()
		{
			return reinterpret_cast<T*> (tbAlloc.Malloc(sizeof(T)));
		}

	public :

		inline UniquePtr () : ptr (tbAlloc.Malloc(sizeof(T))) 
			{ assert (ptr); new (ptr) T; }

		inline ~UniquePtr () { assert(ptr); ptr->~T(); tbAlloc.Free(ptr); }

		inline void MoveFrom_ (UniquePtr& u) { ptr = u.ptr; }

		inline operator T*    () { assert(ptr); return  ptr; }
		inline T* operator -> () { assert(ptr); return  ptr; }
		inline T& operator *  () { assert(ptr); return *ptr; }

		template <typename A>
		inline UniquePtr (const A& a) : ptr (GetMem()) 
			{ assert (ptr); new (ptr) T(a); }
			
		template <typename A1, typename A2>
		inline UniquePtr (const A1& a1, const A2& a2) : ptr (GetMem())  
			{ assert (ptr); new (ptr) T(a1,a2); }
			
		template <typename A1, typename A2, typename A3>
		inline UniquePtr (const A1& a1, const A2& a2, const A3& a3) : ptr (GetMem()) 
			{ assert (ptr); new (ptr) T(a1,a2,a3); }
			
		template <typename A1, typename A2, typename A3, typename A4>
		inline UniquePtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4) : ptr (GetMem()) 
			{ assert (ptr); new (ptr) T(a1,a2,a3,a4); }
			
		template <typename A1, typename A2, typename A3, typename A4, typename A5>
		inline UniquePtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) : ptr (GetMem()) 
			{ assert (ptr); new (ptr) T(a1,a2,a3,a4,a5); }
			
		template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
		inline UniquePtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) : ptr (GetMem()) 
			{ assert (ptr); new (ptr) T(a1,a2,a3,a4,a5,a6); }

		inline UniquePtr (const IFStream& f) : ptr (GetMem())  
		{
			assert(ptr);
			new (ptr) T(f);
		}

		inline friend const OFStream& operator << (const OFStream& f, const UniquePtr &u)
		{
			f << *u;
		}
	};
};