#pragma once
#include "fixed.h"	
#include "tbarr.h"
#include "tbarr2.h"
#include "tbstring.h"

namespace tbal {	
	class Font 	{
		tblib::CArray<tblib::Array2D<int>, 256> images;
		tblib::CArray<int, 256> widths;
		int				 height;
		Fixed scale, width;
	public:
		Font () : scale(fx0), width(fx0) {}
		void Init  (Fixed scale, Fixed width);		
		int Width  (tblib::StringRef c) const;
		int Height (tblib::StringRef c) const;
		void OutText (const tbal::Bitmap &b, int x, int y, tblib::StringRef c, tbal::Color color, int alignX=0, int alignY=0) const;
	};
};
