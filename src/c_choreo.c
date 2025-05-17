// SPDX-License-Identifier: GPL-3.0-or-later

#define C_WALKTHRESHOLD 0.12f
#define C_WALKTHRESHOLD2 (C_WALKTHRESHOLD * C_WALKTHRESHOLD)
#define C_LOOKTHRESHOLD 0.005f

typedef enum c_actiontype
{
	C_TELEPORT = 0,
	C_TELEPORTTO,
	C_WALK,
	C_WALKTO,
	C_LOOK,
	C_LOOKAT,
	C_LOOKWALKTO,
	C_SETTEXTURE,
	C_WAIT,
	C_SPEAK,
	C_SWAPMODEL,
	C_SETLIGHTINTENSITY,
	C_PANCAMERA,
	C_SETDURAKPHASE,
	C_QUIT,
	C_PLAYSFX,
	C_GLOBALSHADE,
	C_SHAKECAMERA
} c_actiontype_t;

typedef union c_action
{
	u8 type;
	
	struct
	{
		u8 type;
		u8 actor;
		vec2 dst;
	} teleport;
	
	struct
	{
		u8 type;
		char point;
		u8 actor;
	} teleportto;
	
	struct
	{
		u8 type;
		u8 actor;
		vec2 dst;
	} walk;
	
	struct
	{
		u8 type;
		char point;
		u8 actor;
	} walkto;
	
	struct
	{
		u8 type;
		u8 actor;
		f32 pitch, yaw;
	} look;
	
	struct
	{
		u8 type;
		char point;
		u8 actor;
	} lookat;
	
	struct
	{
		u8 type;
		char point;
		u8 actor;
	} lookwalkto;
	
	struct
	{
		u8 type;
		u8 tex;
		u8 actor;
	} settexture;
	
	struct
	{
		u8 type;
		i64 ms;
	} wait;
	
	struct
	{
		u8 type;
		u8 textsprite;
		char const *msg;
	} speak;
	
	struct
	{
		u8 type;
		u8 newmodel;
		usize propidx;
	} swapmodel;
	
	struct
	{
		u8 type;
		f32 newintensity;
		usize lightidx;
	} setlightintensity;
	
	struct
	{
		u8 type;
		f32 pitch, yaw;
		vec3 pos;
	} pancamera;
	
	struct
	{
		u8 type;
		d_phase_t phase;
	} setdurakphase;
	
	struct
	{
		u8 type;
		s_sfx_t sfx;
	} playsfx;
	
	struct
	{
		u8 type;
		vec3 shade;
	} globalshade;
	
	struct
	{
		u8 type;
		f32 intensity;
	} shakecamera;
} c_action_t;

typedef struct c_actordata
{
	vec2 pos;
	f32 pitch, yaw;
	f32 bobtime;
	u8 activetex;
} c_actordata_t;

c_map_t c_map;
c_prop_t c_props[O_MAXPROPS];
usize c_nprops;

static void c_getpoint(char point, OUT vec2 pos);

static c_actordata_t c_actors[C_ACTOR_END] =
{
	// arkady (player).
	{
		.activetex = 0 // dummy value, player is not drawn.
	},
	
	// peter.
	{
		.activetex = R_EYESDUMMY
	},
	
	// matthew.
	{
		.activetex = R_GLASSESDUMMY
	},
	
	// gerasim.
	{
		.activetex = R_DUMMY
	}
};

static c_action_t c_actions[O_MAXACTIONS];
static usize c_nactions;

void
c_teleport(c_actor_t a, vec2 pos)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.teleport =
			{
				.type = C_TELEPORT,
				.dst = {pos[0], pos[1]},
				.actor = a
			}
		};
	}
}

void
c_teleportto(c_actor_t a, char point)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.teleportto =
			{
				.type = C_TELEPORTTO,
				.point = point,
				.actor = a
			}
		};
	}
}

void
c_walk(c_actor_t a, vec2 pos)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.walk =
			{
				.type = C_WALK,
				.dst = {pos[0], pos[1]},
				.actor = a
			}
		};
	}
}

