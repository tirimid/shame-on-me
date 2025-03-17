#define WALK_DST_THRESHOLD2 0.015f
#define LOOK_THRESHOLD 0.005f

enum ActionType
{
	AT_TELEPORT = 0,
	AT_TELEPORT_TO,
	AT_WALK,
	AT_WALK_TO,
	AT_LOOK,
	AT_LOOK_AT,
	AT_LOOK_WALK_TO,
	AT_SET_TEXTURE,
	AT_WAIT,
	AT_SPEAK,
	AT_SWAP_MODEL
};

struct Action
{
	union
	{
		vec2 Dst;
		char Point;
		u8 Type;
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
struct Prop g_Props[O_MAX_CHOREO_PROPS];
usize g_PropCnt;

static void GetPointPos(char Point, vec2 Out);

static struct ActorData ActorData[A_END__] =
{
	// arkady (player).
	{
		.ActiveTex = 0 // dummy value, player is not drawn.
	},
	
	// peter.
	{
		.ActiveTex = T_C_HEARTS_J
	},
	
	// matthew.
	{
		.ActiveTex = T_GLASSES_DUMMY
	},
	
	// gerasim.
	{
		.ActiveTex = T_DUMMY
	}
};

static struct Action Actions[O_MAX_CHOREO_ACTIONS];
static usize ActionCnt;

void
C_Teleport(enum Actor a, vec2 Pos)
{
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Dst = {Pos[0], Pos[1]},
			.Type = AT_TELEPORT,
			.Actor = a
		};
	}
}

void
C_TeleportTo(enum Actor a, char Point)
{
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Point = Point,
			.Type = AT_TELEPORT_TO,
			.Actor = a
		};
	}
}

void
C_Walk(enum Actor a, vec2 Pos)
{
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Type = t,
			.Type = AT_SET_TEXTURE,
			.Actor = a
		};
	}
}

void
C_Wait(u32 Ms)
{
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
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
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
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
C_SwapModel(i32 Idx, enum Model NewModel)
{
	if (ActionCnt < O_MAX_CHOREO_ACTIONS)
	{
		Actions[ActionCnt++] = (struct Action)
		{
			.Data.Type = NewModel,
			.Actor = Idx,
			.Type = AT_SWAP_MODEL
		};
	}
}

void
C_Update(void)
{
	// update render camera.
	f32 VBob = fabs(sin(ActorData[A_ARKADY].BobTime));
	f32 HBob = fabs(cos(ActorData[A_ARKADY].BobTime));
	
	g_Camera.Pos[0] = ActorData[A_ARKADY].Pos[0] + O_HORIZ_BOB_INTENSITY * HBob;
	g_Camera.Pos[1] = O_VERT_BOB_INTENSITY * VBob;
	g_Camera.Pos[2] = ActorData[A_ARKADY].Pos[1] + O_HORIZ_BOB_INTENSITY * HBob;
	g_Camera.Pitch = ActorData[A_ARKADY].Pitch;
	g_Camera.Yaw = ActorData[A_ARKADY].Yaw;
	
	// update choreography actions.
	if (ActionCnt == 0)
		return;
	
	struct Action *Action = &Actions[0];
	struct ActorData *Actor = &ActorData[Action->Actor];
	
	switch (Action->Type)
	{
	case AT_TELEPORT:
		Actor->Pos[0] = Action->Data.Dst[0];
		Actor->Pos[1] = Action->Data.Dst[1];
		break;
	case AT_TELEPORT_TO:
		GetPointPos(Action->Data.Point, Actor->Pos);
		break;
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
		glm_vec2_scale(Move, O_WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += O_BOB_FREQUENCY;
		
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
		glm_vec2_scale(Move, O_WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += O_BOB_FREQUENCY;
		
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
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, Action->Data.Dst[0], O_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, Action->Data.Dst[1], O_LOOK_SPEED);
		
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
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, O_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, O_LOOK_SPEED);
		
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
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, O_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, O_LOOK_SPEED);
		
		vec2 Move = {0};
		glm_vec2_sub(Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, O_WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += O_BOB_FREQUENCY;
		
		return;
	}
	case AT_SET_TEXTURE:
		Actor->ActiveTex = Action->Data.Type;
		break;
	case AT_WAIT:
		if (Action->Data.Ms <= 0)
			break;
		
		Action->Data.Ms -= O_TICK_MS;
		
		return;
	case AT_SPEAK:
		if (!T_IsActive())
			T_Show(Action->Actor, Action->Data.Msg);
		
		if (I_KeyPressed(O_KEY_NEXT))
			break;
		
		return;
	case AT_SWAP_MODEL:
		g_Props[Action->Actor].m = Action->Data.Type;
		break;
	}
	
	// dequeue completed action.
	memmove(&Actions[0], &Actions[1], sizeof(struct Action) * --ActionCnt);
}

