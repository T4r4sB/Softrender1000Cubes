#include "tbarr2.h"
#include "point.h"

namespace tblib
{	
	template <typename T>	
	struct FillArray
	{
    const Array2DWindow<T>& window;
		typename Array2DWindow<T>::Line line;
		T color;
		int y, ly;

		FillArray(const Array2DWindow<T>& window, T color) : window(window), color(color) {}

		void Init  (int hy1, int hy2) 
		{ 
			y  = Max(hy1, 0); 
			ly = Min(hy2, window.SizeY());
			line = window[y]; 
		}

		void Next ()
		{  
			window.ProcessLine(line); 
			++y; 
		}

		bool Valid ()
		{
			return y<ly;
		}

		int Line ()
		{
			return y;
		}

		void Fill (int x1, int x2) 
		{	
			int ax1 = Max(x1, 0);
			int ax2 = Min(x2, window.SizeX());
			for (int i=ax1; i<ax2; ++i) 
				line[i] = color;	
		}

	private :
		FillArray& operator = (const FillArray&); // компилятор, заткнись

	};


	template <typename T>	
	void AbstractPoly (T t, tblib::Slice<const Point> points)
	{
		int hy1 = +1000;
		int hy2 = -1000;
		int M0 = points.Size();

		bool firstp=true;
		for (int i=points.Low(); i<points.High(); ++i)
		{
			if (firstp || int(points[i].y)<hy1)
			{
				hy1 = int(points[i].y);
				M0 = i;
			} 
			if (firstp || int(points[i].y)>hy2)
			{
				hy2 = int(points[i].y);
			}
			firstp=false;
		}
		int ip1, ip2, in1=M0, in2=M0;
		
		Fixed x1, x2, dx1, dx2;

		for (t.Init(hy1, hy2); t.Valid(); t.Next() )
		{
			int y = t.Line();
			if (y>=int(points[in1].y)) // переход к следующему отрезку
			{
				do
				{
					ip1 = in1;
					in1 = in1==points.Low() ? points.High()-1 : in1-1;
				} while (y>=int(points[in1].y));

				if (y+1==int(points[in1].y)) 
				{
					dx1 = points[in1].x>points[ip1].x ? Fixed(1000) : Fixed(-1000);
					x1=points[ip1].x + (points[in1].x-points[ip1].x)
						*((Fixed(y+1) - points[ip1].y)
						/( points[in1].y  - points[ip1].y));
				} else
				{
					dx1 = (points[in1].x-points[ip1].x)/(points[in1].y-points[ip1].y);
					x1=points[ip1].x + (Fixed(y+1)-points[ip1].y)*dx1;
				}
			}

			if (y>=int(points[in2].y)) // переход к следующему отрезку
			{
				do // находим следующий нужный участок
				{  
					ip2 = in2;
					in2 = in2==points.High()-1 ? points.Low() : in2+1;
				} while (y>=int(points[in2].y));

				if (y+1==int(points[in2].y)) // вырожденный случай
				{
					dx2 = points[in2].x>points[ip2].x ? Fixed(1000) : Fixed(-1000);
					x2=points[ip2].x + (points[in2].x-points[ip2].x)
						*((Fixed(y+1) - points[ip2].y)
						/( points[in2].y  - points[ip2].y));
				} else
				{
					dx2 = (points[in2].x-points[ip2].x)/(points[in2].y-points[ip2].y);
					x2  = points[ip2].x + (Fixed(y+1)-points[ip2].y)*dx2;
				}
			}

			t.Fill(int(x1)+1, int(x2)+1);

			x1   += dx1;
			x2   += dx2;
		}
	}
	
	template <typename T>	
	void ArrayPoly (const Array2DWindow<T>& t, T color, tblib::Slice<const Point> points)
	{
		AbstractPoly (FillArray<T>(t, color), points);
	}
};