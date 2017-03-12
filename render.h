#pragma once

#include "tbal.h"
#include "engine.h"
#include <algorithm>
#include "fillrate.h"
#include "level.h"
#include "hud.h"

const int cutcnt = 5;
#ifdef NDEBUG
const Point3D cut[cutcnt] = {
  Point3D( 0.0f, 0.0f, 1.0f),
  Point3D( 0.0f,-1.0f, 1.0f),
  Point3D( 0.0f, 1.0f, 1.0f),
  Point3D(-1.0f, 0.0f, 1.0f),
  Point3D( 1.0f, 0.0f, 1.0f)};
const float cutd[cutcnt] = {-minD, 0.0f,0.0f,0.0f,0.0f};
#else
const Point3D __cut[cutcnt] = {
  Point3D( 0.0f, 0.0f, 1.0f),
  Point3D( 0.0f,-1.0f, 1.0f),
  Point3D( 0.0f, 1.0f, 1.0f),
  Point3D(-1.0f, 0.0f, 1.0f),
  Point3D( 1.0f, 0.0f, 1.0f)};
const float __cutd[cutcnt] = {-minD, 0.0f,0.0f,0.0f,0.0f};

tblib::CArray<Point3D,cutcnt> cut(__cut);
tblib::CArray<float,cutcnt> cutd(__cutd);
#endif

struct Point2D
{
  float x,y;
};

void FastLayoutLine (const TemporaryShowSectorInfo &tsi, ScreenLine& sl,
  Bound& b1, Bound& b2, Point2D sp1, Point2D sp2)
{
  if (sp1.y > sp2.y)
  {
    tblib::Swap(sp1, sp2);
    tblib::Swap(sl.f1, sl.f2);
  }

  int ty1 = HackCeil (sp1.y,0);
  tblib::Enlarge(ty1, tsi.curZ.Low());
  int ty2 = HackCeil (sp2.y,0);
  tblib::Shrink(ty2, tsi.curZ.High());

  sl.Init(ty1,ty2,PK_TEMP);

  tblib::Shrink (b1.v1, tblib::Max(ty1, tsi.curZ.Low()));
  tblib::Enlarge(b1.v2, tblib::Min(ty2, tsi.curZ.High()));
  tblib::Shrink (b2.v1, tblib::Max(ty1, tsi.curZ.Low()));
  tblib::Enlarge(b2.v2, tblib::Min(ty2, tsi.curZ.High()));

  if (ty1<ty2)
  {
    float dxdy = (sp2.x-sp1.x)/(sp2.y-sp1.y);
    float x = sp1.x + dxdy*(float(ty1)-sp1.y);
    int cx = HackCeil(x,0);
    tblib::InBound(cx, tsi.curZ[ty1].v1, tsi.curZ[ty1].v2);
    sl[ty1] = cx;
    if (ty1+1<ty2)
    {
      int x16 = HackCeil(x,16)+0xffff;
      int dx16 = HackCeil(dxdy,16);
      for (int j=ty1+1; j<ty2; ++j)
      {
        x16 += dx16;
        cx = x16>>16;
        tblib::InBound(cx, tsi.curZ[j].v1, tsi.curZ[j].v2);
        sl[j] = cx;
      }
    }
  }
}

