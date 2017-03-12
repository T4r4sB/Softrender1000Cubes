#pragma once

#include "tbal.h"
#include "tbslice.h"
#include "tbstring.h"
 
namespace tbal
{
	class BufferTextReader 
	{
		tblib::StringRef buffer;
    int curlow, curhigh;
		int errlow, errhigh;
		int curline, errline;
		tbal::Asset asset;

		static bool GoodSymbol (char c)
		{
			return ((unsigned char)c>' ' && c!='#');
		}
		
		static bool GoodOrSpace (char c)
		{
			return ((unsigned char)c>=' ' && c!='#');
		}

	protected :

		void NextString ()
		{
			bool wascom=false;
			curlow = curhigh;
			while (curlow<buffer.High() && (!GoodSymbol(buffer[curlow]) || wascom))
			{
				if (buffer[curlow]=='#') wascom = true;
				if (buffer[curlow]=='\r') {++curline; wascom = false;}
				++curlow;
			}
			curhigh = curlow;
			while (curhigh<buffer.High() && GoodSymbol(buffer[curhigh])) 
				++curhigh;
		}

		void EnlargeCurString ()
		{
			while (curhigh<buffer.High() && GoodOrSpace(buffer[curhigh])) 
				++curhigh;
		}

		void ThrowError()
		{
			errlow = curlow;
			errhigh = curhigh;
			errline = curline;
		}

		bool HasString ()
		{
			return curlow<curhigh;
		}

		tblib::StringRef GetString ()
		{
			return buffer.GetSlice(curlow, curhigh);
		}		

		bool HasError ()
		{
			return errlow<errhigh;
		}

		tblib::StringRef ReadLongString ()
		{
			if (HasError()) return tblib::StringRef();
			EnlargeCurString();
			tblib::StringRef result = GetString ();
			NextString();
			return result;
		}		

		tblib::StringRef ReadString ()
		{
			if (HasError()) return tblib::StringRef();
			tblib::StringRef result = GetString ();
			NextString();
			return result;
		}		
		
		tblib::StringRef GetErrString ()
		{
			tblib::StringRef result = buffer.GetSlice(errlow, errhigh);
			return result;
		}

		int GetErrLine ()
		{
      return errline;
		}

		BufferTextReader (tblib::StringRef fileName)
		{
			asset.Open(fileName);
			buffer = asset.GetBuffer();
			curline = 1;
			curlow  = buffer.Low();
			curhigh = curlow;
			errlow  = curlow;
			errhigh = curhigh;
			NextString();
		}
	};
};