#pragma once

#include "tbarr2.h"
#include "tbstring.h"


namespace tbal {  
#ifdef ANDROID
	#define TBAL_DIRECT_COLOR
#endif
	
#ifdef _WINDOWS
	#define TBAL_REVERSE_COLOR
#endif
	
#ifdef WIN32
	#define TBAL_REVERSE_COLOR
#endif

	#ifdef TBAL_DIRECT_COLOR
		#ifdef pf16bits
			#define rBits 5
			#define gBits 6
			#define bBits 5
			#define ColorValue uint16_t
			struct ColorStruct {
				uint16_t r:rBits, g:gBits, b:bBits; }; 
		#else
			#define rBits 8
			#define gBits 8
			#define bBits 8
			#define ColorValue uint32_t 
			struct ColorStruct {
				uint16_t r:rBits, g:gBits, b:bBits, extra:32-rBits-gBits-bBits; }; 
		#endif
		#define hBits rBits
		#define mBits gBits
		#define lBits bBits
	#endif

	#ifdef TBAL_REVERSE_COLOR
		#ifdef TBAL_16_BITS
			#define rBits 5
			#define gBits 5
			#define bBits 5
			#define ColorValue uint16_t
			struct ColorStruct	{
				uint16_t b:bBits, g:gBits, r:rBits;}; 
		#else
			#define rBits 8
			#define gBits 8
			#define bBits 8
			#define ColorValue uint32_t 
			struct ColorStruct	{
				uint16_t b:bBits, g:gBits, r:rBits, extra:32-rBits-gBits-bBits; }; 
		#endif
		#define hBits bBits
		#define mBits gBits
		#define lBits rBits
	#endif
				

	class Color {		
		union {
			ColorStruct components;
			ColorValue  v; 
		};
		Color (ColorValue cv) : v(cv) {}
		ColorValue UsedBits(ColorValue cv) const {return cv & ((1 << (rBits+gBits+bBits))-1);}
	public:
		Color(int r, int g, int b) {
			#ifndef TBAL_16_BITS
				components.extra = 0;
			#endif
			components.r = static_cast<uint16_t>(r) >> (8-rBits);
			components.g = static_cast<uint16_t>(g) >> (8-gBits);
			components.b = static_cast<uint16_t>(b) >> (8-bBits);	}
		#ifndef TBAL_16_BITS
		Color(int r, int g, int b, int extra) {
			components.extra = extra;
			components.r = static_cast<uint16_t>(r) >> (8-rBits);
			components.g = static_cast<uint16_t>(g) >> (8-gBits);
			components.b = static_cast<uint16_t>(b) >> (8-bBits);	}
		#endif
		Color() {}
		Color (const tblib::IFStream &s) : v(tblib::To<ColorValue>::From(s)) {} // компилятор студии бредит в этом месте, ему тут мерещится size_t
		friend const tblib::OFStream& operator << (const tblib::OFStream& s, Color c) { return s<<c.v; }
		bool  operator ==(const Color c) const {return v == c.v;}
		bool  operator !=(const Color c) const {return v != c.v;}
		Color operator & (const Color c) const {return Color(v & c.v);}
		Color operator | (const Color c) const {return Color(v | c.v);}
		Color operator ^ (const Color c) const {return Color(v ^ c.v);}
		Color operator + (const Color c) const {return Color(v + c.v);}
		Color operator << (const int shift) const {return Color(v<<shift);}
		Color operator >> (const int shift) const {return Color(v>>shift);}
		Color operator - (const Color c) const {return Color(v - c.v);}
		Color& operator +=(const Color c) {v+=c.v; return *this;}
		Color& operator -=(const Color c) {v-=c.v; return *this;}
		Color& operator &=(const Color c) {v&=c.v; return *this;}
		Color& operator |=(const Color c) {v|=c.v; return *this;}
		Color& operator ^=(const Color c) {v^=c.v; return *this;}

		Color  operator *  (int s) const { return Color (v*s); }
		Color& operator *= (int s) { v*=s; return *this; }

		Color  operator *  (double s) const { return Color (int(components.r*s), int(components.g*s), int(components.b*s)); }
		Color& operator *= (double s) { components.r = int(components.r*s); components.g = int(components.g*s); components.b = int(components.b*s); return *this; }
		
