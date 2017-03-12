#pragma once
#include "tbarr2.h"
#include "windows.h"
#include "palette.h"

typedef tblib::Array2DWindow<Pixel> FrameWindow;

class FrameBuffer : public FrameWindow 
{
	HDC bufDC;

	FrameBuffer (const FrameBuffer&);
	const FrameBuffer& operator= (const FrameBuffer&);
public :

	FrameBuffer () {
		
		struct DCBuffer 
		{ 
			FrameWindow buffer;
			HBITMAP handle;
			HDC dc;

			DCBuffer() {

				HWND hwnd = tbal::GetWindowHandle();
				RECT rect;
				GetClientRect( hwnd, &rect);
				int sizeX = rect.right - rect.left;
				int sizeY = rect.bottom - rect.top;

				// стандартная тупая байда по заполнению полей винапишной структуры
				int pixelsPerInt = sizeof(int)/sizeof(Pixel);

				// отрицательное смещение строки, так как в винде видеопамять изображения перевёрнута
				int stride = ((sizeX+pixelsPerInt-1)&(~(pixelsPerInt-1)));


				struct MyBitmapInfo {
					BITMAPINFOHEADER    bmiHeader;
					RGBQUAD             bmiColors[256];
				} bi;

				memset(&bi, 0, sizeof(bi));
				bi.bmiHeader.biSize     = sizeof(bi.bmiHeader);
				bi.bmiHeader.biWidth    = sizeX;
				bi.bmiHeader.biHeight   = -sizeY;
				bi.bmiHeader.biPlanes   = 1;
				bi.bmiHeader.biBitCount = WORD(sizeof(Pixel)*8);
				for (int i=0; i<256; ++i) {
					bi.bmiColors[i].rgbRed   = BYTE(palette[i].R());
					bi.bmiColors[i].rgbGreen = BYTE(palette[i].G());
					bi.bmiColors[i].rgbBlue  = BYTE(palette[i].B());
					bi.bmiColors[i].rgbReserved = 0;
				}
				HDC screenDC = GetDC(NULL);
				// создаём описатели
				dc     = CreateCompatibleDC(screenDC);
				Pixel* pixels;
				handle = CreateDIBSection(dc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), 0, 0);


				SelectObject(dc, handle);
				// начало пикселей ставим на верхнюю строку		
				tblib::Recreate(buffer, sizeX, sizeY, stride, pixels);

				ReleaseDC(NULL, screenDC);
			}
		};

		static DCBuffer dcBuffer = DCBuffer();
		static_cast<FrameWindow&>(*this) = dcBuffer.buffer;
		bufDC = dcBuffer.dc;

	}

	~FrameBuffer () {
		HWND hwnd = tbal::GetWindowHandle();		
		HDC dc = GetDC(hwnd);
		BitBlt(dc, 0, 0, SizeX(), SizeY(), bufDC, 0, 0, SRCCOPY);
		ReleaseDC(hwnd, dc);
	}
};