void
c_walkto(c_actor_t a, char point)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.walkto =
			{
				.type = C_WALKTO,
				.point = point,
				.actor = a
			}
		};
	}
}

void
c_look(c_actor_t a, f32 pitchdeg, f32 yawdeg)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.look =
			{
				.type = C_LOOK,
				.pitch = glm_rad(pitchdeg),
				.yaw = glm_rad(yawdeg),
				.actor = a
			}
		};
	}
}

void
c_lookat(c_actor_t a, char point)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.lookat =
			{
				.type = C_LOOKAT,
				.point = point,
				.actor = a
			}
		};
	}
}

void
c_lookwalkto(c_actor_t a, char point)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.lookwalkto =
			{
				.type = C_LOOKWALKTO,
				.point = point,
				.actor = a
			}
		};
	}
}

void
c_settexture(c_actor_t a, r_tex_t t)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.settexture =
			{
				.type = C_SETTEXTURE,
				.tex = t,
				.actor = a
			}
		};
	}
}

void
c_wait(u64 ms)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.wait =
			{
				.type = C_WAIT,
				.ms = ms
			}
		};
	}
}

void
c_speak(t_sprite_t t, char const *msg)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.speak =
			{
				.type = C_SPEAK,
				.msg = msg,
				.textsprite = t
			}
		};
	}
}

void
c_swapmodel(usize idx, r_model_t newmodel)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.swapmodel =
			{
				.type = C_SWAPMODEL,
				.propidx = idx,
				.newmodel = newmodel
			}
		};
	}
}

void
c_setlightintensity(usize idx, f32 intensity)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.setlightintensity =
			{
				.type = C_SETLIGHTINTENSITY,
				.lightidx = idx,
				.newintensity = intensity
			}
		};
	}
}

void
c_pancamera(vec3 pos, f32 pitchdeg, f32 yawdeg)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.pancamera =
			{
				.type = C_PANCAMERA,
				.pos = {pos[0], pos[1], pos[2]},
				.pitch = glm_rad(pitchdeg),
				.yaw = glm_rad(yawdeg)
			}
		};
	}
}

void
c_setdurakphase(d_phase_t phase)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.setdurakphase =
			{
				.type = C_SETDURAKPHASE,
				.phase = phase
			}
		};
	}
}

void
c_quit(void)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.type = C_QUIT
		};
	}
}

void
c_playsfx(s_sfx_t sfx)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.playsfx =
			{
				.type = C_PLAYSFX,
				.sfx = sfx
			}
		};
	}
}

void
c_globalshade(vec3 shade)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.globalshade =
			{
				.type = C_GLOBALSHADE,
				.shade = {shade[0], shade[1], shade[2]}
			}
		};
	}
}

void
c_shakecamera(f32 intensity)
{
	if (c_nactions < O_MAXACTIONS)
	{
		c_actions[c_nactions++] = (c_action_t)
		{
			.shakecamera =
			{
				.type = C_SHAKECAMERA,
				.intensity = intensity
			}
		};
	}
}

