#pragma once
#include <cmath>
#include "memory.h"
#include "palette.h"
#include "tbslice.h"
#include "pasrand.h"

struct Point2d
{
	
	float x,y;
	Point2d() {}
	Point2d(float x, float y) : x(x), y(y) {}
};

Random pasrnd(0);

template <typename Pixel>
class PixelMatrix 
{
	int m_sizeX, m_sizeY, m_shX, m_shY, m_maskX, m_maskY, m_shX16, m_maskAll;
	MSlice<Pixel> pixels;	
public :
	PixelMatrix(int shX, int shY, PoolKind poolKind) :
		m_shX(shX), m_shY(shY), m_shX16(16-shX),
		m_sizeX(1<<shX), m_sizeY(1<<shY),
		m_maskX((1<<shX)-1), m_maskY((1<<shY)-1),
		m_maskAll((1<<(shX+shY))-1),
		pixels(0,m_sizeX*m_sizeY,poolKind)
	{
		//temporaryStackPool.Alloc<int>(1);
	}

	int SizeX() const { return m_sizeX; }
	int SizeY() const { return m_sizeY; }
	int ShX()   const { return m_shX; }
	int ShY()   const { return m_shY; }
  int MaskAll() const { return m_maskAll; }

  Pixel* data ()
  {
    return &pixels[0];
  }

	Pixel& at(int x, int y) // это не индекс в массиве, потому что он ещё и делает зацикливание...
	{
		return pixels[(x&m_maskX) | ((y&m_maskY) << m_shX)];
	}
	
	const Pixel& at(int x, int y) const
	{
		return pixels[(x&m_maskX) | ((y&m_maskY) << m_shX)];
	}	
	
	Pixel fastAt(int x, int y) const
	{
		//assert (x>=0 && x<m_sizeX && y>=0 && y<m_sizeY);
		return pixels[(x + (y << m_shX)) & m_maskAll];
	}
};

class Noise : public PixelMatrix<float>
{			
	Noise (const Noise&);
	Noise& operator=(const Noise&);

	float smoothAt(float x, float y) const
	{
		const float flx = floor(x),	fly = floor(y);
		const int    ix = int(flx),	 iy = int(fly);
		const float  fx = x-flx,     fy = y-fly;

		const float 
			y0 = at(ix,iy  ) + (at(ix  ,iy  )-at(ix,iy  ))*fx,
			y1 = at(ix,iy+1) + (at(ix+1,iy+1)-at(ix,iy+1))*fx;
		return y0+(y1-y0)*fy;
	}

	void Normalize ()
	{
		float min=0.0f,max=0.0f;
		bool fmin=true,fmax=true;
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			float cur = at(i,j);
			if (cur<min || fmin) {min=cur; fmin=false;}
			if (cur>max || fmax) {max=cur; fmax=false;}
		}
		if (max<=min*1.00001f)
		{
			for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
				at(i,j) = 0.0f;
		} else
		{
			float dev = 1.0f / (max-min);
			for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
				at(i,j) = (at(i,j)-min)*dev;
		}		
	}
