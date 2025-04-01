#define C_WALK_DST_THRESHOLD2 0.015f
#define C_LOOK_THRESHOLD 0.005f

typedef enum C_ActionType
{
	C_AT_TELEPORT = 0,
	C_AT_TELEPORT_TO,
	C_AT_WALK,
	C_AT_WALK_TO,
	C_AT_LOOK,
	C_AT_LOOK_AT,
	C_AT_LOOK_WALK_TO,
	C_AT_SET_TEXTURE,
	C_AT_WAIT,
	C_AT_SPEAK,
	C_AT_SWAP_MODEL,
	C_AT_SET_LIGHT_INTENSITY
} C_ActionType;

typedef union C_Action
{
	u8 Type;
	
	union
	{
		struct
		{
			u8 Type;
			vec2 Dst;
			u8 Actor;
		} Teleport;
		
		struct
		{
			u8 Type;
			char Point;
			u8 Actor;
		} TeleportTo;
		
		struct
		{
			u8 Type;
			vec2 Dst;
			u8 Actor;
		} Walk;
		
		struct
		{
			u8 Type;
			char Point;
			u8 Actor;
		} WalkTo;
		
		struct
		{
			u8 Type;
			f32 Pitch, Yaw;
			u8 Actor;
		} Look;
		
		struct
		{
			u8 Type;
			char Point;
			u8 Actor;
		} LookAt;
		
		struct
		{
			u8 Type;
			char Point;
			u8 Actor;
		} LookWalkTo;
		
		struct
		{
			u8 Type;
			u8 Tex;
			u8 Actor;
		} SetTexture;
		
		struct
		{
			u8 Type;
			i64 MS;
		} Wait;
		
		struct
		{
			u8 Type;
			char const *Msg;
			u8 TextSprite;
		} Speak;
		
		struct
		{
			u8 Type;
			usize PropIdx;
			u8 NewModel;
		} SwapModel;
		
		struct
		{
			u8 Type;
			usize LightIdx;
			f32 NewIntensity;
		} SetLightIntensity;
	} Data;
} C_Action;

typedef struct C_ActorData
{
	vec2 Pos;
	f32 Pitch, Yaw;
	f32 BobTime;
	u8 ActiveTex;
} C_ActorData;

C_MapData C_Map;
C_Prop C_Props[O_MAX_CHOREO_PROPS];
usize C_PropCnt;

static void C_GetPointPos(char Point, vec2 Out);

static C_ActorData C_Actors[C_A_END__] =
{
	// arkady (player).
	{
		.ActiveTex = 0 // dummy value, player is not drawn.
	},
	
	// peter.
	{
		.ActiveTex = R_T_EYES_DUMMY
	},
	
	// matthew.
	{
		.ActiveTex = R_T_GLASSES_DUMMY
	},
	
	// gerasim.
	{
		.ActiveTex = R_T_DUMMY
	}
};

static C_Action C_Actions[O_MAX_CHOREO_ACTIONS];
static usize C_ActionCnt;

void
C_Teleport(C_Actor a, vec2 Pos)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.Teleport =
		{
			.Type = C_AT_TELEPORT,
			.Dst = {Pos[0], Pos[1]},
			.Actor = a
		}
	};
}

void
C_TeleportTo(C_Actor a, char Point)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.TeleportTo =
		{
			.Type = C_AT_TELEPORT_TO,
			.Point = Point,
			.Actor = a
		}
	};
}

void
C_Walk(C_Actor a, vec2 Pos)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.Walk =
		{
			.Type = C_AT_WALK,
			.Dst = {Pos[0], Pos[1]},
			.Actor = a
		}
	};
}

void
C_WalkTo(C_Actor a, char Point)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.WalkTo =
		{
			.Type = C_AT_WALK_TO,
			.Point = Point,
			.Actor = a
		}
	};
}

void
C_Look(C_Actor a, f32 PitchDeg, f32 YawDeg)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.Look =
		{
			.Type = C_AT_LOOK,
			.Pitch = glm_rad(PitchDeg),
			.Yaw = glm_rad(YawDeg),
			.Actor = a
		}
	};
}

void
C_LookAt(C_Actor a, char Point)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.LookAt =
		{
			.Type = C_AT_LOOK_AT,
			.Point = Point,
			.Actor = a
		}
	};
}

