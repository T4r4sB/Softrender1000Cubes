#pragma once
#include "fixed.h"
#include "point.h"
#include "bodymass.h"
#include "tbarr.h"

namespace tbphys
{
	// для определения столкновения тел находится точка столкновения
	// глубина проникновения
	// тела расталкиваются, массы берутся виртуальные, потом обмениваются импульсами
	template <int CConvexes, int CPoints>
	struct Body 
	{
		// физика
		int     id;          // это ИД тела, для говнорахитектур
		Point		center, speed;    // центр, скорость
		Fixed		angle, rotspeed;  // угол, скорость поворота

		Mass    mass, moment;   // масса, момент инерции
		Fixed   width;   // толщина, полезно для коллизий
		Point   gravity;      // гравитация

		Point		sincosa; // только для чтения, полезная информация
	
		void Init (int id, const Point &c, const Point &s, Fixed ca, Fixed sa, const Mass &m, const Mass &ma, const Point &gr)
		{
			this->id      = id;
			this->center   = c;
			this->speed    = s;
			this->angle    = ca;
			this->rotspeed = sa;
			this->mass     = m;
			this->moment   = ma;
			this->sincosa  = SinCos(ca);
			this->gravity  = gr;
			this->geometry.Resize(0);
		}	
		
		Point RelToAbsPos   (const Point &p) const
		{
			return center+(p&sincosa);
		}		
		
		Point AbsToRelPos   (const Point &p) const
		{
			return (p-center)&~sincosa;
		}

		Point RelToAbsAngle (const Point &p) const
		{
			return p&sincosa;
		}
        
		Mass	VirtualMass (const Point &p, const Point &v) const
		// точка p (в абс. координатах), где давят, направление (в отн.к), куда давят, возвращаем виртуальную массу тела
		// n обязано быть единичным
		{
			if (moment.order > mass.order) return mass; // гироскоп давит всей массой
			else 
			{
				const Fixed r = (p-center)^v;
				if (moment.order < mass.order)
				{
					const Fixed r2 = r*r;
					if (r2 < (moment.v>>15)) return mass;
					else return moment/r2;
				} else
					return moment*(mass.v/(r*r*mass.v + moment.v));
			}
		}

		Point VirtualSpeed (const Point &p) const // точка p в абсолютных кординатах
		{
			return speed + (center-p).Rot90()*rotspeed;
		}

		void GetShift (const Point &p, const Point &v, Point &dc, Fixed &dca) const 
		{
		// как распределяется воздействия на тело в данной точке в данном направлении
		// принимает абсолютное p
			
			dc  = v; // тупо сдвиг
			dca = fx0;  
			
			if (moment.order > mass.order) 
				return; // нихера не делаем, он слишком мал, чтобы нас толкать!
			const Fixed vx  = (p-center)^v;

			const Fixed max = tblib::Max(tblib::Max(v.x.Abs(), v.y.Abs()), vx.Abs());
			if (max<=fxEps+fxEps) 
				return; 
			const Fixed revmax = fx1/max;
			const Point tv  = v *revmax;
			const Fixed tvx = vx*revmax;
			
			if (moment.order<mass.order) 
			{
				dc = Point(fx0,fx0);					
				const Fixed num = tv*v;
				const Fixed den = tvx;
				if ((num.Abs()>>15)>=den.Abs()) 
					return;
				dca = num/den;
			} else
			{
				const Fixed _n  = moment.v*(tv *tv );
				const Fixed _m  = mass  .v*(tvx*tvx);
				const Fixed va  = _n/(_n+_m); 
				
				dc = v*va;					
				const Fixed num = (fx1-va)*(tv*v);
				const Fixed den = tvx;
				if ((num.Abs()>>15)>=den.Abs()) 
					return;
				dca = num/den;
			}
		}

		void PushPV (const Point &p, const Point &v, bool dp, bool dv)	// воздействие сдвигом на точку p (в абс координатах) со сдвигом n
		{			
			Point dc;
			Fixed dca;			
			GetShift(p, v, dc, dca);

			if (dp) 
			{
				center  += dc;
				angle   += dca;		
				// это даёт +25% скорости		
				if (dca.Abs() <= Fixed(1,10))		
					sincosa -= sincosa.Rot90()*dca; 
				else
					sincosa = SinCos(angle);
			}
			if (dv) 
			{
				speed    += dc;
				rotspeed += dca; 
			}
		}		

		void Move (Fixed dt)
		{
			speed	  += gravity*dt;
			center  += speed *dt;
			angle   += rotspeed*dt;
			angle.g %= fx2Pi.g;
			sincosa  = SinCos(angle);
		}