public:	
	Noise(int shX, int shY) : PixelMatrix<float>(shX,shY,PK_TEMP) {}

	void FillNoiseWaveByCenters ( float aspect, float freq, const tblib::Slice<Point2d> & centers )
	{
		static tblib::Array<float,32> sintable;
		if (sintable.Size()==0)
		{
			for (int i=0; i<sintable.Capacity(); ++i)
				sintable.EmplaceBack( float(sin(float(i)*float(M_PI)*2.0f/float(sintable.Capacity()))) );
		}

    float sq = float(sqrt(float(centers.High()-centers.Low())));
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			float r=0.0f;
			for (int k=centers.Low(); k<centers.High(); ++k)
			{
				float rx = abs(float(i)-centers[k].x);
				if (rx>=float(SizeX())*0.5f) rx = float(SizeX())-rx;
				float ry = abs(float(j)-centers[k].y);
				if (ry>=float(SizeY())*0.5f) ry = float(SizeY())-ry;
				r += sintable[int(freq*(rx+ry*aspect)*4.0f) & (sintable.Capacity()-1)];
			}
			r = abs(r);
			if (r>sq) r=sq; else if (r<-sq) r=-sq;
			at(i,j) = r;
		}
		Normalize();
	}
	
	void FillNoiseWave ( float aspect, float freq, int count )
	{
		MSlice<Point2d> centers(0, count, PK_TEMP);
		for (int i=centers.Low(); i<centers.High(); ++i) 
		{
			const float x = float(pasrnd.frandom())*float(SizeX());
			const float y = float(pasrnd.frandom())*float(SizeY()); 
			centers[i] = Point2d(x,y);
		}

		FillNoiseWaveByCenters(aspect, freq, centers);
	}

	void FillNoiseGrass ()
	{
		for (int i=0; i<SizeX()*5; ++i)
		{
			int 
				x1 = pasrnd.random(SizeX()<<16),
				y1 = pasrnd.random(SizeY()<<16),
				dx = pasrnd.random(SizeX()<<16),
				dy = pasrnd.random(SizeY()<<16);
			int k = (abs(dx)>abs(dy)) ? dx>>16 : dy>>16;
      if (k>0) { dy /=k;	dx /=k;	}
			float c = float(pasrnd.frandom());
			for (int j=0; j<k; ++j)
			{
				at (x1>>16, y1>>16) = c;
				x1 += dx;
				y1 += dy;
			}
		}
		Normalize();
	}

	void FillNoiseStone (int count)
	{		
		MSlice<Point2d> centers (0, count-1, PK_TEMP);
		for (int i=centers.Low(); i<centers.High(); ++i) 
		{
			const float x = float(pasrnd.frandom())*float(SizeX());
			const float y = float(pasrnd.frandom())*float(SizeY()); 
			centers[i] = Point2d(x,y);
		}

		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			float r1 = float(SizeX())*3.0f;
			float r2 = float(SizeY())*3.0f;
			for (int k=centers.Low(); k<centers.High(); ++k)
			{
				float rx = abs(float(i)-centers[k].x);
				if (rx*2>=float(SizeX())) rx = float(SizeX())-rx;
				float ry = abs(float(j)-centers[k].y);
				if (ry*2>=float(SizeY())) ry = float(SizeY())-ry;
        float cr = float(sqrt(rx*rx+ry*ry));
				if (cr<r1) { r2=r1; r1=cr; } else if (cr<r2) { r2=cr; }
			}
			at(i,j) = tblib::Min(abs(r1-r2), 2.0f);
		}
		Normalize();
	}   

	void FillNoiseLines (int count)
	{		
		MSlice<Point2d> centers (0, count-1, PK_TEMP);
		for (int i=centers.Low(); i<centers.High(); ++i) 
		{
			const float x = float(pasrnd.frandom())*float(SizeX());
			const float y = float(pasrnd.frandom())*float(SizeY()); 
			centers[i] = Point2d(x,y);
		}

		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			float r = float(SizeX())*3.0f;
			for (int k=centers.Low(); k<centers.High(); ++k)
			{
				float rx = abs(float(i)-centers[k].x);
				if (rx*2>=float(SizeX())) rx = float(SizeX())-rx;
				float ry = abs(float(j)-centers[k].y);
				if (ry*2>=float(SizeY())) ry = float(SizeY())-ry;
				if (r>rx) r=rx;
				if (r>ry) r=ry;
			}
			at(i,j) = tblib::Min(r, 2.0f);
		}
		Normalize();
	}   

	void FillNoisePerlin ()
	{
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = 0.0f;
		for (int l=1; l<=ShX(); ++l)
		{
			Noise n(l,l);
			for (int j=0; j<n.SizeY(); ++j) for (int i=0; i<n.SizeX(); ++i)
				n.at(i,j) = float(pasrnd.frandom());
			float scale = 1.0f / float(1<<(ShX()-l));
			for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
				at(i,j) += n.smoothAt(float(i)*scale, float(j)*scale) / float(1<<l);
		}
		Normalize();
	}

	void Ray (bool first, int x,int y,int d)
	{
		static tblib::Array<int,4> dx,dy;
		if (dx.Size()==0)
		{		
			dx.EmplaceBack(-1); dx.EmplaceBack( 0); dx.EmplaceBack( 1); dx.EmplaceBack( 0);
			dy.EmplaceBack( 0); dy.EmplaceBack( 1); dy.EmplaceBack( 0); dy.EmplaceBack(-1);	
		}

		for (;;)
		{
			int k = pasrnd.random(SizeX()/2)+SizeX()/4;
			for (int i=0; i<k; ++i)
			{
				if (first && at(x,y)<1.0f) return;
				first = true;
        at(x,y) = 0.0f;
				x += dx[d];
				y += dy[d];
			}
			if (pasrnd.random(2)==0)
			{
				at(x-dx[d]*2-dy[d]*2, y-dy[d]*2+dx[d]*2) = 0.0f;
				d = (d+3) & 3;
			} else
			{
				at(x-dx[d]*2+dy[d]*2, y-dy[d]*2-dx[d]*2) = 0.0f;
				d = (d+1) & 3;
			}
		}
	}

	void RandomLine ()
	{
		const int 
			ix = pasrnd.random(SizeX()),
			iy = pasrnd.random(SizeY()),
			id = pasrnd.random(3);
		Ray(true,  ix, iy,  id);
		Ray(false, ix, iy, (id+1+pasrnd.random(2)*2) & 3);
	}

	void FillNoiseTechLines ()
	{
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j)=1.0f;
		const int cnt = 4+pasrnd.random(5);
		for (int i=0; i<cnt; ++i)
			RandomLine();
	}

	void FillNoiseDirectStone (int count)
	{
		MSlice<Point2d> centers (0, count-1, PK_TEMP);
		for (int i=centers.Low(); i<centers.High(); ++i) 
		{
			const float x = float(pasrnd.frandom())*float(SizeX());
			const float y = float(pasrnd.frandom())*float(SizeY()); 
			centers[i] = Point2d(x,y);
		}

		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			float r1 = float(SizeX())*3.0f;
			float r2 = float(SizeY())*3.0f;
			for (int k=centers.Low(); k<centers.High(); ++k)
			{
				float rx = abs(float(i)-centers[k].x);
				if (rx*2>=float(SizeX())) rx = float(SizeX())-rx;
				float ry = abs(float(j)-centers[k].y);
				if (ry*2>=float(SizeY())) ry = float(SizeY())-ry;
        float cr = abs(rx)+abs(ry);
				if (cr<r1) { r2=r1; r1=cr; } else if (cr<r2) { r2=cr; }
			}
			at(i,j) = tblib::Min(abs(r1-r2), 2.0f);
		}
		Normalize();
	}

	void Reverse ()
	{
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = 1.0f-at(i,j);
	}
};

