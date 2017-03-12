#pragma once

#include "stdint.h"
#include "assert.h"
#include "tbslice.h"
#include "align.h"


#include "tballoc.h"
#include "algorithm.h"
#include "container.h"
#include <new>

// FUCK THE EXCEPTIONS
#ifndef NDEBUG
#define ARRDEBUG
#endif

namespace tblib
{		
	template <typename HOST, typename U>
	class ForwardPointer_
	{
#ifdef ARRDEBUG
	protected :
		HOST* host;
		U* u;

		ForwardPointer_ (HOST* host, U* u) : host(host), u(u) {} 

		bool Valid () const
		{
			return u >= host->begin().u && u<host->end().u;
		}
#else
	protected :
		U* u;
		ForwardPointer_ (HOST* host, U* u) : u(u) { (void)host; } 
#endif
	public :
#ifdef _MSC_VER
		friend typename HOST::container_type;		
		friend typename HOST::iterator;
		friend typename HOST::const_iterator;
#else
		friend class HOST::container_type;		
		friend class HOST::iterator;
		friend class HOST::const_iterator;
#endif
		ForwardPointer_ () {}
		bool operator == (const typename HOST::iterator &other) const { return u==other.u; }
		bool operator == (const typename HOST::const_iterator &other) const { return u==other.u; }
		bool operator != (const typename HOST::iterator &other) const { return u!=other.u; }
		bool operator != (const typename HOST::const_iterator &other) const { return u!=other.u; }
		ForwardPointer_& operator ++ () {  ++u; return *this; }
		ForwardPointer_& operator -- () {  --u; return *this; }
		ForwardPointer_& operator ++ ( int ) { ForwardPointer_ result = *this;  ++u; return result; }
		ForwardPointer_& operator -- ( int ) { ForwardPointer_ result = *this;  --u; return result; }
		ForwardPointer_& operator += ( int n ) {  u+=n; return *this; }
		ForwardPointer_& operator -= ( int n ) {  u-=n; return *this; }
		ForwardPointer_ operator + ( int n ) const { ForwardPointer_ result = *this; result+=n; return result; }
		ForwardPointer_ operator - ( int n ) const { ForwardPointer_ result = *this; result-=n; return result; }
		
		operator U* ()    const { return  u; }
		U& operator * ()  const { return *u; }
		U* operator -> () const { return  u; }
	};

  template <typename HOST, typename U>
	class ReversePointer_ : public ForwardPointer_ <HOST, U>
	{	
		ReversePointer_ (HOST* host, U* u) : ForwardPointer_<HOST, U>(host, u) {} 
	public :
#ifdef _MSC_VER
		friend typename HOST::container_type;		
		friend typename HOST::iterator;
		friend typename HOST::const_iterator;
#else	
		// gcc error: u/sing template type parameter 'HOST' after 'class'		
		friend class HOST::container_type;	
		friend class HOST::iterator;
		friend class HOST::const_iterator;
#endif
		ReversePointer_ () {}
		bool operator == (const typename HOST::reverse_iterator &other) const { return this->u==other.u; }
		bool operator == (const typename HOST::const_reverse_iterator &other) const { return this->u==other.u; }
		bool operator != (const typename HOST::reverse_iterator &other) const { return this->u!=other.u; }
		bool operator != (const typename HOST::const_reverse_iterator &other) const { return this->u!=other.u; }
		ReversePointer_& operator ++ () {  --this->u; return *this; }
		ReversePointer_& operator -- () {  ++this->u; return *this; }
		ReversePointer_& operator ++ ( int ) { ReversePointer_ result = *this;  --this->u; return result; }
		ReversePointer_& operator -- ( int ) { ReversePointer_ result = *this;  ++this->u; return result; }
		ReversePointer_& operator += ( int n ) {  this->u-=n; return *this; }
		ReversePointer_& operator -= ( int n ) {  this->u+=n; return *this; }
		ReversePointer_ operator + ( int n ) const { ReversePointer_ result = *this; result+=n; return result; }
		ReversePointer_ operator - ( int n ) const { ReversePointer_ result = *this; result-=n; return result; }
		
		operator U* ()    const { return  u; }
		U& operator * ()  const { return *u; }
		U* operator -> () const { return  u; }
	};

	template <typename T>
	struct PtrVInfo_ : T
	{
		int index;
	};

	// с полной копипастой проще, поверь(
	template <typename HOST, typename U>
	class ForwardPointerUP_
	{
#ifdef ARRDEBUG
	protected :
		HOST* host;
		PtrVInfo_<U>** u;

