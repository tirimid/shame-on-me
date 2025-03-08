#include "choreo.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

#define ACTION_BUF_CAP 32
#define WALK_SPEED 0.08f
#define WALK_DST_THRESHOLD2 0.0025f
#define BOB_INTENSITY 0.2f
#define BOB_FREQUENCY 1.1f
#define LOOK_SPEED 0.25f
#define LOOK_THRESHOLD 0.01f
#define MAP_SIZE_X 25
#define MAP_SIZE_Y 20

enum ActionType
{
	AT_WALK = 0,
	AT_LOOK,
	AT_WALK_LOOK
};

struct Action
{
	// type-dependent data.
	vec2 Dst0, Dst1;
	u8 Type;
};

struct ActorData
{
	vec2 Pos;
	f32 Pitch, Yaw;
	struct Action ActionBuf[ACTION_BUF_CAP];
	u8 ActionCnt;
	u8 ActiveTex;
	f32 BobTime;
};

static void DequeueAction(enum Actor a);
static f32 InterpolateAngle(f32 a, f32 b, f32 t);
static void GetPointPos(char Point, vec2 Out);

static char Map[MAP_SIZE_X * MAP_SIZE_Y] =
	"..##...####...........###"
	"..##...####...........###"
	"..##...####..a.....b..###"
	"..##...####...........###"
	"..##...####...........###"
	"..##...####...........###"
	"..##...####...........###"
	"..##...####...........###"
	"A.##...####...........###"
	".B..C..####.....I.....###"
	"..##.D.####...........###"
	"..##...####...........###"
	"..##...####...........###"
	"####...#########.########"
	"####...#########.########"
	".....E..........H........"
	"......F........G........."
	"........................."
	"#########................"
	"#########................";

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

void
ActorWalk(enum Actor a, vec2 Pos)
{
	u8 *ActionCnt = &ActorData[a].ActionCnt;
	if (*ActionCnt < ACTION_BUF_CAP)
	{
		ActorData[a].ActionBuf[*ActionCnt] = (struct Action)
		{
			.Type = AT_WALK,
			.Dst0 = {Pos[0], Pos[1]}
		};
		++*ActionCnt;
	}
}

void
ActorWalkToPoint(enum Actor a, char Point)
{
	vec2 Pos;
	GetPointPos(Point, Pos);
	ActorWalk(a, Pos);
}

void
ActorLook(enum Actor a, f32 Pitch, f32 Yaw)
{
	u8 *ActionCnt = &ActorData[a].ActionCnt;
	if (*ActionCnt < ACTION_BUF_CAP)
	{
		ActorData[a].ActionBuf[*ActionCnt] = (struct Action)
		{
			.Type = AT_LOOK,
			.Dst0 = {Pitch, Yaw}
		};
		++*ActionCnt;
	}
}

void
ActorLookDeg(enum Actor a, f32 PitchDeg, f32 YawDeg)
{
	ActorLook(a, glm_rad(PitchDeg), glm_rad(YawDeg));
}

void
ActorWalkLook(enum Actor a, vec2 Pos, f32 Pitch, f32 Yaw)
{
	u8 *ActionCnt = &ActorData[a].ActionCnt;
	if (*ActionCnt < ACTION_BUF_CAP)
	{
		ActorData[a].ActionBuf[*ActionCnt] = (struct Action)
		{
			.Type = AT_WALK_LOOK,
			.Dst0 = {Pos[0], Pos[1]},
			.Dst1 = {Pitch, Yaw}
		};
		++*ActionCnt;
	}
}

void
ActorWalkLookDeg(enum Actor a, vec2 Pos, f32 PitchDeg, f32 YawDeg)
{
	ActorWalkLook(a, Pos, glm_rad(PitchDeg), glm_rad(YawDeg));
}

void
ActorWalkToPointLook(enum Actor a, char Point, f32 Pitch, f32 Yaw)
{
	vec2 Pos;
	GetPointPos(Point, Pos);
	ActorWalkLook(a, Pos, Pitch, Yaw);
}

void
ActorWalkToPointLookDeg(enum Actor a, char Point, f32 PitchDeg, f32 YawDeg)
{
	vec2 Pos;
	GetPointPos(Point, Pos);
	ActorWalkLookDeg(a, Pos, PitchDeg, YawDeg);
}

void
ActorSetTexture(enum Actor a, enum Texture t)
{
	ActorData[a].ActiveTex = t;
}