class Texture : public PixelMatrix<TexturePixel>
{
	bool m_dithering;

	struct GetColor 
	{
		float rb,rd,gb,gd,bb,bd;
		GetColor(float rb, float rd, float gb, float gd, float bb, float bd) : 
		rb(rb),rd(rd),gb(gb),gd(gd),bb(bb),bd(bd) {}
	};

	Texture (const Texture&);
	Texture& operator=(const Texture&);

public :
	Texture (int shX, int shY) : PixelMatrix<TexturePixel> (shX,shY,PK_GLOBAL), m_dithering(true) {}

	bool Dithering () { return m_dithering; }

	Pixel NearestCorrectedIndex (int r, int g, int b)
	{
		return NearestIndex(r, (r+g+b)/3, b);
	}

	void FillTxrByNoise (const Noise& noise, bool light)
	{
		float r1,g1,b1,r2,g2,b2;
		r1 = float(pasrnd.frandom())*0.2f;
		g1 = float(pasrnd.frandom())*0.2f;
		b1 = float(pasrnd.frandom())*0.2f;

		static tblib::Array<GetColor,7> koeff;
		if (koeff.Size()==0)
		{
			koeff.EmplaceBack(GetColor(1.0f,0.0f, 1.0f,0.0f, 1.0f,0.0f));
			koeff.EmplaceBack(GetColor(0.2f,0.4f, 1.0f,0.0f, 1.0f,0.0f));
			koeff.EmplaceBack(GetColor(1.0f,0.0f, 0.4f,0.4f, 0.2f,0.4f));
			koeff.EmplaceBack(GetColor(1.0f,0.0f, 0.2f,0.4f, 0.2f,0.4f));
			koeff.EmplaceBack(GetColor(0.5f,0.4f, 1.0f,0.0f, 0.2f,0.4f));
			koeff.EmplaceBack(GetColor(1.0f,0.0f, 1.0f,0.0f, 0.4f,0.4f));
			koeff.EmplaceBack(GetColor(0.2f,0.4f, 0.2f,0.4f, 1.0f,0.0f));
		}

		int koeffn = pasrnd.random(7);
		r2 = koeff[koeffn].rb + float(pasrnd.frandom())*koeff[koeffn].rd;
		g2 = koeff[koeffn].gb + float(pasrnd.frandom())*koeff[koeffn].gd;
		b2 = koeff[koeffn].bb + float(pasrnd.frandom())*koeff[koeffn].bd;

		Noise nr(ShX(), ShY()), ng(ShX(), ShY()), nb(ShX(), ShY());

		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			nr.at(i,j) = r1 + noise.at(i,j)*(r2-r1);
			ng.at(i,j) = g1 + noise.at(i,j)*(g2-g1);
			nb.at(i,j) = b1 + noise.at(i,j)*(b2-b1);
		}