void
c_update(void)
{
	// update render camera.
	f32 vbob = fabs(sin(c_actors[C_ARKADY].bobtime));
	f32 hbob = fabs(cos(c_actors[C_ARKADY].bobtime));
	
	r_cam.base.pos[0] = c_actors[C_ARKADY].pos[0] + O_HORIZBOB * hbob;
	r_cam.base.pos[1] = O_VERTBOB * vbob;
	r_cam.base.pos[2] = c_actors[C_ARKADY].pos[1] + O_HORIZBOB * hbob;
	r_cam.base.pitch = c_actors[C_ARKADY].pitch;
	r_cam.base.yaw = c_actors[C_ARKADY].yaw;
	
	// update choreography actions.
	if (c_nactions == 0)
	{
		return;
	}
	
	c_action_t *action = &c_actions[0];
	
	switch (action->type)
	{
	case C_TELEPORT:
	{
		c_actordata_t *actor = &c_actors[action->teleport.actor];
		actor->pos[0] = action->teleport.dst[0];
		actor->pos[1] = action->teleport.dst[1];
		break;
	}
	case C_TELEPORTTO:
	{
		c_actordata_t *actor = &c_actors[action->teleportto.actor];
		c_getpoint(action->teleportto.point, actor->pos);
		break;
	}
	case C_WALK:
	{
		c_actordata_t *actor = &c_actors[action->walk.actor];
		
		if (glm_vec2_distance2(actor->pos, action->walk.dst) < C_WALKTHRESHOLD2)
		{
			glm_vec2_copy(action->walk.dst, actor->pos);
			break;
		}
		
		vec2 move = {0};
		glm_vec2_sub(action->walk.dst, actor->pos, move);
		glm_vec2_normalize(move);
		glm_vec2_scale(move, O_WALKSPEED, move);
		
		glm_vec2_add(actor->pos, move, actor->pos);
		actor->bobtime += O_BOBFREQ;
		
		return;
	}
	case C_WALKTO:
	{
		c_actordata_t *actor = &c_actors[action->walkto.actor];
		
		vec2 dst = {0};
		c_getpoint(action->walkto.point, dst);
		
		if (glm_vec2_distance2(actor->pos, dst) < C_WALKTHRESHOLD2)
		{
			glm_vec2_copy(dst, actor->pos);
			break;
		}
		
		vec2 move = {0};
		glm_vec2_sub(dst, actor->pos, move);
		glm_vec2_normalize(move);
		glm_vec2_scale(move, O_WALKSPEED, move);
		
		glm_vec2_add(actor->pos, move, actor->pos);
		actor->bobtime += O_BOBFREQ;
		
		return;
	}
	case C_LOOK:
	{
		c_actordata_t *actor = &c_actors[action->look.actor];
		
		if (fabs(shortestangle(action->look.pitch, actor->pitch)) < C_LOOKTHRESHOLD
			&& fabs(shortestangle(action->look.yaw, actor->yaw)) < C_LOOKTHRESHOLD)
		{
			actor->pitch = action->look.pitch;
			actor->yaw = action->look.yaw;
			break;
		}
		
		actor->pitch = interpangle(actor->pitch, action->look.pitch, O_LOOKSPEED);
		actor->yaw = interpangle(actor->yaw, action->look.yaw, O_LOOKSPEED);
		
		return;
	}
	case C_LOOKAT:
	{
		c_actordata_t *actor = &c_actors[action->lookat.actor];
		
		vec2 dir = {0};
		c_getpoint(action->lookat.point, dir);
		glm_vec2_sub(dir, actor->pos, dir);
		
		f32 dstpitch = 0.0f;
		f32 dstyaw = atan2f(dir[1], dir[0]);
		
		if (fabs(shortestangle(actor->pitch, dstpitch)) < C_LOOKTHRESHOLD
			&& fabs(shortestangle(actor->yaw, dstyaw)) < C_LOOKTHRESHOLD)
		{
			actor->pitch = dstpitch;
			actor->yaw = dstyaw;
			break;
		}
		
		actor->pitch = interpangle(actor->pitch, dstpitch, O_LOOKSPEED);
		actor->yaw = interpangle(actor->yaw, dstyaw, O_LOOKSPEED);
		
		return;
	}
	case C_LOOKWALKTO:
	{
		c_actordata_t *actor = &c_actors[action->lookwalkto.actor];
		
		vec2 dst = {0}, dir = {0};
		c_getpoint(action->lookwalkto.point, dst);
		glm_vec2_sub(dst, actor->pos, dir);
		
		f32 dstpitch = 0.0f;
		f32 dstyaw = atan2f(dir[1], dir[0]);
		
		bool rotdone = false;
		if (fabs(shortestangle(dstpitch, actor->pitch)) < C_LOOKTHRESHOLD
			&& fabs(shortestangle(dstyaw, actor->yaw)) < C_LOOKTHRESHOLD)
		{
			actor->pitch = dstpitch;
			actor->yaw = dstyaw;
			rotdone = true;
		}
		
		bool movedone = false;
		if (glm_vec2_distance2(actor->pos, dst) < C_WALKTHRESHOLD2)
		{
			glm_vec2_copy(dst, actor->pos);
			movedone = true;
		}
		
		if (!rotdone)
		{
			actor->pitch = interpangle(actor->pitch, dstpitch, O_LOOKSPEED);
			actor->yaw = interpangle(actor->yaw, dstyaw, O_LOOKSPEED);
		}
		
		if (!movedone)
		{
			vec2 move = {0};
			glm_vec2_sub(dst, actor->pos, move);
			glm_vec2_normalize(move);
			glm_vec2_scale(move, O_WALKSPEED, move);
			
			glm_vec2_add(actor->pos, move, actor->pos);
			actor->bobtime += O_BOBFREQ;
		}
		
		if (rotdone && movedone)
		{
			break;
		}
		
		return;
	}
	case C_SETTEXTURE:
	{
		c_actordata_t *actor = &c_actors[action->settexture.actor];
		actor->activetex = action->settexture.tex;
		break;
	}
	case C_WAIT:
		if (action->wait.ms <= 0)
		{
			break;
		}
		action->wait.ms -= O_TICKMS;
		return;
	case C_SPEAK:
		t_show(action->speak.textsprite, action->speak.msg);
		if (t_scrolldone() && i_kpressed(O_KSEL))
		{
			break;
		}
		return;
	case C_SWAPMODEL:
		c_props[action->swapmodel.propidx].model = action->swapmodel.newmodel;
		break;
	case C_SETLIGHTINTENSITY:
		r_setlightintensity(
			action->setlightintensity.lightidx,
			action->setlightintensity.newintensity
		);
		break;
	case C_PANCAMERA:
		r_pancam(action->pancamera.pos, action->pancamera.pitch, action->pancamera.yaw);
		break;
	case C_SETDURAKPHASE:
		d_setphase(action->setdurakphase.phase);
		break;
	case C_QUIT:
		exit(0);
		break;
	case C_PLAYSFX:
		s_playsfx(action->playsfx.sfx);
		break;
	case C_GLOBALSHADE:
		r_globalshade(action->globalshade.shade);
		break;
	case C_SHAKECAMERA:
		r_cam.base.shake = action->shakecamera.intensity;
		break;
	}
	
	// dequeue completed action.
	memmove(&c_actions[0], &c_actions[1], sizeof(c_action_t) * --c_nactions);
}

