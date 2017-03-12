#pragma once

#define PURE_RENDER

#include "tbal.h"
#include "tbarr.h"

typedef uint8_t Pixel;
typedef uint16_t TexturePixel;
typedef tblib::CArray<tbal::Color, 256> Palette;

const int FOG_SIZE = 32;

Palette palette;
tblib::CArray<tblib::CArray<Pixel, 256>, FOG_SIZE> fogTable;

void InitPalette ()
{
	tblib::Array<tbal::Color, 15> baseColors;
  
	baseColors.EmplaceBack(tbal::Color(255,140,128));
	baseColors.EmplaceBack(tbal::Color(255,120,192));
	baseColors.EmplaceBack(tbal::Color(255,100,255));
	baseColors.EmplaceBack(tbal::Color(192,120,255));
  baseColors.EmplaceBack(tbal::Color(128,140,255));

	baseColors.EmplaceBack(tbal::Color(255,64,  0));
	baseColors.EmplaceBack(tbal::Color(255,96,128));
	baseColors.EmplaceBack(tbal::Color(255,128,255));
	baseColors.EmplaceBack(tbal::Color(128,192,255));
	baseColors.EmplaceBack(tbal::Color(  0,128,255));

	baseColors.EmplaceBack(tbal::Color(255, 85,  0));
	baseColors.EmplaceBack(tbal::Color(255,149,128));
	baseColors.EmplaceBack(tbal::Color(255,212,255));
	baseColors.EmplaceBack(tbal::Color(128,149,255));
	baseColors.EmplaceBack(tbal::Color(  0, 85,255));
  
	for (int i=0; i<palette.Capacity(); ++i)
	{
		if (i==0)
			palette[i] = tbal::Color(0,0,0);
		else
		{
			int l = (i-1)/15+1;
			int c = (i-1)%15;

			palette[i] = tbal::Color(
				baseColors[c].R()*l/17,
				baseColors[c].G()*l/17,
				baseColors[c].B()*l/17);
		}
	}
}

Pixel NearestIndex (int r, int g, int b)
{
	int dst = 255*255*3;
	uint8_t result=0;
	for (int i=0; i<palette.Size(); ++i)
	{
		const int 
			dR = r-palette[i].R(),
			dG = g-palette[i].G(), 
			dB = b-palette[i].B(), 
			adst = dR*dR+dG*dG+dB*dB;
		if (adst<dst)
		{
			dst = adst;
			result = uint8_t(i);
		}
	}
  return result;
	//return palette[result];
}

void InitFogTable (tbal::Color fog)
{
	for (int i=0; i<fogTable.Capacity(); ++i)
	{
		const int 
			f = (FOG_SIZE-1-i) * 256 / (FOG_SIZE-1);
    //int f=256;
    
		for (int j=0; j<fogTable[i].Capacity(); ++j)
		{
			const int 
				r = (palette[j].R()*f + fog.R()*(256-f))/256,
				g = (palette[j].G()*f + fog.G()*(256-f))/256,
				b = (palette[j].B()*f + fog.B()*(256-f))/256;
			fogTable[i][j] = NearestIndex(r,g,b);
      //fogTable[i][j] = tbal::Color(r,g,b);
		}
	}
}