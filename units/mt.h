#pragma once

#include "time.h"

class MT {
	unsigned int state [624];
	int index;

	void Init (int seed) {
		index = 0;
		state[0] = seed;
		for (unsigned int i=1; i<624; i++) {
			state[i] =  1812433253*((state[i-1]^(state[i-1]>>30))+i);}}

	unsigned int ExtractNumber () {
		if (index==0) {
			GenerateNumbers (); }
		unsigned int y = state[index];
		y ^= (y>>11);
		y ^= ((y<<7) & 2636928640);
		y ^= ((y<<15) & 4022730752);
		y ^= (y>>18);
		index = (index+1)%624;
		return y; }

	void GenerateNumbers () {
		for (int i=0; i<624; ++i) {
			int y = (state[i]&0x80000000)+(state[(i+1)%624]&0x7fffffff);
			state [i] = state[(i+397)%624]^(y>>1);
			if (y%2!=0) {
				state [i] ^= 2567483615; }}}

public:

	MT () {
		Init (clock());}
	MT (int seed) {
		Init (seed);}
	int random (int range) {
		return int(((long long)(ExtractNumber())*(long long)(range))>>32);}
	double frandom () {
		return (double)(ExtractNumber())*(1./4294967296.);}};
