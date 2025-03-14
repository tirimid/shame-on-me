#define WALK_DST_THRESHOLD2 0.015f
#define LOOK_THRESHOLD 0.005f

enum ActionType
{
	AT_WALK = 0,
	AT_WALK_TO,
	AT_LOOK,
	AT_LOOK_AT,
	AT_LOOK_WALK_TO,
	AT_SET_TEXTURE,
	AT_WAIT,
	AT_SPEAK
};

struct Action
{
	union
	{
		vec2 Dst;
		char Point;
		u8 Tex;
		i64 Ms;
		char const *Msg;
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
	// arkady (player).
	{
		.ActiveTex = 0 // dummy value, player is not drawn.
	},
	
	// peter.
	{
		.ActiveTex = T_DUMMY
	},
	
	// matthew.
	{
		.ActiveTex = T_DUMMY
	},
	
	// gerasim.
	{
		.ActiveTex = T_DUMMY
	}
};

static struct Action Actions[CF_MAX_CHOREO_ACTIONS];
static usize ActionCnt;

void
C_Walk(enum Actor a, vec2 Pos)
{
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Ms = Ms,
			.Type = AT_WAIT
		};
	}
}

void
C_Speak(enum TextboxSprite Ts, char const *Msg)
{
	if (ActionCnt < CF_MAX_CHOREO_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Msg = Msg,
			.Actor = Ts,
			.Type = AT_SPEAK
		};
	}
}

void
C_Update(void)
{
	// update render camera.
	f32 VBob = fabs(sin(ActorData[A_ARKADY].BobTime));
	f32 HBob = fabs(cos(ActorData[A_ARKADY].BobTime));
	
	g_Camera.Pos[0] = ActorData[A_ARKADY].Pos[0] + CF_HORIZ_BOB_INTENSITY * HBob;
	g_Camera.Pos[1] = CF_VERT_BOB_INTENSITY * VBob;
	g_Camera.Pos[2] = ActorData[A_ARKADY].Pos[1] + CF_HORIZ_BOB_INTENSITY * HBob;
	g_Camera.Pitch = ActorData[A_ARKADY].Pitch;
	g_Camera.Yaw = ActorData[A_ARKADY].Yaw;
	
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
		
		vec2 Move = {0};
		glm_vec2_sub(Action->Data.Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, CF_WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += CF_BOB_FREQUENCY;
		
		return;
	}
	case AT_WALK_TO:
	{
		vec2 Dst = {0};
		GetPointPos(Action->Data.Point, Dst);
		
		if (glm_vec2_distance2(Actor->Pos, Dst) < WALK_DST_THRESHOLD2)
		{
			glm_vec2_copy(Dst, Actor->Pos);
			break;
		}
		
		vec2 Move = {0};
		glm_vec2_sub(Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, CF_WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += CF_BOB_FREQUENCY;
		
		return;
	}
	case AT_LOOK:
	{
		if (fabs(ShortestAngle(Action->Data.Dst[0], Actor->Pitch)) < LOOK_THRESHOLD
			&& fabs(ShortestAngle(Action->Data.Dst[1], Actor->Yaw)) < LOOK_THRESHOLD)
		{
			Actor->Pitch = Action->Data.Dst[0];
			Actor->Yaw = Action->Data.Dst[1];
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, Action->Data.Dst[0], CF_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, Action->Data.Dst[1], CF_LOOK_SPEED);
		
		return;
	}
	case AT_LOOK_AT:
	{
		vec2 Dir = {0};
		GetPointPos(Action->Data.Point, Dir);
		glm_vec2_sub(Dir, Actor->Pos, Dir);
		
		f32 DstPitch = 0.0f;
		f32 DstYaw = atan2f(Dir[1], Dir[0]);
		
		if (fabs(ShortestAngle(Actor->Pitch, DstPitch)) < LOOK_THRESHOLD
			&& fabs(ShortestAngle(Actor->Yaw, DstYaw)) < LOOK_THRESHOLD)
		{
			Actor->Pitch = DstPitch;
			Actor->Yaw = DstYaw;
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, CF_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, CF_LOOK_SPEED);
		
		return;
	}
	case AT_LOOK_WALK_TO:
	{
		vec2 Dst = {0}, Dir = {0};
		GetPointPos(Action->Data.Point, Dst);
		glm_vec2_sub(Dst, Actor->Pos, Dir);
		
		f32 DstPitch = 0.0f;
		f32 DstYaw = atan2f(Dir[1], Dir[0]);
		
		if (ShortestAngle(DstPitch, Actor->Pitch) < LOOK_THRESHOLD
			&& ShortestAngle(DstYaw, Actor->Yaw) < LOOK_THRESHOLD
			&& glm_vec2_distance2(Actor->Pos, Dst) < WALK_DST_THRESHOLD2)
		{
			glm_vec2_copy(Dst, Actor->Pos);
			Actor->Pitch = DstPitch;
			Actor->Yaw = DstYaw;
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, CF_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, CF_LOOK_SPEED);
		
		vec2 Move = {0};
		glm_vec2_sub(Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, CF_WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += CF_BOB_FREQUENCY;
		
		return;
	}
	case AT_SET_TEXTURE:
		Actor->ActiveTex = Action->Data.Tex;
		break;
	case AT_WAIT:
		if (Action->Data.Ms <= 0)
			break;
		
		Action->Data.Ms -= CF_TICK_MS;
		
		return;
	case AT_SPEAK:
		if (I_KeyPressed(CF_KEY_NEXT))
			break;
		
		return;
	}
	
	// dequeue completed action.
	memmove(&Actions[0], &Actions[1], sizeof(struct Action) * --ActionCnt);
}

void
C_Render(void)
{
	// draw floor, ceiling, inner walls.
	for (u32 x = 0; x < g_Map.w; ++x)
	{
		for (u32 y = 0; y < g_Map.h; ++y)
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
	for (u32 x = 0; x < g_Map.w; ++x)
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
	
	for (u32 y = 0; y < g_Map.h; ++y)
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
	for (usize i = A_ARKADY + 1; i < A_END__; ++i)
	{
		struct ActorData *Player = &ActorData[A_ARKADY];
		struct ActorData *a = &ActorData[i];
		
		vec2 Dir;
		glm_vec2_sub(Player->Pos, a->Pos, Dir);
		glm_vec2_normalize(Dir);
		f32 Yaw = atan2f(Dir[0], -Dir[1]);
		
		f32 Bob = CF_VERT_BOB_INTENSITY * fabs(sin(a->BobTime));
		
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
	for (u32 x = 0; x < g_Map.w; ++x)
	{
		for (u32 y = 0; y < g_Map.h; ++y)
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
