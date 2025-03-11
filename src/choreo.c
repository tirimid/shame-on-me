#include "choreo.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ACTIONS 256
#define WALK_SPEED 0.08f
#define WALK_DST_THRESHOLD2 0.015f
#define VERT_BOB_INTENSITY 0.2f
#define HORIZ_BOB_INTENSITY 0.1f
#define BOB_FREQUENCY 0.1f
#define LOOK_SPEED 0.25f
#define LOOK_THRESHOLD 0.005f

enum ActionType
{
	AT_WALK = 0,
	AT_WALK_TO,
	AT_LOOK,
	AT_LOOK_AT,
	AT_LOOK_WALK_TO,
	AT_SET_TEXTURE,
	AT_WAIT
};

struct Action
{
	union
	{
		vec2 Dst;
		char Point;
		u8 Tex;
		i64 Ms;
	} Data;
	u8 Type, Actor;
};

struct ActorData
{
	vec2 Pos;
	f32 Pitch, Yaw;
	u8 ActiveTex;
	f32 BobTime;
};

struct Map g_Map;

static void GetPointPos(char Point, vec2 Out);

static struct ActorData ActorData[A_END__] =
{
	// player.
	{
		// dummy value, player is not drawn.
		.ActiveTex = 0,
	},
	
	// dummy.
	{
		.ActiveTex = T_DUMMY0
	}
};

static struct Action Actions[MAX_ACTIONS];
static usize ActionCnt;

void
C_Walk(enum Actor a, vec2 Pos)
{
	if (ActionCnt < MAX_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Dst = {Pos[0], Pos[1]},
			.Type = AT_WALK,
			.Actor = a
		};
	}
}

void
C_WalkTo(enum Actor a, char Point)
{
	if (ActionCnt < MAX_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Point = Point,
			.Type = AT_WALK_TO,
			.Actor = a
		};
	}
}

void
C_Look(enum Actor a, f32 PitchDeg, f32 YawDeg)
{
	if (ActionCnt < MAX_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Dst = {glm_rad(PitchDeg), glm_rad(YawDeg)},
			.Type = AT_LOOK,
			.Actor = a
		};
	}
}

void
C_LookAt(enum Actor a, char Point)
{
	if (ActionCnt < MAX_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Point = Point,
			.Type = AT_LOOK_AT,
			.Actor = a
		};
	}
}

void
C_LookWalkTo(enum Actor a, char Point)
{
	if (ActionCnt < MAX_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Point = Point,
			.Type = AT_LOOK_WALK_TO,
			.Actor = a
		};
	}
}

void
C_SetTexture(enum Actor a, enum Texture t)
{
	if (ActionCnt < MAX_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Tex = t,
			.Type = AT_SET_TEXTURE,
			.Actor = a
		};
	}
}

void
C_Wait(u32 Ms)
{
	if (ActionCnt < MAX_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Ms = Ms,
			.Type = AT_WAIT
		};
	}
}

