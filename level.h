#pragma once

#include "tbarr.h"
#include "geometry.h"
#include "textures.h"

template <int M>
void Add3P (tblib::Array<Point3D*, M> &a, int &fc, Point3D* p1, Point3D* p2, Point3D* p3)
{
	a.EmplaceBack(p1);
	a.EmplaceBack(p2);
	a.EmplaceBack(p3);		
	Point3D* nullP = NULL;
	a.EmplaceBack(nullP);
	++fc;
}

template <int M>
void Add4P (tblib::Array<Point3D*, M> &a, int &fc, Point3D* p1, Point3D* p2, Point3D* p3, Point3D* p4)
{
	a.EmplaceBack(p1);
	a.EmplaceBack(p4);
	a.EmplaceBack(p3);		
	a.EmplaceBack(p2);		
	Point3D* nullP = NULL;
	a.EmplaceBack(nullP);
	++fc;
}	

template <int M>
void Add4PC (tblib::Array<Point3D*, M> &a, int &fc, Point3D* p1, Point3D* p2, Point3D* p3, Point3D* p4)
{
	if (Volume(*p1,*p2,*p3,*p4)>0.0f)
	{
		Add3P(a,fc,p1,p2,p3);
		Add3P(a,fc,p1,p3,p4);
	} else
	{
		Add3P(a,fc,p1,p2,p4);
		Add3P(a,fc,p2,p3,p4);
	}
}

template <int M>
void Add8P (tblib::Array<Point3D*, M> &a, int &fc, Point3D* p1, Point3D* p2, Point3D* p3, Point3D* p4, Point3D* p5, Point3D* p6, Point3D* p7, Point3D* p8)
{
	a.EmplaceBack(p1);
	a.EmplaceBack(p2);
	a.EmplaceBack(p3);		
	a.EmplaceBack(p4);		
	a.EmplaceBack(p5);
	a.EmplaceBack(p6);
	a.EmplaceBack(p7);		
	a.EmplaceBack(p8);
	Point3D* nullP = NULL;
	a.EmplaceBack(nullP);
	++fc;
}

template <int N>
struct Level
{	
	int size;
	Level () {size=N;}
	
	tblib::CArray<Point3D, 16> skyP, hellP, emptyP;
	Sector skyS, hellS, emptyS;
	tblib::CArray<tblib::CArray<tblib::CArray<Sector, 10>, 10>, 10> cubes;
	tblib::CArray<tblib::CArray<tblib::CArray<Point3D, 20>, 20>, 20> points;

	float gameTime;
	//*******************************************************************************************************
  
