#pragma once

#include "tbreader.h"
#include "point.h"
#include "bodymass.h"

namespace tbal
{
	class BufferTextNumberReader : public BufferTextReader
	{
		BufferTextNumberReader (const BufferTextNumberReader&);
		BufferTextNumberReader& operator = (const BufferTextNumberReader&);
	protected :
		BufferTextNumberReader (tblib::StringRef fileName) : BufferTextReader (fileName) {}

    int ReadInt () 
		{
			if (HasError()) return 0;
			bool sign = false;
			int i = 0;
			tblib::StringRef curstr = GetString();
			for (int cur=curstr.Low(); cur<curstr.High(); ++cur)
			{
				if (cur==curstr.Low())
				{
					if (curstr[cur]=='+') { sign = false; continue; }
					else if (curstr[cur]=='-') { sign = true; continue; }
				}
				if (curstr[cur]>='0' && curstr[cur]<='9') i=i*10+(curstr[cur]-'0');
				else { 
					ThrowError(); return 0; 
				}
			}
			if (sign) i = -i;
			NextString();
			return i;
		}
		
    int ReadUInt () 
		{
			if (HasError()) return 0;
			uint32_t i = 0;
			tblib::StringRef curstr = GetString();
			for (int cur=curstr.Low(); cur<curstr.High(); ++cur)
			{
				if (curstr[cur]>='0' && curstr[cur]<='9') i=i*10+(curstr[cur]-'0');
				else { ThrowError(); return 0; }
			}
			NextString();
			return i;
		}
		
    Fixed ReadFixed ()
		{
			if (HasError()) return fx0;
			bool sign = false, wasdot = false;
			int num=0, den=1;
			tblib::StringRef curstr = GetString();
			for (int cur=curstr.Low(); cur<curstr.High(); ++cur)
			{
				if (cur==curstr.Low())
				{
					if (curstr[cur]=='+') { sign = false; continue; }
					else if (curstr[cur]=='-') { sign = true; continue; }
				}
				if (curstr[cur]=='.')
					wasdot = true;
				else if (curstr[cur]>='0' && curstr[cur]<='9')
				{
					num = num*10 + (curstr[cur]-'0');
					if (wasdot) 
						den *= 10;
				}
				else { ThrowError(); return 0; }
			}
			if (sign) num = -num;
			NextString();
			return Fixed(num, den);
		}

		Point ReadPoint ()
		{
			Fixed x = ReadFixed();
			Fixed y = ReadFixed();
			return Point(x,y);
		}

		tbphys::Mass ReadMass ()
		{
			int order = ReadInt();
			Fixed v = ReadFixed();
			return tbphys::Mass(order, v);
		}

		tbal::Color ReadColor ()
		{
			int r = ReadInt();
			int g = ReadInt();
			int b = ReadInt();
			return tbal::Color(r,g,b);
		}
	};

};