void
C_Update(void)
{
	// update render camera.
	f32 VBob = fabs(sin(ActorData[A_PLAYER].BobTime));
	f32 HBob = fabs(cos(ActorData[A_PLAYER].BobTime));
	
	g_Camera.Pos[0] = ActorData[A_PLAYER].Pos[0] + HORIZ_BOB_INTENSITY * HBob;
	g_Camera.Pos[1] = VERT_BOB_INTENSITY * VBob;
	g_Camera.Pos[2] = ActorData[A_PLAYER].Pos[1] + HORIZ_BOB_INTENSITY * HBob;
	g_Camera.Pitch = ActorData[A_PLAYER].Pitch;
	g_Camera.Yaw = ActorData[A_PLAYER].Yaw;
	
	// update choreography actions.
	if (ActionCnt == 0)
		return;
	
	struct Action *Action = &Actions[0];
	struct ActorData *Actor = &ActorData[Action->Actor];
	
	switch (Action->Type)
	{
	case AT_WALK:
	{
		if (glm_vec2_distance2(Actor->Pos, Action->Data.Dst) < WALK_DST_THRESHOLD2)
		{
			glm_vec2_copy(Action->Data.Dst, Actor->Pos);
			break;
		}
		
		vec2 Move;
		glm_vec2_sub(Action->Data.Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += BOB_FREQUENCY;
		
		return;
	}
	case AT_WALK_TO:
	{
		vec2 Dst;
		GetPointPos(Action->Data.Point, Dst);
		
		if (glm_vec2_distance2(Actor->Pos, Dst) < WALK_DST_THRESHOLD2)
		{
			glm_vec2_copy(Dst, Actor->Pos);
			break;
		}
		
		vec2 Move;
		glm_vec2_sub(Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += BOB_FREQUENCY;
		
		return;
	}
	case AT_LOOK:
	{
		if (AngleDiff(Action->Data.Dst[0], Actor->Pitch) < LOOK_THRESHOLD
			&& AngleDiff(Action->Data.Dst[1], Actor->Yaw) < LOOK_THRESHOLD)
		{
			Actor->Pitch = Action->Data.Dst[0];
			Actor->Yaw = Action->Data.Dst[1];
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, Action->Data.Dst[0], LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, Action->Data.Dst[1], LOOK_SPEED);
		
		return;
	}
	case AT_LOOK_AT:
	{
		vec2 Dir;
		GetPointPos(Action->Data.Point, Dir);
		glm_vec2_sub(Dir, Actor->Pos, Dir);
		
		f32 DstPitch = 0.0f;
		f32 DstYaw = Atan2(Dir[1], Dir[0]);
		
		if (AngleDiff(DstPitch, Actor->Pitch) < LOOK_THRESHOLD
			&& AngleDiff(DstYaw, Actor->Yaw) < LOOK_THRESHOLD)
		{
			Actor->Pitch = DstPitch;
			Actor->Yaw = DstYaw;
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, LOOK_SPEED);
		
		return;
	}
	case AT_LOOK_WALK_TO:
	{
		vec2 Dst, Dir;
		GetPointPos(Action->Data.Point, Dst);
		glm_vec2_sub(Dst, Actor->Pos, Dir);
		
		f32 DstPitch = 0.0f;
		f32 DstYaw = Atan2(Dir[1], Dir[0]);
		
		if (AngleDiff(DstPitch, Actor->Pitch) < LOOK_THRESHOLD
			&& AngleDiff(DstYaw, Actor->Yaw) < LOOK_THRESHOLD
			&& glm_vec2_distance2(Actor->Pos, Dst) < WALK_DST_THRESHOLD2)
		{
			glm_vec2_copy(Dst, Actor->Pos);
			Actor->Pitch = DstPitch;
			Actor->Yaw = DstYaw;
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, LOOK_SPEED);
		
		vec2 Move;
		glm_vec2_sub(Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += BOB_FREQUENCY;
		
		return;
	}
	case AT_SET_TEXTURE:
		Actor->ActiveTex = Action->Data.Tex;
		break;
	case AT_WAIT:
		if (Action->Data.Ms <= 0)
			break;
		
		Action->Data.Ms -= TICK_MS;
		
		return;
	}
	
	// dequeue completed action.
	memmove(&Actions[0], &Actions[1], sizeof(struct Action) * --ActionCnt);
}

void
C_Render(void)
{
	// draw floor, ceiling, inner walls.
	for (i32 x = 0; x < g_Map.w; ++x)
	{
		for (i32 y = 0; y < g_Map.h; ++y)
		{
			if (g_Map.Data[g_Map.w * y + x] == '#')
			{
				R_Model(
					M_PLANE,
					T_WALL,
					(vec3){x + 0.5f, -0.5f, y},
					(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				R_Model(
					M_PLANE,
					T_WALL,
					(vec3){x - 0.5f, -0.5f, y},
					(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				R_Model(
					M_PLANE,
					T_WALL,
					(vec3){x, -0.5f, y + 0.5f},
					(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
				R_Model(
					M_PLANE,
					T_WALL,
					(vec3){x, -0.5f, y - 0.5f},
					(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
			}
			else
			{
				R_Model(
					M_PLANE,
					T_FLOOR,
					(vec3){x, -1.5f, y},
					(vec3){0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
				R_Model(
					M_PLANE,
					T_CEILING,
					(vec3){x, 0.5f, y},
					(vec3){GLM_PI, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
			}
		}
	}
	
	// draw map walls.
	for (i32 x = 0; x < g_Map.w; ++x)
	{
		R_Model(
			M_PLANE,
			T_WALL,
			(vec3){x, -0.5f, -0.5f},
			(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
		R_Model(
			M_PLANE,
			T_WALL,
			(vec3){x, -0.5f, g_Map.h - 0.5f},
			(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
	}
	
	for (i32 y = 0; y < g_Map.h; ++y)
	{
		R_Model(
			M_PLANE,
			T_WALL,
			(vec3){-0.5f, -0.5f, y},
			(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
		R_Model(
			M_PLANE,
			T_WALL,
			(vec3){g_Map.w - 0.5f, -0.5f, y},
			(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
	}
	
	// draw all non-player characters.
	for (usize i = A_PLAYER + 1; i < A_END__; ++i)
	{
		struct ActorData *Player = &ActorData[A_PLAYER];
		struct ActorData *a = &ActorData[i];
		
		vec2 Dir;
		glm_vec2_sub(Player->Pos, a->Pos, Dir);
		glm_vec2_normalize(Dir);
		f32 Yaw = Atan2(Dir[0], -Dir[1]);
		
		f32 Bob = VERT_BOB_INTENSITY * fabs(sin(a->BobTime));
		
		R_Model(
			M_PLANE,
			a->ActiveTex,
			(vec3){a->Pos[0], Bob - 0.65f, a->Pos[1]},
			(vec3){-GLM_PI / 2.0f, 0.0f, 2.0f * GLM_PI - Yaw},
			(vec3){0.5f, 1.0f, 0.875f}
		);
	}
}

static void
GetPointPos(char Point, vec2 Out)
{
	for (i32 x = 0; x < g_Map.w; ++x)
	{
		for (i32 y = 0; y < g_Map.h; ++y)
		{
			if (g_Map.Data[y * g_Map.w + x] == Point)
			{
				Out[0] = x;
				Out[1] = y;
				return;
			}
		}
	}
}
