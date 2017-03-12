#pragma once
#include "align.h"
#include "assert.h"

namespace tblib
{			
	template <typename T>
	const T& Max (const T& t, const T& u)
	{
		return t>u ? t : u;
	}
			
	template <typename T>
	const T& Min (const T& t, const T& u)
	{
		return t<u ? t : u;
	}
	
	template <typename T>
	class HasMoveMethod_
	{
		typedef char y;
		typedef y n[2];

		template <typename U, U> struct ReallyHas;
		template <typename C> static y& Test(ReallyHas <void (C::*)(C&), &C::MoveFrom_>*);
		template <typename> static n& Test(...);
	public:
    static bool const value = sizeof(Test<T>(0)) == sizeof(y);	
	};

	template <typename T, bool B=HasMoveMethod_<T>::value>
	struct Mover_;
	
	template <typename T>
	struct Mover_ <T, true>
	{
		static void Do (T& to, T& from) 
		{ 
			to.MoveFrom_(from); 
		}
	};
	
	template <typename T>
	struct Mover_ <T, false>
	{
		static void Do (T& to, T& from) 
		{ 
			new (&to) T(from);
			from.~T(); 
		}
	};
	
	template <typename T>
	void Move_ (T& to, T&from)
	{
		assert (&to != &from);
		Mover_<T>::Do(to, from);
	}

	template <typename T>
	void Swap (T& t, T& u)
	{
		if (&t == &u)
			return;
		union
		{
			char c[sizeof(T)];
			Align<T> a;
		} utmp;
		char* tmpc = &utmp.c[0];
		T& tmp = *reinterpret_cast<T*>(tmpc);
    Move_ (tmp, t);
		Move_ (t, u);
		Move_ (u, tmp);
	}

	template <typename T, typename U>
	void Enlarge(T& t, const U& u)
	{ if (t<u) t=u; }
	
	template <typename T, typename U>
	void Shrink(T& t, const U& u)
	{ if (t>u) t=u; }
	
	template <typename T, typename U>
	void InBound(T& t, const U& u1, const U& u2)
	{ if (t<u1) t=u1; else if (t>u2) t=u2; }

	inline int IntLn2 (int x)
	{
		int result = 0;
		if (x >= (1<<16)) { x>>=16; result += 16; } 
		if (x >= (1<< 8)) { x>>= 8; result +=  8; } 
		if (x >= (1<< 4)) { x>>= 4; result +=  4; } 
		if (x >= (1<< 2)) { x>>= 2; result +=  2; } 
		if (x >= (1<< 1)) { x>>= 1; result +=  1; } 
		return result;
	}

	template <typename T> 
	class TmpSaver
	{
		T& ref;
		T val;
		bool mustsave;
		TmpSaver (const TmpSaver&);
		TmpSaver& operator = (const TmpSaver&);

	public :
		TmpSaver (T& ref) : ref(ref), val(ref), mustsave(true) {}
		~TmpSaver () { if (mustsave) ref=val; }
		void Approve () { mustsave=false; }
	};

	template <typename T> void Recreate (T& t) 
		{ t.~T(); new(&t) T(); }

	template <typename T, typename A1> void Recreate (T& t, const A1& a1)
		{ t.~T(); new(&t) T(a1); }
		
	template <typename T, typename A1, typename A2> void Recreate (T& t, const A1& a1, const A2& a2)
		{ t.~T(); new(&t) T(a1, a2); }
		
	template <typename T, typename A1, typename A2, typename A3> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3)
		{ t.~T(); new(&t) T(a1, a2, a3); }
		
	template <typename T, typename A1, typename A2, typename A3, typename A4> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4)
		{ t.~T(); new(&t) T(a1, a2, a3, a4); }
		
	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6); }
	
	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7, a8); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7, a8, a9); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10> void Recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
};