		Color Half  () const {return Color ((v & (~((((( 1 <<hBits) +  1) <<mBits) +  1) <<lBits)&0xffffff) ) >> 1);}
		Color Quart () const {return Color ((v & (~((((( 3 <<hBits) +  3) <<mBits) +  3) <<lBits)&0xffffff) ) >> 2);}
		Color Eigth () const {return Color ((v & (~((((( 7 <<hBits) +  7) <<mBits) +  7) <<lBits)&0xffffff) ) >> 3);}
		Color Sxtnh () const {return Color ((v & (~(((((15 <<hBits) + 15) <<mBits) + 15) <<lBits)&0xffffff) ) >> 4);}
		Color Saturate (int min, int max) const 	{
			const int ar=components.r<<(8-rBits), ag=components.g<<(8-gBits), ab=components.b<<(8-bBits);
			return Color(
				ar<min?min:ar>max?max:ar, 
				ag<min?min:ag>max?max:ag, 
				ab<min?min:ab>max?max:ab); }
		#ifndef TBAL_16_BITS
			int Extra() const {return components.extra;}
			Color NoExtra() const { return *this & Color(0xff,0xff,0xff);}
			friend Color HalfSum(const Color& color1, const Color& color2);
		#endif
		int R() const {return components.r<<(8-rBits);}
		int G() const {return components.g<<(8-gBits);}
		int B() const {return components.b<<(8-bBits);}
	};	

	#ifndef TBAL_16_BITS
		inline Color HalfSum(const Color& color1, const Color& color2) {
			return Color(
				(((color1.v^color2.v)&0xfefefe) >> 1) + (color1.v&color2.v)
				);
		}
	#endif

	#undef rBits
	#undef gBits
	#undef bBits
	#undef ColorValue
	#undef lBits
	#undef mBits
	#undef hBits

	const Color 
		COLOR_BLACK  (0x00, 0x00, 0x00), 
		COLOR_RED    (0xff, 0x00, 0x00), 
		COLOR_GREEN  (0x00, 0xff, 0x00), 
		COLOR_BLUE   (0x00, 0x00, 0xff), 
		COLOR_PURPLE (0xff, 0x00, 0xff), 
		COLOR_YELLOW (0xff, 0xff, 0x00), 
		COLOR_TEAL   (0x00, 0xff, 0xff), 
		COLOR_WHITE  (0xff, 0xff, 0xff),
		COLOR_IMPOSSIBLE (0,0,0,1);

	
	// это говнофункторы
	class TransparentTest {
		Color tc;
	public:
		TransparentTest(const Color tc) : tc(tc) {}
		inline void operator () (Color &dst, const Color src) const {
			if (src != tc) dst=src;	
		}
	};
	
	class Fill	{
		Color tc;
	public:
		Fill(const Color tc) : tc(tc) {}
		inline void operator () (Color &dst) const {
			dst = tc;
		}
	};
	
	struct CopyPixel {
		inline void operator () (Color &dst, const Color src) const	{
			dst=src;
		}
	};

	// а теперь сам класс изображения!
	typedef tblib::Array2DWindow<Color> BaseBitmap;
		
	const static uint16_t bitmapSignature = 0x4D42;

	struct BitmapFileHeader
	{
		uint32_t    bfSize;
		uint16_t    bfReserved1;
		uint16_t    bfReserved2;
		uint32_t    bfOffBits;
	};
		
	struct BitmapInfoHeader
	{
		uint32_t      biSize;
		int32_t       biWidth;
		int32_t       biHeight;
		uint16_t      biPlanes;
		uint16_t      biBitCount;
		uint32_t      biCompression;
		uint32_t      biSizeImage;
		int32_t       biXPelsPerMeter;
		int32_t       biYPelsPerMeter;
		uint32_t      biClrUsed;
		uint32_t      biClrImportant;
	};

	class Bitmap : public BaseBitmap
	{
	public :
		bool SaveToStrBuf (tblib::StrBuf& buffer) const
		{				
			BitmapFileHeader fileHeader;
			BitmapInfoHeader infoHeader;

			fileHeader.bfSize = sizeof(fileHeader) + sizeof(infoHeader) + m_sizeX*m_sizeY*sizeof(Color);
			fileHeader.bfReserved1 = 0;
			fileHeader.bfReserved2 = 0;
			fileHeader.bfOffBits = sizeof(fileHeader) + sizeof(infoHeader);

			infoHeader.biSize = sizeof(infoHeader);
			infoHeader.biBitCount = sizeof(Color)*8;
			infoHeader.biWidth = m_sizeX;
			infoHeader.biHeight = m_sizeY;
			infoHeader.biPlanes = 1;
			infoHeader.biCompression = 0L;
			infoHeader.biSizeImage = 0;
			infoHeader.biXPelsPerMeter = 3000;
			infoHeader.biYPelsPerMeter = 3000;
			infoHeader.biClrUsed = sizeof(Color)==2 ? 65536 : 0;
			infoHeader.biClrImportant = 0;


			buffer.Append(tblib::StrBuf(tblib::StrBuf::BINARY, &bitmapSignature, 1).str());
			buffer.Append(tblib::StrBuf(tblib::StrBuf::BINARY, &fileHeader, 1).str());
			buffer.Append(tblib::StrBuf(tblib::StrBuf::BINARY, &infoHeader, 1).str());
			
			for (int j=m_sizeY-1; j>=0; --j)
				buffer.Append(tblib::StrBuf(tblib::StrBuf::BINARY, &(*this)[j][0], m_sizeX).str());

			return true;
		}

