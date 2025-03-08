#ifndef CHOREO_H
#define CHOREO_H

#include <cglm/cglm.h>

#include "util.h"

enum Actor
{
	A_PLAYER = 0,
	
	A_END__
};

void ActorWalk(enum Actor a, vec2 Pos);
void ActorLook(enum Actor a, f32 Pitch, f32 Yaw); 
void UpdateChoreo(void);
void RenderChoreo(void);

#endif
