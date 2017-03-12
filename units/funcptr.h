#pragma once
#include "assert.h"

namespace tblib
{
	class Empty {};

	template <typename Sig> 
	struct SigInfo
	{
		enum { argCount=-1 };
		typedef Empty R;
		typedef Empty A1;
		typedef Empty A2;
		typedef Empty A3;
		typedef Empty A4;
		typedef Empty A5;
		typedef Empty A6;
		typedef Empty A7;
		typedef Empty A8;
	};
	
	template <typename TR> 
	struct SigInfo<TR()>
	{
		enum { argCount=0 };
		typedef TR    R;
		typedef Empty A1;
		typedef Empty A2;
		typedef Empty A3;
		typedef Empty A4;
		typedef Empty A5;
		typedef Empty A6;
		typedef Empty A7;
		typedef Empty A8;
	};
	
	template <typename TR, typename TA1> 
	struct SigInfo<TR(TA1)>
	{
		enum { argCount=1 };
		typedef TR    R;
		typedef TA1   A1;
		typedef Empty A2;
		typedef Empty A3;
		typedef Empty A4;
		typedef Empty A5;
		typedef Empty A6;
		typedef Empty A7;
		typedef Empty A8;
	};	
	
	template <typename TR, typename TA1, typename TA2> 
	struct SigInfo<TR(TA1,TA2)>
	{
		enum { argCount=2 };
		typedef TR    R;
		typedef TA1   A1;
		typedef TA2   A2;
		typedef Empty A3;
		typedef Empty A4;
		typedef Empty A5;
		typedef Empty A6;
		typedef Empty A7;
		typedef Empty A8;
	};	
	
	template <typename TR, typename TA1, typename TA2, typename TA3> 
	struct SigInfo<TR(TA1,TA2,TA3)>
	{
		enum { argCount=3 };
		typedef TR    R;
		typedef TA1   A1;
		typedef TA2   A2;
		typedef TA3   A3;
		typedef Empty A4;
		typedef Empty A5;
		typedef Empty A6;
		typedef Empty A7;
		typedef Empty A8;
	};	
	
	template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4> 
	struct SigInfo<TR(TA1,TA2,TA3,TA4)>
	{
		enum { argCount=4 };
		typedef TR    R;
		typedef TA1   A1;
		typedef TA2   A2;
		typedef TA3   A3;
		typedef TA4   A4;
		typedef Empty A5;
		typedef Empty A6;
		typedef Empty A7;
		typedef Empty A8;
	};	
	
	template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4, typename TA5> 
	struct SigInfo<TR(TA1,TA2,TA3,TA4,TA5)>
	{
		enum { argCount=5 };
		typedef TR    R;
		typedef TA1   A1;
		typedef TA2   A2;
		typedef TA3   A3;
		typedef TA4   A4;
		typedef TA5   A5;
		typedef Empty A6;
		typedef Empty A7;
		typedef Empty A8;
	};
	
	template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4, typename TA5, typename TA6> 
	struct SigInfo<TR(TA1,TA2,TA3,TA4,TA5,TA6)>
	{
		enum { argCount=6 };
		typedef TR    R;
		typedef TA1   A1;
		typedef TA2   A2;
		typedef TA3   A3;
		typedef TA4   A4;
		typedef TA5   A5;
		typedef TA6   A6;
		typedef Empty A7;
		typedef Empty A8;
	};
	
	template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4, typename TA5, typename TA6, typename TA7> 
	struct SigInfo<TR(TA1,TA2,TA3,TA4,TA5,TA6,TA7)>
	{
		enum { argCount=7 };
		typedef TR    R;
		typedef TA1   A1;
		typedef TA2   A2;
		typedef TA3   A3;
		typedef TA4   A4;
		typedef TA5   A5;
		typedef TA6   A6;
		typedef TA7   A7;
		typedef Empty A8;
	};	
	
	template <typename TR, typename TA1, typename TA2, typename TA3, typename TA4, typename TA5, typename TA6, typename TA7, typename TA8> 
	struct SigInfo<TR(TA1,TA2,TA3,TA4,TA5,TA6,TA7,TA8)>
	{
		enum { argCount=8 };
		typedef TR    R;
		typedef TA1   A1;
		typedef TA2   A2;
		typedef TA3   A3;
		typedef TA4   A4;
		typedef TA5   A5;
		typedef TA6   A6;
		typedef TA7   A7;
		typedef TA8   A8;
	};

	template <typename F>
	class FPtr
	{
		F* ptr;
	public :
		FPtr () : ptr(NULL) {}
		FPtr (F* ptr) : ptr(ptr) {}
		FPtr (const FPtr& f) : ptr(f.ptr) {}
		FPtr& operator=(F* ptr) { this->ptr=ptr; return *this; }
		FPtr& operator=(const FPtr& f) { this->ptr=f.ptr; return *this; }

		typedef typename SigInfo<F>::R  R;
		typedef typename SigInfo<F>::A1 A1;
		typedef typename SigInfo<F>::A2 A2;
		typedef typename SigInfo<F>::A3 A3;
		typedef typename SigInfo<F>::A4 A4;
		typedef typename SigInfo<F>::A5 A5;
		typedef typename SigInfo<F>::A6 A6;
		typedef typename SigInfo<F>::A7 A7;
		typedef typename SigInfo<F>::A8 A8;

		R operator () () { assert (ptr); return ptr(); }		
		R operator () (A1 a1) { assert (ptr); return ptr(a1); }
		R operator () (A1 a1, A2 a2) { assert (ptr); return ptr(a1,a2); }
		R operator () (A1 a1, A2 a2, A3 a3) { assert (ptr); return ptr(a1,a2,a3); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4) { assert (ptr); return ptr(a1,a2,a3,a4); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) { assert (ptr); return ptr(a1,a2,a3,a4,a5); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) { assert (ptr); return ptr(a1,a2,a3,a4,a5,a6); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) { assert (ptr); return ptr(a1,a2,a3,a4,a5,a6,a7); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) { assert (ptr); return ptr(a1,a2,a3,a4,a5,a6,a7,a8); }
	};

};