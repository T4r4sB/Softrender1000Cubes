#pragma once
#include "aftermodels.h"

void EnginePhys (float dt)
{
	level.gameTime += dt;
	ProcessTextures(dt);
}