void
C_Render(void)
{
	// draw floor.
	R_SetTexture(T_FLOOR);
	for (u32 x = 0; x < g_Map.w; ++x)
	{
		for (u32 y = 0; y < g_Map.h; ++y)
		{
			if (g_Map.Data[g_Map.w * y + x] != '#')
			{
				R_RenderModel(
					M_PLANE,
					(vec3){x, -1.5f, y},
					(vec3){GLM_PI, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
			}
		}
	}
	
	// draw ceiling.
	R_SetTexture(T_CEILING);
	for (u32 x = 0; x < g_Map.w; ++x)
	{
		for (u32 y = 0; y < g_Map.h; ++y)
		{
			if (g_Map.Data[g_Map.w * y + x] != '#')
			{
				R_RenderModel(
					M_PLANE,
					(vec3){x, 0.5f, y},
					(vec3){0.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
			}
		}
	}
	
	// draw walls.
	R_SetTexture(T_WALL);
	
	for (u32 x = 0; x < g_Map.w; ++x)
	{
		for (u32 y = 0; y < g_Map.h; ++y)
		{
			if (g_Map.Data[g_Map.w * y + x] == '#')
			{
				R_RenderModel(
					M_PLANE,
					(vec3){x + 0.5f, -0.5f, y},
					(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				R_RenderModel(
					M_PLANE,
					(vec3){x - 0.5f, -0.5f, y},
					(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				R_RenderModel(
					M_PLANE,
					(vec3){x, -0.5f, y + 0.5f},
					(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
				R_RenderModel(
					M_PLANE,
					(vec3){x, -0.5f, y - 0.5f},
					(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
			}
		}
	}
	
	for (u32 x = 0; x < g_Map.w; ++x)
	{
		R_RenderModel(
			M_PLANE,
			(vec3){x, -0.5f, -0.5f},
			(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
		R_RenderModel(
			M_PLANE,
			(vec3){x, -0.5f, g_Map.h - 0.5f},
			(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
	}
	
	for (u32 y = 0; y < g_Map.h; ++y)
	{
		R_RenderModel(
			M_PLANE,
			(vec3){-0.5f, -0.5f, y},
			(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
		R_RenderModel(
			M_PLANE,
			(vec3){g_Map.w - 0.5f, -0.5f, y},
			(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
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
		
		f32 Bob = O_VERT_BOB_INTENSITY * fabs(sin(a->BobTime));
		
		R_SetTexture(a->ActiveTex);
		R_RenderModel(
			M_PLANE,
			(vec3){a->Pos[0], Bob - 0.65f, a->Pos[1]},
			(vec3){GLM_PI / 2.0f, GLM_PI, 2.0f * GLM_PI - Yaw},
			(vec3){0.5f, 1.0f, 0.875f}
		);
	}
	
	// draw all props.
	for (usize i = 0; i < g_PropCnt; ++i)
	{
		R_SetTexture(g_Props[i].t);
		R_RenderModel(g_Props[i].m, g_Props[i].Pos, g_Props[i].Rot, g_Props[i].Scale);
	}
}

i32
C_PutProp(enum Model m, enum Texture t, vec3 Pos, vec3 Rot, vec3 Scale)
{
	if (g_PropCnt < O_MAX_CHOREO_PROPS)
	{
		g_Props[g_PropCnt] = (struct Prop)
		{
			.m = m,
			.t = t,
			.Pos = {Pos[0], Pos[1], Pos[2]},
			.Rot = {Rot[0], Rot[1], Rot[2]},
			.Scale = {Scale[0], Scale[1], Scale[2]}
		};
		return g_PropCnt++;
	}
	
	return -1;
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