void
C_LookWalkTo(C_Actor a, char Point)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.LookWalkTo =
		{
			.Type = C_AT_LOOK_WALK_TO,
			.Point = Point,
			.Actor = a
		}
	};
}

void
C_SetTexture(C_Actor a, R_Texture t)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.SetTexture =
		{
			.Type = C_AT_SET_TEXTURE,
			.Tex = t,
			.Actor = a
		}
	};
}

void
C_Wait(u64 MS)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.Wait =
		{
			.Type = C_AT_WAIT,
			.MS = MS
		}
	};
}

void
C_Speak(T_TextboxSprite t, char const *Msg)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.Speak =
		{
			.Type = C_AT_SPEAK,
			.Msg = Msg,
			.TextSprite = t
		}
	};
}

void
C_SwapModel(usize Idx, R_Model NewModel)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.SwapModel =
		{
			.Type = C_AT_SWAP_MODEL,
			.PropIdx = Idx,
			.NewModel = NewModel
		}
	};
}

void
C_SetLightIntensity(usize Idx, f32 Intensity)
{
	if (C_ActionCnt >= O_MAX_CHOREO_ACTIONS) {return;}
	C_Actions[C_ActionCnt++] = (C_Action)
	{
		.Data.SetLightIntensity =
		{
			.Type = C_AT_SET_LIGHT_INTENSITY,
			.LightIdx = Idx,
			.NewIntensity = Intensity
		}
	};
}

