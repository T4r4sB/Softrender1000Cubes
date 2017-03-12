#include "fixed.h"
#include "math.h"
#include "point.h"
#include "tbarr.h"
#include "bodymass.h"

namespace tbphys
{
	const Fixed 
		eps  = Fixed(4, 65536),
		SE   = Fixed(5, 4),
		SPK  = Fixed(1, 16);

	struct Ball 
	{
		Fixed r;
		Point pos;
		Point speed;
		Mass  mass;

		Ball () {}

		Ball (Fixed r, Point pos,	Point speed, Mass mass)
			: r(r), pos(pos), speed(speed), mass(mass) {}

		Ball (const tblib::IFStream& s) 
			: r( tblib::To<Fixed>::From(s)),
			pos( tblib::To<Point>::From(s)),
			speed( tblib::To<Point>::From(s)),
			mass( tblib::To<Mass>::From(s))
		{} 
			
		friend const tblib::OFStream& operator << (const tblib::OFStream& s, const Ball &b)
			{ return s << b.r << b.pos << b.speed << b.mass; }

		void Shift (Point delta)
		{
			pos   += delta;
			speed += delta*SPK;
		}
	};

	typedef Ball* PBall;

	bool Test(const Ball &b1, const Ball &b2)
	{
		return ((b1.pos-b2.pos).Length() <= b1.r+b2.r+eps);
	}
	 
	void Saturate(Point& p, Fixed max)
	{
		const Fixed l = p.Length();
		if (l>max)
			p *= (max/l);
	}

	struct PushBallsInfo
	{
		Fixed f;
		Fixed w1;
		Point delta;
		PushBallsInfo(){}
		PushBallsInfo(Fixed f, Fixed w1, Point delta):f(f),w1(w1),delta(delta) {}
		PushBallsInfo Swap() {	return PushBallsInfo(f, fx1-w1, delta);	}
	};

	bool CanPush (Ball &b1, Ball &b2)
	{
		const Fixed dst   = (b1.pos-b2.pos).Length();
		return (dst <= b1.r+b2.r-eps);
	}

	bool PushBalls (Ball &b1, Ball &b2, PushBallsInfo* pbi = NULL)
	{
		if (pbi)
			pbi->f     = fx0;

		const Fixed dst   = (b1.pos-b2.pos).Length();
		if (dst > b1.r+b2.r-eps) 
			return false;

		const Point delta = (dst == fx0) 
			? Point(fx1,fx0)  
			: (b2.pos-b1.pos) / dst;			                 // единичный радиус-вектор между центрами
		const Fixed w     = Fraction1(b1.mass, b2.mass); // доля массы первого шара
		const Point sd    = delta*(b1.r+b2.r-dst);       // вектор проникновения
		const Point d2    = sd*w;				  	           	 // отклонение 1го шара
		const Point d1    = sd-d2;		                 	 // отклонение 2го шара

		b1.Shift(-d1);
		b2.Shift(+d2);		

		const Fixed f     = (b1.speed-b2.speed)*delta;   // сила удара
		if (f<=fx0)
			return false;
		
		const Point ssd   = delta*f*SE;                  // суммарный разлёт, в скобках сила удара с упругостью 0.625
		const Point sd2   = ssd*w;                       // изменение скорости 1го шара
		const Point sd1   = ssd-sd2;                     // изменение скорости 2го шара
		b1.speed -= sd1;
		b2.speed += sd2;

		if (pbi)
		{
			pbi->f     = f;
			pbi->w1    = w;
			pbi->delta = delta;
		}
		return true;
	}
}


BINARY_SERIALIZATION(tbphys::PBall)
// нельзя просто написать tbphys::Ball*, потому что тогда там внутри модификатор const будет относиться не к Ball*, а к Ball
// потому что ёбаные кресты наследуют блядскую говноулиточную систему именования типов из ёбаной в рот сишки
// придуманной двумя долбоёбами
// будь проклят тот черный день, когда родился этот ублюдочный недоязык