		ForwardPointerUP_ (HOST* host, PtrVInfo_<U>** u) : host(host), u(u) {} 

		bool Valid () const
		{
			return u >= host->begin().u && u<host->end().u;
		}
#else
	protected :
		PtrVInfo_<U>** u;
		ForwardPointerUP_ (HOST* host, PtrVInfo_<U>** u) : u(u) { (void)host; } 
#endif
	public :
#ifdef _MSC_VER
		friend typename HOST::container_type;		
		friend typename HOST::iterator;
		friend typename HOST::const_iterator;
#else
		friend class HOST::container_type;		
		friend class HOST::iterator;
		friend class HOST::const_iterator;
#endif

		template <typename X>
		friend class PtrVector;

		ForwardPointerUP_ () {}
		bool operator == (const typename HOST::iterator &other) const { return u==other.u; }
		bool operator == (const typename HOST::const_iterator &other) const { return u==other.u; }
		bool operator != (const typename HOST::iterator &other) const { return u!=other.u; }
		bool operator != (const typename HOST::const_iterator &other) const { return u!=other.u; }
		ForwardPointerUP_& operator ++ () {  ++u; return *this; }
		ForwardPointerUP_& operator -- () {  --u; return *this; }
		ForwardPointerUP_& operator ++ ( int ) { ForwardPointerUP_ result = *this;  ++u; return result; }
		ForwardPointerUP_& operator -- ( int ) { ForwardPointerUP_ result = *this;  --u; return result; }
		ForwardPointerUP_& operator += ( int n ) {  u+=n; return *this; }
		ForwardPointerUP_& operator -= ( int n ) {  u-=n; return *this; }
		ForwardPointerUP_ operator + ( int n ) const { ForwardPointerUP_ result = *this; result+=n; return result; }
		ForwardPointerUP_ operator - ( int n ) const { ForwardPointerUP_ result = *this; result-=n; return result; }
		
		operator U* ()    const { return  *u; }
		U& operator * ()  const { return **u; }
		U* operator -> () const { return  *u; }
	};

  template <typename HOST, typename U>
	class ReversePointerUP_ : public ForwardPointerUP_ <HOST, U>
	{	
		ReversePointerUP_ (HOST* host, PtrVInfo_<U>** u) : ForwardPointerUP_<HOST, U>(host, u) {} 
	public :
#ifdef _MSC_VER
		friend typename HOST::container_type;		
		friend typename HOST::iterator;
		friend typename HOST::const_iterator;
#else	
		// gcc error: u/sing template type parameter 'HOST' after 'class'		
		friend class HOST::container_type;	
		friend class HOST::iterator;
		friend class HOST::const_iterator;
#endif
		template <typename X>
		friend class PtrVector;

		ReversePointerUP_ () {}
		bool operator == (const typename HOST::reverse_iterator &other) const { return this->u==other.u; }
		bool operator == (const typename HOST::const_reverse_iterator &other) const { return this->u==other.u; }
		bool operator != (const typename HOST::reverse_iterator &other) const { return this->u!=other.u; }
		bool operator != (const typename HOST::const_reverse_iterator &other) const { return this->u!=other.u; }
		ReversePointerUP_& operator ++ () {  --this->u; return *this; }
		ReversePointerUP_& operator -- () {  ++this->u; return *this; }
		ReversePointerUP_& operator ++ ( int ) { ReversePointerUP_ result = *this;  --this->u; return result; }
		ReversePointerUP_& operator -- ( int ) { ReversePointerUP_ result = *this;  ++this->u; return result; }
		ReversePointerUP_& operator += ( int n ) {  this->u-=n; return *this; }
		ReversePointerUP_& operator -= ( int n ) {  this->u+=n; return *this; }
		ReversePointerUP_ operator + ( int n ) const { ReversePointerUP_ result = *this; result+=n; return result; }
		ReversePointerUP_ operator - ( int n ) const { ReversePointerUP_ result = *this; result-=n; return result; }
		
		operator U* ()    const { return  *u; }
		U& operator * ()  const { return **u; }
		U* operator -> () const { return  *u; }
	};

	template <typename T, int N>
	class BaseArray_
	{
		union
		{
			char memory [sizeof(T[N])];
			Align<T> aligner;
		};
	protected :
		const T* MemBeg () const
		{			
			const char* tmp = &memory[0];
			return reinterpret_cast<const T*> (tmp);
		}
		