		void Saturate (Fixed ms, Fixed msa)
		{
			rotspeed = tblib::Max(-msa, tblib::Min(rotspeed, msa));
			const Fixed l = speed.Length();
			if (l>ms) speed *= ms/l;
		}

		// геометрия
		struct Convex
		{
			tblib::Array<Point, CPoints> v; // вершины
			tblib::Array<Point, CPoints> n; // нормали	
			tblib::Array<Point, CPoints> na; // нормали с учётом реального поворота фигуры	
			tblib::Array<int, CPoints>  b; // граничная ли
			bool closed;  // замкнутая ли, или открытая, это для стен надо - они не замкнутые
			Convex *last; // для хешни

			Convex () {}

			Convex (const tblib::IFStream& f) :
				v (f),
				n (f),
				b (f),
				closed (tblib::To<bool>::From(f)) 
			{
				na.Resize(n.Size());
			}

			friend const tblib::OFStream& operator << (const tblib::OFStream& f, const Convex& c)
			{
				f << c.v << c.n << c.b << c.closed;
				return f;
			}

			void Clear ()
			{
				v.Resize(0);
				n.Resize(0);
				b.Resize(0);
			}

			void AddVertex (const Point &p, bool border=true)
			{
				v.EmplaceBack(p);
				b.EmplaceBack(border);
			}

			void InitNormals ()
			{
				int count = closed ? v.Size() : v.Size()-1;
				n  .Resize(count);
				na .Resize(count);
				for (int i=0; i<count; ++i)
				{
					int i1 = i;
					int i2 = i==v.Size()-1 ? 0 : i+1;
					n[i] = (v[i2]-v[i1]).Rot90().Normalize();
				}
			}

			void RotateNormals (const Body& b)
			{
				int count = closed ? v.Size() : v.Size()-1;
				for (int i=0; i<count; ++i)
					na[i] = b.RelToAbsAngle(n[i]);
			}

			Fixed Square () const
			{
				Fixed result = fx0;
				for (int i=2; i<v.Size(); ++i) {
					result += (v[i-1]-v[0])^(v[i]-v[0]); }
				return result;
			}

			Point Center () const
			{
				Point result(fx0,fx0);
				Fixed sq;
				for (int i=2; i<v.Size(); ++i) {
					Fixed dsqr = (v[i-1]-v[0])^(v[i]-v[0]);
					result += (v[0]+v[i-1]+v[i])*Fixed(1,3)*dsqr;
					sq     += dsqr;
				}
				return result/sq;
			}
		};

		typedef Body<CConvexes, CPoints> TBody;
		typedef typename TBody::Convex TConvex;

		struct ConvexPtr
		{
			TBody *h;
			TConvex *c;
			ConvexPtr(){}
			ConvexPtr(TBody *h, TConvex *c) : h(h), c(c) {}			

			void GetBounds (bool &first, Point &p1, Point &p2)
			{
				for (int i=0; i<c->v.Size(); ++i)
				{
					const Point av = h->RelToAbsPos(c->v[i]); 
					if (first || av.x<p1.x) p1.x=av.x;
					if (first || av.y<p1.y) p1.y=av.y;
					if (first || av.x>p2.x) p2.x=av.x;
					if (first || av.y>p2.y) p2.y=av.y;
					first = false;
				}
			}
		};		

		struct Collision 
		{
			ConvexPtr c1,c2;
			Fixed cl;
			Point cp1, cp2, cd;
			bool first;
			int kind;

			Collision (const ConvexPtr &c1, const ConvexPtr &c2) 
				: c1(c1), c2(c2), first(true), kind(0) {}

			Collision (TBody &b1, const Point &p1, TBody &b2, const Point &p2, Fixed dist, Fixed k, bool hard)
				: c1(&b1,NULL), c2(&b2,NULL), kind(hard?2:1)
			{
				// всякая хуйня
				cp1 = b1.RelToAbsPos(p1);
				cp2 = b2.RelToAbsPos(p2);
				const Fixed l   = (cp2-cp1).Length();
				cl = (dist-l)*k;
				cd = (l==fx0) ? Point(fx1, fx0) : (cp2-cp1)/l;                
			}

			bool NoCollision ()
			{
				return (!first && cl<=fx0);
			}

			void FindDepth (const Point &n1, const Point &p1, const Point &p2, bool swap) 
			{
			// предполагается, что структура collision уже инициализирована
			// нормаль к ребру, точка на ребре, просто точка
				
				const Fixed acl = (p1-p2)*n1;			
				if (first || acl<cl) 	
				{
					first = false;
					cl    = acl;
					cp1   = p2 + n1*cl;
					cp2   = p2;
					cd    = swap ? -n1 : n1; 
				}
			}	

