#pragma once

#include "memory.h"
#include "points3d.h"
#include "textures.h"
#include "algorithm.h"
#include "renderfunc.h"

struct Sector;
struct Line;

int maxSectors=0;
const int maxMonsters = 2048;
const int maxEntitys  = 8192;
const int maxConvexes = maxEntitys*8;
const int maxBullets  = 512;

struct RotatedPoint
{
  Point3D* original;
  Point3D point;
};

struct Face
{
	int                          inProcess;
	RenderInfo                   renderInfo;
	int                          id;
	Sector                       *nextSector;
	bool                         penetrable;
	Point3D                      n;
	float                        nc;
	MSlice<Point3D*>             points;
	Texture*                     texture;
	int                          light;
	Point3D                      vtx, vty;
	float                        vtxc, vtyc;

	float Dist (const Point3D& p) { return Dot(p,n)-nc; }
};

struct Line 
{
  Point3D      *p1, *p2;
	Face         *f1, *f2;
};

struct Sector
{
	int                   inProcess;
	int                   id;
	float                 gravity;
	bool                  skybox;
	MSlice<Face>          faces;
	MSlice<Line>          lines;
	MSlice<RotatedPoint>  points;

	void InitNormals ()
	{
		for (int f=faces.Low(); f<faces.High(); ++f)
		{
			Face& face = faces[f];
			// вычисляем саму нормаль
			face.n = Point3D(0.0f, 0.0f, 0.0f);
			tblib::Slice<Point3D*>& p = face.points;
			assert(p.High() >= p.Low()+3);
			for (int i=p.Low()+2; i<p.High(); ++i)
				face.n += Cross(*p[i]-*p[p.Low()], *p[i-1]-*p[p.Low()]);
			face.n.Normalize();
			// вычисляем константу
			face.nc = 0;
			for (int i=p.Low(); i<p.High(); ++i)
				face.nc += Dot(*p[i], face.n);
			face.nc /= (p.High()-p.Low());
		}
	}

	Point3D Center ()
	{
		Point3D result = Point3D(0.0f, 0.0f, 0.0f);
		for (int p=points.Low(); p<points.High(); ++p)
			result += *points[p].original;
		return result/float(points.High() - points.Low());
	}

	void Connect (int& l, Face &face, Point3D* p1, Point3D* p2)
	{
		for (int i=lines.Low(); i<l; ++i)
		{
			assert (lines[i].p1 != p1 || lines[i].p2 != p2);
			if (lines[i].p1==p2 && lines[i].p2==p1)
			{
				assert(lines[i].f2==NULL);
				lines[i].f2 = &face;
				return;
			}
		}

		assert (l<lines.High());
		++l;
		lines[l-1].p1 = p1;
		lines[l-1].p2 = p2;
		lines[l-1].f1 = &face;
		lines[l-1].f2 = NULL;
	}
	
	void Create (const tblib::Slice<Point3D*> &pts)
	{
		int cf=0, cl=0, cp=0;
		inProcess=0;
		id=0;
		for (int i=pts.Low(); i<pts.High(); ++i)
		{
			if (pts[i]) 
			{
				++cl; 
				bool same = false;
				for (int j=pts.Low(); j<i; ++j) 
				{
					if (pts[j] && pts[j] == pts[i]) {
						same = true;
						break;
					}
				}
				if (!same) 
					++cp;
			} else
			{
				++cf;
			}
		}    
		cl /= 2;
		gravity = 0.0f;
		skybox  = false;

		// инициализируем точки
		points.Init(0, cp, PK_GLOBAL);
		int lrp = points.Low();

		// инициализируем поверхности
		faces.Init(0, cf, PK_GLOBAL);
		int j1=pts.Low();
		for (int i=faces.Low(); i<faces.High(); ++i)
		{
			Face& face = faces[i];
			face.inProcess = false;
			face.id = i;
			face.nextSector = NULL;
			face.n = Point3D(0.0f, 0.0f, 0.0f);
			face.nc = 0.0f;
			face.light = 0;
			// выгрызаем точки
			int j2=j1;
			while (pts[j2]) ++j2;
			face.points.Init(j1, j2, PK_GLOBAL);
			for (int j=j1; j<j2; ++j) {
				bool same = false;
				for (int k=points.Low(); k<lrp; ++k) 
				{
					if (points[k].original == pts[j]) 
					{
						same = true;
						face.points[j] = &points[k].point;
						break;
					}
				}

				if (!same) 
				{
					points[lrp].original = pts[j];
					points[lrp].point = *points[lrp].original;
					face.points[j] = &points[lrp].point;
					++lrp;
				}
			}
			j1=j2+1;
		}
		// разбираемся с линиями
		lines.Init(0, cl, PK_GLOBAL);
		int l=lines.Low();
		for (int f=faces.Low(); f<faces.High(); ++f)
		{
      for (int i=faces[f].points.Low(); i<faces[f].points.High(); ++i)
			{
				int ni= (i+1>=faces[f].points.High()) ? faces[f].points.Low() : i+1;
				Connect(l, faces[f], faces[f].points[i], faces[f].points[ni]);
			}
		}
		InitNormals ();
		assert(l==lines.High());
	}
};