namespace tbphys
{
	struct Link
	{
		Fixed w; // только для чтения
		PBall b1, b2;
		Fixed l, k;

		Link (PBall b1, PBall b2, Fixed l, Fixed k) 
			: w(Fraction1(b1->mass, b2->mass)), b1(b1), b2(b2), l(l), k(k)
		{}
		
		Link (const tblib::IFStream& s) : 
			w(tblib::To<Fixed>::From(s)), 
			b1(tblib::To<Ball*>::From(s)), 
			b2(tblib::To<Ball*>::From(s)), 
			l(tblib::To<Fixed>::From(s)), 
			k(tblib::To<Fixed>::From(s)) {}
			
		friend const tblib::OFStream& operator << (const tblib::OFStream& s, const Link &l)
		{  
			s<<l.w;
			s<<l.b1; 
			s<<l.b2;
			s<<l.l;
			s<<l.k; 
			return s;
		}

		void Move (int stage, int total) // stage from 0..total-1
		{
			const Point diff = b1->pos - b2->pos;			
			const Point delta = (diff==Point(fx0,fx0)) ? Point(fx1,fx0) : diff.Normalize();
			const Fixed dk = k;
			const Point sd = (delta*l - diff) * (dk/(Fixed(total)-dk*Fixed(stage))); // k/(t-k*s)
			const Point d2 = sd*w;
			const Point d1 = sd-d2;
			b1->Shift(+d1);
			b2->Shift(-d2);
		}
	};

	template <int sizeX, int sizeY, int ballsInNode, typename TBall>
	class BallTable
	{
		struct Node
		{
			tblib::Array<TBall*, ballsInNode> balls;

			void Clear () {	balls.Shrink(0); }

			void Add (TBall* ball)	{
				if (balls.Size()<ballsInNode) balls.EmplaceBack(ball);	}

			void Delete (TBall* ball)
			{
				int j=0;
				for (int i=0; i<balls.Size(); ++i)
					if (balls[i]!=ball)
						balls[j++] = balls[i];
				balls.Shrink(j);
			}
		};

		tblib::CArray<tblib::CArray<Node, sizeX>, sizeY> nodes;

		void GetBallBounds (const TBall &b, int &x1, int &y1, int &x2, int &y2)
		{
			x1 = tblib::Max(0      , int(b.pos.x-b.r));
			y1 = tblib::Max(0      , int(b.pos.y-b.r));
			x2 = tblib::Min(sizeX-1, int(b.pos.x+b.r));
			y2 = tblib::Min(sizeY-1, int(b.pos.y+b.r));
		}		

		// functor
		template <typename F>
		class __PushAll
		{
			F f;
		public:
			__PushAll (F f) : f(f) {}
			void operator () (Node &node, TBall* b)
			{
				for (int k=0; k<node.balls.Size(); ++k)
          f(b, node.balls[k]);
			}
		};	
		
		// functor
		struct __AddToNode
		{
			void operator () (Node &node, TBall* ball)
			{
				node.Add(ball);
			}
		};		
		
		// functor
		struct __DeleteFromNode
		{
			void operator () (Node &node, TBall* ball)
			{
				node.Delete(ball);
			}
		};

	public:		
		void Clear()
		{
			for (int j=0; j<sizeY; ++j) for (int i=0; i<sizeX; ++i)
				nodes[j][i].Clear();
		}  
		
		template <typename F>
		void DoWithBall(F f, TBall* ball)
		{
			int x1,y1,x2,y2;
			GetBallBounds(*ball, x1,y1,x2,y2);
			for (int j=y1; j<=y2; ++j) for (int i=x1; i<=x2; ++i)
				f(nodes[j][i], ball);
		}

		static __AddToNode AddToNode()
		{
			return __AddToNode();
		}
		
		static __DeleteFromNode DeleteFromNode()
		{
			return __DeleteFromNode();
		}

		template <typename F>
		static __PushAll<F> PushAll(F f)
		{
			return __PushAll<F>(f);
		}

	};
};