			inline bool NegAngle (const Point &p1, const Point &p2)
			{
				//x1*y2-x2*y1
				//return (p1^p2)<fx0;						
				if (p1.y>=fx0 && p2.y>fx0) return p1.x<p2.x;
				if (p1.y<=fx0 && p2.y<fx0) return p1.x>p2.x;
				if (p1.x<=fx0 && p2.x<fx0) return p1.y<p2.y;
				if (p1.x>=fx0 && p2.x>fx0) return p1.y>p2.y;
				if (p1.y>=fx0)             return p1.x+p2.x>fx0;
				if (p1.y<=fx0)             return p1.x+p2.x<fx0;				
				return false;
			}

			void FindMinDepth (const ConvexPtr &c1, const ConvexPtr &c2, bool swap)
			{
				// первая перебирается по рёбрам, вторая по вершинам
				if (NoCollision()) return;
				const int 
					b1 = 0,
					e1 = c1.c->closed ? c1.c->v.Size()-1 : c1.c->v.Size()-2,
					b2 = c2.c->closed ? 0 : 1,
					e2 = c2.c->closed ? c2.c->v.Size()-1 : c2.c->v.Size()-2;
				if (b1>e1 || b2>e2) return;
				int i1 = e1, i2 = e2;
				do
				{
					i1 = i1 == e1 ? b1 : i1+1;
					if (!c1.c->b[i1]) continue;
					const int fi2 = i2;
					// линейный перебор начинается со значения, найденного на предыдущей итерации предыдущего цикла
					// из-за упорядоченности вершин это приведёт к тому, что двойной цикл сработает за линейное время					
					const Point  n_i1 = c1.c->na[ i1];
					do
					{
						const int ni2 = i2 == e2 ? b2 : i2+1;

						const Point  n_i2 = c2.c->na[ i2];
						const Point n_ni2 = c2.c->na[ni2];					

						if (NegAngle(n_i2,n_i1) && !NegAngle(n_ni2,n_i1)) 
						{
							const Point p1 = c1.h->RelToAbsPos   (c1.c->v[ i1]);
							const Point p2 = c2.h->RelToAbsPos   (c2.c->v[ni2]);

							FindDepth(n_i1, p1, p2, swap);	
							if (NoCollision()) return;
							break;
						}
						i2 = ni2;
					} while (fi2 != i2);
				} while (i1 != e1);
			}

			void TestConvexes ()
			{
				c1.h->RotateNormals();					
				c2.h->RotateNormals();
				FindMinDepth (c1, c2, false);
				FindMinDepth (c2, c1, true);
				first = false;
				//cl=fx0;
			}

			bool Resolve ()		
			{								
				// та самая функция с физикой по расталкиванию тел
				const Mass bm1 = c1.h->VirtualMass(cp1, cd);
				const Mass bm2 = c2.h->VirtualMass(cp2, cd);
				const Fixed w1 = Fraction1 (bm1, bm2);
				if (kind==0) cl+=Fixed(1,1000);
				const Point sp = cd*cl;
				const Point p2 = sp*w1;
				const Point p1 = sp-p2;

				if (kind==1) 
				{ // нежёсткая 
					c1.h->PushPV(cp1, -p1, false, true);
					c2.h->PushPV(cp2,  p2, false, true); }
				else 
				{//  жёсткая
					c1.h->PushPV(cp1, -p1, true, true);
					c2.h->PushPV(cp2,  p2, true, true);	}

				if (kind==0) 
				{				
					cp1-=p1;
					cp2+=p2;
					const Point s1 = c1.h->VirtualSpeed(cp1);
					const Point s2 = c2.h->VirtualSpeed(cp2);		
					const Fixed f     = (s2-s1)*cd;   // сила удара
					if (f<fx0)
					{
						const Point ssd   = cd*(f*Fixed(2)*Fixed(8,10));                  
						const Point sd2   = ssd*w1;                     
						const Point sd1   = ssd-sd2;                    
						c1.h->PushPV(cp1,  sd1, false, true);
						c2.h->PushPV(cp2, -sd2, false, true);  
						const Point ns1 = c1.h->VirtualSpeed(cp1);
						const Point ns2 = c2.h->VirtualSpeed(cp2);
						return true; }
					else return false;
				} 				
				else return true;
			}

			Collision operator = (const Collision&);
		};		

		/*

		поля, определённые выше :

		// физика
		int id;          // это ИД тела, для говнорахитектур
		Point		c, s;    // центр, скорость
		Fixed		ca, sa;  // угол, скорость поворота

		Mass    m, ma;   // масса, момент инерции
		Fixed   width;   // толщина, полезно для коллизий
		Point   gr;      // гравитация

		Point		sincosa; // только для чтения, полезная информация

		*/
		// ещё одно поле :