	void InitSkyBox (const tblib::Slice<Point3D>& skyPoints, Sector& s, Texture& roof, Texture& floor)
	{
		assert (skyPoints.Low()==0 && skyPoints.High()==16);

    skyPoints[ 0] = Point3D(  64.0f,-160.0f, -32.0f);
    skyPoints[ 1] = Point3D( 160.0f, -64.0f, -32.0f);
    skyPoints[ 2] = Point3D( 160.0f,  64.0f, -32.0f);
    skyPoints[ 3] = Point3D(  64.0f, 160.0f, -32.0f);
    skyPoints[ 4] = Point3D( -64.0f, 160.0f, -32.0f);
    skyPoints[ 5] = Point3D(-160.0f,  64.0f, -32.0f);
    skyPoints[ 6] = Point3D(-160.0f, -64.0f, -32.0f);
    skyPoints[ 7] = Point3D( -64.0f,-160.0f, -32.0f);
		
    skyPoints[ 8] = Point3D(  640.0f,-1600.0f, 232.0f);
    skyPoints[ 9] = Point3D( 1600.0f, -640.0f, 232.0f);
    skyPoints[10] = Point3D( 1600.0f,  640.0f, 232.0f);
    skyPoints[11] = Point3D(  640.0f, 1600.0f, 232.0f);
    skyPoints[12] = Point3D( -640.0f, 1600.0f, 232.0f);
    skyPoints[13] = Point3D(-1600.0f,  640.0f, 232.0f);
    skyPoints[14] = Point3D(-1600.0f, -640.0f, 232.0f);
    skyPoints[15] = Point3D( -640.0f,-1600.0f, 232.0f);
  

		tblib::Array<Point3D*, 58> a;
		int fc=0;

		Add8P(a, fc, &skyPoints[0], &skyPoints[1], &skyPoints[2], &skyPoints[3],
			&skyPoints[4], &skyPoints[5], &skyPoints[6], &skyPoints[7]); 
		
		Add8P(a, fc, &skyPoints[15], &skyPoints[14], &skyPoints[13], &skyPoints[12],
			&skyPoints[11], &skyPoints[10], &skyPoints[9], &skyPoints[8]); 

		Add4P(a, fc, &skyPoints[0], &skyPoints[1], &skyPoints[ 9], &skyPoints[ 8]); 
		Add4P(a, fc, &skyPoints[1], &skyPoints[2], &skyPoints[10], &skyPoints[ 9]); 
		Add4P(a, fc, &skyPoints[2], &skyPoints[3], &skyPoints[11], &skyPoints[10]); 
		Add4P(a, fc, &skyPoints[3], &skyPoints[4], &skyPoints[12], &skyPoints[11]); 
		Add4P(a, fc, &skyPoints[4], &skyPoints[5], &skyPoints[13], &skyPoints[12]); 
		Add4P(a, fc, &skyPoints[5], &skyPoints[6], &skyPoints[14], &skyPoints[13]); 
		Add4P(a, fc, &skyPoints[6], &skyPoints[7], &skyPoints[15], &skyPoints[14]); 
		Add4P(a, fc, &skyPoints[7], &skyPoints[0], &skyPoints[ 8], &skyPoints[15]); 

		s.Create(a.GetSlice());

		s.inProcess = 0;
		s.skybox = true;
		s.gravity = 0.03f;
		for (int i=s.faces.Low(); i<s.faces.High(); ++i)
		{
			Face& face = s.faces[i];
			face.id = i;
			face.nextSector  = NULL;
			face.penetrable  = false;
		  face.texture     = (i==s.faces.Low()+1) ? &floor : &roof;
			face.light       = 255;
			face.vtx         = Point3D(0.32f, 0.00f, 0.00f);
			face.vtxc        = 0.0f;
			face.vty         = Point3D(0.00f, 0.32f, 0.00f);
			face.vtyc        = 0.0f;
		}
	}