		T* MemBeg () 
		{			
			char* tmp = &memory[0];
			return reinterpret_cast<T*> (tmp);
		}
	public :

#ifdef ARRDEBUG
		T* m_begin;
		BaseArray_ () : m_begin(MemBeg()) {}
#endif
		
		typedef BaseArray_<T,N> container_type;
		typedef T value_type;

		typedef value_type & reference;
		typedef const value_type & const_reference;

		reference operator [] (int index) {
			assert (index>=0 && index<N);			
			return MemBeg()[index];
		}

		const_reference operator [] (int index) const {
			assert (index>=0 && index<N);			
			return MemBeg()[index];
		}

		int Capacity () const
		{
			return N;
		}
	};

	template <typename T, int N>
	class CArray : public BaseArray_ <T,N>
	{
	public :
		typedef CArray<T,N> container_type;
		typedef T value_type;

		typedef value_type & reference;
		typedef const value_type & const_reference;
		typedef ForwardPointer_<CArray, T > iterator;
		typedef ForwardPointer_<const CArray, const T> const_iterator;
		typedef ReversePointer_<CArray, T > reverse_iterator;
		typedef ReversePointer_<const CArray, const T> const_reverse_iterator;

		iterator begin ()  {
			return iterator(this, this->MemBeg());
		}		

		const_iterator begin () const {
			return const_iterator(this, this->MemBeg());
		}		
		
		iterator end ()  {
			return iterator(this, this->MemBeg()+N);
		}		

		const_iterator end () const {
			return const_iterator(this, this->MemBeg()+N);
		}		
		
		reverse_iterator rbegin ()  {
			return reverse_iterator(this, this->MemBeg()+N-1);
		}
		
		const_reverse_iterator rbegin () const {
			return const_reverse_iterator(this, this->MemBeg()+N-1);
		}		
				
		reverse_iterator rend ()  {
			return reverse_iterator(this, this->MemBeg()-1);
		}
		
		const_reverse_iterator rend () const {
			return const_reverse_iterator(this, this->MemBeg()-1);
		}				
		
		reference front () {
			return this->MemBeg()[0];
		}
		
		const_reference front () const {
			return this->MemBeg()[0];
		}		
		
		reference back () {
			return this->MemBeg()[N-1];
		}
		
		const_reference back () const {
			return this->MemBeg()[N-1];
		}	

		CArray () {
			for (int i=0; i<N; ++i)
				new(this->MemBeg()+i) T;
		}

		CArray (const T (&a)[N]) {
			for (int i=0; i<N; ++i)
				new(this->MemBeg()+i) T(a[i]);
		}

		CArray (const CArray& a)
		{
			for (int i=0; i<N; ++i)
				new(this->MemBeg()+i) T(a[i]);
		}

		~CArray () 
		{			
			for (int i=0; i<N; ++i)
				(this->MemBeg()+i)->~T();
		}		

		CArray& operator = (const T (&a)[N]) {
			for (int i=0; i<N; ++i)
				this->MemBeg()[i] = a[i];
			return *this;
		}

		CArray& operator = (const CArray &a) {
			if (this!=&a)	{
				for (int i=0; i<N; ++i)
					this->MemBeg()[i] = a[i];
			}
			return *this;
		}		

		Slice<T> GetSlice(int low, int high)	{
			assert (low>=0 && high<=N);
			return Slice<T>(this->MemBeg()+low, low, high);
		}

		Slice<T> GetSlice ()
		{
			return GetSlice(0, this->Size());
		}

		int Size() const { 
			return N; 
		};		

		template <typename Comp>
		void Sort (Comp comp) 
		{ GetSlice().Sort<Comp>(comp); }
		
		void Sort () 
		{ GetSlice().Sort(DefaultComparator<T>()); }
		
		CArray (const IFStream& s) 
		{
			for (int i=0; i<Size(); ++i)
				new (this->MemBeg()+i) T(tblib::To<T>::From(s));
		}

		friend const OFStream& operator << (const OFStream& s, const CArray& t)
		{
			for (int i=0; i<t.Size(); ++i)
				s << t[i];
			return s;
		}

		void MoveFrom_ (CArray& from)
		{
			for (int i=0; i<N; ++i)
				move(this->MemBeg()[i], from.MemBeg()[i]);
		}
	};
	
