#include "fixed.h"
#include "math.h"
#include "point.h"
#include "graph.h"
#include "tbarr.h"
#include "polygon.h"

namespace tbal 
{	
	void Poly (const tbal::Bitmap &b, tblib::Slice<const Point> points, Color color)
	{
		tblib::ArrayPoly (b, color, points);
	}

	const int tc=32;
	Point tt[tc+1];
	bool checkedtt = false;

	void CheckTable ()
	{
		if (!checkedtt)
		{	
			tt[0] = Point(fx1, fx0);
			for (int i=1; i<tc; ++i)
				tt[i] = SinCos(Fixed(i,tc)*fxPi);
			tt[tc] = Point(-fx1, fx0);
			checkedtt = true;
		}
	}
	
	bool CircleInBuffer (const tbal::Bitmap &b, const Point &center, Fixed r)
	{
		return (int(center.x+r)>=0 && int(center.y+r)>=0 && int(center.x-r)<b.SizeX() && int(center.y-r)<b.SizeY());
	}

	void Circle(const tbal::Bitmap &b, const Point &center, Fixed ri, Fixed ro, tbal::Color color)
	{
		if (!CircleInBuffer(b, center, ro))
			return;

		CheckTable();
		if (ri==fx0)
		{
			tblib::CArray<Point,tc*2> cur;

			for (int i=0; i<tc; ++i)
			{
				cur[i   ]=center+tt[i]*ro;
				cur[i+tc]=center-tt[i]*ro;
			}
			Poly(b, cur.GetSlice(0,cur.Size()), color);
		} else {
			tblib::CArray<Point,4> cur;
			for (int i=0; i<tc; ++i)
			{
				cur[0] = center+tt[i  ]*ri;
				cur[1] = center+tt[i  ]*ro;
				cur[2] = center+tt[i+1]*ro;
				cur[3] = center+tt[i+1]*ri;
				Poly(b, cur.GetSlice(0,4), color);
				cur[0] = center+(center-cur[0]);
				cur[1] = center+(center-cur[1]);
				cur[2] = center+(center-cur[2]);
				cur[3] = center+(center-cur[3]);
				Poly(b, cur.GetSlice(0,4), color);
			}
		}
	}

	void Pie (const tbal::Bitmap &b, const Point &center, Fixed r, const Point &a1, const Point &a2, tbal::Color color)
	{
		if (!CircleInBuffer(b, center, r))
			return;
		CheckTable();		
		const Point na1 = a1.Normalize();
		const Point na2 = a2.Normalize();

		tblib::Array<Point,tc+2> cur;
		const Point diff = na1&~na2;
		bool ok=true;
		int i;

		for (i=0; i<=tc; ++i)
		{
			Point da = na1&tt[i];
			if (tt[i].x<diff.x) {da=na2;ok=false;}
			cur.EmplaceBack(center+da*r);
			if (!ok)break;
		}
		cur.EmplaceBack(center);
		Poly(b, cur.GetSlice(0,cur.Size()), color);
	}

	void Line(const tbal::Bitmap &b, const Point &p1, const Point &p2, Fixed width, tbal::Color color, bool sh1, bool sh2)
	{
		width *= fxHalf;
		Point delta = p2-p1;
		if (delta==Point(fx0,fx0))
		{
			if (!sh1 || !sh2)
				Circle(b, p1, 0, width, color);
		} else 
		{
			delta = delta.Normalize().Rot90();
			const Point wdelta = delta*width;

			tblib::CArray<Point, 4> points;
			points[0] = p1 - wdelta; 
			points[1] = p1 + wdelta; 
			points[2] = p2 + wdelta; 
			points[3] = p2 - wdelta; 
			Poly(b, points.GetSlice(0,4), color);

			if (!sh1) Pie(b, p1, width, -delta,  delta, color);
			if (!sh2) Pie(b, p2, width,  delta, -delta, color);
		}
	}

	void Arc (const tbal::Bitmap &b, const Point &center, Fixed r, Fixed width, const Point &a1, const Point &a2, tbal::Color color, bool sh1, bool sh2)
	{
		if (!CircleInBuffer(b, center, r+width))
			return;

		CheckTable();
		if (r<Fixed(1,10)) return;
		width /= Fixed(2);
		const Fixed r1=r-width;
		const Fixed r2=r+width;

		//const Point na1 = a1*(fx1/a1.Length() + a1.Length()/(a1*a1))*fxHalf;
		//const Point na2 = a2*(fx1/a2.Length() + a2.Length()/(a2*a2))*fxHalf;
		
		const Point na1 = a1.Normalize();
		const Point na2 = a2.Normalize();
		
		tblib::CArray<Point,4> cur;
		bool ok=true;
		int i;
		Point da(0,0), oda(0,0);
		for (i=0; i<=tc; ++i)
		{
			oda = da;
			da  = na1&tt[i];
			if ((da&~na2).y>fx0) {da=na2;ok=false;}

			if (i>0)
			{
				cur[0] = center+ da*r2;
				cur[1] = center+ da*r1;
				cur[2] = center+oda*r1;
				cur[3] = center+oda*r2;
				Poly(b, cur.GetSlice(0,4), color);				
			}

			if (!ok)break;
		}
		
		if (!sh1) Pie(b, center+a1*r, width, -na1, na1, color);
		if (!sh2) Pie(b, center+a2*r, width, na2, -na2, color);
	}
};
