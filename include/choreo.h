#ifndef CHOREO_H
#define CHOREO_H

#include <cglm/cglm.h>

#include "render.h"
#include "util.h"

enum Actor
{
	A_PLAYER = 0,
	A_DUMMY,
	
	A_END__
};

void ActorWalk(enum Actor a, vec2 Pos);
void ActorWalkToPoint(enum Actor a, char Point);
void ActorLook(enum Actor a, f32 Pitch, f32 Yaw); 
void ActorLookDeg(enum Actor a, f32 PitchDeg, f32 YawDeg);
void ActorWalkLook(enum Actor a, vec2 Pos, f32 Pitch, f32 Yaw);
void ActorWalkLookDeg(enum Actor a, vec2 Pos, f32 PitchDeg, f32 YawDeg);
void ActorWalkToPointLook(enum Actor a, char Point, f32 Pitch, f32 Yaw);
void ActorWalkToPointLookDeg(enum Actor a, char Point, f32 PitchDeg, f32 YawDeg);
void ActorSetTexture(enum Actor a, enum Texture t);
void UpdateChoreo(void);
void RenderChoreo(void);

#endif
