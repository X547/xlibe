#include <interface/Region.h>
#include <stdio.h>

#include "Drawables.h"
#include "Color.h"
#include "Font.h"

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xutil.h>
}

#include "Debug.h"

extern "C" GC
XCreateGC(Display *display, Window window,
	unsigned long mask, XGCValues *gc_values)
{
	GC gc = new _XGC;
	gc->rects = False;
	gc->values.fill_style = FillSolid;
	gc->values.line_style = LineSolid;
	gc->values.line_width = 0;
	gc->values.cap_style = CapButt;
	gc->values.join_style = JoinMiter;
	gc->values.font = 0;
	gc->values.clip_x_origin = gc->values.clip_y_origin = 0;
	gc->dirty = True;
	XChangeGC(display, gc, mask, gc_values);
	return gc;
}

extern "C" int
XChangeGC(Display *display, GC gc, unsigned long mask, XGCValues *values)
{
	if (mask & GCFunction)
		gc->values.function = values->function;
	if (mask & GCPlaneMask)
		gc->values.plane_mask = values->plane_mask;
	if (mask & GCForeground)
		gc->values.foreground = values->foreground;
	if (mask & GCBackground)
		gc->values.background = values->background;
	if (mask & GCLineWidth)
		gc->values.line_width = values->line_width;
	if (mask & GCLineStyle)
		gc->values.line_style = values->line_style;
	if (mask & GCCapStyle)
		gc->values.cap_style = values->cap_style;
	if (mask & GCJoinStyle)
		gc->values.join_style = values->join_style;
	if (mask & GCFillStyle)
		gc->values.fill_style = values->fill_style;
	if (mask & GCFillRule)
		gc->values.fill_rule = values->fill_rule;
	if (mask & GCArcMode)
		gc->values.arc_mode = values->arc_mode;
	if (mask & GCTile)
		gc->values.tile = values->tile;
	if (mask & GCStipple)
		gc->values.stipple = values->stipple;
	if (mask & GCTileStipXOrigin)
		gc->values.ts_x_origin = values->ts_x_origin;
	if (mask & GCTileStipYOrigin)
		gc->values.ts_y_origin = values->ts_y_origin;
	if (mask & GCFont)
		gc->values.font = values->font;
	if (mask & GCSubwindowMode)
		gc->values.subwindow_mode = values->subwindow_mode;
	if (mask & GCGraphicsExposures)
		gc->values.graphics_exposures = values->graphics_exposures;
	if (mask & GCClipXOrigin)
		gc->values.clip_x_origin = values->clip_x_origin;
	if (mask & GCClipYOrigin)
		gc->values.clip_y_origin = values->clip_y_origin;
	if (mask & GCClipMask)
		XSetClipMask(display, gc, values->clip_mask);
	if (mask & GCDashOffset)
		gc->values.dash_offset = values->dash_offset;
#if 0
	// TODO
	if (mask & GCDashList)
		XSetDashes(display, gc, &values->dashes, 2);
#endif
	gc->dirty = True;
	return 0;
}

extern "C" int
XCopyGC(Display *display, GC src, unsigned long mask, GC dest)
{
	return XChangeGC(display, dest, mask, &src->values);
}

extern "C" int
XFreeGC(Display* display, GC gc)
{
	delete gc;
	return Success;
}

extern "C" int
XSetForeground(Display *display, GC gc, unsigned long color)
{
	gc->values.foreground = color;
	gc->dirty = True;
	return 0;
}

extern "C" int
XSetBackground(Display *display, GC gc, unsigned long color)
{
	gc->values.background = color;
	gc->dirty = True;
	return 0;
}

extern "C" int
XSetGraphicsExposures(Display *display, GC gc, Bool graphics_exposures)
{
	gc->values.graphics_exposures = graphics_exposures;
	return 0;
}

extern "C" int
XSetLineAttributes(Display* display, GC gc,
	unsigned int line_width, int line_style, int cap_style, int join_style)
{
	gc->values.line_width = line_width;
	gc->values.line_style = line_style;
	gc->values.cap_style = cap_style;
	gc->values.join_style = join_style;
	gc->dirty = True;
	return 0;
}

