/* Copyright (c) 1997-1999 Miller Puckette.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution. */

/* g_7_guis.c written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001 */
/* thanks to Miller Puckette, Guenther Geiger and Krzystof Czaja */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "m_pd.h"
#include "g_canvas.h"
#include "t_tk.h"
#include "g_all_guis.h"
#include <math.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

extern int gfxstub_haveproperties(void *key);
void toggle_draw_select(t_toggle* x, t_glist* glist);
 

/* --------------- tgl     gui-toggle ------------------------- */

t_widgetbehavior toggle_widgetbehavior;
static t_class *toggle_class;

/* widget helper functions */

void toggle_draw_update(t_gobj *xgobj, t_glist *glist)
{
	t_toggle *x = (t_toggle *)xgobj;
	if (x->x_gui.x_changed) {
	    if(glist_isvisible(glist))
	    {
	        t_canvas *canvas=glist_getcanvas(glist);

	        sys_vgui(".x%lx.c itemconfigure %lxX1 -stroke #%6.6x\n", canvas, x,
	                 (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol);
	        sys_vgui(".x%lx.c itemconfigure %lxX2 -stroke #%6.6x\n", canvas, x,
	                 (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol);
	    }
	    x->x_gui.x_changed = 0;
	}
}

void toggle_draw_new(t_toggle *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int w=1, xx=text_xpix(&x->x_gui.x_obj, glist), yy=text_ypix(&x->x_gui.x_obj, glist);

	t_scalehandle *sh = (t_scalehandle *)x->x_gui.x_handle;
	sprintf(sh->h_pathname, ".x%lx.h%lx", (t_int)canvas, (t_int)sh);

	t_scalehandle *lh = (t_scalehandle *)x->x_gui.x_lhandle;
	sprintf(lh->h_pathname, ".x%lx.h%lx", (t_int)canvas, (t_int)lh);

	//if (glist_isvisible(canvas)) {

		char *nlet_tag = iem_get_tag(glist, (t_iemgui *)x);

		if(x->x_gui.x_w >= 30)
		    w = 2;
		if(x->x_gui.x_w >= 60)
		    w = 3;
		sys_vgui(".x%lx.c create prect %d %d %d %d -stroke $pd_colors(iemgui_border) -fill #%6.6x -tags {%lxBASE %lxTGL %s text iemgui border}\n",
		         canvas, xx, yy, xx + x->x_gui.x_w, yy + x->x_gui.x_h,
		         x->x_gui.x_bcol, x, x, nlet_tag);
		sys_vgui(".x%lx.c create polyline %d %d %d %d -strokewidth %d -stroke #%6.6x -tags {%lxX1 %lxTGL %s text iemgui}\n",
		         canvas, xx+w+1, yy+w+1, xx + x->x_gui.x_w-w-1, yy + x->x_gui.x_h-w-1, w,
		         (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol, x, x, nlet_tag);
		sys_vgui(".x%lx.c create polyline %d %d %d %d -strokewidth %d -stroke #%6.6x -tags {%lxX2 %lxTGL %s text iemgui}\n",
		         canvas, xx+w+1, yy + x->x_gui.x_h-w-1, xx + x->x_gui.x_w-w-1, yy+w+1, w,
		         (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol, x, x, nlet_tag);
		sys_vgui(".x%lx.c create text %d %d -text {%s} -anchor w \
		         -font {{%s} -%d %s} -fill #%6.6x -tags {%lxLABEL %lxTGL %s text iemgui}\n",
		         canvas, xx+x->x_gui.x_ldx,
		         yy+x->x_gui.x_ldy,
		         strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"",
		         x->x_gui.x_font, x->x_gui.x_fontsize, sys_fontweight,
		         x->x_gui.x_lcol, x, x, nlet_tag);
		if(!x->x_gui.x_fsf.x_snd_able && canvas == x->x_gui.x_glist)
		    sys_vgui(".x%lx.c create prect %d %d %d %d -stroke $pd_colors(iemgui_nlet) -tags {%lxTGL%so%d %so%d %lxTGL %s outlet iemgui}\n",
		         canvas, xx, yy + x->x_gui.x_h-1, xx + IOWIDTH, yy + x->x_gui.x_h, x, nlet_tag, 0, nlet_tag, 0, x, nlet_tag);
		if(!x->x_gui.x_fsf.x_rcv_able && canvas == x->x_gui.x_glist)
		    sys_vgui(".x%lx.c create prect %d %d %d %d -stroke $pd_colors(iemgui_nlet) -tags {%lxTGL%si%d %si%d %lxTGL %s inlet iemgui}\n",
		         canvas, xx, yy, xx + IOWIDTH, yy+1, x, nlet_tag, 0, nlet_tag, 0, x, nlet_tag);
	//}
}

void toggle_draw_move(t_toggle *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int w=1, xx=text_xpix(&x->x_gui.x_obj, glist), yy=text_ypix(&x->x_gui.x_obj, glist);

	if (glist_isvisible(canvas)) {

		char *nlet_tag = iem_get_tag(glist, (t_iemgui *)x);

		if(x->x_gui.x_w >= 30)
		    w = 2;

		if(x->x_gui.x_w >= 60)
		    w = 3;
		sys_vgui(".x%lx.c coords %lxBASE %d %d %d %d\n",
		         canvas, x, xx, yy, xx + x->x_gui.x_w, yy + x->x_gui.x_h);
		sys_vgui(".x%lx.c itemconfigure %lxX1 -strokewidth %d\n", canvas, x, w);
		sys_vgui(".x%lx.c coords %lxX1 %d %d %d %d\n",
		         canvas, x, xx+w+1, yy+w+1, xx + x->x_gui.x_w-w-1, yy + x->x_gui.x_h-w-1);
		sys_vgui(".x%lx.c itemconfigure %lxX2 -strokewidth %d\n", canvas, x, w);
		sys_vgui(".x%lx.c coords %lxX2 %d %d %d %d\n",
		         canvas, x, xx+w+1, yy + x->x_gui.x_h-w-1, xx + x->x_gui.x_w-w-1, yy+w+1);
		sys_vgui(".x%lx.c coords %lxLABEL %d %d\n",
		         canvas, x, xx+x->x_gui.x_ldx, yy+x->x_gui.x_ldy);
		if(!x->x_gui.x_fsf.x_snd_able && canvas == x->x_gui.x_glist)
		    sys_vgui(".x%lx.c coords %lxTGL%so%d %d %d %d %d\n",
		         canvas, x, nlet_tag, 0, xx, yy + x->x_gui.x_h-1, xx + IOWIDTH, yy + x->x_gui.x_h);
		if(!x->x_gui.x_fsf.x_rcv_able && canvas == x->x_gui.x_glist)
		    sys_vgui(".x%lx.c coords %lxTGL%si%d %d %d %d %d\n",
		         canvas, x, nlet_tag, 0, xx, yy, xx + IOWIDTH, yy+1);
		/* redraw scale handle rectangle if selected */
		if (x->x_gui.x_fsf.x_selected)
			toggle_draw_select(x, x->x_gui.x_glist);
	}
}

void toggle_draw_erase(t_toggle* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);

	sys_vgui(".x%lx.c delete %lxTGL\n", canvas, x);
	sys_vgui(".x%lx.c dtag all %lxTGL\n", canvas, x);
	if (x->x_gui.x_fsf.x_selected) {
		t_scalehandle *sh = (t_scalehandle *)(x->x_gui.x_handle);
		sys_vgui("destroy %s\n", sh->h_pathname);
		sys_vgui(".x%lx.c delete %lxSCALE\n", canvas, x);
		t_scalehandle *lh = (t_scalehandle *)(x->x_gui.x_lhandle);
		sys_vgui("destroy %s\n", lh->h_pathname);
		sys_vgui(".x%lx.c delete %lxLABELH\n", canvas, x);
	}
}

void toggle_draw_config(t_toggle* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);

	/*
	char color[64];
	if (x->x_gui.x_fsf.x_selected)
		sprintf(color, "$pd_colors(selection)");
	else
		sprintf(color, "#%6.6x", x->x_gui.x_lcol); 


    sys_vgui(".x%lx.c itemconfigure %lxLABEL -font {{%s} %d %s} -fill %s -text {%s} \n",
             canvas, x, x->x_gui.x_font, x->x_gui.x_fontsize, sys_fontweight,
             color,
             strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"");
	*/
	if (x->x_gui.x_fsf.x_selected && x->x_gui.x_glist == canvas) {
    	sys_vgui(".x%lx.c itemconfigure %lxLABEL -font {{%s} -%d %s} -fill $pd_colors(selection) -text {%s} \n",
             canvas, x, x->x_gui.x_font, x->x_gui.x_fontsize, sys_fontweight,
             strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"");
	}
	else {
    	sys_vgui(".x%lx.c itemconfigure %lxLABEL -font {{%s} -%d %s} -fill #%6.6x -text {%s} \n",
             canvas, x, x->x_gui.x_font, x->x_gui.x_fontsize, sys_fontweight,
			 x->x_gui.x_lcol,
             strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"");
	}
    sys_vgui(".x%lx.c itemconfigure %lxBASE -fill #%6.6x\n .x%lx.c itemconfigure %lxX1 -stroke #%6.6x\n .x%lx.c itemconfigure %lxX2 -stroke #%6.6x\n", canvas, x,
             x->x_gui.x_bcol , canvas, x,
             x->x_on?x->x_gui.x_fcol:x->x_gui.x_bcol, canvas, x,
             x->x_on?x->x_gui.x_fcol:x->x_gui.x_bcol);
	/*
    sys_vgui(".x%lx.c itemconfigure %lxX1 -fill #%6.6x\n", canvas, x,
             x->x_on?x->x_gui.x_fcol:x->x_gui.x_bcol);
    sys_vgui(".x%lx.c itemconfigure %lxX2 -fill #%6.6x\n", canvas, x,
             x->x_on?x->x_gui.x_fcol:x->x_gui.x_bcol);
	*/
}

void toggle_draw_io(t_toggle* x, t_glist* glist, int old_snd_rcv_flags)
{
    int xpos=text_xpix(&x->x_gui.x_obj, glist);
    int ypos=text_ypix(&x->x_gui.x_obj, glist);
    t_canvas *canvas=glist_getcanvas(glist);

	if (glist_isvisible(canvas) && canvas == x->x_gui.x_glist) {

		char *nlet_tag = iem_get_tag(glist, (t_iemgui *)x);

		if((old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && !x->x_gui.x_fsf.x_snd_able)
		    sys_vgui(".x%lx.c create prect %d %d %d %d -stroke $pd_colors(iemgui_nlet) -tags {%lxTGL%so%d %so%d %lxTGL %s outlet iemgui}\n",
		         canvas, xpos,
		         ypos + x->x_gui.x_h-1, xpos + IOWIDTH,
		         ypos + x->x_gui.x_h, x, nlet_tag, 0, nlet_tag, 0, x, nlet_tag);
		if(!(old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && x->x_gui.x_fsf.x_snd_able)
		    sys_vgui(".x%lx.c delete %lxTGL%so%d\n", canvas, x, nlet_tag, 0);
		if((old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && !x->x_gui.x_fsf.x_rcv_able)
		    sys_vgui(".x%lx.c create prect %d %d %d %d -stroke $pd_colors(iemgui_nlet) -tags {%lxTGL%si%d %si%d %lxTGL %s inlet iemgui}\n",
		         canvas, xpos, ypos,
		         xpos + IOWIDTH, ypos+1, x, nlet_tag, 0, nlet_tag, 0, x, nlet_tag);
		if(!(old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && x->x_gui.x_fsf.x_rcv_able)
		    sys_vgui(".x%lx.c delete %lxTGL%si%d\n", canvas, x, nlet_tag, 0);
	}
}

void toggle_draw_select(t_toggle* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
	t_scalehandle *sh = (t_scalehandle *)(x->x_gui.x_handle);
	t_scalehandle *lh = (t_scalehandle *)(x->x_gui.x_lhandle);

	//if (glist_isvisible(canvas)) {

		if(x->x_gui.x_fsf.x_selected)
		{

			// check if we are drawing inside a gop abstraction visible on parent canvas
			// if so, disable highlighting
			if (x->x_gui.x_glist == glist_getcanvas(glist)) {

			char *nlet_tag = iem_get_tag(glist, (t_iemgui *)x);

		    sys_vgui(".x%lx.c itemconfigure %lxBASE -stroke $pd_colors(selection)\n", canvas, x);
		    sys_vgui(".x%lx.c itemconfigure %lxLABEL -fill $pd_colors(selection)\n", canvas, x);

				if (x->x_gui.scale_vis) {
					sys_vgui("destroy %s\n", sh->h_pathname);
					sys_vgui(".x%lx.c delete %lxSCALE\n", canvas, x);
				}

				sys_vgui("canvas %s -width %d -height %d -bg $pd_colors(selection) -bd 0 -cursor bottom_right_corner\n",
					 sh->h_pathname, SCALEHANDLE_WIDTH, SCALEHANDLE_HEIGHT);
				sys_vgui(".x%x.c create window %d %d -anchor nw -width %d -height %d -window %s -tags {%lxSCALE %lxTGL %s}\n",
					 canvas, x->x_gui.x_obj.te_xpix + x->x_gui.x_w - SCALEHANDLE_WIDTH - 1,
					 x->x_gui.x_obj.te_ypix + x->x_gui.x_h - SCALEHANDLE_HEIGHT - 1,
					 SCALEHANDLE_WIDTH, SCALEHANDLE_HEIGHT,
					 sh->h_pathname, x, x, nlet_tag);
				sys_vgui("bind %s <Button> {pd [concat %s _click 1 %%x %%y \\;]}\n",
					 sh->h_pathname, sh->h_bindsym->s_name);
				sys_vgui("bind %s <ButtonRelease> {pd [concat %s _click 0 0 0 \\;]}\n",
					 sh->h_pathname, sh->h_bindsym->s_name);
				sys_vgui("bind %s <Motion> {pd [concat %s _motion %%x %%y \\;]}\n",
					 sh->h_pathname, sh->h_bindsym->s_name);
				x->x_gui.scale_vis = 1;

				if (strcmp(x->x_gui.x_lab->s_name, "empty") != 0)
				{
					if (x->x_gui.label_vis) {
						sys_vgui("destroy %s\n", lh->h_pathname);
						sys_vgui(".x%lx.c delete %lxLABELH\n", canvas, x);
					}

					sys_vgui("canvas %s -width %d -height %d -bg $pd_colors(selection) -bd 0 -cursor crosshair\n",
						lh->h_pathname, LABELHANDLE_WIDTH, LABELHANDLE_HEIGHT);
					sys_vgui(".x%x.c create window %d %d -anchor nw -width %d -height %d -window %s -tags {%lxLABEL %lxLABELH %lxTGL %s}\n",
						canvas, x->x_gui.x_obj.te_xpix+ x->x_gui.x_ldx - LABELHANDLE_WIDTH,
						x->x_gui.x_obj.te_ypix + x->x_gui.x_ldy - LABELHANDLE_HEIGHT,
						LABELHANDLE_WIDTH, LABELHANDLE_HEIGHT,
						lh->h_pathname, x, x, x, nlet_tag);
					sys_vgui("bind %s <Button> {pd [concat %s _click 1 %%x %%y \\;]}\n",
						lh->h_pathname, lh->h_bindsym->s_name);
					sys_vgui("bind %s <ButtonRelease> {pd [concat %s _click 0 0 0 \\;]}\n",
						lh->h_pathname, lh->h_bindsym->s_name);
					sys_vgui("bind %s <Motion> {pd [concat %s _motion %%x %%y \\;]}\n",
						lh->h_pathname, lh->h_bindsym->s_name);
					x->x_gui.label_vis = 1;
				}
			}

			sys_vgui(".x%lx.c addtag selected withtag %lxTGL\n", canvas, x);
		}
		else
		{
		    sys_vgui(".x%lx.c itemconfigure %lxBASE -stroke %s\n", canvas, x, IEM_GUI_COLOR_NORMAL);
		    sys_vgui(".x%lx.c itemconfigure %lxLABEL -fill #%6.6x\n", canvas, x, x->x_gui.x_lcol);
			sys_vgui(".x%lx.c dtag %lxTGL selected\n", canvas, x);
			sys_vgui("destroy %s\n", sh->h_pathname);
			sys_vgui(".x%lx.c delete %lxSCALE\n", canvas, x);
			x->x_gui.scale_vis = 0;
			sys_vgui("destroy %s\n", lh->h_pathname);
			sys_vgui(".x%lx.c delete %lxLABELH\n", canvas, x);
			x->x_gui.label_vis = 0;
		}
	//}
}

static void toggle__clickhook(t_scalehandle *sh, t_floatarg f, t_floatarg xxx, t_floatarg yyy)
{

	t_toggle *x = (t_toggle *)(sh->h_master);

	if (xxx) {
		x->x_gui.scale_offset_x = xxx;
		x->x_gui.label_offset_x = xxx;
	}
	if (yyy) {
		x->x_gui.scale_offset_y = yyy;
		x->x_gui.label_offset_y = yyy;
	}

    int newstate = (int)f;
    if (sh->h_dragon && newstate == 0 && sh->h_scale)
    {
		/* done dragging */

		/* first set up the undo apply */
		canvas_apply_setundo(x->x_gui.x_glist, (t_gobj *)x);

		if (sh->h_dragx || sh->h_dragy) {

			if (sh->h_dragx > sh->h_dragy)
				sh->h_dragx = sh->h_dragy;
			else sh->h_dragy = sh->h_dragx;

			x->x_gui.x_w = x->x_gui.x_w + sh->h_dragx - x->x_gui.scale_offset_x;
			if (x->x_gui.x_w < SCALE_TGL_MINWIDTH)
				x->x_gui.x_w = SCALE_TGL_MINWIDTH;
			x->x_gui.x_h = x->x_gui.x_h + sh->h_dragy - x->x_gui.scale_offset_y;
			if (x->x_gui.x_h < SCALE_TGL_MINHEIGHT)
				x->x_gui.x_h = SCALE_TGL_MINHEIGHT;

			canvas_dirty(x->x_gui.x_glist, 1);
		}

		int properties = gfxstub_haveproperties((void *)x);

		if (properties) {
			sys_vgui(".gfxstub%lx.dim.w_ent delete 0 end\n", properties);
			sys_vgui(".gfxstub%lx.dim.w_ent insert 0 %d\n", properties, x->x_gui.x_w);
			//sys_vgui(".gfxstub%lx.dim.h_ent delete 0 end\n", properties);
			//sys_vgui(".gfxstub%lx.dim.h_ent insert 0 %d\n", properties, x->x_gui.x_h);
		}

		if (glist_isvisible(x->x_gui.x_glist))
		{
			sys_vgui(".x%x.c delete %s\n", x->x_gui.x_glist, sh->h_outlinetag);
			toggle_draw_move(x, x->x_gui.x_glist);
			iemgui_select((t_gobj *)x, x->x_gui.x_glist, 1);
			canvas_fixlinesfor(x->x_gui.x_glist, (t_text *)x);
			sys_vgui("pdtk_canvas_getscroll .x%lx.c\n", x->x_gui.x_glist);
		}
    }
    else if (!sh->h_dragon && newstate && sh->h_scale)
    {
		/* dragging */
		if (glist_isvisible(x->x_gui.x_glist))
		{
			sys_vgui("lower %s\n", sh->h_pathname);
			sys_vgui(".x%x.c create prect %d %d %d %d\
	 -stroke $pd_colors(selection) -strokewidth 1 -tags %s\n",
				 x->x_gui.x_glist, x->x_gui.x_obj.te_xpix, x->x_gui.x_obj.te_ypix,
					x->x_gui.x_obj.te_xpix + x->x_gui.x_w,
					x->x_gui.x_obj.te_ypix + x->x_gui.x_h, sh->h_outlinetag);
		}

		sh->h_dragx = 0;
		sh->h_dragy = 0;
    }
	else if (sh->h_dragon && !newstate && !sh->h_scale)
	{
		/* done dragging */

		/* first set up the undo apply */
		canvas_apply_setundo(x->x_gui.x_glist, (t_gobj *)x);

		if (sh->h_dragx || sh->h_dragy) {

			x->x_gui.x_ldx = x->x_gui.x_ldx + sh->h_dragx - x->x_gui.label_offset_x;
			x->x_gui.x_ldy = x->x_gui.x_ldy + sh->h_dragy - x->x_gui.label_offset_y;

			canvas_dirty(x->x_gui.x_glist, 1);
		}

		int properties = gfxstub_haveproperties((void *)x);

		if (properties) {
			sys_vgui(".gfxstub%lx.dim.w_ent delete 0 end\n", properties);
			sys_vgui(".gfxstub%lx.dim.w_ent insert 0 %d\n", properties, x->x_gui.x_w);
			//sys_vgui(".gfxstub%lx.dim.h_ent delete 0 end\n", properties);
			//sys_vgui(".gfxstub%lx.dim.h_ent insert 0 %d\n", properties, x->x_gui.x_h);
		}

		if (glist_isvisible(x->x_gui.x_glist))
		{
			sys_vgui(".x%x.c delete %s\n", x->x_gui.x_glist, sh->h_outlinetag);
			toggle_draw_move(x, x->x_gui.x_glist);
			iemgui_select((t_gobj *)x, x->x_gui.x_glist, 1);
			canvas_fixlinesfor(x->x_gui.x_glist, (t_text *)x);
			sys_vgui("pdtk_canvas_getscroll .x%lx.c\n", x->x_gui.x_glist);
		}
	}
	else if(!sh->h_dragon && newstate && !sh->h_scale)
	{
		/* dragging */
		if (glist_isvisible(x->x_gui.x_glist)) {
			sys_vgui("lower %s\n", sh->h_pathname);
			t_scalehandle *othersh = (t_scalehandle *)x->x_gui.x_handle;
			sys_vgui("lower .x%lx.h%lx\n", (t_int)glist_getcanvas(x->x_gui.x_glist), (t_int)othersh);
		}

		sh->h_dragx = 0;
		sh->h_dragy = 0;
	}

    sh->h_dragon = newstate;
}

static void toggle__motionhook(t_scalehandle *sh,
				    t_floatarg f1, t_floatarg f2)
{
    if (sh->h_dragon && sh->h_scale)
    {
		t_toggle *x = (t_toggle *)(sh->h_master);
		int dx = (int)f1, dy = (int)f2;
		int newx, newy;

		if (dx > dy) {
			dx = dy;
			x->x_gui.scale_offset_x = x->x_gui.scale_offset_y;
		}
		else {
			dy = dx;
			x->x_gui.scale_offset_y = x->x_gui.scale_offset_x;
		}

		newx = x->x_gui.x_obj.te_xpix + x->x_gui.x_w - x->x_gui.scale_offset_x + dx;
		newy = x->x_gui.x_obj.te_ypix + x->x_gui.x_h - x->x_gui.scale_offset_y + dy;

		if (newx < x->x_gui.x_obj.te_xpix + SCALE_TGL_MINWIDTH)
			newx = x->x_gui.x_obj.te_xpix + SCALE_TGL_MINWIDTH;
		if (newy < x->x_gui.x_obj.te_ypix + SCALE_TGL_MINHEIGHT)
			newy = x->x_gui.x_obj.te_ypix + SCALE_TGL_MINHEIGHT;

		if (glist_isvisible(x->x_gui.x_glist)) {
			sys_vgui(".x%x.c coords %s %d %d %d %d\n",
				 x->x_gui.x_glist, sh->h_outlinetag, x->x_gui.x_obj.te_xpix,
				 x->x_gui.x_obj.te_ypix, newx, newy);
		}
		sh->h_dragx = dx;
		sh->h_dragy = dy;

		int properties = gfxstub_haveproperties((void *)x);

		if (properties) {
			int new_w = x->x_gui.x_w - x->x_gui.scale_offset_x + sh->h_dragx;
			int new_h = x->x_gui.x_h - x->x_gui.scale_offset_y + sh->h_dragy;
			sys_vgui(".gfxstub%lx.dim.w_ent delete 0 end\n", properties);
			sys_vgui(".gfxstub%lx.dim.w_ent insert 0 %d\n", properties, new_w);
			//sys_vgui(".gfxstub%lx.dim.h_ent delete 0 end\n", properties);
			//sys_vgui(".gfxstub%lx.dim.h_ent insert 0 %d\n", properties, new_h);
		}
    }
	else if (sh->h_dragon && !sh->h_scale)
	{
		t_bng *x = (t_bng *)(sh->h_master);
		int dx = (int)f1, dy = (int)f2;
		int newx, newy;
		newx = x->x_gui.x_obj.te_xpix + x->x_gui.x_w - x->x_gui.scale_offset_x + dx;
		newy = x->x_gui.x_obj.te_ypix + x->x_gui.x_h - x->x_gui.scale_offset_y + dy;

		sh->h_dragx = dx;
		sh->h_dragy = dy;

		int properties = gfxstub_haveproperties((void *)x);

		if (properties) {
			int new_x = x->x_gui.x_ldx - x->x_gui.label_offset_x + sh->h_dragx;
			int new_y = x->x_gui.x_ldy - x->x_gui.label_offset_y + sh->h_dragy;
			sys_vgui(".gfxstub%lx.label.xy.x_entry delete 0 end\n", properties);
			sys_vgui(".gfxstub%lx.label.xy.x_entry insert 0 %d\n", properties, new_x);
			sys_vgui(".gfxstub%lx.label.xy.y_entry delete 0 end\n", properties);
			sys_vgui(".gfxstub%lx.label.xy.y_entry insert 0 %d\n", properties, new_y);
		}

		if (glist_isvisible(x->x_gui.x_glist)) {
			int xpos=text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);
    		int ypos=text_ypix(&x->x_gui.x_obj, x->x_gui.x_glist);
    		t_canvas *canvas=glist_getcanvas(x->x_gui.x_glist);
			sys_vgui(".x%lx.c coords %lxLABEL %d %d\n",
 		    	canvas, x, xpos+x->x_gui.x_ldx + sh->h_dragx - x->x_gui.label_offset_x,
 		    	ypos+x->x_gui.x_ldy + sh->h_dragy - x->x_gui.label_offset_y);
		}
	}
}


void toggle_draw(t_toggle *x, t_glist *glist, int mode)
{
    if(mode == IEM_GUI_DRAW_MODE_UPDATE)
    	sys_queuegui((t_gobj*)x, x->x_gui.x_glist, toggle_draw_update);
        //toggle_draw_update(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_MOVE)
        toggle_draw_move(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_NEW) {
        toggle_draw_new(x, glist);
		sys_vgui(".x%lx.c raise all_cords\n", glist_getcanvas(glist));
	}
    else if(mode == IEM_GUI_DRAW_MODE_SELECT)
        toggle_draw_select(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_ERASE)
        toggle_draw_erase(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_CONFIG)
        toggle_draw_config(x, glist);
    else if(mode >= IEM_GUI_DRAW_MODE_IO)
        toggle_draw_io(x, glist, mode - IEM_GUI_DRAW_MODE_IO);
}

/* ------------------------ tgl widgetbehaviour----------------------------- */

static void toggle_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1, int *xp2, int *yp2)
{
    t_toggle *x = (t_toggle *)z;

    *xp1 = text_xpix(&x->x_gui.x_obj, glist);
    *yp1 = text_ypix(&x->x_gui.x_obj, glist);
    *xp2 = *xp1 + x->x_gui.x_w;
    *yp2 = *yp1 + x->x_gui.x_h;

	iemgui_label_getrect(x->x_gui, glist, xp1, yp1, xp2, yp2);
}

static void toggle_save(t_gobj *z, t_binbuf *b)
{
    t_toggle *x = (t_toggle *)z;
    int bflcol[3];
    t_symbol *srl[3];

    iemgui_save(&x->x_gui, srl, bflcol);
    binbuf_addv(b, "ssiisiisssiiiiiiiff", gensym("#X"),gensym("obj"),
                (int)x->x_gui.x_obj.te_xpix,
                (int)x->x_gui.x_obj.te_ypix,
                gensym("tgl"), x->x_gui.x_w,
                iem_symargstoint(&x->x_gui.x_isa),
                srl[0], srl[1], srl[2],
                x->x_gui.x_ldx, x->x_gui.x_ldy,
                iem_fstyletoint(&x->x_gui.x_fsf), x->x_gui.x_fontsize,
                bflcol[0], bflcol[1], bflcol[2], x->x_on, x->x_nonzero);
    binbuf_addv(b, ";");
}

static void toggle_properties(t_gobj *z, t_glist *owner)
{
    t_toggle *x = (t_toggle *)z;
    char buf[800];
    t_symbol *srl[3];

    iemgui_properties(&x->x_gui, srl);
    sprintf(buf, "pdtk_iemgui_dialog %%s |tgl| \
            ----------dimensions(pix):----------- %d %d size: 0 0 empty \
            -----------non-zero-value:----------- %g value: 0.0 empty %g \
            -1 lin log %d %d empty %d \
            {%s} {%s} \
            {%s} %d %d \
            %d %d \
            %d %d %d\n",
            x->x_gui.x_w, IEM_GUI_MINSIZE,
            x->x_nonzero, 1.0,/*non_zero-schedule*/
            x->x_gui.x_isa.x_loadinit, -1, -1,/*no multi*/
            srl[0]->s_name, srl[1]->s_name,
            srl[2]->s_name, x->x_gui.x_ldx, x->x_gui.x_ldy,
            x->x_gui.x_fsf.x_font_style, x->x_gui.x_fontsize,
            0xffffff & x->x_gui.x_bcol, 0xffffff & x->x_gui.x_fcol, 0xffffff & x->x_gui.x_lcol);
    gfxstub_new(&x->x_gui.x_obj.ob_pd, x, buf);
}

static void toggle_bang(t_toggle *x)
{
	x->x_gui.x_changed = 1;
    x->x_on = (x->x_on==0.0)?x->x_nonzero:0.0;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
    if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
        pd_float(x->x_gui.x_snd->s_thing, x->x_on);
}

static void toggle_dialog(t_toggle *x, t_symbol *s, int argc, t_atom *argv)
{
	canvas_apply_setundo(x->x_gui.x_glist, (t_gobj *)x);

    t_symbol *srl[3];
    int a = (int)atom_getintarg(0, argc, argv);
    t_float nonzero = (t_float)atom_getfloatarg(2, argc, argv);
    int sr_flags;

    if(nonzero == 0.0)
        nonzero = 1.0;
    x->x_nonzero = nonzero;
    if(x->x_on != 0.0)
        x->x_on = x->x_nonzero;
    sr_flags = iemgui_dialog(&x->x_gui, srl, argc, argv);
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_CONFIG);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO + sr_flags);
    //(*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_MOVE);
    //canvas_fixlinesfor(glist_getcanvas(x->x_gui.x_glist), (t_text*)x);
	iemgui_shouldvis((void *)x, &x->x_gui, IEM_GUI_DRAW_MODE_MOVE);

	/* forcing redraw of the scale handle */
	if (x->x_gui.x_fsf.x_selected) {
		toggle_draw_select(x, x->x_gui.x_glist);
	}

	//ico@bukvic.net 100518 update scrollbars when object potentially exceeds window size
    t_canvas *canvas=(t_canvas *)glist_getcanvas(x->x_gui.x_glist);
	sys_vgui("pdtk_canvas_getscroll .x%lx.c\n", (long unsigned int)canvas);
}

static void toggle_click(t_toggle *x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{toggle_bang(x);}

static int toggle_newclick(t_gobj *z, struct _glist *glist, int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
    if(doit)
        toggle_click((t_toggle *)z, (t_floatarg)xpix, (t_floatarg)ypix, (t_floatarg)shift, 0, (t_floatarg)alt);
    return (1);
}

static void toggle_set(t_toggle *x, t_floatarg f)
{
	if (x->x_on != f) x->x_gui.x_changed = 1;
    x->x_on = f;
    if(f != 0.0)
        x->x_nonzero = f;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void toggle_float(t_toggle *x, t_floatarg f)
{
    toggle_set(x, f);
    if(x->x_gui.x_fsf.x_put_in2out)
    {
        outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
        if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
            pd_float(x->x_gui.x_snd->s_thing, x->x_on);
    }
}

static void toggle_fout(t_toggle *x, t_floatarg f)
{
    toggle_set(x, f);
    outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
    if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
        pd_float(x->x_gui.x_snd->s_thing, x->x_on);
}

static void toggle_loadbang(t_toggle *x)
{
    if(!sys_noloadbang && x->x_gui.x_isa.x_loadinit)
        toggle_fout(x, (t_float)x->x_on);
}

static void toggle_size(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{
    x->x_gui.x_w = iemgui_clip_size((int)atom_getintarg(0, ac, av));
    x->x_gui.x_h = x->x_gui.x_w;
    iemgui_size((void *)x, &x->x_gui);
}

static void toggle_delta(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_delta((void *)x, &x->x_gui, s, ac, av);}

static void toggle_pos(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_pos((void *)x, &x->x_gui, s, ac, av);}

static void toggle_color(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_color((void *)x, &x->x_gui, s, ac, av);}

static void toggle_send(t_toggle *x, t_symbol *s)
{iemgui_send(x, &x->x_gui, s);}

static void toggle_receive(t_toggle *x, t_symbol *s)
{iemgui_receive(x, &x->x_gui, s);}

static void toggle_label(t_toggle *x, t_symbol *s)
{iemgui_label((void *)x, &x->x_gui, s);}

static void toggle_label_font(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_label_font((void *)x, &x->x_gui, s, ac, av);}

static void toggle_label_pos(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_label_pos((void *)x, &x->x_gui, s, ac, av);}

static void toggle_init(t_toggle *x, t_floatarg f)
{
    x->x_gui.x_isa.x_loadinit = (f==0.0)?0:1;
}

static void toggle_nonzero(t_toggle *x, t_floatarg f)
{
    if(f != 0.0)
        x->x_nonzero = f;
}

static void *toggle_new(t_symbol *s, int argc, t_atom *argv)
{
    t_toggle *x = (t_toggle *)pd_new(toggle_class);
    int bflcol[]={-262144, -1, -1};
    int a=IEM_GUI_DEFAULTSIZE, f=0;
    int ldx=17, ldy=7;
    int fs=10;
    t_float on=0.0, nonzero=1.0;
    char str[144];

    iem_inttosymargs(&x->x_gui.x_isa, 0);
    iem_inttofstyle(&x->x_gui.x_fsf, 0);

    if(((argc == 13)||(argc == 14))&&IS_A_FLOAT(argv,0)
       &&IS_A_FLOAT(argv,1)
       &&(IS_A_SYMBOL(argv,2)||IS_A_FLOAT(argv,2))
       &&(IS_A_SYMBOL(argv,3)||IS_A_FLOAT(argv,3))
       &&(IS_A_SYMBOL(argv,4)||IS_A_FLOAT(argv,4))
       &&IS_A_FLOAT(argv,5)&&IS_A_FLOAT(argv,6)
       &&IS_A_FLOAT(argv,7)&&IS_A_FLOAT(argv,8)&&IS_A_FLOAT(argv,9)
       &&IS_A_FLOAT(argv,10)&&IS_A_FLOAT(argv,11)&&IS_A_FLOAT(argv,12))
    {
        a = (int)atom_getintarg(0, argc, argv);
        iem_inttosymargs(&x->x_gui.x_isa, atom_getintarg(1, argc, argv));
        iemgui_new_getnames(&x->x_gui, 2, argv);
        ldx = (int)atom_getintarg(5, argc, argv);
        ldy = (int)atom_getintarg(6, argc, argv);
        iem_inttofstyle(&x->x_gui.x_fsf, atom_getintarg(7, argc, argv));
        fs = (int)atom_getintarg(8, argc, argv);
        bflcol[0] = (int)atom_getintarg(9, argc, argv);
        bflcol[1] = (int)atom_getintarg(10, argc, argv);
        bflcol[2] = (int)atom_getintarg(11, argc, argv);
        on = (t_float)atom_getfloatarg(12, argc, argv);
    }
    else iemgui_new_getnames(&x->x_gui, 2, 0);
    if((argc == 14)&&IS_A_FLOAT(argv,13))
        nonzero = (t_float)atom_getfloatarg(13, argc, argv);
    x->x_gui.x_draw = (t_iemfunptr)toggle_draw;

    x->x_gui.x_fsf.x_snd_able = 1;
    x->x_gui.x_fsf.x_rcv_able = 1;
    x->x_gui.x_glist = (t_glist *)canvas_getcurrent();
    if (!strcmp(x->x_gui.x_snd->s_name, "empty"))
        x->x_gui.x_fsf.x_snd_able = 0;
    if (!strcmp(x->x_gui.x_rcv->s_name, "empty"))
        x->x_gui.x_fsf.x_rcv_able = 0;
    if(x->x_gui.x_fsf.x_font_style == 1) strcpy(x->x_gui.x_font, "helvetica");
    else if(x->x_gui.x_fsf.x_font_style == 2) strcpy(x->x_gui.x_font, "times");
    else { x->x_gui.x_fsf.x_font_style = 0;
        strcpy(x->x_gui.x_font, sys_font); }
    x->x_nonzero = (nonzero!=0.0)?nonzero:1.0;
    if(x->x_gui.x_isa.x_loadinit)
        x->x_on = (on!=0.0)?nonzero:0.0;
    else
        x->x_on = 0.0;
    if (x->x_gui.x_fsf.x_rcv_able)
        pd_bind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
    x->x_gui.x_ldx = ldx;
    x->x_gui.x_ldy = ldy;

    if(fs < 4)
        fs = 4;
    x->x_gui.x_fontsize = fs;
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    iemgui_all_colfromload(&x->x_gui, bflcol);
    iemgui_verify_snd_ne_rcv(&x->x_gui);
    outlet_new(&x->x_gui.x_obj, &s_float);

	/* scale handle init */
    t_scalehandle *sh;
    char buf[64];
    x->x_gui.x_handle = pd_new(scalehandle_class);
    sh = (t_scalehandle *)x->x_gui.x_handle;
    sh->h_master = (t_gobj*)x;
    sprintf(buf, "_h%lx", (t_int)sh);
    pd_bind(x->x_gui.x_handle, sh->h_bindsym = gensym(buf));
    sprintf(sh->h_outlinetag, "h%lx", (t_int)sh);
    sh->h_dragon = 0;
	sh->h_scale = 1;
	x->x_gui.scale_offset_x = 0;
	x->x_gui.scale_offset_y = 0;
	x->x_gui.scale_vis = 0;

	/* label handle init */
	t_scalehandle *lh;
	char lhbuf[64];
	x->x_gui.x_lhandle = pd_new(scalehandle_class);
	lh = (t_scalehandle *)x->x_gui.x_lhandle;
	lh->h_master = (t_gobj*)x;
	sprintf(lhbuf, "_h%lx", (t_int)lh);
	pd_bind(x->x_gui.x_lhandle, lh->h_bindsym = gensym(lhbuf));
	sprintf(lh->h_outlinetag, "h%lx", (t_int)lh);
	lh->h_dragon = 0;
	lh->h_scale = 0;
	x->x_gui.label_offset_x = 0;
	x->x_gui.label_offset_y = 0;
	x->x_gui.label_vis = 0;

	x->x_gui.x_obj.te_iemgui = 1;

	x->x_gui.x_changed = 1;

    return (x);
}

static void toggle_ff(t_toggle *x)
{
    if(x->x_gui.x_fsf.x_rcv_able)
        pd_unbind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
    gfxstub_deleteforkey(x);

	/* scale handle deconstructor */
    if (x->x_gui.x_handle)
    {
		pd_unbind(x->x_gui.x_handle, ((t_scalehandle *)x->x_gui.x_handle)->h_bindsym);
		pd_free(x->x_gui.x_handle);
    }

	/* label handle deconstructor */
	if (x->x_gui.x_lhandle)
	{
		pd_unbind(x->x_gui.x_lhandle, ((t_scalehandle *)x->x_gui.x_lhandle)->h_bindsym);
		pd_free(x->x_gui.x_lhandle);
	}
}

void g_toggle_setup(void)
{
    toggle_class = class_new(gensym("tgl"), (t_newmethod)toggle_new,
                             (t_method)toggle_ff, sizeof(t_toggle), 0, A_GIMME, 0);
    class_addcreator((t_newmethod)toggle_new, gensym("toggle"), A_GIMME, 0);
    class_addbang(toggle_class, toggle_bang);
    class_addfloat(toggle_class, toggle_float);
    class_addmethod(toggle_class, (t_method)toggle_click, gensym("click"),
                    A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(toggle_class, (t_method)toggle_dialog, gensym("dialog"),
                    A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_loadbang, gensym("loadbang"), 0);
    class_addmethod(toggle_class, (t_method)toggle_set, gensym("set"), A_FLOAT, 0);
    class_addmethod(toggle_class, (t_method)toggle_size, gensym("size"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_delta, gensym("delta"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_pos, gensym("pos"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_color, gensym("color"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_send, gensym("send"), A_DEFSYM, 0);
    class_addmethod(toggle_class, (t_method)toggle_receive, gensym("receive"), A_DEFSYM, 0);
    class_addmethod(toggle_class, (t_method)toggle_label, gensym("label"), A_DEFSYM, 0);
    class_addmethod(toggle_class, (t_method)toggle_label_pos, gensym("label_pos"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_label_font, gensym("label_font"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_init, gensym("init"), A_FLOAT, 0);
    class_addmethod(toggle_class, (t_method)toggle_nonzero, gensym("nonzero"), A_FLOAT, 0);
 
    scalehandle_class = class_new(gensym("_scalehandle"), 0, 0,
				  sizeof(t_scalehandle), CLASS_PD, 0);
    class_addmethod(scalehandle_class, (t_method)toggle__clickhook,
		    gensym("_click"), A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(scalehandle_class, (t_method)toggle__motionhook,
		    gensym("_motion"), A_FLOAT, A_FLOAT, 0);

    toggle_widgetbehavior.w_getrectfn = toggle_getrect;
    toggle_widgetbehavior.w_displacefn = iemgui_displace;
    toggle_widgetbehavior.w_selectfn = iemgui_select;
    toggle_widgetbehavior.w_activatefn = NULL;
    toggle_widgetbehavior.w_deletefn = iemgui_delete;
    toggle_widgetbehavior.w_visfn = iemgui_vis;
    toggle_widgetbehavior.w_clickfn = toggle_newclick;
	toggle_widgetbehavior.w_displacefnwtag = iemgui_displace_withtag;
    class_setwidget(toggle_class, &toggle_widgetbehavior);
    class_sethelpsymbol(toggle_class, gensym("toggle"));
    class_setsavefn(toggle_class, toggle_save);
    class_setpropertiesfn(toggle_class, toggle_properties);
}
