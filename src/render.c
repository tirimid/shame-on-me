#define R_INCMODEL(name) \
	{ \
		.verts = name##_obj_VertData, \
		.idxs = name##_obj_IdxData, \
		.nverts = name##_obj_VERT_CNT, \
		.nidxs = name##_obj_IDX_CNT \
	}

#define R_INCSHADER(name) \
	{ \
		.vertsrc = (char *)name##_vert_glsl, \
		.fragsrc = (char *)name##_frag_glsl, \
		.vertlen = sizeof(name##_vert_glsl), \
		.fraglen = sizeof(name##_frag_glsl) \
	}

#define R_INCGEOSHADER(name) \
	{ \
		.geosrc = (char *)name##_geo_glsl, \
		.vertsrc = (char *)name##_vert_glsl, \
		.fragsrc = (char *)name##_frag_glsl, \
		.geolen = sizeof(name##_geo_glsl), \
		.vertlen = sizeof(name##_vert_glsl), \
		.fraglen = sizeof(name##_frag_glsl) \
	}

#define R_INCTEX(name) \
	{ \
		.data = name##_png, \
		.size = sizeof(name##_png) \
	}

#define R_INCFONT(name) \
	{ \
		.data = name##_ttf, \
		.size = sizeof(name##_ttf) \
	}

typedef struct r_modeldata
{
	f32 const *verts;
	usize nverts;
	u32 const *idxs;
	usize nidxs;
	u32 vbo, ibo, vao;
} r_modeldata;

typedef struct r_shaderdata
{
	char *geosrc, *vertsrc, *fragsrc;
	i32 geolen, vertlen, fraglen;
	u32 prog;
} r_shaderdata;

typedef struct r_texdata
{
	u8 const *data;
	usize size;
	SDL_Surface *surf;
	u32 tex;
} r_texdata;

typedef struct r_fontdata
{
	u8 const *data;
	usize size;
	TTF_Font *font;
} r_fontdata;

typedef struct r_uniformdata
{
	u32 modelmats, normalmats;
	u32 viewmat, projmat;
	u32 tex;
	u32 lights, shadowmaps;
	u32 lightpos, shadowviewmats;
	u32 fadebrightness;
} r_uniformdata;

typedef struct r_renderstate
{
	u32 framebuf, colorbuf, depthbuf;
	vec4 lights[O_MAXLIGHTS];
	u32 shadowmaps[O_MAXLIGHTS];
	u32 shadowfbos[O_MAXLIGHTS];
	usize nlights;
	f32 fadebrightness;
	r_fadestatus fadestatus;
	mat4 batchmodelmats[O_MAXTILEBATCH];
	mat3 batchnormmats[O_MAXTILEBATCH];
	usize batchsize;
} r_renderstate;

r_camera r_cam;

static void r_preproc(char *src, usize len);
static void r_deleteglctx(void);

static SDL_Window *r_wnd;
static SDL_GLContext r_glctx;
static r_renderstate r_state;
static r_uniformdata r_uniforms;

// data tables.
static r_modeldata r_models[R_MODEL_END] =
{
	R_INCMODEL(plane),
	R_INCMODEL(cube),
	R_INCMODEL(door_open),
	R_INCMODEL(door_closed),
	R_INCMODEL(table),
	R_INCMODEL(window),
	R_INCMODEL(lightbulb),
	R_INCMODEL(card_stack)
};

static r_shaderdata r_shaders[R_SHADER_END] =
{
	R_INCSHADER(base),
	R_INCSHADER(overlay),
	R_INCGEOSHADER(shadow)
};

static r_texdata r_texs[R_TEX_END] =
{
	R_INCTEX(something),
	R_INCTEX(floor),
	R_INCTEX(ceiling),
	R_INCTEX(wall),
	R_INCTEX(dummy),
	R_INCTEX(black),
	R_INCTEX(dummy_face),
	R_INCTEX(black50),
	R_INCTEX(glasses_dummy),
	R_INCTEX(glasses_dummy_face),
	R_INCTEX(c_back),
	R_INCTEX(c_spades_6),
	R_INCTEX(c_spades_7),
	R_INCTEX(c_spades_8),
	R_INCTEX(c_spades_9),
	R_INCTEX(c_spades_10),
	R_INCTEX(c_spades_j),
	R_INCTEX(c_spades_q),
	R_INCTEX(c_spades_k),
	R_INCTEX(c_spades_a),
	R_INCTEX(c_diamonds_6),
	R_INCTEX(c_diamonds_7),
	R_INCTEX(c_diamonds_8),
	R_INCTEX(c_diamonds_9),
	R_INCTEX(c_diamonds_10),
	R_INCTEX(c_diamonds_j),
	R_INCTEX(c_diamonds_q),
	R_INCTEX(c_diamonds_k),
	R_INCTEX(c_diamonds_a),
	R_INCTEX(c_clubs_6),
	R_INCTEX(c_clubs_7),
	R_INCTEX(c_clubs_8),
	R_INCTEX(c_clubs_9),
	R_INCTEX(c_clubs_10),
	R_INCTEX(c_clubs_j),
	R_INCTEX(c_clubs_q),
	R_INCTEX(c_clubs_k),
	R_INCTEX(c_clubs_a),
	R_INCTEX(c_hearts_6),
	R_INCTEX(c_hearts_7),
	R_INCTEX(c_hearts_8),
	R_INCTEX(c_hearts_9),
	R_INCTEX(c_hearts_10),
	R_INCTEX(c_hearts_j),
	R_INCTEX(c_hearts_q),
	R_INCTEX(c_hearts_k),
	R_INCTEX(c_hearts_a),
	R_INCTEX(door),
	R_INCTEX(table),
	R_INCTEX(window),
	R_INCTEX(lightbulb),
	R_INCTEX(eyes_dummy),
	R_INCTEX(eyes_dummy_face)
};

static r_fontdata r_fonts[R_FONT_END] =
{
	R_INCFONT(vcr_osd_mono)
};

i32
r_init(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// set up rendering data and structures.
	r_wnd = SDL_CreateWindow(
		O_WNDTITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		O_WNDWIDTH,
		O_WNDHEIGHT,
		O_WNDFLAGS
	);
	if (!r_wnd)
	{
		showerr("render: failed to create window - %s!", SDL_GetError());
		return 1;
	}
	
	r_glctx = SDL_GL_CreateContext(r_wnd);
	if (!r_glctx)
	{
		showerr("render: failed to create GL context - %s!", SDL_GetError());
		return 1;
	}
	atexit(r_deleteglctx);
	
	glewExperimental = GL_TRUE;
	i32 rc = glewInit();
	if (rc != GLEW_OK)
	{
		showerr("render: failed to init GLEW - %s!", glewGetErrorString(rc));
		return 1;
	}
	
	// set up models.
	for (usize i = 0; i < R_MODEL_END; ++i)
	{
		// generate GL state.
		glGenVertexArrays(1, &r_models[i].vao);
		glGenBuffers(1, &r_models[i].vbo);
		glGenBuffers(1, &r_models[i].ibo);
		
		glBindVertexArray(r_models[i].vao);
		glBindBuffer(GL_ARRAY_BUFFER, r_models[i].vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_models[i].ibo);
		
		glBufferData(
			GL_ARRAY_BUFFER,
			8 * sizeof(f32) * r_models[i].nverts,
			r_models[i].verts,
			GL_STATIC_DRAW
		);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(u32) * r_models[i].nidxs,
			r_models[i].idxs,
			GL_STATIC_DRAW
		);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(3 * sizeof(f32)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(5 * sizeof(f32)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
	
	// set up shader programs.
	char shaderlog[O_MAXLOGLEN];
	for (usize i = 0; i < R_SHADER_END; ++i)
	{
		// create geometry shader if present.
		u32 geo = 0;
		if (r_shaders[i].geosrc)
		{
			r_preproc(r_shaders[i].geosrc, r_shaders[i].geolen);
			geo = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(
				geo,
				1,
				(char const *const *)&r_shaders[i].geosrc,
				&r_shaders[i].geolen
			);
			glCompileShader(geo);
			glGetShaderiv(geo, GL_COMPILE_STATUS, &rc);
			if (!rc)
			{
				glGetShaderInfoLog(geo, O_MAXLOGLEN, NULL, shaderlog);
				showerr("render: failed to compile geometry shader %zu - %s!", i, shaderlog);
				return 1;
			}
		}

		// create vertex shader.
		r_preproc(r_shaders[i].vertsrc, r_shaders[i].vertlen);
		u32 vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(
			vert,
			1,
			(char const *const *)&r_shaders[i].vertsrc,
			&r_shaders[i].vertlen
		);
		glCompileShader(vert);
		glGetShaderiv(vert, GL_COMPILE_STATUS, &rc);
		if (!rc)
		{
			glGetShaderInfoLog(vert, O_MAXLOGLEN, NULL, shaderlog);
			showerr("render: failed to compile vertex shader %zu - %s!", i, shaderlog);
			return 1;
		}
		
		// create fragment shader.
		r_preproc(r_shaders[i].fragsrc, r_shaders[i].fraglen);
		u32 frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(
			frag,
			1,
			(char const *const *)&r_shaders[i].fragsrc,
			&r_shaders[i].fraglen
		);
		glCompileShader(frag);
		glGetShaderiv(frag, GL_COMPILE_STATUS, &rc);
		if (!rc)
		{
			glGetShaderInfoLog(frag, O_MAXLOGLEN, NULL, shaderlog);
			showerr("render: failed to compile fragment shader %zu - %s!", i, shaderlog);
			return 1;
		}
		
		// create program.
		u32 prog = glCreateProgram();
		if (r_shaders[i].geosrc)
		{
			glAttachShader(prog, geo);
		}
		glAttachShader(prog, vert);
		glAttachShader(prog, frag);
		glLinkProgram(prog);
		glGetProgramiv(prog, GL_LINK_STATUS, &rc);
		if (!rc)
		{
			glGetProgramInfoLog(prog, O_MAXLOGLEN, NULL, shaderlog);
			showerr("render: failed to link shader program %zu - %s!", i, shaderlog);
			return 1;
		}
		
		if (geo)
		{
			glDeleteShader(geo);
		}
		glDeleteShader(vert);
		glDeleteShader(frag);
		
		r_shaders[i].prog = prog;
	}
	
	// set up all textures.
	for (usize i = 0; i < R_TEX_END; ++i)
	{
		// load texture.
		SDL_RWops *rwops = SDL_RWFromConstMem(r_texs[i].data, r_texs[i].size);
		if (!rwops)
		{
			showerr("render: failed to create image texture RWops - %s!", SDL_GetError());
			return 1;
		}
		
		r_texs[i].surf = IMG_Load_RW(rwops, 1);
		if (!r_texs[i].surf)
		{
			showerr("render: failed to create SDL surface - %s!", IMG_GetError());
			return 1;
		}
		
		// generate GL state.
		glGenTextures(1, &r_texs[i].tex);
		glBindTexture(GL_TEXTURE_2D, r_texs[i].tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			r_texs[i].surf->w,
			r_texs[i].surf->h,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			r_texs[i].surf->pixels
		);
	}
	
	// set up all fonts.
	for (usize i = 0; i < R_FONT_END; ++i)
	{
		SDL_RWops *rwops = SDL_RWFromConstMem(r_fonts[i].data, r_fonts[i].size);
		if (!rwops)
		{
			showerr("render: failed to create font RWops - %s!", SDL_GetError());
			return 1;
		}
		
		r_fonts[i].font = TTF_OpenFontRW(rwops, 1, O_FONTSIZE);
		if (!r_fonts[i].font)
		{
			showerr("render: failed to open font - %s!", TTF_GetError());
			return 1;
		}
	}
	
	// create initial framebuffer data.
	glGenFramebuffers(1, &r_state.framebuf);
	glGenRenderbuffers(1, &r_state.colorbuf);
	glGenRenderbuffers(1, &r_state.depthbuf);
	
	// create initial shadowmap data.
	glGenTextures(O_MAXLIGHTS, r_state.shadowmaps);
	glGenFramebuffers(O_MAXLIGHTS, r_state.shadowfbos);
	for (usize i = 0; i < O_MAXLIGHTS; ++i)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, r_state.shadowmaps[i]);
		for (i32 j = 0; j < 6; ++j)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + j,
				0,
				GL_DEPTH_COMPONENT,
				O_SHADOWMAPSIZE,
				O_SHADOWMAPSIZE,
				0,
				GL_DEPTH_COMPONENT,
				GL_FLOAT,
				NULL
			);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		glBindFramebuffer(GL_FRAMEBUFFER, r_state.shadowfbos[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, r_state.shadowmaps[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	
	// set GL state.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	return 0;
}

bool
r_lightenabled(usize idx)
{
	return r_state.lights[idx][3] > 0.0f;
}

void
r_renderbounds(OUT i32 *w, OUT i32 *h)
{
	SDL_GetWindowSize(r_wnd, w, h);
	*w /= O_PIXELATION;
	*h /= O_PIXELATION;
}

void
r_beginshadow(usize idx)
{
	vec3 lightpos = {r_state.lights[idx][0], r_state.lights[idx][1], r_state.lights[idx][2]};
	f32 aspect = (f32)O_SHADOWMAPSIZE / (f32)O_SHADOWMAPSIZE;
	
	glViewport(0, 0, O_SHADOWMAPSIZE, O_SHADOWMAPSIZE);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r_state.shadowfbos[idx]);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// compute matrices.
	mat4 modelmat;
	glm_translate_make(modelmat, lightpos);
	
	mat4 shadowviewmats[6];
	glm_look(lightpos, (vec3){1.0f, 0.0f, 0.0f}, (vec3){0.0f, -1.0f, 0.0f}, shadowviewmats[0]);
	glm_look(lightpos, (vec3){-1.0f, 0.0f, 0.0f}, (vec3){0.0f, -1.0f, 0.0f}, shadowviewmats[1]);
	glm_look(lightpos, (vec3){0.0f, 1.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, shadowviewmats[2]);
	glm_look(lightpos, (vec3){0.0f, -1.0f, 0.0f}, (vec3){0.0f, 0.0f, -1.0f}, shadowviewmats[3]);
	glm_look(lightpos, (vec3){0.0f, 0.0f, 1.0f}, (vec3){0.0f, -1.0f, 0.0f}, shadowviewmats[4]);
	glm_look(lightpos, (vec3){0.0f, 0.0f, -1.0f}, (vec3){0.0f, -1.0f, 0.0f}, shadowviewmats[5]);
	
	mat4 projmat;
	glm_perspective(GLM_PI / 2.0f, aspect, O_CAMCLIPNEAR, O_CAMCLIPFAR, projmat);
	
	// upload uniforms.
	glUniform3fv(r_uniforms.lightpos, 1, (f32 *)lightpos);
	glUniformMatrix4fv(r_uniforms.modelmats, 1, GL_FALSE, (f32 *)modelmat);
	glUniformMatrix4fv(r_uniforms.shadowviewmats, 6, GL_FALSE, (f32 *)shadowviewmats);
	glUniformMatrix4fv(r_uniforms.projmat, 1, GL_FALSE, (f32 *)projmat);
	glUniform4fv(r_uniforms.lights, O_MAXLIGHTS, (f32 *)&r_state.lights[0]);
}

void
r_beginbase(void)
{
	i32 w, h;
	SDL_GetWindowSize(r_wnd, &w, &h);
	f32 aspect = (f32)w / (f32)h;
	
	glViewport(0, 0, w / O_PIXELATION, h / O_PIXELATION);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r_state.framebuf);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// compute matrices.
	vec3 pos;
	f32 pitch, yaw;
	r_effcamstate(pos, &pitch, &yaw);
	vec3 camdir =
	{
		cos(yaw) * cos(pitch),
		sin(pitch),
		sin(yaw) * cos(pitch)
	};
	glm_normalize(camdir);
	
	mat4 viewmat, projmat;
	glm_look(pos, camdir, (vec3){0.0f, 1.0f, 0.0f}, viewmat);
	glm_perspective(O_CAMFOV, aspect, O_CAMCLIPNEAR, O_CAMCLIPFAR, projmat);
	
	// upload uniforms.
	glUniformMatrix4fv(r_uniforms.viewmat, 1, GL_FALSE, (f32 *)viewmat);
	glUniformMatrix4fv(r_uniforms.projmat, 1, GL_FALSE, (f32 *)projmat);
	glUniform4fv(r_uniforms.lights, O_MAXLIGHTS, (f32 *)&r_state.lights[0]);
	glUniform1f(r_uniforms.fadebrightness, r_state.fadebrightness);
	
	u32 shadowsamplers[O_MAXLIGHTS] = {0};
	for (usize i = 0; i < O_MAXLIGHTS; ++i)
	{
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, r_state.shadowmaps[i]);
		shadowsamplers[i] = 1 + i;
	}
	glUniform1iv(r_uniforms.shadowmaps, O_MAXLIGHTS, (i32 *)shadowsamplers);
}

void
r_beginoverlay(void)
{
	i32 w, h;
	SDL_GetWindowSize(r_wnd, &w, &h);
	
	glViewport(0, 0, w / O_PIXELATION, h / O_PIXELATION);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r_state.framebuf);
	glDisable(GL_DEPTH_TEST);
	
	// compute matrices.
	mat4 projmat;
	glm_ortho(0.0f, w / O_PIXELATION, 0.0f, h / O_PIXELATION, -1.0f, 1.0f, projmat);
	
	// upload uniforms.
	glUniformMatrix4fv(r_uniforms.projmat, 1, GL_FALSE, (f32 *)projmat);
}

void
r_present(void)
{
	i32 w, h;
	SDL_GetWindowSize(r_wnd, &w, &h);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, r_state.framebuf);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0,
		0,
		w / O_PIXELATION,
		h / O_PIXELATION,
		0,
		0,
		w,
		h,
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		GL_NEAREST
	);
	SDL_GL_SwapWindow(r_wnd);
}

void
r_resize(i32 x, i32 y)
{
	// regenerate frame buffer.
	glDeleteBuffers(1, &r_state.colorbuf);
	glGenRenderbuffers(1, &r_state.colorbuf);
	glDeleteBuffers(1, &r_state.depthbuf);
	glGenRenderbuffers(1, &r_state.depthbuf);
	
	glBindFramebuffer(GL_FRAMEBUFFER, r_state.framebuf);
	
	glBindRenderbuffer(GL_RENDERBUFFER, r_state.colorbuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, x / O_PIXELATION, y / O_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, r_state.colorbuf);
	
	glBindRenderbuffer(GL_RENDERBUFFER, r_state.depthbuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, x / O_PIXELATION, y / O_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, r_state.depthbuf);
}

void
r_setshader(r_shader s)
{
	u32 p = r_shaders[s].prog;
	
	glUseProgram(p);
	
	// set uniforms.
	r_uniforms.modelmats = glGetUniformLocation(p, "modelmats");
	r_uniforms.normalmats = glGetUniformLocation(p, "normalmats");
	r_uniforms.viewmat = glGetUniformLocation(p, "viewmat");
	r_uniforms.projmat = glGetUniformLocation(p, "projmat");
	r_uniforms.tex = glGetUniformLocation(p, "tex");
	r_uniforms.lights = glGetUniformLocation(p, "lights");
	r_uniforms.shadowmaps = glGetUniformLocation(p, "shadowmaps");
	r_uniforms.lightpos = glGetUniformLocation(p, "lightpos");
	r_uniforms.shadowviewmats = glGetUniformLocation(p, "shadowviewmats");
	r_uniforms.fadebrightness = glGetUniformLocation(p, "fadebrightness");
}

void
r_settex(r_tex t)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, r_texs[t].tex);
	glUniform1i(r_uniforms.tex, 0);
}

void
r_rendermodel(r_model m, vec3 pos, vec3 rot, vec3 scale)
{
	// create transformation matrices.
	mat4 modelmat;
	makexform(pos, rot, scale, modelmat);
	
	mat3 normmat;
	makenorm(modelmat, normmat);
	
	// render model.
	glUniformMatrix4fv(r_uniforms.modelmats, 1, GL_FALSE, (f32 *)modelmat);
	glUniformMatrix3fv(r_uniforms.normalmats, 1, GL_FALSE, (f32 *)normmat);
	glBindVertexArray(r_models[m].vao);
	glDrawElements(GL_TRIANGLES, r_models[m].nidxs, GL_UNSIGNED_INT, 0);
}

i32
r_putlight(vec3 pos, f32 intensity)
{
	if (r_state.nlights >= O_MAXLIGHTS)
	{
		return -1;
	}
	
	vec4 new = {pos[0], pos[1], pos[2], intensity};
	glm_vec4_copy(new, r_state.lights[r_state.nlights]);
	return r_state.nlights++;
}

void
r_setlightintensity(usize idx, f32 intensity)
{
	r_state.lights[idx][3] = intensity;
}

void
r_renderrect(r_tex t, i32 x, i32 y, i32 w, i32 h, f32 angle)
{
	vec3 pos = {x + w / 2.0f, y + h / 2.0f, 0.0f};
	vec3 rot = {GLM_PI / 2.0f, GLM_PI + angle, GLM_PI};
	vec3 scale = {w / 2.0f, 1.0f, h / 2.0f};
	
	r_settex(t);
	r_rendermodel(R_PLANE, pos, rot, scale);
}

void
r_rendertext(r_font f, char const *text, i32 x, i32 y, i32 w, i32 h)
{
	// TTF render out text to create texture.
	SDL_Surface *solid = TTF_RenderUTF8_Solid_Wrapped(
		r_fonts[f].font,
		text,
		(SDL_Color)O_FONTCOLOR,
		w
	);
	solid->h = solid->h < h ? solid->h : h;
	
	SDL_Surface *rgba = SDL_ConvertSurfaceFormat(solid, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_FreeSurface(solid);
	
	u32 tex;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		rgba->w,
		rgba->h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		rgba->pixels
	);
	
	y += h - rgba->h;
	
	// create transformation matrix.
	vec3 pos =
	{
		x + rgba->w / 2.0f,
		y + rgba->h / 2.0f,
		0.0f
	};
	
	vec3 scale =
	{
		rgba->w / 2.0f,
		1.0f,
		rgba->h / 2.0f
	};
	
	mat4 transm, rotm, scalem;
	glm_translate_make(transm, pos);
	glm_euler((vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI}, rotm);
	glm_scale_make(scalem, scale);
	
	mat4 modelmat = GLM_MAT4_IDENTITY_INIT;
	glm_mat4_mul(modelmat, transm, modelmat);
	glm_mat4_mul(modelmat, rotm, modelmat);
	glm_mat4_mul(modelmat, scalem, modelmat);
	
	// render model.
	glUniformMatrix4fv(r_uniforms.modelmats, 1, GL_FALSE, (f32 *)modelmat);
	glUniform1i(r_uniforms.tex, GL_TEXTURE0);
	glBindVertexArray(r_models[R_PLANE].vao);
	glDrawElements(GL_TRIANGLES, r_models[R_PLANE].nidxs, GL_UNSIGNED_INT, NULL);
	
	glDeleteTextures(1, &tex);
	SDL_FreeSurface(rgba);
}

void
r_batchtile(vec3 pos, vec3 rot, vec3 scale)
{
	if (r_state.batchsize >= O_MAXTILEBATCH)
	{
		r_flushtiles();
	}
	
	makexform(pos, rot, scale, r_state.batchmodelmats[r_state.batchsize]);
	makenorm(r_state.batchmodelmats[r_state.batchsize], r_state.batchnormmats[r_state.batchsize]);
	++r_state.batchsize;
}

void
r_flushtiles(void)
{
	if (r_state.batchsize == 0)
	{
		return;
	}
	
	glUniformMatrix4fv(r_uniforms.modelmats, r_state.batchsize, GL_FALSE, (f32 *)r_state.batchmodelmats);
	glUniformMatrix3fv(r_uniforms.normalmats, r_state.batchsize, GL_FALSE, (f32 *)r_state.batchnormmats);
	glBindVertexArray(r_models[R_PLANE].vao);
	glDrawElementsInstanced(GL_TRIANGLES, r_models[R_PLANE].nidxs, GL_UNSIGNED_INT, NULL, r_state.batchsize);
	r_state.batchsize = 0;
}

void
r_update(void)
{
	// update fade brightness.
	if (r_state.fadestatus)
	{
		r_state.fadebrightness += O_FADESPEED;
		r_state.fadebrightness = r_state.fadebrightness > 1.0f ? 1.0f : r_state.fadebrightness;
	}
	else
	{
		r_state.fadebrightness -= O_FADESPEED;
		r_state.fadebrightness = r_state.fadebrightness < 0.0f ? 0.0f : r_state.fadebrightness;
	}
	
	// update camera pan.
	r_cam.pan.pitch = interpangle(r_cam.pan.pitch, r_cam.pan.dstpitch, O_PANROTSPEED);
	r_cam.pan.yaw = interpangle(r_cam.pan.yaw, r_cam.pan.dstyaw, O_PANROTSPEED);
	glm_vec3_lerp(r_cam.pan.pos, r_cam.pan.dstpos, O_PANPOSSPEED, r_cam.pan.pos);
}

void
r_fade(r_fadestatus fs)
{
	r_state.fadestatus = fs;
}

void
r_pancam(vec3 pos, f32 pitch, f32 yaw)
{
	r_cam.pan.dstpitch = pitch;
	r_cam.pan.dstyaw = yaw;
	glm_vec3_copy(pos, r_cam.pan.dstpos);
}

void
r_effcamstate(OUT vec3 pos, OUT f32 *pitch, OUT f32 *yaw)
{
	if (pos)
	{
		glm_vec3_copy(r_cam.base.pos, pos);
		glm_vec3_add(r_cam.pan.pos, pos, pos);
	}
	if (pitch)
	{
		*pitch = r_cam.base.pitch + r_cam.pan.pitch;
	}
	if (yaw)
	{
		*yaw = r_cam.base.yaw + r_cam.pan.yaw;
	}
}

static void
r_preproc(char *src, usize len)
{
	// substitute all constants for in-game values prior to shader compilation.
	for (usize i = 0; i < len; ++i)
	{
		if (len - i >= 12 && !strncmp(&src[i], "$O_MAXLIGHTS", 12))
		{
			usize cl = snprintf(&src[i], 12, "%u", O_MAXLIGHTS);
			memset(&src[i + cl], ' ', 12 - cl);
		}
		else if (len - i >= 15 && !strncmp(&src[i], "$O_AMBIENTLIGHT", 15))
		{
			usize cl = snprintf(&src[i], 15, "%f", O_AMBIENTLIGHT);
			memset(&src[i + cl], ' ', 15 - cl);
		}
		else if (len - i >= 12 && !strncmp(&src[i], "$O_LIGHTSTEP", 12))
		{
			usize cl = snprintf(&src[i], 12, "%f", O_LIGHTSTEP);
			memset(&src[i + cl], ' ', 12 - cl);
		}
		else if (len - i >= 13 && !strncmp(&src[i], "$O_SHADOWBIAS", 13))
		{
			usize cl = snprintf(&src[i], 13, "%f", O_SHADOWBIAS);
			memset(&src[i + cl], ' ', 13 - cl);
		}
		else if (len - i >= 13 && !strncmp(&src[i], "$O_CAMCLIPFAR", 13))
		{
			usize cl = snprintf(&src[i], 13, "%f", O_CAMCLIPFAR);
			memset(&src[i + cl], ' ', 13 - cl);
		}
		else if (len - i >= 15 && !strncmp(&src[i], "$O_MAXTILEBATCH", 15))
		{
			usize cl = snprintf(&src[i], 15, "%u", O_MAXTILEBATCH);
			memset(&src[i + cl], ' ', 15 - cl);
		}
	}
}

static void
r_deleteglctx(void)
{
	SDL_GL_DeleteContext(r_glctx);
}