extern "C" int
XSetFillStyle(Display* display, GC gc, int fill_style)
{
	gc->values.fill_style = fill_style;
	gc->dirty = True;
	return 0;
}

extern "C" int
XSetFont(Display *display, GC gc, Font font)
{
	gc->values.font = font;
	gc->dirty = True;
	return 0;
}

extern "C" int
XSetClipOrigin(Display *display, GC gc, int clip_x_origin, int clip_y_origin)
{
	gc->values.clip_x_origin = clip_x_origin;
	gc->values.clip_y_origin = clip_y_origin;
	gc->dirty = True;
	return 0;
}

static void
clear_clip_mask(GC gc)
{
	if (gc->rects)
		delete (BRegion*)(Region)gc->values.clip_mask;
	gc->values.clip_mask = None;
}

extern "C" int
XSetRegion(Display *display, GC gc, Region r)
{
	clear_clip_mask(gc);

	BRegion* region = (BRegion*)r;
	gc->rects = True;
	gc->values.clip_mask = region ? (Pixmap)(Region)new BRegion(*region) : None;
	gc->dirty = True;
	return Success;
}

extern "C" int
XSetClipRectangles(Display *display, GC gc, int clip_x_origin, int clip_y_origin,
	XRectangle* rect, int count, int ordering)
{
	clear_clip_mask(gc);

	XSetClipOrigin(display, gc, clip_x_origin, clip_y_origin);

	gc->values.clip_mask = (Pixmap)XCreateRegion();
	gc->rects = True;
	for (int i = 0; i < count; i++)
		XUnionRectWithRegion(&rect[i], (Region)gc->values.clip_mask, (Region)gc->values.clip_mask);

	gc->dirty = True;
	return Success;
}

extern "C" Status
XSetClipMask(Display *display, GC gc, Pixmap pixmap)
{
	clear_clip_mask(gc);

	// Not supported!
	gc->dirty = True;
	return BadImplementation;
}

extern "C" Status
XSetDashes(Display *display, GC gc, int dash_offset, const char *dash_list, int n)
{
	// Not supported.
	return BadImplementation;
}

void
bex_check_gc(XDrawable* drawable, GC gc)
{
	if (!gc) {
		// Use the window's default GC, or make one for it.
		if (!drawable->default_gc)
			drawable->default_gc = XCreateGC(NULL, 0, 0, NULL);
		gc = drawable->default_gc;
	}
	if (drawable->gc == gc && !gc->dirty)
		return;

	BView* view = drawable->view();

	drawable->gc = gc;
	view->SetHighColor(create_rgb(gc->values.foreground));
	view->SetLowColor(create_rgb(gc->values.background));
	view->SetPenSize(gc->values.line_width);

	cap_mode cap;
	switch(gc->values.cap_style) {
	case CapRound:
		cap = B_ROUND_CAP;
		break;
	case CapProjecting:
		cap = B_SQUARE_CAP;
		break;
	case CapNotLast:
	case CapButt:
	default:
		cap = B_ROUND_CAP;
		break;
	}

	join_mode join;
	switch(gc->values.join_style) {
	case JoinRound:
		join = B_ROUND_JOIN;
		break;
	case JoinBevel:
		join = B_BEVEL_JOIN;
		break;
	case JoinMiter:
	default:
		join = B_MITER_JOIN;
		break;
	}
	view->SetLineMode(cap, join);

	// TODO: use mask!
	if (gc->values.font) {
		BFont bfont = bfont_from_font(gc->values.font);
		view->SetFont(&bfont);
	}

	// TODO: use mask!
	// TODO: clip_x_origin/clip_y_origin!
	if (gc->rects) {
		view->ConstrainClippingRegion((BRegion*)(Region)gc->values.clip_mask);
	} else {
		view->ConstrainClippingRegion(NULL);
	}

	gc->dirty = False;
}