		tblib::Array<Convex, CConvexes> geometry;
		
		Body () {}

		Body (const tblib::IFStream& f) :
			id       (tblib::To<int>::From(f)),
			center   (tblib::To<Point>::From(f)),
			speed    (tblib::To<Point>::From(f)),
			angle    (tblib::To<Fixed>::From(f)),
			rotspeed (tblib::To<Fixed>::From(f)),
			mass     (tblib::To<Mass>::From(f)),
			moment   (tblib::To<Mass>::From(f)),
			width    (tblib::To<Fixed>::From(f)),
			gravity  (tblib::To<Point>::From(f)),
			sincosa  (tblib::To<Point>::From(f)),
			geometry (f)
		{}

		friend const tblib::OFStream& operator << (const tblib::OFStream& f, const Body& b)
		{
			f << b.id << b.center << b.speed << b.angle << b.rotspeed << b.mass << b.moment << b.width << b.gravity << b.sincosa << b.geometry;
			return f;
		}

		void AddConvex (bool closed=true) {
			geometry.EmplaceBack();
			geometry.back().closed = closed;}

		void AddVertex (const Point &p, bool border=true)	{
			geometry.back().AddVertex(p, border);	}

		void InitNormals ()	{
			for (int i=0; i<geometry.Size(); ++i) geometry[i].InitNormals();		
			sincosa = SinCos(angle);
			RotateNormals();	}

		void RotateNormals () {
			for (int i=0; i<geometry.Size(); ++i) geometry[i].RotateNormals(*this);}

		typedef tblib::Array<tblib::Array<Point, CPoints>, CConvexes> Geometry;

		Geometry GetGeometry ()
		{
			Geometry result;
			result.Resize(geometry.Size());
			for (int i=0; i<result.Size(); ++i)
			{
				result[i].Resize(geometry[i].v.Size());
				for (int j=0; j<result[i].Size(); ++j)
					result[i][j] = geometry[i].v[j];
			}
			return result;
		}

		Fixed Square () const 
		{
			Fixed result = fx0;
			for (int i=0; i<geometry.Size(); ++i) 
				result += geometry[i].Square(); 
			return result; 
		}		
	};		

	template <typename TBody, int sizeX, int sizeY, int maxCnt>
	class Table
	{		
		struct Node
		{
			tblib::Array<typename TBody::ConvexPtr, maxCnt> convexes;
			void Clear () { convexes.Shrink(0); }
			void Add (const typename TBody::ConvexPtr& cp) {
				if (convexes.Size()<maxCnt) convexes.EmplaceBack(cp); }
		};

		tblib::CArray<tblib::CArray<Node, sizeX>, sizeY> n;
		Node on;
	public :
		void Clear ()
		{
			for (int y=0; y<sizeY; ++y) for (int x=0; x<sizeX; ++x) 
				n[y][x].Clear();
			on.Clear();
		}

		// function takes functor
		template <typename F>
		void EachConvex(F f, TBody& b)
		{
			for (int j=0; j<b.geometry.Size(); ++j)
			{
				typename TBody::ConvexPtr cp(&b, &b.geometry[j]);

				Point p1,p2;

				bool first = true;
				cp.GetBounds(first,p1,p2);

				const int x1 = p1.x;
				const int y1 = p1.y;
				const int x2 = p2.x;
				const int y2 = p2.y;
				
				const bool out = (x1<0 || y1<0 || x2>=sizeX || y2>=sizeY);

				const int bx1 = tblib::Max(      0, x1);
				const int by1 = tblib::Max(      0, y1);
				const int bx2 = tblib::Min(sizeX-1, x2);
				const int by2 = tblib::Min(sizeY-1, y2);

				for (int y=by1; y<=by2; ++y) for (int x=bx1; x<=bx2; ++x)
					f(n[y][x], cp);
				if (out) 
					f(on, cp);
			}
		}
		
		// functor
		struct AddToNode
		{
			void operator () (Node &node, const typename TBody::ConvexPtr& cp)
			{
				cp.c->last = NULL;
				node.Add(cp);
			}
		};

		// functor
		template <typename F>
		class PushAll
		{
			F f;
		public:
			void operator () (Node &node, const typename TBody::ConvexPtr& cp)
			{
				for (int k=0; k<node.convexes.Size(); ++k)
				{
					if (cp.h != node.convexes[k].h && cp.c != node.convexes[k].c->last)
					{
						f(cp, node.convexes[k]);
						node.convexes[k].c->last = cp.c;					
					}
				}
			}
		};
	};
};