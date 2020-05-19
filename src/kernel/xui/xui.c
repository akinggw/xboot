/*
 * kernel/xui/xui.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <xui/xui.h>

#define xui_push(stk, val)	do { assert((stk).idx < (int)(sizeof((stk).items) / sizeof(*(stk).items))); (stk).items[(stk).idx] = (val); (stk).idx++; } while(0)
#define xui_pop(stk)		do { assert((stk).idx > 0); (stk).idx--; } while(0)

/*
 * https://designrevision.com/demo/shards/
 */
static const struct xui_style_t xui_style_default = {
	.background_color = { 0xff, 0xff, 0xff, 0xff },

	.font = NULL,
	.width = 68,
	.height = 10,
	.padding = 5,
	.spacing = 4,
	.indent = 24,

	.colors = {
		{ 25,  25,  205,  255 }, /* XUI_COLOR_BORDER */
		{ 30,  230,  30,  255 }, /* XUI_COLOR_BASE */
		{ 235,  35,  35,  255 }, /* XUI_COLOR_BASEHOVER */
		{ 40,  140,  140,  255 },  /* XUI_COLOR_BASEFOCUS */
	},

	.window = {
		.border_radius = 6,
		.border_width = 6,
		.title_height = 24,
		.face_color = { 0xf0, 0xf0, 0xf0, 0xff },
		.border_color = { 0x00, 0x6f, 0xe6, 0xff },
		.title_color = { 0x00, 0x7b, 0xff, 0xff },
		.text_color = { 0x21, 0x25, 0x2a, 0xff },
	},

	.panel = {
		.face_color = { 0x00, 0x00, 0x00, 0x00 },
	},

	.scroll = {
		.scroll_size = 12,
		.scroll_radius = 6,
		.thumb_size = 8,
		.thumb_radius = 6,
		.scroll_color = { 0x5a, 0x61, 0x69, 0xff },
		.thumb_color = { 0x17, 0xc6, 0x71, 0xff },
	},

	.treenode = {
		.border_radius = 0,
		.border_width = 1,
		.normal = {
			.face_color = { 0xc4, 0x18, 0x3c, 0xff },
			.border_color = { 0x00, 0x00, 0x00, 0x00 },
			.text_color = { 0xa0, 0xa0, 0xa0, 0xff },
		},
		.hover = {
			.face_color = { 0xad, 0x15, 0x35, 0xff },
			.border_color = { 0x00, 0x00, 0x00, 0x00 },
			.text_color = { 0xa0, 0xa0, 0xa0, 0xff },
		},
		.focus = {
			.face_color = { 0xad, 0x15, 0x35, 0xff },
			.border_color = { 0xad, 0x15, 0x35, 0x60 },
			.text_color = { 0xa0, 0xa0, 0xa0, 0xff },
		},
	},

	.button = {
		.border_radius = 6,
		.border_width = 6,
		.outline_width = 2,
		.primary = {
			.normal = {
				.face_color = { 0x00, 0x7b, 0xff, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.hover = {
				.face_color = { 0x00, 0x6f, 0xe6, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.focus = {
				.face_color = { 0x00, 0x6f, 0xe6, 0xff },
				.border_color = { 0x00, 0x6f, 0xe6, 0x60 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
		},
		.secondary = {
			.normal = {
				.face_color = { 0x5a, 0x61, 0x69, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.hover = {
				.face_color = { 0x4e, 0x54, 0x5b, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.focus = {
				.face_color = { 0x4e, 0x54, 0x5b, 0xff },
				.border_color = { 0x4e, 0x54, 0x5b, 0x60 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
		},
		.success = {
			.normal = {
				.face_color = { 0x17, 0xc6, 0x71, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.hover = {
				.face_color = { 0x14, 0xaf, 0x64, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.focus = {
				.face_color = { 0x14, 0xaf, 0x64, 0xff },
				.border_color = { 0x14, 0xaf, 0x64, 0x60 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
		},
		.info = {
			.normal = {
				.face_color = { 0x00, 0xb8, 0xd8, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.hover = {
				.face_color = { 0x00, 0xa2, 0xbf, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.focus = {
				.face_color = { 0x00, 0xa2, 0xbf, 0xff },
				.border_color = { 0x00, 0xa2, 0xbf, 0x60 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
		},
		.warning = {
			.normal = {
				.face_color = { 0xff, 0xb4, 0x00, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.hover = {
				.face_color = { 0xe6, 0xa2, 0x00, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.focus = {
				.face_color = { 0xe6, 0xa2, 0x00, 0xff },
				.border_color = { 0xe6, 0xa2, 0x00, 0x60 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
		},
		.danger = {
			.normal = {
				.face_color = { 0xc4, 0x18, 0x3c, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.hover = {
				.face_color = { 0xad, 0x15, 0x35, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.focus = {
				.face_color = { 0xad, 0x15, 0x35, 0xff },
				.border_color = { 0xad, 0x15, 0x35, 0x60 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
		},
		.light = {
			.normal = {
				.face_color = { 0xe9, 0xec, 0xef, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0x74, 0x74, 0x74, 0xff },
			},
			.hover = {
				.face_color = { 0xda, 0xdf, 0xe4, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0x74, 0x74, 0x74, 0xff },
			},
			.focus = {
				.face_color = { 0xda, 0xdf, 0xe4, 0xff },
				.border_color = { 0xda, 0xdf, 0xe4, 0x60 },
				.text_color = { 0x74, 0x74, 0x74, 0xff },
			},
		},
		.dark = {
			.normal = {
				.face_color = { 0x21, 0x25, 0x29, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.hover = {
				.face_color = { 0x27, 0x28, 0x29, 0xff },
				.border_color = { 0x00, 0x00, 0x00, 0x00 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
			.focus = {
				.face_color = { 0x27, 0x28, 0x29, 0xff },
				.border_color = { 0x27, 0x28, 0x29, 0x60 },
				.text_color = { 0xff, 0xff, 0xff, 0xff },
			},
		},
	},

	.checkbox = {
		.border_radius = 6,
		.border_width = 2,
		.normal = {
			.face_color = { 0xc4, 0x18, 0x3c, 0xff },
			.border_color = { 0x00, 0x00, 0x00, 0x00 },
			.text_color = { 0xa0, 0xa0, 0xa0, 0xff },
		},
		.hover = {
			.face_color = { 0xad, 0x15, 0x35, 0xff },
			.border_color = { 0x00, 0x00, 0x00, 0x00 },
			.text_color = { 0xa0, 0xa0, 0xa0, 0xff },
		},
		.focus = {
			.face_color = { 0xad, 0x15, 0x35, 0xff },
			.border_color = { 0xad, 0x15, 0x35, 0x60 },
			.text_color = { 0xa0, 0xa0, 0xa0, 0xff },
		},
	},

	.text = {
		.text_color = { 0xa0, 0xa0, 0xa0, 0xff },
	},
};

static struct region_t unclipped_region = {
	.x = 0,
	.y = 0,
	.w = INT_MAX,
	.h = INT_MAX,
	.area = -1,
};

void xui_begin(struct xui_context_t * ctx)
{
	ctx->cmd_list.idx = 0;
	ctx->root_list.idx = 0;
	ctx->scroll_target = NULL;
	ctx->hover_root = ctx->next_hover_root;
	ctx->next_hover_root = NULL;
	ctx->mouse_delta_x = ctx->mouse_pos_x - ctx->last_mouse_pos_x;
	ctx->mouse_delta_y = ctx->mouse_pos_y - ctx->last_mouse_pos_y;
	ctx->frame++;
}

static int compare_zindex(const void * a, const void * b)
{
	return (*(struct xui_container_t **)a)->zindex - (*(struct xui_container_t **)b)->zindex;
}

void xui_end(struct xui_context_t * ctx)
{
	int i, n;

	assert(ctx->container_stack.idx == 0);
	assert(ctx->clip_stack.idx      == 0);
	assert(ctx->id_stack.idx        == 0);
	assert(ctx->layout_stack.idx    == 0);
	if(ctx->scroll_target)
	{
		ctx->scroll_target->scroll_x += ctx->scroll_delta_x;
		ctx->scroll_target->scroll_y += ctx->scroll_delta_y;
	}
	if(!ctx->updated_focus)
		ctx->focus = 0;
	ctx->updated_focus = 0;
	if(ctx->mouse_pressed && ctx->next_hover_root && (ctx->next_hover_root->zindex < ctx->last_zindex) && (ctx->next_hover_root->zindex >= 0))
		xui_set_front(ctx, ctx->next_hover_root);
	ctx->key_pressed = 0;
	ctx->input_text[0] = '\0';
	ctx->mouse_pressed = 0;
	ctx->scroll_delta_x = 0;
	ctx->scroll_delta_y = 0;
	ctx->last_mouse_pos_x = ctx->mouse_pos_x;
	ctx->last_mouse_pos_y = ctx->mouse_pos_y;
	n = ctx->root_list.idx;
	qsort(ctx->root_list.items, n, sizeof(struct xui_container_t *), compare_zindex);
	for(i = 0; i < n; i++)
	{
		struct xui_container_t * c = ctx->root_list.items[i];
		if(i == 0)
		{
			union xui_cmd_t * cmd = (union xui_cmd_t *)ctx->cmd_list.items;
			cmd->jump.addr = (char *)c->head + sizeof(struct xui_cmd_jump_t);
		}
		else
		{
			struct xui_container_t * prev = ctx->root_list.items[i - 1];
			prev->tail->jump.addr = (char *)c->head + sizeof(struct xui_cmd_jump_t);
		}
		if(i == n - 1)
			c->tail->jump.addr = ctx->cmd_list.items + ctx->cmd_list.idx;
	}
}

const char * xui_format(struct xui_context_t * ctx, const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(ctx->tempbuf, sizeof(ctx->tempbuf), fmt, ap);
	va_end(ap);
	return (const char *)ctx->tempbuf;
}

void xui_set_front(struct xui_context_t * ctx, struct xui_container_t * c)
{
	c->zindex = ++ctx->last_zindex;
}

void xui_set_focus(struct xui_context_t * ctx, unsigned int id)
{
	ctx->focus = id;
	ctx->updated_focus = 1;
}

static void xui_hash(unsigned int * hash, const void * data, int size)
{
	const unsigned char * p = data;
	while(size--)
		*hash = (*hash ^ *p++) * 16777619;
}

unsigned int xui_get_id(struct xui_context_t * ctx, const void * data, int size)
{
	int idx = ctx->id_stack.idx;
	unsigned int res = (idx > 0) ? ctx->id_stack.items[idx - 1] : 2166136261;
	xui_hash(&res, data, size);
	ctx->last_id = res;
	return res;
}

void xui_push_id(struct xui_context_t * ctx, const void * data, int size)
{
	xui_push(ctx->id_stack, xui_get_id(ctx, data, size));
}

void xui_pop_id(struct xui_context_t * ctx)
{
	xui_pop(ctx->id_stack);
}

struct region_t * xui_get_clip(struct xui_context_t * ctx)
{
	assert(ctx->clip_stack.idx > 0);
	return &ctx->clip_stack.items[ctx->clip_stack.idx - 1];
}

void xui_push_clip(struct xui_context_t * ctx, struct region_t * r)
{
	struct region_t region;
	if(!region_intersect(&region, r, xui_get_clip(ctx)))
		region_init(&region, 0, 0, 0, 0);
	xui_push(ctx->clip_stack, region);
}

void xui_pop_clip(struct xui_context_t * ctx)
{
	xui_pop(ctx->clip_stack);
}

int xui_check_clip(struct xui_context_t * ctx, struct region_t * r)
{
	struct region_t * cr = xui_get_clip(ctx);

	if((r->w <= 0) || (r->h <= 0) || (r->x > cr->x + cr->w) || (r->x + r->w < cr->x) || (r->y > cr->y + cr->h) || (r->y + r->h < cr->y))
		return 0;
	else if((r->x >= cr->x) && (r->x + r->w <= cr->x + cr->w) && (r->y >= cr->y) && (r->y + r->h <= cr->y + cr->h))
		return 1;
	else
		return -1;
}

static int xui_pool_init(struct xui_context_t * ctx, struct xui_pool_item_t * items, int len, unsigned int id)
{
	int i, n = -1, f = ctx->frame;
	for(i = 0; i < len; i++)
	{
		if(items[i].last_update < f)
		{
			f = items[i].last_update;
			n = i;
		}
	}
	assert(n > -1);
	items[n].id = id;
	items[n].last_update = ctx->frame;
	return n;
}

static int xui_pool_get(struct xui_context_t * ctx, struct xui_pool_item_t * items, int len, unsigned int id)
{
	int i;
	for(i = 0; i < len; i++)
	{
		if(items[i].id == id)
			return i;
	}
	return -1;
}

static void xui_pool_update(struct xui_context_t * ctx, struct xui_pool_item_t * items, int idx)
{
	items[idx].last_update = ctx->frame;
}

static void push_layout(struct xui_context_t * ctx, struct region_t * body, int scrollx, int scrolly)
{
	struct xui_layout_t layout;
	int width = 0;
	memset(&layout, 0, sizeof(layout));
	region_init(&layout.body, body->x - scrollx, body->y - scrolly, body->w, body->h);
	layout.max_width = INT_MIN;
	layout.max_height = INT_MIN;
	xui_push(ctx->layout_stack, layout);
	xui_layout_row(ctx, 1, &width, 0);
}

static struct xui_layout_t * get_layout(struct xui_context_t * ctx)
{
	return &ctx->layout_stack.items[ctx->layout_stack.idx - 1];
}

static void pop_container(struct xui_context_t * ctx)
{
	struct xui_container_t * c = xui_get_current_container(ctx);
	struct xui_layout_t * layout = get_layout(ctx);
	c->content_width = layout->max_width - layout->body.x;
	c->content_height = layout->max_height - layout->body.y;
	xui_pop(ctx->container_stack);
	xui_pop(ctx->layout_stack);
	xui_pop_id(ctx);
}

static struct xui_container_t * get_container(struct xui_context_t * ctx, unsigned int id, int opt)
{
	struct xui_container_t * c;
	int idx = xui_pool_get(ctx, ctx->container_pool, XUI_CONTAINER_POOL_SIZE, id);
	if(idx >= 0)
	{
		if(ctx->containers[idx].open || (~opt & XUI_OPT_CLOSED))
			xui_pool_update(ctx, ctx->container_pool, idx);
		return &ctx->containers[idx];
	}
	if(opt & XUI_OPT_CLOSED)
		return NULL;
	idx = xui_pool_init(ctx, ctx->container_pool, XUI_CONTAINER_POOL_SIZE, id);
	c = &ctx->containers[idx];
	memset(c, 0, sizeof(struct xui_container_t));
	c->open = 1;
	xui_set_front(ctx, c);
	return c;
}

struct xui_container_t * xui_get_container(struct xui_context_t * ctx, const char * name)
{
	unsigned int id = xui_get_id(ctx, name, strlen(name));
	return get_container(ctx, id, 0);
}

struct xui_container_t * xui_get_current_container(struct xui_context_t * ctx)
{
	assert(ctx->container_stack.idx > 0);
	return ctx->container_stack.items[ctx->container_stack.idx - 1];
}

void xui_layout_width(struct xui_context_t * ctx, int width)
{
	get_layout(ctx)->size_width = width;
}

void xui_layout_height(struct xui_context_t * ctx, int height)
{
	get_layout(ctx)->size_height = height;
}

void xui_layout_row(struct xui_context_t * ctx, int items, const int * widths, int height)
{
	struct xui_layout_t * layout = get_layout(ctx);
	if(widths)
	{
		assert(items <= XUI_MAX_WIDTHS);
		memcpy(layout->widths, widths, items * sizeof(widths[0]));
	}
	layout->items = items;
	layout->position_x = layout->indent;
	layout->position_y = layout->next_row;
	layout->size_height = height;
	layout->item_index = 0;
}

void xui_layout_begin_column(struct xui_context_t * ctx)
{
	push_layout(ctx, xui_layout_next(ctx), 0, 0);
}

void xui_layout_end_column(struct xui_context_t * ctx)
{
	struct xui_layout_t * a, * b;
	b = get_layout(ctx);
	xui_pop(ctx->layout_stack);
	a = get_layout(ctx);
	a->position_x = max(a->position_x, b->position_x + b->body.x - a->body.x);
	a->next_row = max(a->next_row, b->next_row + b->body.y - a->body.y);
	a->max_width = max(a->max_width, b->max_width);
	a->max_height = max(a->max_height, b->max_height);
}

void xui_layout_set_next(struct xui_context_t * ctx, struct region_t * r, int relative)
{
	struct xui_layout_t * layout = get_layout(ctx);
	region_clone(&layout->next, r);
	layout->next_type = relative ? 1 : 2;
}

struct region_t * xui_layout_next(struct xui_context_t * ctx)
{
	struct xui_layout_t * layout = get_layout(ctx);
	struct xui_style_t * style = &ctx->style;
	struct region_t r;

	if(layout->next_type)
	{
		int type = layout->next_type;
		layout->next_type = 0;
		region_clone(&r, &layout->next);
		if(type == 2)
		{
			region_clone(&ctx->last_rect, &r);
			return &ctx->last_rect;
		}
	}
	else
	{
		if(layout->item_index == layout->items)
			xui_layout_row(ctx, layout->items, NULL, layout->size_height);
		r.x = layout->position_x;
		r.y = layout->position_y;
		r.w = layout->items > 0 ? layout->widths[layout->item_index] : layout->size_width;
		r.h = layout->size_height;
		if(r.w == 0)
			r.w = style->width + style->padding * 2;
		if(r.h == 0)
			r.h = style->height + style->padding * 2;
		if(r.w < 0)
			r.w += layout->body.w - r.x + 1;
		if(r.h < 0)
			r.h += layout->body.h - r.y + 1;
		layout->item_index++;
	}

	layout->position_x += r.w + style->spacing;
	layout->next_row = max(layout->next_row, r.y + r.h + style->spacing);
	r.x += layout->body.x;
	r.y += layout->body.y;
	layout->max_width = max(layout->max_width, r.x + r.w);
	layout->max_height = max(layout->max_height, r.y + r.h);

	region_clone(&ctx->last_rect, &r);
	return &ctx->last_rect;
}

static int xui_cmd_next(struct xui_context_t * ctx, union xui_cmd_t ** cmd)
{
	if(*cmd)
		*cmd = (union xui_cmd_t *)(((char *)*cmd) + (*cmd)->base.size);
	else
		*cmd = (union xui_cmd_t *)ctx->cmd_list.items;
	while((char *)(*cmd) != ctx->cmd_list.items + ctx->cmd_list.idx)
	{
		if((*cmd)->base.type != XUI_CMD_TYPE_JUMP)
			return 1;
		*cmd = (*cmd)->jump.addr;
	}
	return 0;
}

static union xui_cmd_t * xui_cmd_push(struct xui_context_t * ctx, enum xui_cmd_type_t type, int size)
{
	union xui_cmd_t * cmd = (union xui_cmd_t *)(ctx->cmd_list.items + ctx->cmd_list.idx);
	assert(ctx->cmd_list.idx + size < XUI_COMMAND_LIST_SIZE);
	cmd->base.type = type;
	cmd->base.size = size;
	ctx->cmd_list.idx += size;
	return cmd;
}

static union xui_cmd_t * xui_cmd_push_jump(struct xui_context_t * ctx, union xui_cmd_t * addr)
{
	union xui_cmd_t * cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_JUMP, sizeof(struct xui_cmd_jump_t));
	cmd->jump.addr = addr;
	return cmd;
}

static union xui_cmd_t * xui_cmd_push_clip(struct xui_context_t * ctx, struct region_t * r)
{
	union xui_cmd_t * cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_CLIP, sizeof(struct xui_cmd_clip_t));
	region_clone(&cmd->clip.r, r);
	return cmd;
}

void xui_draw_triangle(struct xui_context_t * ctx, struct point_t * p0, struct point_t * p1, struct point_t * p2, int thickness, struct color_t * c)
{
}

void xui_draw_rectangle(struct xui_context_t * ctx, int x, int y, int w, int h, int radius, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x, y, w, h);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_RECTANGLE, sizeof(struct xui_cmd_rectangle_t));
		cmd->rectangle.x = x;
		cmd->rectangle.y = y;
		cmd->rectangle.w = w;
		cmd->rectangle.h = h;
		cmd->rectangle.radius = radius;
		cmd->rectangle.thickness = thickness;
		memcpy(&cmd->rectangle.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unclipped_region);
	}
}

void xui_draw_text(struct xui_context_t * ctx, void * font, const char * txt, int len, int x, int y, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x, y, ctx->text_width(font, txt, len), ctx->text_height(font));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		if(len < 0)
			len = strlen(txt);
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_TEXT, sizeof(struct xui_cmd_text_t) + len);
		cmd->text.font = font;
		cmd->text.x = x;
		cmd->text.y = y;
		memcpy(&cmd->text.c, c, sizeof(struct color_t));
		memcpy(cmd->text.utf8, txt, len);
		cmd->text.utf8[len] = 0;
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unclipped_region);
	}
}

void xui_draw_icon(struct xui_context_t * ctx, int id, struct region_t * r, struct color_t * c)
{
	union xui_cmd_t * cmd;
	int clip;

	if((clip = xui_check_clip(ctx, r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_ICON, sizeof(struct xui_cmd_icon_t));
		cmd->icon.id = id;
		cmd->icon.x = r->x;
		cmd->icon.y = r->y;
		cmd->icon.w = r->w;
		cmd->icon.h = r->h;
		memcpy(&cmd->icon.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unclipped_region);
	}
}

static int in_hover_root(struct xui_context_t * ctx)
{
	int i = ctx->container_stack.idx;
	while(i--)
	{
		if(ctx->container_stack.items[i] == ctx->hover_root)
			return 1;
		if(ctx->container_stack.items[i]->head)
			break;
	}
	return 0;
}

static int xui_mouse_over(struct xui_context_t * ctx, struct region_t * r)
{
	return region_hit(r, ctx->mouse_pos_x, ctx->mouse_pos_y) && region_hit(xui_get_clip(ctx), ctx->mouse_pos_x, ctx->mouse_pos_y) && in_hover_root(ctx);
}

void xui_control_update(struct xui_context_t * ctx, unsigned int id, struct region_t * r, int opt)
{
	int over = xui_mouse_over(ctx, r);

	if(ctx->focus == id)
		ctx->updated_focus = 1;
	if(opt & XUI_OPT_NOINTERACT)
		return;
	if(over && !ctx->mouse_down)
		ctx->hover = id;
	if(ctx->focus == id)
	{
		if(ctx->mouse_pressed && !over)
			xui_set_focus(ctx, 0);
		if(!ctx->mouse_down && (~opt & XUI_OPT_HOLDFOCUS))
			xui_set_focus(ctx, 0);
	}
	if(ctx->hover == id)
	{
		if(ctx->mouse_pressed)
			xui_set_focus(ctx, id);
		else if(!over)
			ctx->hover = 0;
	}
}

void xui_control_draw_text(struct xui_context_t * ctx, const char * txt, struct region_t * r, struct color_t * c, int opt)
{
	void * font = ctx->style.font;
	int tw = ctx->text_width(font, txt, -1);
	int th = ctx->text_height(font);
	int x, y;
	xui_push_clip(ctx, r);
	switch(opt & (0x7 << 5))
	{
	case XUI_OPT_TEXT_LEFT:
		x = r->x + ctx->style.padding;
		y = r->y + (r->h - th) / 2;
		break;
	case XUI_OPT_TEXT_RIGHT:
		x = r->x + r->w - tw - ctx->style.padding;
		y = r->y + (r->h - th) / 2;
		break;
	case XUI_OPT_TEXT_TOP:
		x = r->x + (r->w - tw) / 2;
		y = r->y + ctx->style.padding;
		break;
	case XUI_OPT_TEXT_BOTTOM:
		x = r->x + (r->w - tw) / 2;
		y = r->y + r->h - th - ctx->style.padding;
		break;
	case XUI_OPT_TEXT_CENTER:
		x = r->x + (r->w - tw) / 2;
		y = r->y + (r->h - th) / 2;
		break;
	default:
		x = r->x + ctx->style.padding;
		y = r->y + (r->h - th) / 2;
		break;
	}
	xui_draw_text(ctx, font, txt, -1, x, y, c);
	xui_pop_clip(ctx);
}

void xui_control_draw_frame(struct xui_context_t * ctx, unsigned int id, struct region_t * r, int cid, int opt)
{
	cid += (ctx->focus == id) ? 2 : (ctx->hover == id) ? 1 : 0;
	ctx->draw_frame(ctx, r, cid);
}

static void scrollbars(struct xui_context_t * ctx, struct xui_container_t * c, struct region_t * body)
{
	struct region_t base, thumb;
	int sz = ctx->style.scroll.scroll_size;
	int width = c->content_width;
	int height = c->content_height;
	int maxscroll;
	unsigned int id;

	width += ctx->style.padding * 2;
	height += ctx->style.padding * 2;
	xui_push_clip(ctx, body);
	if(height > c->body.h)
		body->w -= sz;
	if(width > c->body.w)
		body->h -= sz;

	maxscroll = height - body->h;
	if(maxscroll > 0 && body->h > 0)
	{
		id = xui_get_id(ctx, "!scrollbary", 11);
		region_clone(&base, body);
		base.x = body->x + body->w;
		base.w = ctx->style.scroll.scroll_size;
		xui_control_update(ctx, id, &base, 0);
		if((ctx->focus == id) && (ctx->mouse_down & XUI_MOUSE_LEFT))
			c->scroll_y += ctx->mouse_delta_y * height / base.h;
		c->scroll_y = clamp(c->scroll_y, 0, maxscroll);
		xui_draw_rectangle(ctx, base.x, base.y, base.w, base.h, ctx->style.scroll.scroll_radius, 0, &ctx->style.scroll.scroll_color);
		region_clone(&thumb, &base);
		thumb.h = max(ctx->style.scroll.thumb_size, base.h * body->h / height);
		thumb.y += c->scroll_y * (base.h - thumb.h) / maxscroll;
		xui_draw_rectangle(ctx, thumb.x, thumb.y, thumb.w, thumb.h, ctx->style.scroll.thumb_radius, 0, &ctx->style.scroll.thumb_color);
		if(xui_mouse_over(ctx, body))
			ctx->scroll_target = c;
	}
	else
	{
		c->scroll_y = 0;
	}

	maxscroll = width - body->w;
	if(maxscroll > 0 && body->w > 0)
	{
		id = xui_get_id(ctx, "!scrollbarx", 11);
		region_clone(&base, body);
		base.y = body->y + body->h;
		base.h = ctx->style.scroll.scroll_size;
		xui_control_update(ctx, id, &base, 0);
		if((ctx->focus == id) && (ctx->mouse_down & XUI_MOUSE_LEFT))
			c->scroll_x += ctx->mouse_delta_x * width / base.w;
		c->scroll_x = clamp(c->scroll_x, 0, maxscroll);
		xui_draw_rectangle(ctx, base.x, base.y, base.w, base.h, ctx->style.scroll.scroll_radius, 0, &ctx->style.scroll.scroll_color);
		region_clone(&thumb, &base);
		thumb.w = max(ctx->style.scroll.thumb_size, base.w * body->w / width);
		thumb.x += c->scroll_x * (base.w - thumb.w) / maxscroll;
		xui_draw_rectangle(ctx, thumb.x, thumb.y, thumb.w, thumb.h, ctx->style.scroll.thumb_radius, 0, &ctx->style.scroll.thumb_color);
		if(xui_mouse_over(ctx, body))
			ctx->scroll_target = c;
	}
	else
	{
		c->scroll_x = 0;
	}
	xui_pop_clip(ctx);
}

static void push_container_body(struct xui_context_t * ctx, struct xui_container_t * c, struct region_t * body, int opt)
{
	struct region_t r;
	if(~opt & XUI_OPT_NOSCROLL)
		scrollbars(ctx, c, body);
	region_expand(&r, body, -ctx->style.padding);
	push_layout(ctx, &r, c->scroll_x, c->scroll_y);
	region_clone(&c->body, body);
}

static void begin_root_container(struct xui_context_t * ctx, struct xui_container_t * c)
{
	xui_push(ctx->container_stack, c);
	xui_push(ctx->root_list, c);
	c->head = xui_cmd_push_jump(ctx, NULL);
	if(region_hit(&c->region, ctx->mouse_pos_x, ctx->mouse_pos_y) && (!ctx->next_hover_root || c->zindex > ctx->next_hover_root->zindex))
		ctx->next_hover_root = c;
	xui_push(ctx->clip_stack, unclipped_region);
}

static void end_root_container(struct xui_context_t * ctx)
{
	struct xui_container_t * c = xui_get_current_container(ctx);
	c->tail = xui_cmd_push_jump(ctx, NULL);
	c->head->jump.addr = ctx->cmd_list.items + ctx->cmd_list.idx;
	xui_pop_clip(ctx);
	pop_container(ctx);
}

int xui_begin_window_ex(struct xui_context_t * ctx, const char * title, struct region_t * r, int opt)
{
	unsigned int id = title ? xui_get_id(ctx, title, strlen(title)) : xui_get_id(ctx, &title, sizeof(title));
	struct xui_container_t * c = get_container(ctx, id, opt);
	struct region_t body, region, hr, tr;

	if(c && c->open)
	{
		xui_push(ctx->id_stack, id);
		if(c->region.w == 0)
			region_clone(&c->region, r ? r : &ctx->screen);
		begin_root_container(ctx, c);
		region_clone(&body, &c->region);
		region_clone(&region, &c->region);
		if(ctx->style.window.border_color.a && (ctx->style.window.border_width > 0))
			xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, ctx->style.window.border_radius, ctx->style.window.border_width, &ctx->style.window.border_color);
		xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, ctx->style.window.border_radius, 0, &ctx->style.window.face_color);
		if(~opt & XUI_WINDOW_NOTITLE)
		{
			region_clone(&hr, &region);
			hr.h = ctx->style.window.title_height;
			xui_draw_rectangle(ctx, hr.x, hr.y, hr.w, hr.h, (0xc << 16) | ctx->style.window.border_radius, 0, &ctx->style.window.title_color);
			id = xui_get_id(ctx, "!title", 6);
			xui_control_update(ctx, id, &hr, opt);
			xui_control_draw_text(ctx, title, &hr, &ctx->style.window.text_color, opt);
			if((id == ctx->focus) && (ctx->mouse_down & XUI_MOUSE_LEFT))
			{
				c->region.x += ctx->mouse_delta_x;
				c->region.y += ctx->mouse_delta_y;
			}
			body.y += hr.h;
			body.h -= hr.h;
			if(~opt & XUI_WINDOW_NOCLOSE)
			{
				id = xui_get_id(ctx, "!close", 6);
				region_init(&tr, hr.x + hr.w - hr.h, hr.y, hr.h, hr.h);
				hr.w -= tr.w;
				xui_draw_icon(ctx, XUI_ICON_CLOSE, &tr, &ctx->style.window.text_color);
				xui_control_update(ctx, id, &tr, opt);
				if((ctx->mouse_pressed & XUI_MOUSE_LEFT) && (id == ctx->focus))
					c->open = 0;
			}
		}
		push_container_body(ctx, c, &body, opt);
		if(~opt & XUI_WINDOW_NORESIZE)
		{
			int sz = ctx->style.window.title_height;
			id = xui_get_id(ctx, "!resize", 7);
			region_init(&tr, region.x + region.w - sz, region.y + region.h - sz, sz, sz);
			xui_control_update(ctx, id, &tr, opt);
			if((id == ctx->focus) && (ctx->mouse_down & XUI_MOUSE_LEFT))
			{
				c->region.w = max(96, c->region.w + ctx->mouse_delta_x);
				c->region.h = max(64, c->region.h + ctx->mouse_delta_y);
			}
		}
		if(opt & XUI_WINDOW_AUTOSIZE)
		{
			struct region_t * pr = &get_layout(ctx)->body;
			c->region.w = c->content_width + (c->region.w - pr->w);
			c->region.h = c->content_height + (c->region.h - pr->h);
		}
		if((opt & XUI_WINDOW_POPUP) && ctx->mouse_pressed && (ctx->hover_root != c))
			c->open = 0;
		xui_push_clip(ctx, &c->body);
		return 1;
	}
	return 0;
}

int xui_begin_window(struct xui_context_t * ctx, const char * title, struct region_t * r)
{
	return xui_begin_window_ex(ctx, title, r, XUI_OPT_TEXT_LEFT);
}

void xui_end_window(struct xui_context_t * ctx)
{
	xui_pop_clip(ctx);
	end_root_container(ctx);
}

int xui_begin_popup(struct xui_context_t * ctx, const char * name)
{
	return xui_begin_window_ex(ctx, name, NULL, XUI_WINDOW_POPUP | XUI_WINDOW_AUTOSIZE | XUI_WINDOW_NORESIZE | XUI_WINDOW_NOTITLE | XUI_OPT_NOSCROLL | XUI_OPT_CLOSED);
}

void xui_end_popup(struct xui_context_t * ctx)
{
	xui_end_window(ctx);
}

void xui_open_popup(struct xui_context_t * ctx, const char * name)
{
	struct xui_container_t * c = xui_get_container(ctx, name);
	ctx->hover_root = ctx->next_hover_root = c;
	region_init(&c->region, ctx->mouse_pos_x, ctx->mouse_pos_y, 1, 1);
	c->open = 1;
	xui_set_front(ctx, c);
}

void xui_begin_panel_ex(struct xui_context_t * ctx, const char * name, int opt)
{
	struct xui_container_t * c;
	struct region_t * r;
	xui_push_id(ctx, name, strlen(name));
	c = get_container(ctx, ctx->last_id, opt);
	r = xui_layout_next(ctx);
	region_clone(&c->region, r);
	if(ctx->style.panel.face_color.a)
		xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, &ctx->style.panel.face_color);
	xui_push(ctx->container_stack, c);
	push_container_body(ctx, c, &c->region, opt);
	xui_push_clip(ctx, &c->body);
}

void xui_begin_panel(struct xui_context_t * ctx, const char * name)
{
	xui_begin_panel_ex(ctx, name, 0);
}

void xui_end_panel(struct xui_context_t * ctx)
{
	xui_pop_clip(ctx);
	pop_container(ctx);
}

static int header(struct xui_context_t * ctx, const char * label, int istreenode, int opt)
{
	unsigned int id = xui_get_id(ctx, label, strlen(label));
	int idx = xui_pool_get(ctx, ctx->treenode_pool, XUI_TREENODE_POOL_SIZE, id);
	int active, expanded;
	struct region_t region, r;
	struct color_t * fc, * bc, * tc;
	int radius, width;

	xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
	active = (idx >= 0);
	expanded = (opt & XUI_OPT_EXPANDED) ? !active : active;
	region_clone(&r, xui_layout_next(ctx));
	xui_control_update(ctx, id, &r, 0);
	active ^= ((ctx->mouse_pressed & XUI_MOUSE_LEFT) && (ctx->focus == id));
	if(idx >= 0)
	{
		if(active)
			xui_pool_update(ctx, ctx->treenode_pool, idx);
		else
			memset(&ctx->treenode_pool[idx], 0, sizeof(struct xui_pool_item_t));
	}
	else if(active)
	{
		xui_pool_init(ctx, ctx->treenode_pool, XUI_TREENODE_POOL_SIZE, id);
	}
	radius = ctx->style.treenode.border_radius;
	width = ctx->style.treenode.border_width;
	if(ctx->focus == id)
	{
		fc = &ctx->style.treenode.focus.face_color;
		bc = &ctx->style.treenode.focus.border_color;
		tc = &ctx->style.treenode.focus.text_color;
	}
	else if(ctx->hover == id)
	{
		fc = &ctx->style.treenode.hover.face_color;
		bc = &ctx->style.treenode.hover.border_color;
		tc = &ctx->style.treenode.hover.text_color;
	}
	else
	{
		fc = &ctx->style.treenode.normal.face_color;
		bc = &ctx->style.treenode.normal.border_color;
		tc = &ctx->style.treenode.normal.text_color;
	}
	if(istreenode)
	{
		if(ctx->hover == id)
		{
			if(bc->a && (width > 0))
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, width, bc);
			if(fc->a)
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, 0, fc);
		}
	}
	else
	{
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, 0, fc);
	}
	region_init(&region, r.x, r.y, r.h, r.h);
	xui_draw_icon(ctx, expanded ? XUI_ICON_EXPANDED : XUI_ICON_COLLAPSED, &region, tc);
	r.x += r.h - ctx->style.padding;
	r.w -= r.h - ctx->style.padding;
	if(label && tc->a)
		xui_control_draw_text(ctx, label, &r, tc, opt);

	return expanded ? 1 : 0;
}

int xui_begin_treenode_ex(struct xui_context_t * ctx, const char * label, int opt)
{
	int res = header(ctx, label, 1, opt);
	if(res)
	{
		get_layout(ctx)->indent += ctx->style.indent;
		xui_push(ctx->id_stack, ctx->last_id);
	}
	return res;
}

int xui_begin_treenode(struct xui_context_t * ctx, const char * label)
{
	return xui_begin_treenode_ex(ctx, label, 0);
}

void xui_end_treenode(struct xui_context_t * ctx)
{
	get_layout(ctx)->indent -= ctx->style.indent;
	xui_pop_id(ctx);
}

int xui_header_ex(struct xui_context_t * ctx, const char * label, int opt)
{
	return header(ctx, label, 0, opt);
}

int xui_header(struct xui_context_t * ctx, const char * label)
{
	return header(ctx, label, 0, 0);
}

int xui_button_ex(struct xui_context_t * ctx, const char * label, int opt)
{
	unsigned int id = label ? xui_get_id(ctx, label, strlen(label)) : xui_get_id(ctx, &label, sizeof(label));
	struct region_t * r = xui_layout_next(ctx);
	struct xui_style_button_t * sb;
	struct color_t * fc, * bc, * tc;
	int radius, width;
	int res = 0;

	xui_control_update(ctx, id, r, opt);
	if((ctx->mouse_pressed & XUI_MOUSE_LEFT) && (ctx->focus == id))
		res |= XUI_RES_SUBMIT;
	radius = ctx->style.button.border_radius;
	width = ctx->style.button.border_width;
	switch(opt & (0x7 << 8))
	{
	case XUI_BUTTON_PRIMARY:
		sb = &ctx->style.button.primary;
		break;
	case XUI_BUTTON_SECONDARY:
		sb = &ctx->style.button.secondary;
		break;
	case XUI_BUTTON_SUCCESS:
		sb = &ctx->style.button.success;
		break;
	case XUI_BUTTON_INFO:
		sb = &ctx->style.button.info;
		break;
	case XUI_BUTTON_WARNING:
		sb = &ctx->style.button.warning;
		break;
	case XUI_BUTTON_DANGER:
		sb = &ctx->style.button.danger;
		break;
	case XUI_BUTTON_LIGHT:
		sb = &ctx->style.button.light;
		break;
	case XUI_BUTTON_DARK:
		sb = &ctx->style.button.dark;
		break;
	default:
		sb = &ctx->style.button.primary;
		break;
	}
	if(ctx->focus == id)
	{
		fc = &sb->focus.face_color;
		bc = &sb->focus.border_color;
		tc = &sb->focus.text_color;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
		if(label && tc->a)
			xui_control_draw_text(ctx, label, r, tc, opt);
	}
	else if(ctx->hover == id)
	{
		fc = &sb->hover.face_color;
		bc = &sb->hover.border_color;
		tc = &sb->hover.text_color;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
		if(label && tc->a)
			xui_control_draw_text(ctx, label, r, tc, opt);
	}
	else
	{
		fc = &sb->normal.face_color;
		bc = &sb->normal.border_color;
		tc = &sb->normal.text_color;
		if(opt & XUI_BUTTON_OUTLINE)
		{
			if(fc->a)
			{
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, ctx->style.button.outline_width, fc);
				if(label)
					xui_control_draw_text(ctx, label, r, fc, opt);
			}
		}
		else
		{
			if(bc->a && (width > 0))
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
			if(fc->a)
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
			if(label && tc->a)
				xui_control_draw_text(ctx, label, r, tc, opt);
		}
	}
	return res;
}

int xui_button(struct xui_context_t * ctx, const char * label)
{
	return xui_button_ex(ctx, label, XUI_BUTTON_PRIMARY | XUI_OPT_TEXT_CENTER);
}

int xui_checkbox(struct xui_context_t * ctx, const char * label, int * state)
{
	unsigned int id = xui_get_id(ctx, &state, sizeof(state));
	int res = 0;
	struct region_t r, box, region;
	struct color_t * fc, * bc, * tc;
	int radius, width;

	region_clone(&r, xui_layout_next(ctx));
	xui_control_update(ctx, id, &r, 0);
	if((ctx->mouse_pressed & XUI_MOUSE_LEFT) && (ctx->focus == id))
	{
		*state = !*state;
		res = 1;
	}
	radius = ctx->style.checkbox.border_radius;
	width = ctx->style.checkbox.border_width;
	if(ctx->focus == id)
	{
		fc = &ctx->style.checkbox.focus.face_color;
		bc = &ctx->style.checkbox.focus.border_color;
		tc = &ctx->style.checkbox.focus.text_color;
	}
	else if(ctx->hover == id)
	{
		fc = &ctx->style.checkbox.hover.face_color;
		bc = &ctx->style.checkbox.hover.border_color;
		tc = &ctx->style.checkbox.hover.text_color;
	}
	else
	{
		fc = &ctx->style.checkbox.normal.face_color;
		bc = &ctx->style.checkbox.normal.border_color;
		tc = &ctx->style.checkbox.normal.text_color;
	}
	region_init(&box, r.x, r.y, r.h, r.h);
	region_init(&region, r.x + box.w, r.y, r.w - box.w, r.h);
	if(bc->a && (width > 0))
		xui_draw_rectangle(ctx, box.x, box.y, box.h, box.h, radius, width, bc);
	if(fc->a)
		xui_draw_rectangle(ctx, box.x, box.y, box.h, box.h, radius, 0, fc);
	if(*state)
		xui_draw_icon(ctx, XUI_ICON_CHECK, &box, &ctx->style.text.text_color);
	if(label && tc->a)
		xui_control_draw_text(ctx, label, &region, tc, XUI_OPT_TEXT_LEFT);
	return res;
}

void xui_label(struct xui_context_t * ctx, const char * txt)
{
	xui_control_draw_text(ctx, txt, xui_layout_next(ctx), &ctx->style.text.text_color, 0);
}

void xui_text(struct xui_context_t * ctx, const char * txt)
{
	const char * start, * end, * p = txt;
	int width = -1;
	void * font = ctx->style.font;
	struct color_t * c = &ctx->style.text.text_color;
	xui_layout_begin_column(ctx);
	xui_layout_row(ctx, 1, &width, ctx->text_height(font));
	do {
		struct region_t r;
		region_clone(&r, xui_layout_next(ctx));
		int w = 0;
		start = end = p;
		do
		{
			const char * word = p;
			while(*p && (*p != ' ') && (*p != '\n'))
				p++;
			w += ctx->text_width(font, word, p - word);
			if((w > r.w) && (end != start))
				break;
			w += ctx->text_width(font, p, 1);
			end = p++;
		} while(*end && (*end != '\n'));
		xui_draw_text(ctx, font, start, end - start, r.x, r.y, c);
		p = end + 1;
	} while(*end);
	xui_layout_end_column(ctx);
}

int xui_textbox_raw(struct xui_context_t * ctx, char * buf, int bufsz, unsigned int id, struct region_t * r, int opt)
{
	int res = 0;
	xui_control_update(ctx, id, r, opt | XUI_OPT_HOLDFOCUS);

	if(ctx->focus == id)
	{
		int len = strlen(buf);
		int n = min(bufsz - len - 1, (int ) strlen(ctx->input_text));
		if(n > 0)
		{
			memcpy(buf + len, ctx->input_text, n);
			len += n;
			buf[len] = '\0';
			res |= XUI_RES_CHANGE;
		}
		if((ctx->key_pressed & XUI_KEY_BACK) && len > 0)
		{
			while((buf[--len] & 0xc0) == 0x80 && len > 0)
				;
			buf[len] = '\0';
			res |= XUI_RES_CHANGE;
		}
		if(ctx->key_pressed & XUI_KEY_ENTER)
		{
			xui_set_focus(ctx, 0);
			res |= XUI_RES_SUBMIT;
		}
	}
	xui_control_draw_frame(ctx, id, r, XUI_COLOR_BASE, opt);
	if(ctx->focus == id)
	{
		struct color_t * c = &ctx->style.text.text_color;
		void * font = ctx->style.font;
		int textw = ctx->text_width(font, buf, -1);
		int texth = ctx->text_height(font);
		int ofx = r->w - ctx->style.padding - textw - 1;
		int textx = r->x + min(ofx, ctx->style.padding);
		int texty = r->y + (r->h - texth) / 2;
		xui_push_clip(ctx, r);
		xui_draw_text(ctx, font, buf, -1, textx, texty, c);
		xui_draw_rectangle(ctx, textx + textw, texty, 1, texth, 0, 0, c);
		xui_pop_clip(ctx);
	}
	else
	{
		xui_control_draw_text(ctx, buf, r, &ctx->style.text.text_color, opt);
	}
	return res;
}

static int number_textbox(struct xui_context_t * ctx, float * value, struct region_t * r, unsigned int id)
{
	if((ctx->mouse_pressed & XUI_MOUSE_LEFT) && (ctx->key_down & XUI_KEY_SHIFT) && (ctx->hover == id))
	{
		ctx->number_edit = id;
		sprintf(ctx->number_edit_buf, "%.3g", *value);
	}
	if(ctx->number_edit == id)
	{
		int res = xui_textbox_raw(ctx, ctx->number_edit_buf, sizeof(ctx->number_edit_buf), id, r, 0);
		if((res & XUI_RES_SUBMIT) || ctx->focus != id)
		{
			*value = strtod(ctx->number_edit_buf, NULL);
			ctx->number_edit = 0;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

int xui_textbox_ex(struct xui_context_t * ctx, char * buf, int bufsz, int opt)
{
	unsigned int id = xui_get_id(ctx, &buf, sizeof(buf));
	struct region_t r;
	region_clone(&r, xui_layout_next(ctx));
	return xui_textbox_raw(ctx, buf, bufsz, id, &r, opt);
}

int xui_textbox(struct xui_context_t * ctx, char * buf, int bufsz)
{
	return xui_textbox_ex(ctx, buf, bufsz, 0);
}

int xui_slider_ex(struct xui_context_t * ctx, float * value, float low, float high, float step, const char * fmt, int opt)
{
	char buf[128];
	struct region_t thumb;
	int x, w, res = 0;
	float last = *value, v = last;
	unsigned int id = xui_get_id(ctx, &value, sizeof(value));
	struct region_t base;
	region_clone(&base, xui_layout_next(ctx));

	if(number_textbox(ctx, &v, &base, id))
		return res;
	xui_control_update(ctx, id, &base, opt);
	if((ctx->focus == id) && (ctx->mouse_down & XUI_MOUSE_LEFT))
	{
		v = low + (ctx->mouse_pos_x - base.x) * (high - low) / base.w;
		if(step)
			v = (((v + step / 2) / step)) * step;
	}
	*value = v = clamp(v, low, high);
	if(last != v)
		res |= XUI_RES_CHANGE;

	xui_control_draw_frame(ctx, id, &base, XUI_COLOR_BASE, opt);
	w = ctx->style.scroll.thumb_size;
	x = (v - low) * (base.w - w) / (high - low);
	region_init(&thumb, base.x + x, base.y, w, base.h);
	xui_control_draw_frame(ctx, id, &thumb, XUI_COLOR_BASE, opt);
	sprintf(buf, fmt, v);
	xui_control_draw_text(ctx, buf, &base, &ctx->style.text.text_color, opt);

	return res;
}

int xui_slider(struct xui_context_t * ctx, float * value, float low, float high)
{
	return xui_slider_ex(ctx, value, low, high, 0, "%.2f", 0);
}

int xui_number_ex(struct xui_context_t * ctx, float * value, float step, const char * fmt, int opt)
{
	char buf[128];
	int res = 0;
	unsigned int id = xui_get_id(ctx, &value, sizeof(value));
	struct region_t base;
	region_clone(&base, xui_layout_next(ctx));
	float last = *value;

	if(number_textbox(ctx, value, &base, id))
		return res;
	xui_control_update(ctx, id, &base, opt);
	if((ctx->focus == id) && (ctx->mouse_down & XUI_MOUSE_LEFT))
		*value += ctx->mouse_delta_x * step;
	if(*value != last)
		res |= XUI_RES_CHANGE;
	xui_control_draw_frame(ctx, id, &base, XUI_COLOR_BASE, opt);
	sprintf(buf, fmt, *value);
	xui_control_draw_text(ctx, buf, &base, &ctx->style.text.text_color, opt);

	return res;
}

int xui_number(struct xui_context_t * ctx, float * value, float step)
{
	return xui_number_ex(ctx, value, step, "%.2f", 0);
}

static void draw_frame(struct xui_context_t * ctx, struct region_t * r, int cid)
{
	struct color_t * col = &ctx->style.colors[cid];
	xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, col);
	if(ctx->style.colors[XUI_COLOR_BORDER].a)
	{
		struct region_t region;
		region_expand(&region, r, 1);
		xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, 0, 1, &ctx->style.colors[XUI_COLOR_BORDER]);
	}
}

static int text_width(void * font, const char * txt, int len)
{
	if(len == -1)
		len = strlen(txt);
	return 8 * len;
}

static int text_height(void * font)
{
	return 16;
}

struct xui_context_t * xui_context_alloc(const char * fb, const char * input, struct xui_style_t * style)
{
	struct xui_context_t * ctx;

	ctx = malloc(sizeof(struct xui_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct xui_context_t));
	ctx->w = window_alloc(fb, input, ctx);
	ctx->f = font_context_alloc();
	region_init(&ctx->screen, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));

	memcpy(&ctx->style, style ? style : &xui_style_default, sizeof(struct xui_style_t));
	region_clone(&ctx->clip, &ctx->screen);

	ctx->draw_frame = draw_frame;
	ctx->text_width = text_width;
	ctx->text_height = text_height;
	return ctx;
}

void xui_context_free(struct xui_context_t * ctx)
{
	if(ctx)
	{
		window_free(ctx->w);
		font_context_free(ctx->f);
		free(ctx);
	}
}

static void xui_draw(struct window_t * w, void * o)
{
	struct xui_context_t * ctx = (struct xui_context_t *)o;
	struct surface_t * s = ctx->w->s;
	struct region_t * clip = &ctx->clip;
    union xui_cmd_t * cmd = NULL;

	while(xui_cmd_next(ctx, &cmd))
	{
		switch(cmd->base.type)
		{
		case XUI_CMD_TYPE_JUMP:
			break;
		case XUI_CMD_TYPE_CLIP:
			if(!region_intersect(clip, &ctx->screen, &cmd->clip.r))
				region_init(clip, 0, 0, 0, 0);
			break;
		case XUI_CMD_TYPE_TRIANGLE:
			surface_shape_triangle(s, clip, &cmd->triangle.p0, &cmd->triangle.p1, &cmd->triangle.p2, cmd->triangle.thickness, &cmd->triangle.c);
			break;
		case XUI_CMD_TYPE_RECTANGLE:
			surface_shape_rectangle(s, clip, cmd->rectangle.x, cmd->rectangle.y, cmd->rectangle.w, cmd->rectangle.h, cmd->rectangle.radius, cmd->rectangle.thickness, &cmd->rectangle.c);
			break;
		case XUI_CMD_TYPE_TEXT:
			font_draw(s, clip, cmd->text.x, cmd->text.y, cmd->text.utf8, &cmd->text.c);
			break;
		case XUI_CMD_TYPE_ICON:
			switch(cmd->icon.id)
			{
			case XUI_ICON_CLOSE:
				font_draw(s, clip, cmd->icon.x, cmd->icon.y, "X", &cmd->icon.c);
				break;
			case XUI_ICON_CHECK:
				font_draw(s, clip, cmd->icon.x, cmd->icon.y, "?", &cmd->icon.c);
				break;
			case XUI_ICON_COLLAPSED:
				font_draw(s, clip, cmd->icon.x, cmd->icon.y, "+", &cmd->icon.c);
				break;
			case XUI_ICON_EXPANDED:
				font_draw(s, clip, cmd->icon.x, cmd->icon.y, "-", &cmd->icon.c);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void xui_loop(struct xui_context_t * ctx, void (*func)(struct xui_context_t *))
{
	struct event_t e;
	char utf8[16];
	int l, sz;

	while(1)
	{
		while(window_pump_event(ctx->w, &e))
		{
			switch(e.type)
			{
			case EVENT_TYPE_KEY_DOWN:
				switch(e.e.key_down.key)
				{
				case KEY_POWER:
					ctx->key_down |= XUI_KEY_POWER;
					ctx->key_pressed |= XUI_KEY_POWER;
					break;
				case KEY_UP:
					ctx->key_down |= XUI_KEY_UP;
					ctx->key_pressed |= XUI_KEY_UP;
					break;
				case KEY_DOWN:
					ctx->key_down |= XUI_KEY_DOWN;
					ctx->key_pressed |= XUI_KEY_DOWN;
					break;
				case KEY_LEFT:
					ctx->key_down |= XUI_KEY_LEFT;
					ctx->key_pressed |= XUI_KEY_LEFT;
					break;
				case KEY_RIGHT:
					ctx->key_down |= XUI_KEY_RIGHT;
					ctx->key_pressed |= XUI_KEY_RIGHT;
					break;
				case KEY_VOLUME_UP:
					ctx->key_down |= XUI_KEY_VOLUME_UP;
					ctx->key_pressed |= XUI_KEY_VOLUME_UP;
					break;
				case KEY_VOLUME_DOWN:
					ctx->key_down |= XUI_KEY_VOLUME_DOWN;
					ctx->key_pressed |= XUI_KEY_VOLUME_DOWN;
					break;
				case KEY_VOLUME_MUTE:
					ctx->key_down |= XUI_KEY_VOLUME_MUTE;
					ctx->key_pressed |= XUI_KEY_VOLUME_MUTE;
					break;
				case KEY_TAB:
					ctx->key_down |= XUI_KEY_TAB;
					ctx->key_pressed |= XUI_KEY_TAB;
					break;
				case KEY_TASK:
					ctx->key_down |= XUI_KEY_TASK;
					ctx->key_pressed |= XUI_KEY_TASK;
					break;
				case KEY_HOME:
					ctx->key_down |= XUI_KEY_HOME;
					ctx->key_pressed |= XUI_KEY_HOME;
					break;
				case KEY_BACK:
					ctx->key_down |= XUI_KEY_BACK;
					ctx->key_pressed |= XUI_KEY_BACK;
					break;
				case KEY_ENTER:
					ctx->key_down |= XUI_KEY_ENTER;
					ctx->key_pressed |= XUI_KEY_ENTER;
					break;
				case KEY_L_CTRL:
				case KEY_R_CTRL:
					ctx->key_down |= XUI_KEY_CTRL;
					ctx->key_pressed |= XUI_KEY_CTRL;
					break;
				case KEY_L_ALT:
				case KEY_R_ALT:
					ctx->key_down |= XUI_KEY_ALT;
					ctx->key_pressed |= XUI_KEY_ALT;
					break;
				case KEY_L_SHIFT:
				case KEY_R_SHIFT:
					ctx->key_down |= XUI_KEY_SHIFT;
					ctx->key_pressed |= XUI_KEY_SHIFT;
					break;
				default:
					if(e.e.key_up.key >= KEY_SPACE)
					{
						ucs4_to_utf8(&e.e.key_up.key, 1, utf8, sizeof(utf8));
						l = strlen(ctx->input_text);
						sz = strlen(utf8) + 1;
						if(l + sz <= sizeof(ctx->input_text))
							memcpy(ctx->input_text + l, utf8, sz);
					}
					break;
				}
				break;
			case EVENT_TYPE_KEY_UP:
				switch(e.e.key_up.key)
				{
				case KEY_POWER:
					ctx->key_down &= ~XUI_KEY_POWER;
					break;
				case KEY_UP:
					ctx->key_down &= ~XUI_KEY_UP;
					break;
				case KEY_DOWN:
					ctx->key_down &= ~XUI_KEY_DOWN;
					break;
				case KEY_LEFT:
					ctx->key_down &= ~XUI_KEY_LEFT;
					break;
				case KEY_RIGHT:
					ctx->key_down &= ~XUI_KEY_RIGHT;
					break;
				case KEY_VOLUME_UP:
					ctx->key_down &= ~XUI_KEY_VOLUME_UP;
					break;
				case KEY_VOLUME_DOWN:
					ctx->key_down &= ~XUI_KEY_VOLUME_DOWN;
					break;
				case KEY_VOLUME_MUTE:
					ctx->key_down &= ~XUI_KEY_VOLUME_MUTE;
					break;
				case KEY_TAB:
					ctx->key_down &= ~XUI_KEY_TAB;
					break;
				case KEY_TASK:
					ctx->key_down &= ~XUI_KEY_TASK;
					break;
				case KEY_HOME:
					ctx->key_down &= ~XUI_KEY_HOME;
					break;
				case KEY_BACK:
					ctx->key_down &= ~XUI_KEY_BACK;
					break;
				case KEY_ENTER:
					ctx->key_down &= ~XUI_KEY_ENTER;
					break;
				case KEY_L_CTRL:
				case KEY_R_CTRL:
					ctx->key_down &= ~XUI_KEY_CTRL;
					break;
				case KEY_L_ALT:
				case KEY_R_ALT:
					ctx->key_down &= ~XUI_KEY_ALT;
					break;
				case KEY_L_SHIFT:
				case KEY_R_SHIFT:
					ctx->key_down &= ~XUI_KEY_SHIFT;
					break;
				default:
					break;
				}
				break;
			case EVENT_TYPE_MOUSE_DOWN:
				ctx->mouse_pos_x = e.e.mouse_down.x;
				ctx->mouse_pos_y = e.e.mouse_down.y;
				ctx->mouse_down |= e.e.mouse_down.button;
				ctx->mouse_pressed |= e.e.mouse_down.button;
				break;
			case EVENT_TYPE_MOUSE_MOVE:
				ctx->mouse_pos_x = e.e.mouse_move.x;
				ctx->mouse_pos_y = e.e.mouse_move.y;
				break;
			case EVENT_TYPE_MOUSE_UP:
				ctx->mouse_pos_x = e.e.mouse_up.x;
				ctx->mouse_pos_y = e.e.mouse_up.y;
				ctx->mouse_down &= ~e.e.mouse_up.button;
				break;
			case EVENT_TYPE_MOUSE_WHEEL:
				ctx->scroll_delta_x += e.e.mouse_wheel.dx * 30;
				ctx->scroll_delta_y -= e.e.mouse_wheel.dy * 30;
				break;
			default:
				break;
			}
		}
		if(func)
			func(ctx);
		ctx->cmd_hash_new = xui_get_id(ctx, &ctx->cmd_list, sizeof(int) + ctx->cmd_list.idx);
		if((ctx->cmd_hash_old != ctx->cmd_hash_new) || (ctx->w->wm->cursor.show && ctx->w->wm->cursor.dirty))
		{
			ctx->cmd_hash_old = ctx->cmd_hash_new;
			if(window_is_active(ctx->w))
			{
				ctx->w->wm->refresh = 1;
				window_present(ctx->w, &ctx->style.background_color, ctx, xui_draw);
			}
		}
		task_yield();
	}
}