void LayoutLine (const ShowSectorInfo& si, const TemporaryShowSectorInfo &tsi,
  const Line& l, ScreenLine& sl,
  Bound& b1, Bound& b2 )
{
  Point3D p1 = *l.p1;
  Point3D p2 = *l.p2;

  int w = si.b.SizeX(), h = si.b.SizeY();
  int py1=h, py2=-1;
  float x1,y1,x2,y2;


  for (int i=0; i<cutcnt; ++i) // нарезка
  {
    float t1 = Dot(p1, cut[i]) + cutd[i];
    float t2 = Dot(p2, cut[i]) + cutd[i];
    if (t1<0.0f && t2<0.0f) // вне экрана
    {
      if (i==3)
        for (int j=py1; j<py2; ++j)
          sl[j] = tsi.curZ[j].v2;
      if (i==4)
        for (int j=py1; j<py2; ++j)
          sl[j] = tsi.curZ[j].v1;
      return;
    } else if (t1<0.0f)
    {
      p1 += (p2-p1)*(t1/(t1-t2));
      p1.z = (cutd[i] - p1.x*cut[i].x - p1.y*cut[i].y)/cut[i].z;
    } else if (t2<0.0f)
    {
      p2 += (p1-p2)*(t2/(t2-t1));
      p2.z = (cutd[i] - p2.x*cut[i].x - p2.y*cut[i].y)/cut[i].z;
    }
    if (i==2)
    {
      y1 = h*(p1.y+p1.z)/(p1.z+p1.z);
      y2 = h*(p2.y+p2.z)/(p2.z+p2.z);
      if (y1>y2)
      {
        std::swap(y1,y2);
        std::swap(sl.f1,sl.f2);
        std::swap(p1,p2);
      }
      py1 = HackCeil(y1,0);
      tblib::Enlarge(py1, tsi.curZ.Low());
      py2 = HackCeil(y2,0);
      tblib::Shrink (py2, tsi.curZ.High());

      if (py1<py2)
      {
        sl.Init(py1,py2,PK_TEMP);
        // раздвинем границы!
        tblib::Shrink (b1.v1, py1);
        tblib::Enlarge(b1.v2, py2);
        tblib::Shrink (b2.v1, py1);
        tblib::Enlarge(b2.v2, py2);
      }
      else
        return;
    }
  } // конец нарезки

  x1 = w*(p1.x+p1.z)/(p1.z+p1.z);
  y1 = h*(p1.y+p1.z)/(p1.z+p1.z);
  x2 = w*(p2.x+p2.z)/(p2.z+p2.z);
  y2 = h*(p2.y+p2.z)/(p2.z+p2.z);

  int ty1 = HackCeil (y1,0);
  tblib::InBound(ty1, py1, tsi.curZ.High());
  int ty2 = HackCeil (y2,0);
  tblib::InBound(ty2, tsi.curZ.Low(), py2);

  if (int(x1)< w/2) for (int j=py1; j<ty1; ++j) sl[j] = tsi.curZ[j].v1;
  else for (int j=py1; j<ty1; ++j) sl[j] = tsi.curZ[j].v2;
  if (int(x2)< w/2) for (int j=ty2; j<py2; ++j) sl[j] = tsi.curZ[j].v1;
  else for (int j=ty2; j<py2; ++j) sl[j] = tsi.curZ[j].v2;

  if (ty1<ty2)
  {
    float dxdy = (x2-x1)/(y2-y1);
    float x = x1 + dxdy*(float(ty1)-y1);
    sl[ty1] = HackCeil(x,0);
    if (ty1+1<ty2)
    {
      int x16 = HackCeil(x,16)+0xffff;
      int dx16 = HackCeil(dxdy,16);
      for (int j=ty1+1; j<ty2; ++j)
      {
        x16 += dx16;
        sl[j] = x16>>16;
      }
    }
  }
}

void ApplyLineToZones (const ScreenLine& sl, ScreenZone& z1, ScreenZone& z2)
{
  for (int i=sl.Low(); i<sl.High(); ++i)
  {
    tblib::Enlarge (z1[i].v1, sl[i]);
    tblib::Shrink (z2[i].v2, sl[i]);
  }
}

bool NoZoneIntersect (const TemporaryShowSectorInfo& tsi, MSlice<ScreenZone> &zones)
{
  for (int k = tsi.curZ.Low(); k<tsi.curZ.High(); ++k)
  {
    for (int i=zones.Low(); i<zones.High(); ++i) if (zones[i].f && k>=zones[i].Low() && k<zones[i].High())
      for (int j=i+1; j<zones.High(); ++j) if (zones[j].f && k>=zones[j].Low() && k<zones[j].High())
      {
        Bound& b1 = zones[i][k];
        Bound& b2 = zones[j][k];
        if (b1.v2<=b2.v1) // тогда одна перед другой
        {} else if (b2.v2<=b1.v1) // Тогда наоборот
        {} else
        {
          return(false);
        }
      }
  }
  return true;
}