void
C_Update(void)
{
	// update render camera.
	f32 VBob = fabs(sin(C_Actors[C_A_ARKADY].BobTime));
	f32 HBob = fabs(cos(C_Actors[C_A_ARKADY].BobTime));
	
	R_Cam.Pos[0] = C_Actors[C_A_ARKADY].Pos[0] + O_HORIZ_BOB_INTENSITY * HBob;
	R_Cam.Pos[1] = O_VERT_BOB_INTENSITY * VBob;
	R_Cam.Pos[2] = C_Actors[C_A_ARKADY].Pos[1] + O_HORIZ_BOB_INTENSITY * HBob;
	R_Cam.Pitch = C_Actors[C_A_ARKADY].Pitch;
	R_Cam.Yaw = C_Actors[C_A_ARKADY].Yaw;
	
	// update choreography actions.
	if (C_ActionCnt == 0) {return;}
	
	C_Action *Action = &C_Actions[0];
	
	switch (Action->Type)
	{
	case C_AT_TELEPORT:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.Teleport.Actor];
		Actor->Pos[0] = Action->Data.Teleport.Dst[0];
		Actor->Pos[1] = Action->Data.Teleport.Dst[1];
		break;
	}
	case C_AT_TELEPORT_TO:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.TeleportTo.Actor];
		C_GetPointPos(Action->Data.TeleportTo.Point, Actor->Pos);
		break;
	}
	case C_AT_WALK:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.Walk.Actor];
		
		if (glm_vec2_distance2(Actor->Pos, Action->Data.Walk.Dst) < C_WALK_DST_THRESHOLD2)
		{
			glm_vec2_copy(Action->Data.Walk.Dst, Actor->Pos);
			break;
		}
		
		vec2 Move = {0};
		glm_vec2_sub(Action->Data.Walk.Dst, Actor->Pos, Move);
		glm_vec2_normalize(Move);
		glm_vec2_scale(Move, O_WALK_SPEED, Move);
		
		glm_vec2_add(Actor->Pos, Move, Actor->Pos);
		Actor->BobTime += O_BOB_FREQUENCY;
		
		return;
	}
	case C_AT_WALK_TO:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.WalkTo.Actor];
		
		vec2 Dst = {0};
		C_GetPointPos(Action->Data.WalkTo.Point, Dst);
		
		if (glm_vec2_distance2(Actor->Pos, Dst) < C_WALK_DST_THRESHOLD2)
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
	case C_AT_LOOK:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.Look.Actor];
		
		if (fabs(ShortestAngle(Action->Data.Look.Pitch, Actor->Pitch)) < C_LOOK_THRESHOLD
			&& fabs(ShortestAngle(Action->Data.Look.Yaw, Actor->Yaw)) < C_LOOK_THRESHOLD)
		{
			Actor->Pitch = Action->Data.Look.Pitch;
			Actor->Yaw = Action->Data.Look.Yaw;
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, Action->Data.Look.Pitch, O_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, Action->Data.Look.Yaw, O_LOOK_SPEED);
		
		return;
	}
	case C_AT_LOOK_AT:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.LookAt.Actor];
		
		vec2 Dir = {0};
		C_GetPointPos(Action->Data.LookAt.Point, Dir);
		glm_vec2_sub(Dir, Actor->Pos, Dir);
		
		f32 DstPitch = 0.0f;
		f32 DstYaw = atan2f(Dir[1], Dir[0]);
		
		if (fabs(ShortestAngle(Actor->Pitch, DstPitch)) < C_LOOK_THRESHOLD
			&& fabs(ShortestAngle(Actor->Yaw, DstYaw)) < C_LOOK_THRESHOLD)
		{
			Actor->Pitch = DstPitch;
			Actor->Yaw = DstYaw;
			break;
		}
		
		Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, O_LOOK_SPEED);
		Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, O_LOOK_SPEED);
		
		return;
	}
	case C_AT_LOOK_WALK_TO:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.LookWalkTo.Actor];
		
		vec2 Dst = {0}, Dir = {0};
		C_GetPointPos(Action->Data.LookWalkTo.Point, Dst);
		glm_vec2_sub(Dst, Actor->Pos, Dir);
		
		f32 DstPitch = 0.0f;
		f32 DstYaw = atan2f(Dir[1], Dir[0]);
		
		bool RotDone = false;
		if (fabs(ShortestAngle(DstPitch, Actor->Pitch)) < C_LOOK_THRESHOLD
			&& fabs(ShortestAngle(DstYaw, Actor->Yaw)) < C_LOOK_THRESHOLD)
		{
			Actor->Pitch = DstPitch;
			Actor->Yaw = DstYaw;
			RotDone = true;
		}
		
		bool MoveDone = false;
		if (glm_vec2_distance2(Actor->Pos, Dst) < C_WALK_DST_THRESHOLD2)
		{
			glm_vec2_copy(Dst, Actor->Pos);
			MoveDone = true;
		}
		
		if (!RotDone)
		{
			Actor->Pitch = InterpolateAngle(Actor->Pitch, DstPitch, O_LOOK_SPEED);
			Actor->Yaw = InterpolateAngle(Actor->Yaw, DstYaw, O_LOOK_SPEED);
		}
		
		if (!MoveDone)
		{
			vec2 Move = {0};
			glm_vec2_sub(Dst, Actor->Pos, Move);
			glm_vec2_normalize(Move);
			glm_vec2_scale(Move, O_WALK_SPEED, Move);
			
			glm_vec2_add(Actor->Pos, Move, Actor->Pos);
			Actor->BobTime += O_BOB_FREQUENCY;
		}
		
		if (RotDone && MoveDone) {break;}
		
		return;
	}
	case C_AT_SET_TEXTURE:
	{
		C_ActorData *Actor = &C_Actors[Action->Data.SetTexture.Actor];
		Actor->ActiveTex = Action->Data.SetTexture.Tex;
		break;
	}
	case C_AT_WAIT:
		if (Action->Data.Wait.MS <= 0) {break;}
		Action->Data.Wait.MS -= O_TICK_MS;
		return;
	case C_AT_SPEAK:
		if (!T_IsActive()) {T_Show(Action->Data.Speak.TextSprite, Action->Data.Speak.Msg);}
		if (I_KeyPressed(O_KEY_NEXT)) {break;}
		return;
	case C_AT_SWAP_MODEL:
		C_Props[Action->Data.SwapModel.PropIdx].Model = Action->Data.SwapModel.NewModel;
		break;
	case C_AT_SET_LIGHT_INTENSITY:
		R_SetLightIntensity(
			Action->Data.SetLightIntensity.LightIdx,
			Action->Data.SetLightIntensity.NewIntensity
		);
		break;
	}
	
	// dequeue completed action.
	memmove(&C_Actions[0], &C_Actions[1], sizeof(C_Action) * --C_ActionCnt);
}