		if (light) for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			nr.at(i,j) = tblib::Min(nr.at(i,j)+0.5f, 1.0f);
			ng.at(i,j) = tblib::Min(ng.at(i,j)+0.5f, 1.0f);
			nb.at(i,j) = tblib::Min(nb.at(i,j)+0.5f, 1.0f);
		}

		const float mc = light?0.9f:0.7f;

		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			nr.at(i,j) *= (mc + float(pasrnd.frandom()) * (1-mc));
			ng.at(i,j) *= (mc + float(pasrnd.frandom()) * (1-mc));
			nb.at(i,j) *= (mc + float(pasrnd.frandom()) * (1-mc));
		}
		
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			at(i,j) = NearestCorrectedIndex(
				int(nr.at(i,j)*255.0f),
				int(ng.at(i,j)*255.0f),
				int(nb.at(i,j)*255.0f));
		}
	}	
  
  void FillTechTxr(bool light)
	{
    m_dithering = false;
		Noise n(ShX(),ShY());
		if (pasrnd.random(2)==0)	
			n.FillNoiseTechLines();
		else 	
			n.FillNoiseDirectStone(4+pasrnd.random(5));

		if (!light && pasrnd.random(2)==0) 
		{
			n.Reverse();
			light=true;
		}
		FillTxrByNoise(n, light);
	}

	void FillAncientTxr()
	{
		Noise n(ShX(), ShY());
		switch (pasrnd.random(20))
		{
		case 1 : 
			n.FillNoiseGrass(); break;
		case 0:case 2:case 3:case 7: case 8:
			n.FillNoiseStone(8+pasrnd.random(57)); break;
		case 4:case 5:case 6:
			n.FillNoiseLines(2+pasrnd.random(3)); break;
		case 9:case 10:case 11:case 12:case 13:case 14:case 15:
			{
				const float freq = 0.2f + float(pasrnd.frandom())*0.3f;
				const int   cnt  = 2+pasrnd.random(4);

			n.FillNoiseWave( 1.0f, freq, cnt); 
			if (pasrnd.random(2)==0) n.Reverse();
			}
			break; 
		default : n.FillNoisePerlin();
		}		
    FillTxrByNoise(n, false);
	}

	void FillMonsterTxr()
	{
		m_dithering = false;
    Pixel iw = NearestCorrectedIndex(255,255,255), ib = NearestCorrectedIndex(0,0,0);
		FillAncientTxr();
		m_dithering = false;
		for (int i=-5; i<=5; ++i) for (int j=-5; j<5; ++j)
			if (i*i+j*j<=5)
				at(i+6,j+6) = 
					(i*i+j*j <= 0) ? ib :
					(i*i+j*j <= 2) ? iw :
					ib;
		for (int i=0; i<=8; ++i) for (int j=-1; j<1; ++j)
			at(i,j+12) = (j!=0 && i%2!=1) ? iw : ib;
	}

	void FillMetalTxr()
	{
		m_dithering = false;
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			const int i8 = (i/2 & 7), j8 = (j/2 & 7);
			const float r = (
				(j8==0 && i8!=5 && i8!=7) ||
				(j8==4 && i8!=1 && i8!=3) ||
				(i8==2 && j8!=1 && j8!=7) ||
				(i8==6 && j8!=3 && j8!=5)) 
				*0.7f + float(pasrnd.frandom())*0.3f;
			at(i,j) = NearestCorrectedIndex(
				int(r*255.0f),
				int(r*192.0f),
				int(r*128.0f));
		}
	}	

	void ProcessSkyBoxTxr (float dt, bool hell)
	{
		static const Point2d __pt[4] = {
			Point2d( 0.0f,  5.0f),
			Point2d( 3.0f, 40.0f),
			Point2d(50.0f,  7.0f),
			Point2d( 6.0f, 10.0f)
		};

		static tblib::CArray <Point2d, 4> pt(__pt);
		static tblib::Array <Pixel, 10> csky,chell;
		if (csky.Size()==0)
		{
			for (int i=0; i<csky.Capacity(); ++i)
				csky.EmplaceBack(NearestCorrectedIndex(100,155,150+i*100/csky.Capacity()));
			for (int i=0; i<chell.Capacity(); ++i)
				chell.EmplaceBack(NearestCorrectedIndex(255,50+i*100/chell.Capacity(),50+i*200/chell.Capacity()));
		}
		for (int i=0; i<3; ++i)
		{
			pt[i].x += (float(i*i)*1.8f -          1.5f)*dt;
			pt[i].y += (float(i  )*2.1f + pt[i].x*0.03f)*dt;
			while	(pt[i].x>=float(SizeX())) pt[i].x -= float(SizeX());
			while	(pt[i].x<0)               pt[i].x += float(SizeX());
			while	(pt[i].y>=float(SizeY())) pt[i].y -= float(SizeY());
			while	(pt[i].y<0)               pt[i].y += float(SizeY());
		}
		Noise n(ShX(), ShY());
		n.FillNoiseWaveByCenters(1, 0.1f, pt.GetSlice(0, pt.Capacity()));
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = (hell ? chell : csky) [int(n.at(i,j)*9.0)];
	}

	void FillBaseGrass ()
	{
		Noise n(ShX(), ShY());
		n.FillNoiseGrass();
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex(80, 50+int(n.at(i,j)*150.0f), 40);
	}

	void FillBaseWall ()
	{
		Noise n1(ShX(), ShY()), n2(ShX(), ShY());
		n1.FillNoiseStone(32);
		n2.FillNoiseStone(3);
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
		{
			const int 
				tr = int(n1.at(i,j)*150.0f)+pasrnd.random(55),
				tg = int(n1.at(i,j)*100.0f)+pasrnd.random(55),
				tb = int(200.0f-n2.at(i,j)*200.0f)+pasrnd.random(55),
				tm = (tr+tg+tb)/3;
			at(i,j) = NearestCorrectedIndex((tr+tm)/2, (tg+tm)/2, (tb+tm)/2);
		}
	}
	
	void FillBaseStoneFloor ()
	{
		Noise n(ShX(), ShY());
		n.FillNoiseStone(16);
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex(
				int(n.at(i,j)*250.0f)+pasrnd.random(5),
				int(n.at(i,j)*200.0f),
				int(n.at(i,j)*150.0f)+pasrnd.random(100));	
	}

	void FillBaseUpperWall ()
	{
		Noise n1(ShX(), ShY()), n2(ShX(), ShY());
		n1.FillNoiseWave(1.0f,0.2f,5);
		n2.FillNoiseStone(3);
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex(
				int(n1.at(i,j)*250.0f),
				155,
				155+int(n1.at(i,j)*100.0f)-int(n2.at(i,j)*100.0f));	
	}

	void FillBaseWood ()
	{
		Noise n(ShX(), ShY());
		n.FillNoiseWave(24.0f, 0.2f, 3);
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex ( 
				int (n.at(i,j)*255.0f),
				64,
				0);
	}	

	void FillBlack ()
	{
		m_dithering = false;
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex (0,0,0);
	}
	
	void FillBaseHellFloor ()
	{
		Noise n(ShX(), ShY());
		n.FillNoiseStone(64);
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex ( 
				int (n.at(i,j)*50.0f)+150+pasrnd.random(51),
				int (n.at(i,j)*100.0f)+50,
				int (n.at(i,j)*250.0f));
	}		

	void FillBaseHellWall ()
	{
		Noise n1(ShX(), ShY()), n2(ShX(), ShY());
		n1.FillNoiseStone(32);
		n2.FillNoiseStone(3);
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex(
				int(n1.at(i,j)*120.0f)+pasrnd.random(55),
				40,
				180-int(n2.at(i,j)*180.0f));	
	}

	void FillBaseBlood ()
	{
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
      if (pasrnd.random(10) == 0)
        at(i,j) = NearestIndex(255, pasrnd.random(128)+0, 255);
      else
	  		at(i,j) = NearestCorrectedIndex(255, pasrnd.random(128)+0, 255);
	}
	
	void FillBaseBlue ()
	{
		for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
			at(i,j) = NearestCorrectedIndex(100, pasrnd.random(164), 200);
	}

  void ChessHoles ()
  {
    for (int j=0; j<SizeY(); ++j) for (int i=0; i<SizeX(); ++i)
    {
      if (((i>>3) + (j>>3)) & 1) 
        at(i,j) |= 0x1600;
    }
  }
};