void ShowSector(ShowSectorInfo& si, const TemporaryShowSectorInfo& tsi, bool cutCheck)
{
  for (int i=tsi.s.points.Low(); i<tsi.s.points.High(); ++i)
    tsi.s.points[i].point = tsi.s.skybox ? si.m.RotatePNoTr(*tsi.s.points[i].original) : si.m.RotateP(*tsi.s.points[i].original);

  MSlice<Point2D> screenPoints(tsi.s.points.Low(), tsi.s.points.High(), PK_TEMP);

  if (cutCheck)
  {
    int w = si.b.SizeX(), h = si.b.SizeY();

    int
      x1 = w,
      y1 = h,
      x2 = 0,
      y2 = 0;

    for (int i=tsi.s.points.Low(); i<tsi.s.points.High(); ++i)
    {
      Point3D& p = tsi.s.points[i].point;

      float x = w*(p.x+p.z)/(p.z+p.z);
      float y = h*(p.y+p.z)/(p.z+p.z);

      int nx = HackCeil(x, 0);
      int ny = HackCeil(y, 0);

      screenPoints[i].x = x;
      screenPoints[i].y = y;

      tblib::Shrink (x1, nx);
      tblib::Shrink (y1, ny);
      tblib::Enlarge(x2, nx);
      tblib::Enlarge(y2, ny);
    }

    tblib::Enlarge(x1, 0);
    tblib::Enlarge(y1, 0);
    tblib::Shrink (x2, w);
    tblib::Shrink (y2, h);

    if (x1 >= x2) return;

    bool ok = false;
    for (int i=y1; i<y2; ++i)
    {
      if (si.filled.TestCutLine(x1, x2, i))
      {
        ok = true;
        break;
      }
    }

    if (!ok) return;
  }


  MSlice<Bound> bounds(tsi.s.faces.Low(), tsi.s.faces.High(), PK_TEMP);
  MSlice<bool> zvalid(tsi.s.faces.Low(), tsi.s.faces.High(), PK_TEMP);
  MSlice<ScreenLine> lines(tsi.s.lines.Low(), tsi.s.lines.High(), PK_TEMP);

  for (int i=bounds.Low(); i<bounds.High(); ++i)
  {
    bounds[i].v1 = si.b.SizeY();
    bounds[i].v2 = -1;
  }

  for (int i=zvalid.Low(); i<zvalid.High(); ++i)
    zvalid[i] = tsi.s.faces[i].Dist(tsi.nTest)<0.0f;

  for (int i=lines.Low(); i<lines.High(); ++i)
  {
    int id1 = tsi.s.lines[i].f1->id;
    int id2 = tsi.s.lines[i].f2->id;

    lines[i].valid = zvalid[id1] || zvalid[id2];

    if (lines[i].valid)
    {
      lines[i].f1 = tsi.s.lines[i].f1;
      lines[i].f2 = tsi.s.lines[i].f2;

      LayoutLine (si, tsi, tsi.s.lines[i], lines[i], bounds[id1], bounds[id2]);
    }
  }
  MSlice<ScreenZone> zones(tsi.s.faces.Low(), tsi.s.faces.High(), PK_TEMP);

  /*
  for (int i=lines.Low(); i<lines.High(); ++i) for (int j=lines[i].Low(); j<lines[i].High(); ++j)
  {
  int x = lines[i][j];
  tblib::InBound(x, 0, si.b.SizeX()-1);
  //if (x>=0 && x<si.b.SizeX())
  si.b[j][x] = 248;
  }

  return;*/

  for (int i=zones.Low(); i<zones.High(); ++i)
  {
    if (bounds[i].v1<bounds[i].v2)
    {
      zones[i].Init(&tsi.s.faces[i], bounds[i].v1, bounds[i].v2);
      for (int j=zones[i].Low(); j<zones[i].High(); ++j)
        zones[i][j] = tsi.curZ[j];
    } else
      zones[i].f = NULL;
  }

  for (int i=lines.Low(); i<lines.High(); ++i)
    if (lines[i].valid)
      ApplyLineToZones(lines[i], zones[lines[i].f1->id], zones[lines[i].f2->id]);


  //if (!NoZoneIntersect(tsi, zones)) {}


  for (int i=zones.Low(); i<zones.High(); ++i)
  {
    if (zvalid[i] && zones[i].f)
      Fill(si, tsi, zones[i]);
  }


}

