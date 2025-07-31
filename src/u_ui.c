// SPDX-License-Identifier: GPL-3.0-or-later

typedef enum u_elemtype
{
	U_LABEL = 0,
	U_BUTTON
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
		switch (u_elems[i].any.type)
		{
		case U_LABEL:
			r_rendertext(
				R_VCROSDMONO,
				u_elems[i].button.text,
				u_elems[i].button.x,
				u_elems[i].button.y,
				u_elems[i].button.w,
				u_elems[i].button.h
			);
			break;
		case U_BUTTON:
			// TODO: implement.
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
	
	i32 w, h;
	r_textsize(R_VCROSDMONO, text, &w, &h);
	
	// TODO: implement UI button interaction.
	
	u_elems[u_nelems++] = (u_elem_t)
	{
		.button =
		{
			.x = u_x,
			.y = u_y,
			.w = w,
			.h = h,
			.text = text
		}
	};
	u_y += h;
	
	return false;
}