tblib::Array<Texture, 14>  baseTxr;
tblib::Array<Texture, 1> ancTxr;
tblib::Array<Texture, 1>  techTxr;
tblib::Array<Texture, 1>  mstrTxr;
tblib::Array<Texture, 1>  boxTxr;

enum BaseTxrIndex {
	TXR_SKY=0, TXR_GRASS,	TXR_WALL, TXR_STONE_FLOOR, TXR_UPPER_WALL, 
	TXR_WOOD, TXR_METAL, TXR_HELL, TXR_BLACK, TXR_HELL_FLOOR,
	TXR_LIGHT, TXR_HELL_WALL, TXR_BLOOD, TXR_BLUE };

void InitTextures ()
{	
  for (int k=0; k<1; ++k)
  {
    for (int i=0; i<baseTxr.Capacity(); ++i) 
      baseTxr.EmplaceBack(6,6);
	  baseTxr[TXR_SKY].ProcessSkyBoxTxr(0, false);
	  baseTxr[TXR_GRASS].FillBaseGrass();
	  baseTxr[TXR_WALL].FillBaseWall();
	  baseTxr[TXR_STONE_FLOOR].FillBaseStoneFloor();
	  baseTxr[TXR_UPPER_WALL].FillBaseUpperWall();
	  baseTxr[TXR_WOOD].FillBaseWood();
	  baseTxr[TXR_METAL].FillMetalTxr();
	  baseTxr[TXR_HELL].ProcessSkyBoxTxr(0, true);  
	  baseTxr[TXR_BLACK].FillBaseBlood();
	  baseTxr[TXR_HELL_FLOOR].FillBaseHellFloor();
	  baseTxr[TXR_LIGHT].FillTechTxr(true);
	  baseTxr[TXR_HELL_WALL].FillBaseHellWall();
	  baseTxr[TXR_BLOOD].FillBaseBlood();
	  baseTxr[TXR_BLUE].FillBaseBlue();
    for (int i=0; i<baseTxr.Capacity(); ++i) 
      baseTxr[i].ChessHoles();
  }
  
#ifdef PURE_RENDER
  for (int i=0; i< ancTxr.Capacity(); ++i)  ancTxr.EmplaceBack(6,6);
#else
  for (int i=0; i< ancTxr.Capacity(); ++i)  ancTxr.EmplaceBack(6,6);
#endif
  for (int i=0; i<techTxr.Capacity(); ++i) techTxr.EmplaceBack(6,6);
  for (int i=0; i<mstrTxr.Capacity(); ++i) mstrTxr.EmplaceBack(6,6);
  for (int i=0; i< boxTxr.Capacity(); ++i)  boxTxr.EmplaceBack(6,6);

  
	for (int i=0; i<techTxr.Size(); ++i) 
		techTxr[i].FillTechTxr(false);

  for (int i = 0; i < ancTxr.Size(); ++i) 
    ancTxr[i].FillAncientTxr();

  for (int i = 0; i < mstrTxr.Size(); ++i) 
    mstrTxr[i].FillMonsterTxr();

  for (int i = 0; i < boxTxr.Size(); ++i) 
    boxTxr[i].FillAncientTxr();
}

void ProcessTextures (float dt)
{
	baseTxr[TXR_SKY].ProcessSkyBoxTxr(dt, false);
	baseTxr[TXR_HELL].ProcessSkyBoxTxr(dt, true);  

}