void
UpdateChoreo(void)
{
	// update actor actions.
	for (usize i = 0; i < A_END__; ++i)
	{
		struct ActorData *Ad = &ActorData[i];
		if (!Ad->ActionCnt)
			continue;
		
		struct Action *a = &Ad->ActionBuf[0];
		switch (a->Type)
		{
		case AT_WALK:
		{
			if (glm_vec2_distance2(Ad->Pos, a->Dst0) < WALK_DST_THRESHOLD2)
			{
				DequeueAction(i--);
				continue;
			}
			
			vec2 Move;
			glm_vec2_sub(a->Dst0, Ad->Pos, Move);
			glm_vec2_normalize(Move);
			glm_vec2_scale(Move, WALK_SPEED, Move);
			
			glm_vec2_add(Ad->Pos, Move, Ad->Pos);
			Ad->BobTime += BOB_FREQUENCY * glm_vec2_norm(Move);
			
			break;
		}
		case AT_LOOK:
			if (fabs(a->Dst0[0] - Ad->Pitch) < LOOK_THRESHOLD
				&& fabs(a->Dst0[1] - Ad->Yaw) < LOOK_THRESHOLD)
			{
				DequeueAction(i--);
				continue;
			}
			
			Ad->Pitch = InterpolateAngle(Ad->Pitch, a->Dst0[0], LOOK_SPEED);
			Ad->Yaw = InterpolateAngle(Ad->Yaw, a->Dst0[1], LOOK_SPEED);
			
			break;
		case AT_WALK_LOOK:
		{
			bool WalkDone = glm_vec2_distance2(Ad->Pos, a->Dst0) < WALK_DST_THRESHOLD2;
			bool LookDone = fabs(a->Dst1[0] - Ad->Pitch) < LOOK_THRESHOLD
				&& fabs(a->Dst1[1] - Ad->Yaw) < LOOK_THRESHOLD;
			
			if (WalkDone && LookDone)
			{
				DequeueAction(i--);
				continue;
			}
			
			if (!WalkDone)
			{
				vec2 Move;
				glm_vec2_sub(a->Dst0, Ad->Pos, Move);
				glm_vec2_normalize(Move);
				glm_vec2_scale(Move, WALK_SPEED, Move);
				
				glm_vec2_add(Ad->Pos, Move, Ad->Pos);
				Ad->BobTime += BOB_FREQUENCY * glm_vec2_norm(Move);
			}
			
			if (!LookDone)
			{
				Ad->Pitch = InterpolateAngle(Ad->Pitch, a->Dst1[0], LOOK_SPEED);
				Ad->Yaw = InterpolateAngle(Ad->Yaw, a->Dst1[1], LOOK_SPEED);
			}
			
			break;
		}
		}
	}
	
	// update render camera.
	g_Camera.Pos[0] = ActorData[A_PLAYER].Pos[0];
	g_Camera.Pos[1] = BOB_INTENSITY * fabs(sin(ActorData[A_PLAYER].BobTime));
	g_Camera.Pos[2] = ActorData[A_PLAYER].Pos[1];
	g_Camera.Pitch = ActorData[A_PLAYER].Pitch;
	g_Camera.Yaw = ActorData[A_PLAYER].Yaw;
}

void
RenderChoreo(void)
{
	// draw floor, ceiling, inner walls.
	for (i32 x = 0; x < MAP_SIZE_X; ++x)
	{
		for (i32 y = 0; y < MAP_SIZE_Y; ++y)
		{
			if (Map[MAP_SIZE_X * y + x] == '#')
			{
				RenderModel(
					M_PLANE,
					T_WALL,
					(vec3){x + 0.5f, -0.5f, y},
					(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				RenderModel(
					M_PLANE,
					T_WALL,
					(vec3){x - 0.5f, -0.5f, y},
					(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				RenderModel(
					M_PLANE,
					T_WALL,
					(vec3){x, -0.5f, y + 0.5f},
					(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
				RenderModel(
					M_PLANE,
					T_WALL,
					(vec3){x, -0.5f, y - 0.5f},
					(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
			}
			else
			{
				RenderModel(
					M_PLANE,
					T_FLOOR,
					(vec3){x, -1.5f, y},
					(vec3){0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
				RenderModel(
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
	for (i32 x = 0; x < MAP_SIZE_X; ++x)
	{
		RenderModel(
			M_PLANE,
			T_WALL,
			(vec3){x, -0.5f, -0.5f},
			(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
		RenderModel(
			M_PLANE,
			T_WALL,
			(vec3){x, -0.5f, MAP_SIZE_Y - 0.5f},
			(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
	}
	
	for (i32 y = 0; y < MAP_SIZE_Y; ++y)
	{
		RenderModel(
			M_PLANE,
			T_WALL,
			(vec3){-0.5f, -0.5f, y},
			(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
		RenderModel(
			M_PLANE,
			T_WALL,
			(vec3){MAP_SIZE_X - 0.5f, -0.5f, y},
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
		
		f32 Bob = BOB_INTENSITY * fabs(sin(a->BobTime));
		
		RenderModel(
			M_PLANE,
			a->ActiveTex,
			(vec3){a->Pos[0], Bob - 0.65f, a->Pos[1]},
			(vec3){-GLM_PI / 2.0f, 0.0f, 2.0f * GLM_PI - Yaw},
			(vec3){0.5f, 1.0f, 0.875f}
		);
	}
}

static void
DequeueAction(enum Actor a)
{
	memmove(
		&ActorData[a].ActionBuf[0],
		&ActorData[a].ActionBuf[1],
		sizeof(struct Action) * (ActorData[a].ActionCnt - 1)
	);
	--ActorData[a].ActionCnt;
}

static f32
InterpolateAngle(f32 a, f32 b, f32 t)
{
	f32 d = fmod(b - a, 2.0f * GLM_PI);
	f32 Shortest = fmod(2.0f * d, 2.0f * GLM_PI) - d;
	return a + Shortest * t;
}

static void
GetPointPos(char Point, vec2 Out)
{
	for (i32 x = 0; x < MAP_SIZE_X; ++x)
	{
		for (i32 y = 0; y < MAP_SIZE_Y; ++y)
		{
			if (Map[y * MAP_SIZE_X + x] == Point)
			{
				Out[0] = x;
				Out[1] = y;
				return;
			}
		}
	}
}
