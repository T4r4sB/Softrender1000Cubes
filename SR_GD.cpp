#include "textures.h"
#include "engine.h"
#include "render.h"

int lastNumber = -1;

PositionInfo positionInfo;
bool rotating = true;
bool useTransparent = false;


void InitPosition ()
{
	positionInfo.p = Point3D((9.5f + 38.0f/55.0f) * 0.5f, 0.0f, 0.0f);
	positionInfo.dp = Point3D(0.0f, 0.0f, 0.0f);	
	positionInfo.anglex = +float(M_PI) * 0.5f - 0.001f;
	positionInfo.anglez = -float(M_PI) * 0.5f;
  positionInfo.angley = 0.0f;
}

void Draw (const FrameWindow& b)
{
	float sc = 1.5f * 55.0f/38.0f;
/*
	positionInfo.p = Point3D(5.095454692841f, 0.000000000000f, 0.000000000000f);	
	positionInfo.anglex=1.843500137329f; 
	positionInfo.anglez=0.076500006020f;*/



	ShowLevel(b, float(b.SizeX())/float(b.SizeY()) * sc, sc, positionInfo, useTransparent);
}

void Phys (const FrameWindow& b, float dt)
{
	POINT p;
	HWND h = tbal::GetWindowHandle();
	GetCursorPos(&p);
	ScreenToClient(h,&p);

	float 
		cz = cos(positionInfo.anglez),
		sz = sin(positionInfo.anglez),
		cx = cos(positionInfo.anglex),
		sx = sin(positionInfo.anglex);

	
	if (tbal::Pressed(tbal::CODE_W)) 
		positionInfo.dp += Point3D(sz * sx, -cz * sx, cx) * dt * 5.0;
	if (tbal::Pressed(tbal::CODE_D)) 
		positionInfo.dp += Point3D(cz, sz, 0.0f) * dt * 5.0;
	if (tbal::Pressed(tbal::CODE_S)) 
		positionInfo.dp += Point3D(-sz * sx, cz * sx, -cx) * dt * 5.0;
	if (tbal::Pressed(tbal::CODE_A)) 
		positionInfo.dp += Point3D(-cz, -sz, 0.0f) * dt * 5.0;
	if (tbal::Pressed(tbal::CODE_SPACE)) 
		positionInfo.dp += Point3D(0.0f, 0.0f, -1.0f) * dt * 5.0;
	if (tbal::Pressed(tbal::CODE_CTRL)) 
		positionInfo.dp += Point3D(0.0f, 0.0f, 1.0f) * dt * 5.0;

  if (tbal::Pressed(tbal::CODE_Q)) 
    positionInfo.angley += dt;
  if (tbal::Pressed(tbal::CODE_E)) 
    positionInfo.angley -= dt;

	if (rotating)
	{
		positionInfo.anglex -= float(p.y - b.SizeY()/2)*0.1f*dt;
		positionInfo.anglez +=  float(p.x - b.SizeX()/2)*0.1f*dt;

		p.x = b.SizeX()/2;
		p.y = b.SizeY()/2;
		ClientToScreen(h,&p);
		SetCursorPos(p.x, p.y);
	}

	positionInfo.p += positionInfo.dp * dt;
	positionInfo.dp *= (1.0f - dt);
  

#ifndef PURE_RENDER
  static int skyp=0;
  if (((skyp++)&3)==0)	EnginePhys(dt);
#endif
}

bool TbalMain (tbal::Action action, tbal::Code code, int x, int y)
{
	(void)y;
	switch (code)
	{
	case tbal::CODE_GETNAME : 
		tbal::SetProjectName("Lab3D");
		break;
	case tbal::CODE_START : 		
		{
/*
			// sbuffer testing
			tbal::Picture p(100, 100);
			tblib::HeapCArray<uint16_t> hc(100);
			p.Fill(tbal::Fill(tbal::COLOR_BLACK));
			SBuffer sb(100, 1);

			for (int i=0; i<3333; ++i)
			{				
				tblib::Array2D<int16_t>::Line line = sb.data[0];	
				for (int segf = line[0], segl = line[segf]; segf < p.SizeX(); segf = line[segl], segl = line[segf])
					p.Window(segf-1, i*3, segl-segf, 1).Fill(tbal::Fill(tbal::COLOR_GREEN));

				if (i==50) break;

				int nx1 = pasrnd.random(100);
				int nx2 = pasrnd.random(20);

				nx1 = nx1 - nx2/2;
				nx2 = nx1 + nx2;
				
				tblib::Enlarge(nx1, 0);
				tblib::Shrink(nx2, 100);

				if (nx1<nx2) {
					p.Window(nx1, i*3+1, nx2-nx1, 1).Fill(tbal::Fill(tbal::COLOR_BLUE));
					int l = sb.CutLine(nx1, nx2, 0, hc.GetSlice());
					for (int j=0; j<l; ++j)
						p.Window(hc[j*2], i*3+2, hc[j*2+1]-hc[j*2], 1).Fill(tbal::Fill(tbal::COLOR_RED));
				}

			}
		
			p.SaveToFile("ololo.bmp");

			exit(0);*/

			InitPalette();
			InitFogTable(tbal::COLOR_BLACK);
			InitTextures();		
			level.Create();
			InitPosition();

			tbal::SetTimer(0);
		}
		break;
	case tbal::CODE_B : 
		if (action == tbal::ACTION_UP) 
			InitPosition();
		break;
  case tbal::CODE_1 : 
    if (action == tbal::ACTION_UP) 
      useTransparent = false;
    break;
  case tbal::CODE_2 : 
    if (action == tbal::ACTION_UP) 
      useTransparent = true;
    break;
	case tbal::CODE_TIMER : 
		{
			static int fpsc=0;
			static int rem=1000;

			++fpsc;
			rem -= x;
			if (rem<0)
			{
				fps = fpsc * 1000 / (1000-rem);
				rem = 1000;
				fpsc = 0;
			}
		}

		{
			tblib::StrBuf fpss = tblib::Format("fps=%").i(fps);
			SendMessage(tbal::GetWindowHandle(), WM_SETTEXT, fpss.Length(), LPARAM((const char*)(fpss)));
		}

		{	
			FrameBuffer b;
			const int delta=5;
      static int rem = 0;
			rem += x;
			tblib::Shrink(rem,100);
			while (rem>0)
			{
				rem -= delta;
				Phys(b, float(delta)*0.001f);
			}
			Draw(b);

      Pixel cl1 = NearestIndex(255,128,0), cl2 = NearestIndex(255,255,255), cl3 = NearestIndex(0,128,255);

			for (int i=0; i<fps/10; ++i) {
				b[10][i*5+10] = i<5 ? cl1 : i<10 ? cl2 : cl3;
				b[11][i*5+10] = i<5 ? cl3 : i<10 ? cl1 : cl3;
			}
			
			for (int i=0; i<(fps%10); ++i) {
				b[15][i*5+10] = i<5 ? cl1 : cl2;
				b[16][i*5+10] = i<5 ? cl3 : cl1;
			}
		}
		break;
	case tbal::CODE_ESCAPE : 
		if (action == tbal::ACTION_UP)
			return false;
	case tbal::CODE_EXIT : 
		finish = true;
		break;
	}
	return true;
}