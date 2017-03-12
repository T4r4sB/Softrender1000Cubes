#pragma once

#include "framebuffer.h"
#include "renderfunc.h"
#include "sbuffer.h"
#include "trunc.h"


const float minD = 0.001f;

struct ScreenLine : MSlice<int>
{
	Face *f1, *f2;
	bool valid;
};

struct Bound 
{ 
	int v1, v2; 
};

struct ScreenZone : MSlice<Bound>
{
  Face *f;

	ScreenZone() {}

	void Init (Face* af, int b1, int b2) {
		this->f = af;
		MSlice<Bound>::Init(b1,b2,PK_TEMP);
	}

	ScreenZone (Face* f, int b1, int b2) : f(f), MSlice<Bound> (b1,b2,PK_TEMP) {}
};

struct ShowSectorInfo
{  
	FrameWindow                 b;
	SBuffer                     filled;
	MSlice<int16_t>             cut;
	Matrix3D                    m;
	Point3D                     viewP, plt,dx,dy;
	ScreenZone                  mainZ;
	MArray<CheckPtr<Face> >     faces;
	MArray<CheckPtr<Sector> >   nearSectors;
  bool                        useTransparent;
};

struct TemporaryShowSectorInfo
{
	Sector             &s;
	bool               onlyAff;
	const ScreenZone   &curZ;
	Point3D            nTest;

	TemporaryShowSectorInfo (Sector&s, bool onlyAff, const ScreenZone &curZ, Point3D nTest) 
		: s(s),onlyAff(onlyAff),curZ(curZ),nTest(nTest) {}
private :
	TemporaryShowSectorInfo(const TemporaryShowSectorInfo&);
	TemporaryShowSectorInfo&operator=(const TemporaryShowSectorInfo&);
};

void ShowSector(ShowSectorInfo& si, const TemporaryShowSectorInfo& tsi, bool cutCheck);

const int startDiseringX[4] = { -0x6000, +0x2000, -0x2000, +0x6000 };
const int startDiseringY[4] = { -0x2000, +0x6000, -0x6000, +0x2000 };