	void InitSectors()
	{
		for (int k=0; k<20; ++k) for (int j=0; j<20; ++j) for (int i=0; i<20; ++i)
		{
			points[k][j][i] = Point3D(
				float(k)-9.5f + float((i^k)&1)*0.0f, 
				float(j)-9.5f + float((k^j)&1)*0.0f, 
				float(i)-9.5f + float((j^i)&1)*0.0f) * 0.5f;
		}
		
		for (int k=0; k<10; ++k) for (int j=0; j<10; ++j) for (int i=0; i<10; ++i)
		{				
			tblib::Array<Point3D*, 30> a;
			int fc=0;

			int k1 = i*2, j1 = j*2, i1 = k*2;

			Add4P(a, fc, &points[k1+1][j1+0][i1+0], &points[k1+1][j1+0][i1+1], &points[k1+1][j1+1][i1+1], &points[k1+1][j1+1][i1+0]); 
			Add4P(a, fc, &points[k1+1][j1+0][i1+1], &points[k1+1][j1+0][i1+0], &points[k1+0][j1+0][i1+0], &points[k1+0][j1+0][i1+1]); 
			Add4P(a, fc, &points[k1+1][j1+1][i1+1], &points[k1+1][j1+0][i1+1], &points[k1+0][j1+0][i1+1], &points[k1+0][j1+1][i1+1]); 
			Add4P(a, fc, &points[k1+1][j1+1][i1+0], &points[k1+1][j1+1][i1+1], &points[k1+0][j1+1][i1+1], &points[k1+0][j1+1][i1+0]); 
			Add4P(a, fc, &points[k1+1][j1+0][i1+0], &points[k1+1][j1+1][i1+0], &points[k1+0][j1+1][i1+0], &points[k1+0][j1+0][i1+0]); 
			Add4P(a, fc, &points[k1+0][j1+1][i1+0], &points[k1+0][j1+1][i1+1], &points[k1+0][j1+0][i1+1], &points[k1+0][j1+0][i1+0]); 
			
			Sector& s = cubes[k][j][i];
			s.Create(a.GetSlice(0, a.Size()));
			
			s.inProcess = 0;
			s.skybox = false;
			s.gravity = 0.03f;
			for (int f=s.faces.Low(); f<s.faces.High(); ++f)
			{
				Face& face = s.faces[f];
				face.id = f;
				face.nextSector = NULL;
				face.penetrable = false;
        int txrIndex    = pasrnd.random(baseTxr.Size());
				face.texture    = &baseTxr[txrIndex];
				face.light      = 10;
				face.vtx        = Point3D(150.0f, 50.0f, 0.0f);
				face.vtxc       = 0.0f;
				face.vty        = Point3D(0.0f, 100.0f, 50.0f);
				face.vtyc       = 0.0f;
			}

			const float scale = float(s.faces[s.faces.Low()].texture->SizeX()-1)*2.0f, shift = 4.75f;

			s.faces[0].vtx  = Point3D( 0.0f, scale,  0.0f);
			s.faces[0].vtxc = (float(j)-shift) * scale - 0.5f;
			s.faces[0].vty  = Point3D( 0.0f,  0.0f, scale);
			s.faces[0].vtyc = (float(k)-shift) * scale - 0.5f;
			
			s.faces[1].vtx  = Point3D(scale,  0.0f,  0.0f);
			s.faces[1].vtxc = (float(i)-shift) * scale - 0.5f;
			s.faces[1].vty  = Point3D( 0.0f,  0.0f, scale);
			s.faces[1].vtyc = (float(k)-shift) * scale - 0.5f;
			
			s.faces[2].vtx  = Point3D(scale,  0.0f,  0.0f);
			s.faces[2].vtxc = (float(i)-shift) * scale - 0.5f;
			s.faces[2].vty  = Point3D( 0.0f, scale,  0.0f);
			s.faces[2].vtyc = (float(j)-shift) * scale - 0.5f;

			s.faces[3].vtx  = Point3D(scale,  0.0f,  0.0f);
			s.faces[3].vtxc = (float(i)-shift) * scale - 0.5f;
			s.faces[3].vty  = Point3D( 0.0f,  0.0f, scale);
			s.faces[3].vtyc = (float(k)-shift) * scale - 0.5f;

			s.faces[4].vtx  = Point3D(scale,  0.0f,  0.0f);
			s.faces[4].vtxc = (float(i)-shift) * scale - 0.5f;
			s.faces[4].vty  = Point3D( 0.0f, scale,  0.0f);
			s.faces[4].vtyc = (float(j)-shift) * scale - 0.5f;
			
			s.faces[5].vtx  = Point3D( 0.0f, scale,  0.0f);
			s.faces[5].vtxc = (float(j)-shift) * scale - 0.5f;
			s.faces[5].vty  = Point3D( 0.0f,  0.0f, scale);
			s.faces[5].vtyc = (float(k)-shift) * scale - 0.5f;
		}
	}

	void Create ()
	{
		InitSkyBox(skyP.GetSlice(), skyS, baseTxr[TXR_SKY], baseTxr[TXR_STONE_FLOOR]);
		InitSkyBox(hellP.GetSlice(), hellS, baseTxr[TXR_HELL], baseTxr[TXR_BLACK]);
		InitSkyBox(emptyP.GetSlice(), emptyS, baseTxr[TXR_BLACK], baseTxr[TXR_BLACK]);
		
		InitSectors(); // устанавливаем точки

		maxSectors = 19*19*19+3; // инициализируем важный глобальный параметр

    gameTime = 0.0f;
	}

private :
	Level (const Level&) {}
	Level&operator=(const Level&) {}
};

Level<24> level;