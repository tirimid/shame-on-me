// SPDX-License-Identifier: GPL-3.0-or-later

typedef enum u_elemtype
{
	U_LABEL = 0,
	U_BUTTON,
	U_HOLDBUTTON,
	U_SLIDER
} u_elemtype_t;

typedef union u_elem
{
	// all UI elements have at least these properties.
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
	} any;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		char const *text;
	} label;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		char const *text;
	} button;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		char const *text;
	} holdbutton;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		char const *text;
		f64 *val;
	} slider;
} u_elem_t;

static i32 u_x, u_y;
static u_elem_t u_elems[O_MAXUIELEMS];
static usize u_nelems;

void
u_begin(i32 x, i32 y)
{
	u_x = x;
	u_y = y;
	u_nelems = 0;
}

void
u_render(void)
{
	if (!u_nelems)
	{
		return;
	}
	
	// find render boundaries to draw panel.
	i32 minx = INT32_MAX, miny = INT32_MAX;
	i32 maxx = INT32_MIN, maxy = INT32_MIN;
	for (usize i = 0; i < u_nelems; ++i)
	{
		i32 x = u_elems[i].any.x, y = u_elems[i].any.y;
		i32 w = u_elems[i].any.w, h = u_elems[i].any.h;
		
		minx = x < minx ? x : minx;
		miny = y < miny ? y : miny;
		maxx = x + w > maxx ? x + w : maxx;
		maxy = y + h > maxy ? y + h : maxy;
	}
	
	// draw panel.
	r_renderrect(
		R_BLACK50,
		minx - O_UIPANELPAD,
		miny - O_UIPANELPAD,
		maxx + O_UIPANELPAD,
		maxy + O_UIPANELPAD,
		0.0f
	);
	
	// draw UI elements.
	for (usize i = 0; i < u_nelems; ++i)
	{
		i32 x = u_elems[i].any.x, y = u_elems[i].any.y;
		i32 w = u_elems[i].any.w, h = u_elems[i].any.h;
		
		switch (u_elems[i].any.type)
		{
		case U_LABEL:
			r_rendertext(R_VCROSDMONO, u_elems[i].label.text, x, y, w, h);
			break;
		case U_BUTTON:
		{
			i32 mx, my;
			i_rectmpos(&mx, &my);
			
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				r_renderrect(
					i_mdown(SDL_BUTTON_LEFT) ? R_BLACK : R_GRAY,
					x,
					y,
					w,
					h,
					0.0f
				);
			}
			else
			{
				r_renderrect(R_BLACK50, x, y, w, h, 0.0f);
			}
			
			r_rendertext(
				R_VCROSDMONO,
				u_elems[i].button.text,
				x + O_UIBUTTONPAD,
				y + O_UIBUTTONPAD,
				w - 2 * O_UIBUTTONPAD,
				h - 2 * O_UIBUTTONPAD
			);
			
			break;
		}
		case U_HOLDBUTTON:
		{
			i32 mx, my;
			i_rectmpos(&mx, &my);
			
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				r_renderrect(
					i_mdown(SDL_BUTTON_LEFT) ? R_BLACK : R_GRAY,
					x,
					y,
					w,
					h,
					0.0f
				);
			}
			else
			{
				r_renderrect(R_BLACK50, x, y, w, h, 0.0f);
			}
			
			r_rendertext(
				R_VCROSDMONO,
				u_elems[i].button.text,
				x + O_UIBUTTONPAD,
				y + O_UIBUTTONPAD,
				w - 2 * O_UIBUTTONPAD,
				h - 2 * O_UIBUTTONPAD
			);
			
			break;
		}
		case U_SLIDER:
			r_renderrect(R_BLACK, x, y, w, h, 0.0f);
			r_renderrect(R_GRAY, x, y, *u_elems[i].slider.val * w, h, 0.0f);
			r_rendertext(
				R_VCROSDMONO,
				u_elems[i].slider.text,
				x + O_UISLIDERPAD,
				y + O_UISLIDERPAD,
				w - 2 * O_UISLIDERPAD,
				h - 2 * O_UISLIDERPAD
			);
			break;
		}
	}
}

void
u_pad(i32 dx, i32 dy)
{
	u_x += dx;
	u_y += dy;
}

void
u_label(char const *text)
{
	if (u_nelems >= O_MAXUIELEMS)
	{
		return;
	}
	
	i32 w, h;
	r_textsize(R_VCROSDMONO, text, &w, &h);
	
	u_elems[u_nelems++] = (u_elem_t)
	{
		.label =
		{
			.type = U_LABEL,
			.x = u_x,
			.y = u_y,
			.w = w,
			.h = h,
			.text = text
		}
	};
	u_y += h;
}

bool
u_button(char const *text)
{
	if (u_nelems >= O_MAXUIELEMS)
	{
		return false;
	}
	
	bool state = false;
	
	i32 w, h;
	r_textsize(R_VCROSDMONO, text, &w, &h);
	w += 2 * O_UIBUTTONPAD;
	h += 2 * O_UIBUTTONPAD;
	
	i32 mx, my;
	i_rectmpos(&mx, &my);
	
	if (i_mreleased(SDL_BUTTON_LEFT)
		&& mx >= u_x
		&& my >= u_y
		&& mx < u_x + w
		&& my < u_y + h)
	{
		state = true;
	}
	
	u_elems[u_nelems++] = (u_elem_t)
	{
		.button =
		{
			.type = U_BUTTON,
			.x = u_x,
			.y = u_y,
			.w = w,
			.h = h,
			.text = text
		}
	};
	u_y += h;
	
	return state;
}

bool
u_holdbutton(char const *text)
{
	if (u_nelems >= O_MAXUIELEMS)
	{
		return false;
	}
	
	bool state = false;
	
	i32 w, h;
	r_textsize(R_VCROSDMONO, text, &w, &h);
	w += 2 * O_UIBUTTONPAD;
	h += 2 * O_UIBUTTONPAD;
	
	i32 mx, my;
	i_rectmpos(&mx, &my);
	
	if (i_mdown(SDL_BUTTON_LEFT)
		&& mx >= u_x
		&& my >= u_y
		&& mx < u_x + w
		&& my < u_y + h)
	{
		state = true;
	}
	
	u_elems[u_nelems++] = (u_elem_t)
	{
		.holdbutton =
		{
			.type = U_HOLDBUTTON,
			.x = u_x,
			.y = u_y,
			.w = w,
			.h = h,
			.text = text
		}
	};
	u_y += h;
	
	return state;
}

bool
u_slider(char const *text, INOUT f64 *val)
{
	if (u_nelems >= O_MAXUIELEMS)
	{
		return false;
	}
	
	bool state = false;
	
	i32 w, h;
	r_textsize(R_VCROSDMONO, text, &w, &h);
	w += 2 * O_UISLIDERPAD;
	h += 2 * O_UISLIDERPAD;
	
	i32 mx, my;
	i_rectmpos(&mx, &my);
	
	if (i_mdown(SDL_BUTTON_LEFT)
		&& mx >= u_x
		&& my >= u_y
		&& mx < u_x + w
		&& my < u_y + h)
	{
		*val = (f64)(mx - u_x) / w;
		state = true;
	}
	
	*val = *val < 0.0f ? 0.0f : *val;
	*val = *val > 1.0f ? 1.0f : *val;
	
	u_elems[u_nelems++] = (u_elem_t)
	{
		.slider =
		{
			.type = U_SLIDER,
			.x = u_x,
			.y = u_y,
			.w = w,
			.h = h,
			.text = text,
			.val = val
		}
	};
	u_y += h;
	
	return state;
}