template <typename T>
class CheckPtr
{
	T* value;
	CheckPtr(const CheckPtr&);
	CheckPtr& operator = (const CheckPtr&);
public :
	T* operator -> () { return value; }
	operator T* () { return value; }
	CheckPtr (T* ptr) : value(ptr) { ++value->inProcess; }
	CheckPtr& operator = (T* ptr) { tblib::recreate(*this, ptr); return *this; }
	~CheckPtr() { if (value) --value->inProcess; }

	static void swap (CheckPtr& l, CheckPtr& r)
	{
		T* tmp = l.value;
		l.value = r.value;
		r.value = tmp;
	}
};

namespace std
{
	template <typename T>
		void swap(CheckPtr<T>&l, CheckPtr<T>&r) { CheckPtr<T>::swap(l,r); }
};

struct SPoint
{
	Point3D p;
	Sector *s;

	SPoint () {}
	SPoint (const Point3D& p, Sector* s) : p(p),s(s) {}

	bool Correct () const
	{
		for (int i=s->faces.Low(); i<s->faces.High(); ++i)
			if (s->faces[i].Dist(p) > 0.0f)
				return false;
		return true;
	}

	void DoCorrect ()
	{
		const float eps = 0.000001f;
		bool correct;
		do 
		{
			correct=true;
			for (int i=s->faces.Low(); i<s->faces.High(); ++i)
			{
				float ad = s->faces[i].Dist(p);
				if (ad<=-eps*0.5f) // тут более слабое условие
					continue;
				float j=1.0f;
				Point3D np=p;
				for (;;)
				{
					correct=false;
					np = p - s->faces[i].n*((ad+eps)*j);
					j += 1.0f;
					float add = s->faces[i].Dist(np);
					if (add<=-eps) //тут более сильное
						break;
				}
				p=np;
			}
		} while (!correct);
	}

	Face* getIntersectionFace(const Point3D& delta)
	{
		MSlice<float> gf(s->faces.Low(), s->faces.High(), PK_TEMP);
		for (int i=gf.Low(); i<gf.High(); ++i) gf[i]=0.0f;

		for (int i=s->lines.Low(); i<s->lines.High(); ++i)
		{
			float d = Volume(*s->lines[i].p1 - p, *s->lines[i].p2 - p, delta);
			if      (d>0.0f) tblib::Enlarge(gf[s->lines[i].f1->id],  d);
			else if (d<0.0f) tblib::Enlarge(gf[s->lines[i].f2->id], -d);
		}
		Face* result=NULL;
		float md=-1.0f;
		for (int i=s->faces.Low(); i<s->faces.High(); ++i)
		{
			if ((md<0.0f || gf[i]<md) && Dot(delta, s->faces[i].n)>0.0f)
			{
				result = &s->faces[i];
				md = gf[i];
			}
		}
		assert(result);
		return result;	
	}
	

	void GetSectorsFrom (Sector* vs, float r, bool push, MArray<CheckPtr<Sector> > &sectors);

	void GetSectors (float r, MArray<CheckPtr<Sector> > &sectors)
	{
		GetSectorsFrom(s,r,false,sectors);
	}
	
	void GetSectorsAndPush (float r, MArray<CheckPtr<Sector> > &sectors)
	{
		GetSectorsFrom(s,r,true,sectors);
	}
	
	void Push (float r)
	{
		MArray<CheckPtr<Sector> > a(0,maxSectors,PK_TEMP);
		GetSectorsFrom(s,r,true,a);
	}

	bool Trace (const Point3D& delta)
	{		
		(void) delta;
		return false;
	}

};

Point3D RandomPoint ()
{	
	const float
		h = float(pasrnd.frandom())*2.0f-1.0f,
		r = sqrt(1.0f-h),
		a = float(pasrnd.frandom())*2.0f+float(M_PI);
	return Point3D(r*cos(a),r*sin(a),h);
}

