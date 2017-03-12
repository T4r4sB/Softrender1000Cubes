#pragma once

namespace std
{	
	template <typename T1, typename T2>
	struct double_ref
	{
		T1& t1;
		T2& t2;
		double_ref(T1 &t1, T2 &t2):t1(t1),t2(t2){}
	};

	template <typename T1, typename T2>
	class double_iterator
	{
		T1 t1;
		T2 t2;
	public :
		double_iterator(T1 t1, T2 t2):t1(t1),t2(t2){}
		double_ref<T1,T2> operator *() const {return double_ref<T1,T2>(*t1,*t2);}
		double_iterator<T1,T2> operator +(int i) const {return double_iterator<T1,T2>(t1+i, t2+i);}
		double_iterator<T1,T2> operator -(int i) const {return double_iterator<T1,T2>(t1-i, t2-i);}
		
		double_iterator<T1,T2> &operator ++ ()
		{
			++t1; 
			++t2;
			return *this;
		}

		double_iterator<T1,T2> operator ++ (int)
		{
			T1 ot1=t1;
			T2 ot2=t2;
			++t1; 
			++t2;
			return double_iterator<T1,T2>(ot1, ot2);
		}

		double_iterator<T1,T2> & operator += (int i)
		{
			t1+=i;
			t2+=i;
			return *this;
		}

		bool Comparable (double_iterator<T1,T2> di) const
		{
			return (t1<=di.t1 && t2<=di.t2) || (t1>=di.t1 && t2>=di.t2);
		}

		bool operator <  (double_iterator<T1,T2> di) const { assert (Comparable(di)); return t1< di.t1; }
		bool operator >  (double_iterator<T1,T2> di) const { assert (Comparable(di)); return t1> di.t1; }
		bool operator <= (double_iterator<T1,T2> di) const { assert (Comparable(di)); return t1<=di.t1; }
		bool operator >= (double_iterator<T1,T2> di) const { assert (Comparable(di)); return t1>=di.t1; }
		bool operator == (double_iterator<T1,T2> di) const { assert (Comparable(di)); return t1==di.t1 && t2==di.t2; }
		bool operator != (double_iterator<T1,T2> di) const { assert (Comparable(di)); return t1!=di.t1 || t2!=di.t2; }

	};

	template <typename T1, typename T2>
	double_iterator<T1,T2> make_double_iterator (T1 t1, T2 t2)
	{
		return double_iterator<T1,T2>(t1, t2);
	}

	template <typename T1, typename T2>
	struct const_double_ref
	{
		const T1& t1;
		const T2& t2;
		const_double_ref(T1 &t1, T2 &t2):t1(t1),t2(t2){}
	};

	template <typename T1, typename T2>
	class const_double_iterator
	{
		T1 t1;
		T2 t2;
	public :
		const_double_iterator(T1 t1, T2 t2):t1(t1),t2(t2){}
		const_double_ref<T1,T2> operator *() const {return const_double_ref<T1,T2>(*t1,*t2);}
		const_double_iterator<T1,T2> operator +(int i) const {return const_double_iterator<T1,T2>(t1+i, t2+i);}
		const_double_iterator<T1,T2> operator -(int i) const {return const_double_iterator<T1,T2>(t1-i, t2-i);}
		
		const_double_iterator<T1,T2> &operator ++ ()
		{
			++t1; 
			++t2;
			return *this;
		}

		const_double_iterator<T1,T2> operator ++ (int)
		{
			T1 ot1=t1;
			T2 ot2=t2;
			++t1; 
			++t2;
			return double_iterator<T1,T2>(ot1, ot2);
		}		

		const_double_iterator<T1,T2> & operator += (int i)
		{
			t1+=i;
			t2+=i;
			return *this;
		}

		bool operator < (const_double_iterator<T1,T2> di) const { return t1<di.t1 }
		bool operator > (const_double_iterator<T1,T2> di) const { return t1>di.t1 }
		bool operator <= (const_double_iterator<T1,T2> di) const { return t1<=di.t1 }
		bool operator >= (const_double_iterator<T1,T2> di) const { return t1>=di.t1 }
		bool operator == (const_double_iterator<T1,T2> di) const { return t1==di.t1 && t2==di.t2 }
		bool operator != (const_double_iterator<T1,T2> di) const { return t1!=di.t1 || t2!=di.t2 }

	};

	template <typename T1, typename T2>
	const_double_iterator<T1,T2> make_const_double_iterator (T1 t1, T2 t2)
	{
		return const_double_iterator<T1,T2>(t1, t2);
	}
};