		bool SaveToFile (tblib::StringRef fileName) const
		{
			tblib::StrBuf buffer;
			SaveToStrBuf(buffer);
			tblib::OFStream f(fileName);
			if (!f.IsOpen())
				return false;
			f << buffer.GetSlice();
			return (f.Flush()==0);
		}
		  
		Bitmap () {}
		Bitmap (int sizeX, int sizeY, int stride, Color *begin)  
			: BaseBitmap(sizeX,sizeY,stride,begin) {}      
		Bitmap (const BaseBitmap& b) 
			: BaseBitmap(b) {} 

		Bitmap Window (int wposX, int wposY, int wsizeX, int wsizeY) const 
		{
			return BaseBitmap::Window<Bitmap>(wposX,wposY,wsizeX,wsizeY);
		}
	};

	typedef tblib::Array2D<Color, Bitmap> BasePicture;

	class Picture : public BasePicture
	{
	public :
		Picture () {}
		Picture (int sx, int sy) : BasePicture (sx, sy) {}
		Picture (const tblib::IFStream& s) : BasePicture (s) {}
		Picture (const Picture& p) : BasePicture (static_cast<const BasePicture&>(p)) {}

		bool LoadFromBytes (tblib::StringRef bytes)
		{			
			int i = bytes.Low();
			
			BitmapFileHeader fileHeader;
			BitmapInfoHeader infoHeader;

			uint16_t signature;
			if (bytes.High()-i < sizeof(signature))
				return false;
			memcpy(&signature, &bytes[i], sizeof(signature));
			if (signature != bitmapSignature)
				return false;
			i += sizeof(signature);

			if (bytes.High()-i < sizeof(fileHeader))
				return false;
			memcpy(&fileHeader, &bytes[i], sizeof(fileHeader));
			if (int(fileHeader.bfSize) > bytes.High()-bytes.Low())
				return false;
			i += sizeof(fileHeader);
			
			if (bytes.High()-i < sizeof(infoHeader))
				return false;
			memcpy(&infoHeader, &bytes[i], sizeof(infoHeader));

			bool b24=false;
			if (infoHeader.biBitCount==24 
			&& infoHeader.biBitCount+8 == sizeof(Color)*8) // мудацкое сравнение из-за бесполезного предупреждения
				b24=true;
			else if (infoHeader.biBitCount != sizeof(Color)*8)
				return false;

			i = fileHeader.bfOffBits;
			if (int(infoHeader.biWidth*infoHeader.biHeight*(infoHeader.biBitCount/8)) > bytes.High()-i)
				return false;

			tblib::Recreate(*this, infoHeader.biWidth, infoHeader.biHeight);
			
			for (int j=m_sizeY-1; j>=0; --j)
			{
				if (b24)
				{
					tbal::Bitmap::Line l=(*this)[j];
					int oi=i;
					for (int li=0; li<m_sizeX; ++li)
					{
						l[li] = tbal::Color(uint8_t(bytes[i+2]), uint8_t(bytes[i+1]), uint8_t(bytes[i+0]));
						i+=3;
					}
					i = oi+((i-oi+3)&(~3));
				}	else
				{
					memcpy(&(*this)[j][0], &bytes[i], m_sizeX*sizeof(Color));
					i += m_sizeX*sizeof(Color);
				}

			}

			return true;
		}

		bool LoadFromFile (tblib::StringRef fileName)
		{
			tblib::IFStream f(fileName);
			if (!f.IsOpen())
				return false;
			int size = f.Size();
			if (size==0) 
				return false;
			tblib::Vector<char> buf;
			buf.Resize(size);
			f.BinaryRead(buf[0], size);
			return LoadFromBytes(buf.GetSlice());
		}
	};
};