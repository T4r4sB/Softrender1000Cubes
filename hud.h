#pragma once

#include "tbal.h"
#include "level.h"
#include "font.h"

int fps;

void ShowHUD (const tbal::Bitmap &b)
{
	static tbal::Font f;
	f.Init(Fixed(10,1), Fixed(3,2));	
	//b.Fill(tbal::Fill(tbal::COLOR_BLACK));

	f.OutText(b, 0, 0, tblib::Format("fps=%").i(fps).str(), tbal::COLOR_RED);
	
	f.OutText(b, 0, 20, tblib::Format("cut=%").f(float(TIMER1)/float(TIMER0), 4).str(), tbal::COLOR_RED);
	f.OutText(b, 0, 40, tblib::Format("fill=%").f(float(TIMER2)/float(TIMER0), 4).str(), tbal::COLOR_RED);
	f.OutText(b, 0, 60, tblib::Format("txc=%").f(float(TIMER3)/float(TIMER0), 4).str(), tbal::COLOR_RED);
	f.OutText(b, 0, 80, tblib::Format("sec=%").f(float(TIMER4)/float(TIMER0), 4).str(), tbal::COLOR_RED);

}