void
c_rendertiles(void)
{
	// draw floor.
	r_settex(R_FLOOR);
	for (u32 x = 0; x < c_map.w; ++x)
	{
		for (u32 y = 0; y < c_map.h; ++y)
		{
			if (c_map.data[c_map.w * y + x] != '#')
			{
				r_batchtile(
					(vec3){x, -1.5f, y},
					(vec3){GLM_PI, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
			}
		}
	}
	
	r_flushtiles();
	
	// draw ceiling.
	r_settex(R_CEILING);
	for (u32 x = 0; x < c_map.w; ++x)
	{
		for (u32 y = 0; y < c_map.h; ++y)
		{
			if (c_map.data[c_map.w * y + x] != '#')
			{
				r_batchtile(
					(vec3){x, 0.5f, y},
					(vec3){0.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 0.5f}
				);
			}
		}
	}
	
	r_flushtiles();
	
	// draw walls.
	r_settex(R_WALL);
	
	for (u32 x = 0; x < c_map.w; ++x)
	{
		for (u32 y = 0; y < c_map.h; ++y)
		{
			if (c_map.data[c_map.w * y + x] == '#')
			{
				r_batchtile(
					(vec3){x + 0.5f, -0.5f, y},
					(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				r_batchtile(
					(vec3){x - 0.5f, -0.5f, y},
					(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
					(vec3){0.5f, 0.5f, 1.0f}
				);
				r_batchtile(
					(vec3){x, -0.5f, y + 0.5f},
					(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
				r_batchtile(
					(vec3){x, -0.5f, y - 0.5f},
					(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
					(vec3){0.5f, 1.0f, 1.0f}
				);
			}
		}
	}
	
	for (u32 x = 0; x < c_map.w; ++x)
	{
		r_batchtile(
			(vec3){x, -0.5f, -0.5f},
			(vec3){-GLM_PI / 2.0f, 0.0f, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
		r_batchtile(
			(vec3){x, -0.5f, c_map.h - 0.5f},
			(vec3){GLM_PI / 2.0f, GLM_PI, 0.0f},
			(vec3){0.5f, 1.0f, 1.0f}
		);
	}
	
	for (u32 y = 0; y < c_map.h; ++y)
	{
		r_batchtile(
			(vec3){-0.5f, -0.5f, y},
			(vec3){-GLM_PI / 2.0f, 0.0f, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
		r_batchtile(
			(vec3){c_map.w - 0.5f, -0.5f, y},
			(vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI / 2.0f},
			(vec3){0.5f, 0.5f, 1.0f}
		);
	}
	
	r_flushtiles();
}

void
c_rendermodels(void)
{
	// draw all non-player characters.
	for (usize i = C_ARKADY + 1; i < C_ACTOR_END; ++i)
	{
		vec3 campos;
		r_effcamstate(campos, NULL, NULL);
		
		vec2 lookpos = {campos[0], campos[2]};
		c_actordata_t *a = &c_actors[i];
		
		vec2 dir;
		glm_vec2_sub(lookpos, a->pos, dir);
		glm_vec2_normalize(dir);
		f32 yaw = atan2f(dir[0], -dir[1]);
		
		f32 bob = O_VERTBOB * fabs(sin(a->bobtime));
		
		r_settex(a->activetex);
		r_rendermodel(
			R_PLANE,
			(vec3){a->pos[0], bob - 0.65f, a->pos[1]},
			(vec3){GLM_PI / 2.0f, GLM_PI, 2.0f * GLM_PI - yaw},
			(vec3){0.5f, 1.0f, 0.875f}
		);
	}
	
	// draw all props.
	for (usize i = 0; i < c_nprops; ++i)
	{
		r_settex(c_props[i].tex);
		r_rendermodel(c_props[i].model, c_props[i].pos, c_props[i].rot, c_props[i].scale);
	}
}

i64
c_putprop(r_model_t m, r_tex_t t, vec3 pos, vec3 rot, vec3 scale)
{
	if (c_nprops >= O_MAXPROPS)
	{
		return -1;
	}
	c_props[c_nprops] = (c_prop_t)
	{
		.model = m,
		.tex = t,
		.pos = {pos[0], pos[1], pos[2]},
		.rot = {rot[0], rot[1], rot[2]},
		.scale = {scale[0], scale[1], scale[2]}
	};
	return c_nprops++;
}

i64
c_putpropat(r_model_t m, r_tex_t t, char point, vec3 trans, vec3 rot, vec3 scale)
{
	if (c_nprops >= O_MAXPROPS)
	{
		return -1;
	}
	vec2 pos = {0};
	c_getpoint(point, pos);
	c_props[c_nprops] = (c_prop_t)
	{
		.model = m,
		.tex = t,
		.pos = {pos[0] + trans[0], trans[1], pos[1] + trans[2]},
		.rot = {rot[0], rot[1], rot[2]},
		.scale = {scale[0], scale[1], scale[2]}
	};
	return c_nprops++;
}

i32
c_putlightat(char point, vec3 trans, f32 intensity)
{
	vec2 pos = {0};
	c_getpoint(point, pos);
	return r_putlight(
		(vec3){pos[0] + trans[0], trans[1], pos[1] + trans[2]},
		intensity
	);
}

static void
c_getpoint(char point, OUT vec2 pos)
{
	for (u32 x = 0; x < c_map.w; ++x)
	{
		for (u32 y = 0; y < c_map.h; ++y)
		{
			if (c_map.data[y * c_map.w + x] == point)
			{
				pos[0] = x;
				pos[1] = y;
				return;
			}
		}
	}
}
