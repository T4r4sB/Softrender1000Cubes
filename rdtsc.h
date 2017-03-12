#pragma once
#include "stdint.h"

/*
uint64_t rdtsc() {
	__asm {
		rdtsc
	}
}
*/
/*
struct TaktCounter 
{
	uint64_t& value;
	TaktCounter(uint64_t& value) : value(value) 
	{
		value -= rdtsc();
	}

	~TaktCounter ()
	{
		value += rdtsc();
	}
};*/

struct TaktCounter 
{
	TaktCounter(uint64_t& ) {}
	~TaktCounter () {}
};

uint64_t TIMER0, TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER6, TIMER7;