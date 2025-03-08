#include "choreo.h"

#include <math.h>
#include <string.h>

#include "render.h"

#define ACTION_BUF_CAP 32
#define WALK_SPEED 0.08f
#define WALK_DST_THRESHOLD2 0.0025f
#define BOB_INTENSITY 0.2f
#define BOB_FREQUENCY 1.5f
#define LOOK_SPEED 0.1f
#define LOOK_THRESHOLD 0.01f
#define MAP_SIZE_X 25
#define MAP_SIZE_Y 25

enum ActionType
{
	AT_WALK = 0,
	AT_LOOK
};

struct Action
{
	union
	{
		vec2 Dst;
	} Data;
	u8 Type;
};

struct ActorData
{
	vec2 Pos;
	f32 Pitch, Yaw;
	struct Action ActionBuf[ACTION_BUF_CAP];
	u8 ActionCnt;
	f32 BobTime;
};

static void DequeueAction(enum Actor a);

static struct ActorData ActorData[A_END__];

void
ActorWalk(enum Actor a, vec2 Pos)
{
	u8 *ActionCnt = &ActorData[a].ActionCnt;
	if (*ActionCnt < ACTION_BUF_CAP)
	{
		ActorData[a].ActionBuf[*ActionCnt].Type = AT_WALK;
		glm_vec2_copy(Pos, ActorData[a].ActionBuf[*ActionCnt].Data.Dst);
		++*ActionCnt;
	}
}

void
ActorLook(enum Actor a, f32 Pitch, f32 Yaw)
{
	u8 *ActionCnt = &ActorData[a].ActionCnt;
	if (*ActionCnt < ACTION_BUF_CAP)
	{
		ActorData[a].ActionBuf[*ActionCnt].Type = AT_LOOK;
		vec2 Dst = {Pitch, Yaw};
		glm_vec2_copy(Dst, ActorData[a].ActionBuf[*ActionCnt].Data.Dst);
		++*ActionCnt;
	}
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
			if (glm_vec2_distance2(Ad->Pos, a->Data.Dst) < WALK_DST_THRESHOLD2)
			{
				DequeueAction(i);
				--i;
				continue;
			}
			
			vec2 Move;
			glm_vec2_sub(a->Data.Dst, Ad->Pos, Move);
			glm_vec2_normalize(Move);
			glm_vec2_scale(Move, WALK_SPEED, Move);
			
			glm_vec2_add(Ad->Pos, Move, Ad->Pos);
			Ad->BobTime += BOB_FREQUENCY * glm_vec2_norm(Move);
			
			break;
		}
		case AT_LOOK:
			if (fabs(a->Data.Dst[0] - Ad->Pitch) < LOOK_THRESHOLD
				&& fabs(a->Data.Dst[1] - Ad->Yaw) < LOOK_THRESHOLD)
			{
				DequeueAction(i);
				--i;
				continue;
			}
			
			Ad->Pitch = glm_lerp(Ad->Pitch, a->Data.Dst[0], LOOK_SPEED);
			Ad->Yaw = glm_lerp(Ad->Yaw, a->Data.Dst[1], LOOK_SPEED);
			
			break;
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
	// draw floor and ceiling.
	for (i32 x = 0; x < MAP_SIZE_X; ++x)
	{
		for (i32 y = 0; y < MAP_SIZE_Y; ++y)
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
			(vec3){x, -0.5f, MAP_SIZE_X - 0.5f},
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
			(vec3){MAP_SIZE_Y - 0.5f, -0.5f, y},
			(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
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