void
C_RenderTiles(void)
{
	// draw floor.
	R_SetTexture(R_T_FLOOR);
	for (u32 x = 0; x < C_Map.w; ++x)
	{
		for (u32 y = 0; y < C_Map.h; ++y)
		{
			if (C_Map.Data[C_Map.w * y + x] != '#')
			{
				R_BatchRenderTile(
					(vec3){x, -1.5f, y},
					(vec3){GLM_PI, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
			}
		}
	}
	
	R_FlushTileBatch();
	
	// draw ceiling.
	R_SetTexture(R_T_CEILING);
	for (u32 x = 0; x < C_Map.w; ++x)
	{
		for (u32 y = 0; y < C_Map.h; ++y)
		{
			if (C_Map.Data[C_Map.w * y + x] != '#')
			{
				R_BatchRenderTile(
					(vec3){x, 0.5f, y},
					(vec3){0.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
			}
		}
	}
	
	R_FlushTileBatch();
	
	// draw walls.
	R_SetTexture(R_T_WALL);
	
	for (u32 x = 0; x < C_Map.w; ++x)
	{
		for (u32 y = 0; y < C_Map.h; ++y)
		{
			if (C_Map.Data[C_Map.w * y + x] == '#')
			{
				R_BatchRenderTile(
					(vec3){x + 0.5f, -0.5f, y},
					(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				R_BatchRenderTile(
					(vec3){x - 0.5f, -0.5f, y},
					(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				R_BatchRenderTile(
					(vec3){x, -0.5f, y + 0.5f},
					(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
				R_BatchRenderTile(
					(vec3){x, -0.5f, y - 0.5f},
					(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
			}
		}
	}
	
	for (u32 x = 0; x < C_Map.w; ++x)
	{
		R_BatchRenderTile(
			(vec3){x, -0.5f, -0.5f},
			(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
		R_BatchRenderTile(
			(vec3){x, -0.5f, C_Map.h - 0.5f},
			(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
	}
	
	for (u32 y = 0; y < C_Map.h; ++y)
	{
		R_BatchRenderTile(
			(vec3){-0.5f, -0.5f, y},
			(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
		R_BatchRenderTile(
			(vec3){C_Map.w - 0.5f, -0.5f, y},
			(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
	}
	
	R_FlushTileBatch();
}

void
C_RenderModels(void)
{
	// draw all non-player characters.
	for (usize i = C_A_ARKADY + 1; i < C_A_END__; ++i)
	{
		C_ActorData *Player = &C_Actors[C_A_ARKADY];
		C_ActorData *a = &C_Actors[i];
		
		vec2 Dir;
		glm_vec2_sub(Player->Pos, a->Pos, Dir);
		glm_vec2_normalize(Dir);
		f32 Yaw = atan2f(Dir[0], -Dir[1]);
		
		f32 Bob = O_VERT_BOB_INTENSITY * fabs(sin(a->BobTime));
		
		R_SetTexture(a->ActiveTex);
		R_RenderModel(
			R_M_PLANE,
			(vec3){a->Pos[0], Bob - 0.65f, a->Pos[1]},
			(vec3){GLM_PI / 2.0f, GLM_PI, 2.0f * GLM_PI - Yaw},
			(vec3){0.5f, 1.0f, 0.875f}
		);
	}
	
	// draw all props.
	for (usize i = 0; i < C_PropCnt; ++i)
	{
		R_SetTexture(C_Props[i].Tex);
		R_RenderModel(C_Props[i].Model, C_Props[i].Pos, C_Props[i].Rot, C_Props[i].Scale);
	}
}

i64
C_PutProp(R_Model m, R_Texture t, vec3 Pos, vec3 Rot, vec3 Scale)
{
	if (C_PropCnt >= O_MAX_CHOREO_PROPS) {return -1;}
	C_Props[C_PropCnt] = (C_Prop)
	{
		.Model = m,
		.Tex = t,
		.Pos = {Pos[0], Pos[1], Pos[2]},
		.Rot = {Rot[0], Rot[1], Rot[2]},
		.Scale = {Scale[0], Scale[1], Scale[2]}
	};
	return C_PropCnt++;
}

static void
C_GetPointPos(char Point, vec2 Out)
{
	for (u32 x = 0; x < C_Map.w; ++x)
	{
		for (u32 y = 0; y < C_Map.h; ++y)
		{
			if (C_Map.Data[y * C_Map.w + x] == Point)
			{
				Out[0] = x;
				Out[1] = y;
				return;
			}
		}
	}
}