struct PositionInfo {
  Point3D p, dp;
  float anglex, anglez, angley;
};

void ShowLevel (const FrameWindow &b, float vx, float vy, PositionInfo& pi, bool useTransparent)
{
  {
    TaktCounter tc(TIMER0);

    // а теперь долго-долго инициализируем одну сраную структуру
    ShowSectorInfo si;
    si.b = b.Window<FrameWindow>(0,0,b.SizeX(), b.SizeY());
    si.filled.Init(si.b.SizeX(), si.b.SizeY());
    si.cut.Init(0, b.SizeX()*2, PK_TEMP);
    si.mainZ.Init(NULL,0,si.b.SizeY());
    si.faces.Init(0, maxSectors*14, PK_TEMP);
    si.nearSectors.Init(0, 1, PK_TEMP);
    si.useTransparent = useTransparent;

    //assert(pi.sp.Correct());
    const Point3D viewP = pi.p;

    //const float ax = 0.0f, az=0.0f;
    //const float ax = float(M_PI_2), az=float(M_PI_2);
    const float ax=pi.anglex, az=pi.anglez, ay=pi.angley;
    //const float ax = float(M_PI_2), az=float(M_PI);

    si.m.Scale(1.0f/vx, 1.0f/vy, 1.0f);
    si.m.Rotate(ay, 2);
    si.m.Rotate(ax, 0);
    si.m.Rotate(az, 2);
    si.m.Translate(viewP*-1.0f);
    si.viewP = viewP;

    Matrix3D revm;

    revm.Rotate(-az, 2);
    revm.Rotate(-ax, 0);
    revm.Rotate(-ay, 2);
    revm.Scale(vx, vy, 1.0f);

    si.plt = revm.RotateP(Point3D(-1.0f,-1.0f, 1.0f));
    si.dx = revm.RotateP(Point3D( 2.0f, 0.0f, 0.0f));
    si.dy = revm.RotateP(Point3D( 0.0f, 2.0f, 0.0f));


    Point3D
      c1 = revm.RotateP(Point3D(-1.0f,-1.0f, 1.0f)),
      c2 = revm.RotateP(Point3D(+1.0f,-1.0f, 1.0f)),
      c3 = revm.RotateP(Point3D(+1.0f,+1.0f, 1.0f)),
      c4 = revm.RotateP(Point3D(-1.0f,+1.0f, 1.0f)),
      cc = revm.RotateP(Point3D( 0.0f, 0.0f, 1.0f)).Normalized();

    Point3D
      cp1 = Cross(c1, c2).Normalized(),
      cp2 = Cross(c2, c3).Normalized(),
      cp3 = Cross(c3, c4).Normalized(),
      cp4 = Cross(c4, c1).Normalized();

#ifndef NDEBUG
    Point3D test_plt = si.m.RotatePNoTr(si.plt);
    Point3D test_dx = si.m.RotatePNoTr(si.dx);
    Point3D test_dy = si.m.RotatePNoTr(si.dy);
#endif

    for (int i=0; i<si.b.SizeY(); ++i)
    {
      si.mainZ[i].v1 = 0;
      si.mainZ[i].v2 = si.b.SizeX();
    }

    
    static Pixel night = NearestIndex(0x00, 0x20, 0x40);
    FrameWindow::Line line = b[0];
    for (int j=0; j<b.SizeY(); ++j)
    {
      memset(&line[0], night, b.SizeX());
      b.ProcessLine(line);
    }

    int ci = int((viewP.x * 1.0f + 4.75f));
    int cj = int((viewP.y * 1.0f + 4.75f));
    int ck = int((viewP.z * 1.0f + 4.75f));

    tblib::InBound(ci, 0, 9);
    tblib::InBound(cj, 0, 9);
    tblib::InBound(ck, 0, 9);

    tblib::Array<int, 10> indi, indj, indk;

    for (int k=ck; k<10; ++k)  indk.EmplaceBack(k);
    for (int k=ck-1; k>=0; --k)  indk.EmplaceBack(k);
    for (int j=cj; j<10; ++j)  indj.EmplaceBack(j);
    for (int j=cj-1; j>=0; --j)  indj.EmplaceBack(j);
    for (int i=ci; i<10; ++i)  indi.EmplaceBack(i);
    for (int i=ci-1; i>=0; --i)  indi.EmplaceBack(i);

    if (si.useTransparent) {
      for (int k1=0, k2=indk.Size()-1; k1<k2; ++k1, --k2) tblib::Swap(indk[k1], indk[k2]);
      for (int j1=0, j2=indj.Size()-1; j1<j2; ++j1, --j2) tblib::Swap(indj[j1], indj[j2]);
      for (int i1=0, i2=indi.Size()-1; i1<i2; ++i1, --i2) tblib::Swap(indi[i1], indi[i2]);
    }

    for (int k=0; k<10; ++k)
      for (int j=0; j<10; ++j)
        for (int i=0; i<10; ++i)
        {
          Sector* viewS = &level.cubes[indk[k]][indj[j]][indi[i]];
          Point3D c = viewS->Center() - viewP;
          const float mind = -0.44f;
          if (Dot(c, cp1) > mind && Dot(c, cp2) > mind && Dot(c, cp3) > mind && Dot(c, cp4) > mind)
            ShowSector(si, TemporaryShowSectorInfo (*viewS, Dot(c, cc) > 3.0f, si.mainZ, viewP), Dot(c, cc) > 0.5f);
        }
    /*
    for (int k=4; k<5; ++k)
    for (int j=4; j<5; ++j)
    for (int i=4; i<5; ++i)
    {
    Sector* viewS = &l.cubes[k][j][i];
    ShowSector(si, TemporaryShowSectorInfo (*viewS, false, si.mainZ, viewP), true);
    }*/

#ifdef PURE_RENDER   
    /*
    static Pixel night = NearestIndex(0x00, 0x20, 0x40);
    FrameWindow::Line line = b[0];
    for (int j=0; j<b.SizeY(); ++j)
    {
      int cnt = si.filled.CutLine(0, b.SizeX(), j, si.cut);
      for (int i=0; i<cnt; ++i)
        for (int k=si.cut[i*2]; k<si.cut[i*2+1]; ++k)
          line[k] = night;
      b.ProcessLine(line);
    }*/
#else
    ShowSector(si, TemporaryShowSectorInfo (level.skyS, false, si.mainZ, Point3D(0.0f,0.0f,0.0f)), false);
#endif

    /*
    for (int j=0; j<b.SizeY(); ++j)
    {
    if (b[j][b.SizeX()-1] != night)
    {
    tblib::OFStream f("fuck.txt");
    f << tblib::Format("coords=(%, %, %), anglex=%; anglez=%;\r\n")
    .f(pi.p.x, 12)
    .f(pi.p.y, 12)
    .f(pi.p.z, 12)
    .f(pi.anglex, 12)
    .f(pi.anglez, 12);
    }
    }*/
  }

  //ShowHUD(b.Window(0, 0, b.SizeX(), b.SizeY()/4));
}