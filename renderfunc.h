#pragma once

#include "rdtsc.h"
#include "trunc.h"

struct RenderFunc
{
	float vplt, vdx, vdy;

	float operator () (int x, int y) const
	{
		return vplt + vdx*float(x) + vdy*float(y);
	}

	void Correct (int sx, int sy)
	{
		vdx /= float (sx);
		vdy /= float (sy);
		vplt += vdx*1.00f - vdy*1.00f;
	}
};

struct RenderInfo
{
	RenderFunc w,tx,ty;
	Texture* texture;
	int light;
  bool useTransparent;
};

struct RenderPoint
{
	int x,y;
	float w,tx,ty,txw,tyw;

	void Shift (int dx, float dw, float dtx, float dty)
	{
		x += dx;
		w += dw;
		tx += dtx;
		ty += dty;
	}

	RenderPoint (const RenderInfo& r, int x, int y) : x(x),y(y)
	{		
		w  = r.w (x,y);
		tx = r.tx(x,y);
		ty = r.ty(x,y);
	}

	void Precompute ()
	{
		const float minf = 0.0001f;
		float z = w<minf ? 1.0f/minf : 1.0f/w;
		txw = tx*z;
		tyw = ty*z;
	}
};

