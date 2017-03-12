#pragma once
#include "algorithm.h"
#include "tbstream.h"
#include "tbdynarr.h"

namespace tblib
{
	template <typename T>
	class Array2DWindow
	{
	protected :
		int	m_sizeX, m_sizeY;
		int m_stride;
		T *m_begin;
	public :
		Array2DWindow () 
			: m_sizeX(0), m_sizeY(0), m_stride(0), m_begin(NULL) {}

		Array2DWindow (int sizeX, int sizeY, int stride, T *begin)	
			: m_sizeX(sizeX), m_sizeY(sizeY), m_stride(stride), m_begin(begin) 
		{				
			assert (sizeX>=0);
			assert (sizeY>=0);			
		}
			
#ifdef NDEBUG
		class Line
		{
			T* m_begin;
		public:
		friend class Array2DWindow;

		Line() : m_begin(NULL) {}

			Line (const Array2DWindow<T>& window, int index) 
				: m_begin(window.m_begin+index*window.m_stride)		{}

			Line (const Line& Line) : m_begin(Line.m_begin) {}

			Line& operator = (const Line& Line) { 
				if (&Line!=this) tblib::Recreate(*this, Line);
				return *this; 
			}

			T& operator [] (int index) const {
				return m_begin[index];
			}
		};		

		void ProcessLine (Line& l) const { l.m_begin += m_stride; }
#else
		class Line
		{
			const Array2DWindow<T>* m_window;
			int m_index;
			T* m_begin;
		public:
		friend class Array2DWindow;

		Line() : m_begin(NULL) {}

			Line (const Array2DWindow<T>& window, int index) 
				: m_window(&window), m_index(index), m_begin(window.m_begin+index*window.m_stride)		{}

			Line (const Line& Line) : m_window(Line.m_window), m_index(Line.m_index), m_begin(Line.m_begin) {}
      
			Line& operator = (const Line& Line) { 
				if (&Line!=this) tblib::Recreate(*this, Line);
				return *this; 
			}

			T& operator [] (int index) const {
				assert (m_index>=0 && m_index<m_window->m_sizeY && index>=0 && index<m_window->m_sizeX);
				return m_begin[index];
			}
		};		

		void ProcessLine (Line& l) const { l.m_begin += m_stride; ++l.m_index; }
#endif

		int SizeX() const { return m_sizeX; }
		int SizeY() const { return m_sizeY; }

		Line operator [] (int y) const { return Line(*this, y); }
		
		template <typename C>
		C Window (int wposX, int wposY, int wsizeX, int wsizeY) const 
		{
			const int 
				fx = Max(0      , wposX),
				fy = Max(0      , wposY),
				lx = Min(m_sizeX, wposX+wsizeX),
				ly = Min(m_sizeY, wposY+wsizeY);
			if (lx>fx && ly>fy)
				return C(lx-fx, ly-fy, m_stride, m_begin+fx+fy*m_stride);
			else 
				return C(0, 0, 0, NULL);
		}

		template <typename F, typename U> 
		void Copy (F f, const Array2DWindow<U>& src, const int dstX, const int dstY) const 
		{
			const int 
				fx  = Max(0      , dstX),
				fy  = Max(0      , dstY),
				lx  = Min(SizeX(), dstX+src.SizeX()),
				ly  = Min(SizeY(), dstY+src.SizeY());

			typename Array2DWindow<U>::Line srcl = src[fy-dstY];
			Line dstl = (*this)[fy];

			if (lx>fx)
			{
				for (int j=fy; j<ly; ++j) 
				{
					for (int i=fx; i<lx; ++i)
						f(dstl[i], srcl[i-dstX]);
					src.ProcessLine(srcl);
					ProcessLine(dstl);
				}
			}
		}
		
		template <typename F> 
		void Fill (F f) const 
		{
			const int
				sx=SizeX(), 
				sy=SizeY();
			Line dstl = (*this)[0];
			if (sy>0)
			{
				for (int j=0; j<sy; ++j) 
				{
					for (int i=0; i<sx; ++i)
						f(dstl[i]);
					ProcessLine(dstl);
				}
			}
		}
	};

	template <typename T, class C=Array2DWindow<T> >
	class Array2D : public C
	{ 
		tblib::HeapCArray<T> memory;

		T* GetBegin()	
		{
			if  (this->m_sizeX>0 && this->m_sizeY>0) 
				return memory.begin() ;
			else
				return NULL;
		}
	public:
		Array2D () {}

		Array2D (int sx, int sy) 
		: C(sx,sy,sx,NULL), memory(sx*sy)	
		{
			assert (sx>=0);
			assert (sy>=0);
			this->m_begin = GetBegin();
		}

		Array2D(const IFStream& s)
		{
			// нема делегирующих конструкторов и прочего такого
			int sx = To<int>::From(s);
			int sy = To<int>::From(s);
			tblib::Recreate(*this, sx, sy);
			// наконец-то можно читать
			for (int j=0; j<sy; ++j)
				for (int i=0; i<sx; ++i)
					(*this)[j][i] = To<T>::From(s);
		}

		friend const OFStream& operator << (const OFStream &s, const Array2D& a)
		{
			s << a.SizeX() << a.SizeY();
			for (int j=0; j<a.SizeY(); ++j)
				for (int i=0; i<a.SizeX(); ++i)
					s << a[j][i];
			return s;
		}

		void MoveFrom_ (Array2D<T,C>& a)
		{
			this->m_begin  = a.m_begin;
			this->m_sizeX  = a.m_sizeX;
			this->m_sizeY  = a.m_sizeY;
			this->m_stride = a.m_stride;
			memory.MoveFrom_(a.memory);
			assert (this->m_begin == GetBegin());
		}
	
		Array2D& operator = (const Array2D &p) {			
			if (&p!=this)
				tblib::Recreate(*this, p); // копируем данные окна
			return *this; 
		}

		Array2D (const Array2D &p) 
			: C(p.SizeX(), p.SizeY(), p.m_stride, NULL), memory(p.memory)  
		{
			this->m_begin = GetBegin();
		}

	};
};