	template <typename T, int N>
	class ModArray : public CArray <T,N>
	{
		static int Mod (int index, int n)
		{
			if (n&(n-1))
			{
				index %= n;
				if (index<0) index += n;
				return index;
			} else
				return index & (n-1);
		}
	public :		
		ModArray () : CArray<T,N> () {}		
		ModArray (const T& t) : CArray<T,N> (t) {}
		ModArray (const CArray<T,N> &a) : CArray<T,N> (a) {}
		ModArray (const T (&a)[N]) : CArray<T,N> (a) {}

		T& operator [] (int index) {
			return this->CArray<T,N>::operator[] (Mod(index,N));
		}

		const T& operator [] (int index) const {
			return this->CArray<T,N>::operator[] (Mod(index,N));
		}		

		void MoveFrom_ (ModArray& from)
		{
			for (int i=0; i<N; ++i)
				Move_(this->MemBeg()[i], from.MemBeg()[i]);
		}
	};
	
	template <typename T, int N>
	class Array_ : public BaseArray_ <T,N>
	{
		int m_size;

	public :
		typedef Array_<T,N> container_type;
		typedef T value_type;

		typedef value_type & reference;
		typedef const value_type & const_reference;
		typedef ForwardPointer_<Array_, T > iterator;
		typedef ForwardPointer_<const Array_, const T> const_iterator;
		typedef ReversePointer_<Array_, T > reverse_iterator;
		typedef ReversePointer_<const Array_, const T> const_reverse_iterator;

		iterator begin ()  {
			return iterator(this, this->MemBeg());
		}		

		const_iterator begin () const {
			return const_iterator(this, this->MemBeg());
		}		
		
		iterator end ()  {
			return iterator(this, this->MemBeg()+m_size);
		}		

		const_iterator end () const {
			return const_iterator(this, this->MemBeg()+m_size);
		}		
		
		reverse_iterator rbegin ()  {
			return reverse_iterator(this, this->MemBeg()+m_size-1);
		}
		
		const_reverse_iterator rbegin () const {
			return const_reverse_iterator(this, this->MemBeg()+m_size-1);
		}		
				
		reverse_iterator rend ()  {
			return reverse_iterator(this, this->MemBeg()-1);
		}
		
		const_reverse_iterator rend () const {
			return const_reverse_iterator(this, this->MemBeg()-1);
		}		
		
		reference front () {
			assert(m_size>0);
			return this->MemBeg()[0];
		}
		
		const_reference front () const {
			assert(m_size>0);
			return this->MemBeg()[0];
		}		
		
		reference back () {
			assert(m_size>0);
			return this->MemBeg()[m_size-1];
		}
		
		const_reference back () const {
			assert(m_size>0);
			return this->MemBeg()[m_size-1];
		}		

		Array_ () : m_size(0) {}
	
		int Size() const {
			return m_size;
		}

		T& operator [] (int index) {
			assert (index>=0 && index<m_size);			
			return this->BaseArray_<T,N>::operator[] (index);
		}

		const T& operator [] (int index) const {
			assert (index>=0 && index<m_size);			
			return this->BaseArray_<T,N>::operator[] (index);
		}				

		Slice<T> GetSlice(int low, int high)	{
			assert (low>=0 && high<=this->Size());
			return tblib::Slice<T>(this->MemBeg()+low, low, high);
		}		

		Slice<T> GetSlice ()
		{
			return GetSlice(0, this->Size());
		}

		template <typename Comp>
		void Sort (Comp comp) 
		{ GetSlice().Sort<Comp>(comp); }
		
		void Sort () 
		{ GetSlice().Sort(DefaultComparator<T>()); }

		template <typename U>
		friend class Container_;

		template <typename U, int V>
		friend class Array;
		
		
	private : 

		iterator PrepareSize (int a_size) 
		{
			assert (a_size>=0 && a_size<=Capacity());
			iterator result = this->end();
			m_size = a_size;
			return result;
		}
	};		

	template <typename T, int N>
	class Array : public Container_ <Array_<T,N> >
	{
		// шаблонного тайпдефа нет, поэтому пишем хуиту и копируем каструкторы
		typedef Array_<T,N> base; 
		// поскольку ёбаный пропроцессор эту запятую между параметрами шаблона 
		// восприринимает как запятую между параметрами макроса, то приходится
		// лепить лишний тюпедеф
		// сишное дерьмо мамонта опять мешает жить, короче
	public :	
		CONTAINER_CONSTRUCTORS(Array, base)

		void MoveFrom_ (Array& from)
		{
			this->m_size = from.m_size;
			for (int i=0; i<this->m_size; ++i)
				move(this->MemBeg()[i], from.MemBeg()[i]);
		}				
	};
}