void LoopDith(Pixel* l, int count, Pixel* txrBeg, int shX, int mask, int tx, int dtx, int ty, int dty, int ord)
{
  /*
  assert (ord>=0 && ord<4);
  int dsx = startDiseringX[ord], dsy = startDiseringY[ord];*/

  switch (ord) 
  {
  case 0 :
    tx += 0x4000;

    for (;;)
    {
      if (count-- == 0) break;      

      *l++ = txrBeg [ (((tx)>>16) + (((ty)>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;

      if (count-- == 0) break;

      *l++ = txrBeg [ (((tx + 0x4000)>>16) + (((ty + 0xc000)>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;
    }
    break;
  case 1 :
    tx += 0x4000;

    for (;;)
    {
      if (count-- == 0) break;

      *l++ = txrBeg [ (((tx + 0x4000)>>16) + (((ty + 0xc000)>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;

      if (count-- == 0) break;      

      *l++ = txrBeg [ (((tx)>>16) + (((ty)>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;
    }
    break;
  case 2 :
    ty += 0x4000;

    for (;;)
    {
      if (count-- == 0) break;

      *l++ = txrBeg [ (((tx + 0xc000)>>16) + (((ty + 0x4000)>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;

      if (count-- == 0) break;

      *l++ = txrBeg [ (((tx)>>16) + (((ty )>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;
    }
    break;
  case 3 :
    ty += 0x4000;

    for (;;)
    {
      if (count-- == 0) break;

      *l++ = txrBeg [ (((tx)>>16) + (((ty )>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;

      if (count-- == 0) break;

      *l++ = txrBeg [ (((tx + 0xc000)>>16) + (((ty + 0x4000)>>16)<<shX)) & mask ];
      tx += dtx;
      ty += dty;
    }
    break;
  }
}

void LoopNoDith(Pixel* l, int count, TexturePixel* txrBeg, int tx, int dtx, int ty, int dty)
{
	for(;;)
	{
    *l++ = Pixel(txrBeg [ ((tx>>16) + ((ty>>16)<<6)) & 4095 ]);
    if (--count == 0) 
      break;
		tx += dtx;
		ty += dty;
	}
}

void TransparentLoopNoDith(Pixel* l, int count, TexturePixel* txrBeg, int tx, int dtx, int ty, int dty)
{
  for(;;)
  {
    TexturePixel src = txrBeg [ ((tx>>16) + ((ty>>16)<<6)) & 4095 ];
    *l += Pixel(Pixel(src) - *l) >> (src >> 8);     
    ++l;  
    if (--count == 0) 
      break;
    tx += dtx;
    ty += dty;
  }
}

void RenderLine (FrameWindow::Line l, RenderPoint& p1, RenderPoint& p2, int tail, const RenderInfo &ri)								 
{	
	int dtx,dty;
	if (p1.x<p2.x+tail-1)
	{
		const float dx = 1.0f / (p2.x-p1.x);
		dtx = HackTrunc((p2.txw-p1.txw)*dx, 16);
		dty = HackTrunc((p2.tyw-p1.tyw)*dx, 16);
	} else 
	{
		dtx = 0;
		dty = 0;
	}
	int tx  = HackTrunc(p1.txw, 16);
	int ty  = HackTrunc(p1.tyw, 16);	

  /*
  if (ri.texture->Dithering())
    LoopDith(&l[p1.x], p2.x+tail-p1.x, ri.texture->data(), ri.texture->ShX(), ri.texture->MaskAll(), tx, dtx, ty, dty, (p1.x&1) + ((p1.y&1)<<1));	
  else*/
  if (ri.useTransparent)
    TransparentLoopNoDith(&l[p1.x], p2.x+tail-p1.x, ri.texture->data(), tx, dtx, ty, dty);
  else
	  LoopNoDith(&l[p1.x], p2.x+tail-p1.x, ri.texture->data(), tx, dtx, ty, dty);	
}

void ComputeRenderInfo(ShowSectorInfo& si, const TemporaryShowSectorInfo& tsi, const ScreenZone &z, RenderInfo& ri)
{	
	TaktCounter tc(TIMER3);

	const float den = 1.0f / (-z.f->Dist(tsi.nTest));//dist отрицательно если мы внутри

	const float dxn  = Dot(si.dx,  z.f->n);
	const float dyn  = Dot(si.dy,  z.f->n);
	const float pltn = Dot(si.plt, z.f->n);

	const float dxtx  = Dot(si.dx,  z.f->vtx);
	const float dytx  = Dot(si.dy,  z.f->vtx);
	const float plttx = Dot(si.plt, z.f->vtx);
	const float nvtx  = Dot(tsi.nTest, z.f->vtx);
	
	const float dxty  = Dot(si.dx,  z.f->vty);
	const float dyty  = Dot(si.dy,  z.f->vty);
	const float pltty = Dot(si.plt, z.f->vty);				
	const float nvty  = Dot(tsi.nTest, z.f->vty);

	ri.w.vdx  = dxn  * den;
	ri.w.vdy  = dyn  * den;
	ri.w.vplt = pltn * den;

	ri.tx.vdx  = ri.w.vdx  * (nvtx - z.f->vtxc) + dxtx;
	ri.tx.vdy  = ri.w.vdy  * (nvtx - z.f->vtxc) + dytx;
	ri.tx.vplt = ri.w.vplt * (nvtx - z.f->vtxc) + plttx;
														
	ri.ty.vdx  = ri.w.vdx  * (nvty - z.f->vtyc) + dxty;
	ri.ty.vdy  = ri.w.vdy  * (nvty - z.f->vtyc) + dyty;
	ri.ty.vplt = ri.w.vplt * (nvty - z.f->vtyc) + pltty;

	ri.w .Correct(si.b.SizeX(), si.b.SizeY());
	ri.tx.Correct(si.b.SizeX(), si.b.SizeY());
	ri.ty.Correct(si.b.SizeX(), si.b.SizeY());
  
	ri.texture = z.f->texture;
  assert(ri.texture);
	ri.light   = z.f->light;
  ri.useTransparent = si.useTransparent;
}

void Fill (ShowSectorInfo& si, const TemporaryShowSectorInfo& tsi, const ScreenZone &z)
{

	int y1 = z.Low(), y2 = z.High()-1;
	if (y1>y2) return;
	while (y1<=y2 && z[y1].v1 >= z[y1].v2) 
		++y1;
	while (y1<=y2 && z[y2].v1 >= z[y2].v2) 
		--y2;

	if (y1<=y2)
	{
		Sector* s = z.f->nextSector;
		
		if (s)
		{			
			if (s->inProcess==0)
				ShowSector(si, TemporaryShowSectorInfo(*s, tsi.onlyAff, z, s->skybox ? Point3D(0.0f,0.0f,0.0f) : tsi.nTest), false);
		} else	
		{
			assert(z.f->texture);			
			FrameWindow::Line l = si.b[y1];		

			
			const int afflen = si.b.SizeX()<=400 ? 16 : 32; // длина аффинных блоков

			RenderInfo& ri = z.f->renderInfo;
			bool computed = false;	
			

			for (int y = y1; y<=y2; ++y)
			{
				assert(z[y].v1>=0 && z[y].v2<=si.b.SizeX());
      
				if (z[y].v1<z[y].v2)
				{		  
          int cutlines = si.useTransparent ?
				  	si.filled.CutLineNoChange(z[y].v1, z[y].v2, y, si.cut):
            si.filled.CutLine(z[y].v1, z[y].v2, y, si.cut);
          
					for (int sbl=0; sbl<cutlines; ++sbl)
					{					
						if (!computed)
						{
							ComputeRenderInfo(si, tsi, z, ri);				
							computed = true;
						}		     
											
						RenderPoint rp1(ri, si.cut[sbl*2  ]  , y);
						RenderPoint rp2(ri, si.cut[sbl*2+1]-1, y);			

						rp1.Precompute();
						rp2.Precompute();
                                    
						if (rp1.x<=rp2.x)
						{
							
							if (tsi.onlyAff)
							{
								RenderLine(l,rp1,rp2,1,ri);								
							} else for (;;)
							{
								const int nx = (rp1.x+afflen) & ~(afflen-1);
								if (nx<=rp2.x)
								{
									RenderPoint mid(ri,nx,y);
									mid.Precompute();
									RenderLine(l,rp1,mid,0,ri);
									rp1=mid;
								} else
								{
									RenderLine(l,rp1,rp2,1,ri);
									break;
								}
							}
						}
					}
				}
				si.b.ProcessLine(l);
			}	
		}
	}
}