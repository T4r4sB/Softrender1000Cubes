#pragma once

#include "tbarr2.h"
#include "tbdynarr.h"
#include "winnt.h"
#include "rdtsc.h"

class SBuffer {
public :
	tblib::Array2D<int16_t> data;

public :
	SBuffer() {}
	SBuffer(int width, int height) {
		Init(width, height);
	}

	void Init(int width, int height) 
	{ 
		if (width+2 != data.SizeX() || height != data.SizeY())
		  tblib::Recreate(data, width+2, height);

		for (int j=0; j<height; ++j) 
		{
			tblib::Array2D<int16_t>::Line line = data[j];
			line[0] = 1;
			line[1] = int16_t(data.SizeX()-1);
			line[data.SizeX()-1] = int16_t(data.SizeX()-1);
		}
	}

	int CutLine(int x1, int x2, int y, tblib::Slice<int16_t> coords ) 
	{		
		TaktCounter tc(TIMER1);

		assert(x1 < x2);
		assert(x1 >= 0 && x2 <= data.SizeX()-2);
		int result = 0;
		tblib::Array2D<int16_t>::Line line = data[y];

		int lastprevc = -1;
		for (int prevc = 0,	segf = line[prevc], segl = line[segf]; segf-1 < x2; prevc = segl, segf = line[prevc], segl = line[segf])
		{
			assert(segf<segl);
			if (segl-1 <= x1) 
			{
			  // ничё не делать
			} else {
				int rx1, rx2;
				
				int lsegf = line[segf];

				if (x1 <= segf-1)
				{
					if (lastprevc < 0) 
						lastprevc = prevc;
					rx1 = segf-1;
				} else 
				{
					line[segf] = int16_t(x1+1);
					lastprevc = x1+1;
					rx1 = x1;
				}

				if (x2 >= segl-1) {
					line[lastprevc] = int16_t(line[segl]);
					rx2 = segl-1;
				} else {
					line[x2+1] = int16_t(lsegf);
					line[lastprevc] = int16_t(x2+1);
					rx2 = x2;
				}


				assert (rx1 < rx2);

				coords[result++] = int16_t(rx1);
				coords[result++] = int16_t(rx2);
			}
		}    

		return result/2;
	}
	
	int CutLineNoChange(int x1, int x2, int y, tblib::Slice<int16_t> coords ) 
	{		
		assert(x1 < x2);
		assert(x1 >= 0 && x2 <= data.SizeX()-2);
		int result = 0;
		tblib::Array2D<int16_t>::Line line = data[y];

		for (int segf = line[0], segl = line[segf]; segf-1 < x2; segf = line[segl], segl = line[segf])
		{
			assert(segf<segl);
			if (segl-1 <= x1) 
			{
			  // ничё не делать
			} else {
				int rx1 = tblib::Max(x1, segf-1);
				int rx2 = tblib::Min(x2, segl-1);
				assert (rx1 < rx2);
				coords[result++] = int16_t(rx1);
				coords[result++] = int16_t(rx2);
			}
		}    

		return result/2;
	}
	
	bool TestCutLine(int x1, int x2, int y) 
	{		
		assert(x1 < x2);
		assert(x1 >= 0 && x2 <= data.SizeX()-2);
		tblib::Array2D<int16_t>::Line line = data[y];

		for (int segf = line[0], segl = line[segf]; segf-1 < x2; segf = line[segl], segl = line[segf])
		{
			assert(segf<segl);
			if (segl-1 <= x1) 
			{
			  // ничё не делать
			} else {
				return true;
			}
		}    

		return false;
	}
  

};