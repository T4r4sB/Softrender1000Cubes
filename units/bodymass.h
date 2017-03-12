#pragma once

#include "fixed.h"

namespace tbphys
{
	struct Mass
	{
		int order;
		Fixed v;
		inline Mass () {}
		inline Mass (int order, Fixed v) : order(order), v(v) {}
		inline Mass operator * (Fixed s) const {return Mass (order, v*s);}
		inline Mass operator / (Fixed s) const {return Mass (order, v/s);}
		
		Mass (const tblib::IFStream& s) : order(tblib::To<int>::From(s)),v(tblib::To<Fixed>::From(s)) {}
		friend const tblib::OFStream& operator << (const tblib::OFStream& s, const Mass& m) 
			{ return s << m.order << m.v; }
	};

	inline Fixed Fraction1 (const Mass &m1, const Mass &m2)
	{
		if (m1.order > m2.order) return fx1;
		else if (m1.order < m2.order) return fx0;
		else return m1.v/(m1.v+m2.v);
	}		
};