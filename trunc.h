#pragma once

int HackTrunc(float f, int order)
{
  
	union
  {
    float f;
    int32_t i;
  } u; // cant work with strict aliasing))))
	u.f = f;
	int e = ( u.i >> 23 ) & 0xFF;
	int m =
		e ?
			( u.i & 0x7FFFFF ) | 0x800000 :
			( u.i & 0x7FFFFF ) << 1;

	e += order-150;

	if (abs(e)>=32) return 0;
  if (e>0) m<<=e; else m>>=(-e);
  if (u.i>=0) return m; else return -m;
}

int HackCeil(float f, int order)
{
  /*
	union
  {
    float f;
    int32_t i;
  } u; // cant work with strict aliasing))))
	u.f = f;
	int e = ( u.i >> 23 ) & 0xFF;
	int m =
		e ?
			( u.i & 0x7FFFFF ) | 0x800000 :
			( u.i & 0x7FFFFF ) << 1;

	e += order-150;

	if (abs(e)>=32) return (u.i>=0 ? (m>0 ? 1 : 0) : (m>0 ? -1 : 0)); 
	else if (e>0) return (u.i>=0) ? (m<<e) : -(m<<e);
	else return (u.i>=0) ? 
		+((m+((1<<(-e))-1))>>(-e)) : 
		-((m+((1<<(-e))-1))>>(-e));*/
  return int(ceil(f*float(1<<order)));
}