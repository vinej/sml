#include "../kexpr.h"
#include "plot.h"
#include <plplot/plplot.h>
#include <plplot/plplotP.h>
#include <stdio.h>
#include <Windows.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#if defined(_MSC_VER) || defined(_WIN32)
#define SML_EXPORT __declspec(dllexport) 
#define SML_CALL __cdecl
#else
#define SML_EXPORT __declspec(dllexport) 
#define SML_CALL __cdecl
#endif


#include "..\stb\stb_image.h"
#include "..\stb\stb_image_write.h"
#include "..\stb\stb_image_resize.h"
//#include "..\stb\stb_image_resize.h"


//ke1_t ** sml->fields = NULL;
int(*dllke_hash_add)(sml_t* sml, fncp key, char * name) = NULL;
void dllke_plot_hash(sml_t* sml);

SML_EXPORT int SML_CALL ke_dll_hash_add(sml_t* sml) {
	dllke_plot_hash(sml);
	return 0;
}

SML_EXPORT int SML_CALL ke_plimload(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *filename, *x, *y, *n, *zero;
	zero = stack[--top];
	n = stack[--top];
	y = stack[--top];
	x = stack[--top];
	filename = stack[top-1];

	int xx,yy,nn;

	unsigned char *data = stbi_load(filename->obj.s,&xx,&yy,&nn,(int)zero->i);
	//ke_inc_memory();
	filename->vtype = KEV_IMAGE;
	filename->obj.image = data;
	sml->fields[x->ifield]->i = xx;
	sml->fields[y->ifield]->i = yy;
	sml->fields[n->ifield]->i = nn;
	sml->fields[x->ifield]->r = xx;
	sml->fields[y->ifield]->r = yy;
	sml->fields[n->ifield]->r = nn;
	sml->fields[x->ifield]->vtype = KEV_INT;
	sml->fields[y->ifield]->vtype = KEV_INT;
	sml->fields[n->ifield]->vtype = KEV_INT;
	return top;
}

SML_EXPORT int __cdecl ke_plimfree(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p = stack[--top];
	stbi_image_free(p->obj.image);
	//ke_dec_memory();
	return top;
}

SML_EXPORT int __cdecl ke_plimwrite(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *filename, *w, *h, *comp, *data, *quality, *type;

	type = stack[--top];
	quality = stack[--top];
	data = stack[--top];
	comp = stack[--top];
	h = stack[--top];
	w = stack[--top];
	filename = stack[--top];
	stbi_flip_vertically_on_write(1);
	int st;
	char * name = type->obj.s;
	if (strcmp(name, "png") == 0) {
		st = stbi_write_png(filename->obj.s, (int)w->i, (int)h->i, (int)comp->i, data->obj.image, (int)quality->i);
	} else if (strcmp(name, "bmp") == 0) {
		st = stbi_write_bmp(filename->obj.s, (int)w->i, (int)h->i, (int)comp->i, data->obj.image);
	}
	else if (strcmp(name, "tga") == 0) {
		st = stbi_write_tga(filename->obj.s, (int)w->i, (int)h->i, (int)comp->i, data->obj.image);
	} 
	else if (strcmp(name, "jpg") == 0) {
		st = stbi_write_jpg(filename->obj.s, (int)w->i, (int)h->i, (int)comp->i, data->obj.image, (int)quality->i);
	}
	else if (strcmp(name, "hdr") == 0) {
		st = stbi_write_hdr(filename->obj.s, (int)w->i, (int)h->i, (int)comp->i, (float *)data->obj.image);
	}
	return top;
}

SML_EXPORT int __cdecl ke_plimresize(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *in, *in_w, *in_h, *in_stride, *out_w, *out_h, *out_stride, *nb_channel;

	nb_channel = stack[--top];
	out_stride = stack[--top];
	out_h = stack[--top];
	out_w = stack[--top];
	in_stride = stack[--top];
	in_h = stack[--top];
	in_w = stack[--top];
	in = stack[top-1];

	char *tout = malloc(out_h->i * out_w->i * nb_channel->i);
	stbir_resize_uint8(in->obj.image, (int)in_w->i, (int)in_h->i, (int)in_stride->i,
		tout, (int)out_w->i, (int)out_h->i, (int)out_stride->i, (int)nb_channel->i);
	in->vtype = KEV_IMAGE;
	in->ttype = KET_VAL;
	in->obj.image = tout;
	return top;
}

SML_EXPORT int __cdecl ke_plsdev(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p;
	p = stack[--top];
	plsdev(p->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plinit(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p;
	p = stack[--top];
	plsdev(p->obj.s);
	plinit();
	return top;
}

SML_EXPORT int __cdecl ke_plenv(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *xmax, *ymin, *ymax, *just, *axis;
	axis = stack[--top];
	just = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	plenv((PLFLT)xmin->r, (PLFLT)xmax->r, (PLFLT)ymin->r, (PLFLT)ymax->r, (PLINT)just->i, (PLINT)axis->i);
	return top;
}

SML_EXPORT int __cdecl ke_plline(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y;
	y = stack[--top];
	x = stack[--top]; 
	n = stack[--top];
	plline((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0));
	return top;
}

SML_EXPORT int __cdecl ke_plpause(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p;
	p = stack[--top];
	plspause((PLBOOL)p->i);
	return top;
}

SML_EXPORT int __cdecl ke_plend(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plend();
	return --top;
}

SML_EXPORT int __cdecl ke_pl_setcontlabelformat(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *lexp, *sigdig;
	sigdig = stack[--top];
	lexp = stack[--top];

	pl_setcontlabelformat((PLINT)lexp->i, (PLINT)sigdig->i);
	return top;
}

SML_EXPORT int __cdecl ke_pl_setcontlabelparam(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *offset, *size, *spacing, *active;
	active = stack[--top];
	spacing = stack[--top];
	size = stack[--top];
	offset = stack[--top];

	pl_setcontlabelparam((PLFLT)offset->r, (PLFLT)size->r, (PLFLT)spacing->r, (PLINT)active->i);
	return top;
}

SML_EXPORT int __cdecl ke_pladv(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *page;
	page = stack[--top];
	pladv((PLINT)page->i);
	return top;
}

SML_EXPORT int __cdecl ke_plarc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *a, *b, *angle1, *angle2, *rotate, *fill;
	fill = stack[--top];
	rotate = stack[--top];
	angle2 = stack[--top];
	angle1 = stack[--top];
	b = stack[--top];
	a = stack[--top];
	y = stack[--top];
	x = stack[--top];
	plarc((PLFLT)x->r, (PLFLT)y->r, (PLFLT)a->r, (PLFLT)b->r, (PLFLT)angle1->r, (PLFLT)angle2->r, (PLFLT)rotate->r,(PLBOOL)x->i);
	return top;
}

SML_EXPORT int __cdecl ke_plaxes(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x0, *y0, *xopt, *xtick, *nxsub, *yopt, *ytick, *nysub;
	nysub= stack[--top];
	ytick = stack[--top];
	yopt = stack[--top];
	nxsub = stack[--top];
	xtick = stack[--top];
	xopt = stack[--top];
	y0 = stack[--top];
	x0 = stack[--top];
	plaxes((PLFLT)x0->r, (PLFLT)y0->r, (PLCHAR_VECTOR)xopt->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i,
		(PLCHAR_VECTOR)yopt->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i);
	return top;
}

SML_EXPORT int __cdecl ke_plbin(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *nbin, *x, *y, *opt;
	opt = stack[--top];
	y = stack[--top];
	x = stack[--top];
	nbin = stack[--top];
	plbin((PLINT)nbin->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0), (PLINT)opt->i);
	return top;
}

SML_EXPORT int __cdecl ke_plbop(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plbop();
	return --top;
}

SML_EXPORT int __cdecl ke_plbox(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xopt, *xtick, *nxsub, *yopt, *ytick, *nysub;
	nysub = stack[--top];
	ytick = stack[--top];
	yopt = stack[--top];
	nxsub = stack[--top];
	xtick = stack[--top];
	xopt = stack[--top];
	plbox((PLCHAR_VECTOR)xopt->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i);
	return top;
}

SML_EXPORT int __cdecl ke_plbox3(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xopt, *xlabel, *xtick, *nxsub, *yopt, *ylabel, *ytick, *nysub, *zopt, *zlabel, *ztick, *nzsub;
	nzsub = stack[--top];
	ztick = stack[--top];
	zlabel = stack[--top];
	zopt = stack[--top];
	nysub = stack[--top];
	ytick = stack[--top];
	ylabel = stack[--top];
	yopt = stack[--top];
	nxsub = stack[--top];
	xtick = stack[--top];
	xlabel = stack[--top];
	xopt = stack[--top];
	plbox3((PLCHAR_VECTOR)xopt->obj.s, (PLCHAR_VECTOR)xlabel->obj.s, 
		(PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLCHAR_VECTOR)ylabel->obj.s, 
		(PLFLT)ytick->r, (PLINT)nysub->i, (PLCHAR_VECTOR)zopt->obj.s, (PLCHAR_VECTOR)zlabel->obj.s, 
		(PLFLT)ztick->r, (PLINT)nzsub->i);
	return top;
}

SML_EXPORT int __cdecl ke_plbtime(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *year, *month, *day, *hour, *min, *sec, *ctime;
	ctime = stack[--top];
	sec = stack[--top];
	min = stack[--top];
	hour = stack[--top];
	day = stack[--top];
	month = stack[--top];
	year = stack[--top];


	ctime = stack[--top];
	sml->fields[year->ifield]->vtype = KEV_INT;
	sml->fields[month->ifield]->vtype = KEV_INT;
	sml->fields[day->ifield]->vtype = KEV_INT;
	sml->fields[hour->ifield]->vtype = KEV_INT;
	sml->fields[min->ifield]->vtype = KEV_INT;
	sml->fields[sec->ifield]->vtype = KEV_REAL;
	plbtime((PLINT_NC_SCALAR)(&(sml->fields[year->ifield]->i)),
			(PLINT_NC_SCALAR)(&(sml->fields[month->ifield]->i)),
			(PLINT_NC_SCALAR)(&(sml->fields[day->ifield]->i)),
			(PLINT_NC_SCALAR)(&(sml->fields[hour->ifield]->i)),
			(PLINT_NC_SCALAR)(&(sml->fields[min->ifield]->i)),
			(PLFLT_NC_SCALAR)(&(sml->fields[sec->ifield]->r)),
			(PLFLT)ctime->r);
	return top;
}

SML_EXPORT int __cdecl ke_plctime(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *year, *month, *day, *hour, *min, *sec, *ctime;
	ctime = stack[--top];
	sec = stack[--top];
	min = stack[--top];
	hour = stack[--top];
	day = stack[--top];
	month = stack[--top];
	year = stack[--top];
	sml->fields[ctime->ifield]->vtype = KEV_REAL;
	plctime((PLINT)year->i, (PLINT)month->i-1, (PLINT)day->i, (PLINT)hour->i, (PLINT)min->i, (PLFLT)sec->r, (PLFLT_NC_SCALAR)(&(sml->fields[ctime->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plcalc_world(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *rx, *ry, *wx, *wy, *window;
	window = stack[--top];
	wy = stack[--top];
	wx = stack[--top];
	ry = stack[--top];
	rx = stack[--top];
	sml->fields[wx->ifield]->vtype = KEV_REAL;
	sml->fields[wy->ifield]->vtype = KEV_REAL;
	sml->fields[window->ifield]->vtype = KEV_INT;
	plcalc_world((PLFLT)rx->r, (PLFLT)ry->r,
		(PLFLT_NC_SCALAR)(&(sml->fields[wx->ifield]->r)), 
		(PLFLT_NC_SCALAR)(&(sml->fields[wy->ifield]->r)), 
		(PLINT_NC_SCALAR)(&(sml->fields[window->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plclear(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plclear();
	return top;
}

SML_EXPORT int __cdecl ke_plcol0(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *col0;
	col0 = stack[--top];
	plcol0((PLINT)col0->i);
	return top;
}

SML_EXPORT int __cdecl ke_plcol1(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *col1;
	col1 = stack[--top];
	plcol1((PLFLT)col1->r);
	return top;
}

SML_EXPORT int __cdecl ke_plcolorbar(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_colorbar_width, *p_colorbar_height, *opt, *position, *x, *y, *x_length, *y_length,
		*bg_color, *bb_color, *bb_style, *low_cap_color, *high_cap_color, *cont_color, *cont_width,
		*n_labels, *label_opts, *labels, *naxes, *axis_opts, *ticks, *sub_ticks, *n_values, *values;

	values = stack[--top];
	n_values = stack[--top];
	sub_ticks = stack[--top];
	ticks = stack[--top];
	axis_opts = stack[--top];
	naxes = stack[--top];
	labels = stack[--top];
	label_opts = stack[--top];
	n_labels = stack[--top];
	cont_width = stack[--top];
	cont_color = stack[--top];
	high_cap_color = stack[--top];
	low_cap_color = stack[--top];
	bb_style = stack[--top];
	bb_color = stack[--top];
	bg_color = stack[--top];
	y_length = stack[--top];
	x_length = stack[--top];
	y = stack[--top];
	x = stack[--top];
	position = stack[--top];
	opt = stack[--top];
	p_colorbar_height = stack[--top];
	p_colorbar_width = stack[--top];

	sml->fields[p_colorbar_width->ifield]->vtype = KEV_REAL;
	sml->fields[p_colorbar_height->ifield]->vtype = KEV_REAL;
	plcolorbar(
		(PLFLT_NC_SCALAR)(&(sml->fields[p_colorbar_width->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_colorbar_height->ifield]->r)),
		(PLINT)opt->i,
		(PLINT)position->i,
		(PLFLT)x->r,
		(PLFLT)y->r,
		(PLFLT)x_length->r,
		(PLFLT)y_length->r,
		(PLINT)bg_color->i,
		(PLINT)bb_color->i,
		(PLINT)bb_style->i,
		(PLFLT)low_cap_color->r,
		(PLFLT)high_cap_color->r,
		(PLINT)cont_color->i,
		(PLFLT)cont_width->r,
		(PLINT)n_labels->i,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(label_opts->obj.vector_int,0),
		(PLCHAR_MATRIX)labels->obj.s,
		(PLINT)naxes->i,
		(PLCHAR_MATRIX)axis_opts->obj.s,
		(PLFLT_VECTOR)gsl_vector_const_ptr(ticks->obj.vector,0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(sub_ticks->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(n_values->obj.vector_int, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(label_opts->obj.matrix, 0,0));
	sml->fields[p_colorbar_width->ifield]->vtype = KEV_REAL;
	sml->fields[p_colorbar_height->ifield]->vtype = KEV_REAL;
	return top;
}

SML_EXPORT int __cdecl ke_plconfigtime(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *scale, *oﬀset1, *oﬀset2, *ccontrol, *ifbtime_oﬀset, *year, *month, *day, *hour, *min, *sec;
	sec = stack[--top];
	min = stack[--top];
	hour = stack[--top];
	day = stack[--top];
	month = stack[--top];
	year = stack[--top];
	ifbtime_oﬀset = stack[--top];
	ccontrol = stack[--top];
	oﬀset2 = stack[--top];
	oﬀset1 = stack[--top];
	scale = stack[--top];

	plconfigtime((PLFLT)scale->r, (PLFLT)oﬀset1->r, (PLFLT)oﬀset2->r, (PLINT)ccontrol->i, 
		(PLINT)ifbtime_oﬀset->i, (PLINT)year->i, (PLINT)month->i, (PLINT)day->i, 
		(PLINT)hour->i, (PLINT)min->i, (PLFLT)sec->r);
	return top;
}

SML_EXPORT int __cdecl ke_plcont(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *f, *nx, *ny, *kx, *lx, *ky, *ly, *clevel, *nlevel, *pltr, *pltr_data;

	pltr_data = stack[--top];
	pltr = stack[--top];
	nlevel = stack[--top];
	clevel = stack[--top];
	ly = stack[--top];
	ky = stack[--top];
	lx = stack[--top];
	kx = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	f = stack[--top];

	plcont((PLFLT_MATRIX)gsl_matrix_ptr(f->obj.matrix,0,0), (PLINT)nx->i, (PLINT)ny->i, (PLINT)kx->i, (PLINT)lx->i,
		(PLINT)ky->i, (PLINT)ly->i, (PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0), (PLINT)nlevel->i,
		(PLTRANSFORM_callback)pltr->obj.tokp, (PLPointer)pltr_data->obj.tokp);
	return top;
}

SML_EXPORT int __cdecl ke_plcpstrm(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *iplsr, *ﬂags;
	ﬂags = stack[--top];
	iplsr = stack[--top];

	plcpstrm((PLINT)iplsr->i, (PLBOOL)ﬂags->i);
	return top;
}

SML_EXPORT int __cdecl ke_plend1(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plend1();
	return top;
}

SML_EXPORT int __cdecl ke_plenv0(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *xmax, *ymin, *ymax, *just, *axis;
	axis = stack[--top];
	just = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	plenv0((PLFLT)xmin->r, (PLFLT)xmax->r, (PLFLT)ymin->r, (PLFLT)ymax->r, (PLINT)just->i, (PLINT)axis->i);
	return top;
}

SML_EXPORT int __cdecl ke_pleop(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	pleop();
	return top;
}

SML_EXPORT int __cdecl ke_plerrx(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *xmin, *xmax, *y;
	y = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	n = stack[--top];
	plerrx((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(xmin->obj.vector, 0),
						(PLFLT_VECTOR)gsl_vector_const_ptr(xmax->obj.vector, 0),
						(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plerry(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *ymin, *ymax, *y;
	y = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	n = stack[--top];
	plerry((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(ymin->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(ymax->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plfamadv(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plfamadv();
	return top;
}

SML_EXPORT int __cdecl ke_plfill(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y;
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];
	plfill((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plfill3(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *z;
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];
	plfill3((PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plflush(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plflush();
	return top;
}

SML_EXPORT int __cdecl ke_plfont(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *ifont;
	ifont = stack[--top];
	plfont((PLINT)ifont->i);
	return top;
}

SML_EXPORT int __cdecl ke_plfontld(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *fnt;
	fnt = stack[--top];
	plfontld((PLINT)fnt->i);
	return top;
}

SML_EXPORT int __cdecl ke_plGetCursor(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *gin;
	gin = stack[--top];
	gin->i = plGetCursor((PLGraphicsIn *)gin->obj.plgrin);
	gin->vtype = KEV_INT;
	gin->ttype = KET_VAL;
	return top;
}

SML_EXPORT int __cdecl ke_plgchr(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_def, *p_ht;
	p_ht = stack[--top];
	p_def = stack[--top];

	plgchr((PLFLT_NC_SCALAR)(&(sml->fields[p_def->ifield]->r)), (PLFLT_NC_SCALAR)(&(sml->fields[p_ht->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgcmap1_range(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *min_color, *max_color;
	max_color = stack[--top];
	min_color = stack[--top];

	sml->fields[min_color->ifield]->vtype = KEV_REAL;
	sml->fields[max_color->ifield]->vtype = KEV_REAL;
	plgcmap1_range((PLFLT_NC_SCALAR)(&(sml->fields[min_color->ifield]->r)), (PLFLT_NC_SCALAR)(&(sml->fields[max_color->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgcol0(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *icol0, *r, *g, *b;
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];
	icol0 = stack[--top];

	sml->fields[r->ifield]->vtype = KEV_INT;
	sml->fields[g->ifield]->vtype = KEV_INT;
	sml->fields[b->ifield]->vtype = KEV_INT;
	plgcol0((PLINT)icol0->i, 
		(PLINT_NC_SCALAR)(&(sml->fields[r->ifield]->i)), 
		(PLINT_NC_SCALAR)(&(sml->fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[b->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgcol0a(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *icol0, *r, *g, *b, *alpha;
	alpha = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];
	icol0 = stack[--top];
	sml->fields[r->ifield]->vtype = KEV_INT;
	sml->fields[g->ifield]->vtype = KEV_INT;
	sml->fields[b->ifield]->vtype = KEV_INT;
	sml->fields[alpha->ifield]->vtype = KEV_REAL;

	plgcol0a((PLINT)icol0->i,
		(PLINT_NC_SCALAR)(&(sml->fields[r->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[b->ifield]->i)),
		(PLFLT_NC_SCALAR)(&(sml->fields[alpha->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgcolbg(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b;
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];
	sml->fields[r->ifield]->vtype = KEV_INT;
	sml->fields[g->ifield]->vtype = KEV_INT;
	sml->fields[b->ifield]->vtype = KEV_INT;

	plgcolbg(
		(PLINT_NC_SCALAR)(&(sml->fields[r->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[b->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgcolbga(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b, *alpha;
	alpha = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	sml->fields[r->ifield]->vtype = KEV_INT;
	sml->fields[g->ifield]->vtype = KEV_INT;
	sml->fields[b->ifield]->vtype = KEV_INT;
	sml->fields[alpha->ifield]->vtype = KEV_REAL;

	plgcolbga(
		(PLINT_NC_SCALAR)(&(sml->fields[r->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[b->ifield]->i)),
		(PLFLT_NC_SCALAR)(&(sml->fields[alpha->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgcompression(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *compression;
	compression = stack[--top];

	sml->fields[compression->ifield]->vtype = KEV_INT;
	plgcompression((PLINT_NC_SCALAR)(&(sml->fields[compression->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgdidev(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_mar, *p_aspect, *p_jx, *p_jy;
	p_jy = stack[--top];
	p_jx = stack[--top];
	p_aspect = stack[--top];
	p_mar = stack[--top];

	sml->fields[p_mar->ifield]->vtype = KEV_REAL;
	sml->fields[p_aspect->ifield]->vtype = KEV_REAL;
	sml->fields[p_jx->ifield]->vtype = KEV_REAL;
	sml->fields[p_jy->ifield]->vtype = KEV_REAL;

	plgdidev(
		(PLFLT_NC_SCALAR)(&(sml->fields[p_mar->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_aspect->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_jx->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_jy->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgdiori(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_rot;
	p_rot = stack[--top];

	sml->fields[p_rot->ifield]->vtype = KEV_REAL;

	plgdiori((PLFLT_NC_SCALAR)(&(sml->fields[p_rot->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgdiplt(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_xmin, *p_ymin, *p_xmax, *p_ymax;
	p_ymax = stack[--top];
	p_xmax = stack[--top];
	p_ymin = stack[--top];
	p_xmin = stack[--top];

	sml->fields[p_xmin->ifield]->vtype = KEV_REAL;
	sml->fields[p_ymin->ifield]->vtype = KEV_REAL;
	sml->fields[p_xmax->ifield]->vtype = KEV_REAL;
	sml->fields[p_ymax->ifield]->vtype = KEV_REAL;

	plgdiplt(
		(PLFLT_NC_SCALAR)(&(sml->fields[p_xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_ymax->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_function_plgdrawmode(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p;
	p = stack[top - 1];
	stack[top++] = *p;
	p = stack[top - 1];
	p->i = (PLINT)plgdrawmode();
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

SML_EXPORT int __cdecl ke_plgfam(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_fam, *p_num, *p_bmax;
	p_bmax = stack[--top];
	p_num = stack[--top];
	p_fam = stack[--top];

	sml->fields[p_fam->ifield]->vtype = KEV_INT;
	sml->fields[p_num->ifield]->vtype = KEV_INT;
	sml->fields[p_bmax->ifield]->vtype = KEV_INT;

	plgfam(
		(PLINT_NC_SCALAR)(&(sml->fields[p_fam->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_num->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_bmax->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgfci(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_fci;
	p_fci = stack[--top];

	sml->fields[p_fci->ifield]->vtype = KEV_INT;

	plgfci((PLUNICODE_NC_SCALAR)(&(sml->fields[p_fci->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgfnam(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *fnam;
	fnam = stack[--top];

	sml->fields[fnam->ifield]->vtype = KEV_STR;

	plgfnam((PLCHAR_NC_VECTOR)(&(sml->fields[fnam->ifield]->obj.s)));
	return top;
}

SML_EXPORT int __cdecl ke_plgfont(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_family, *p_style, *p_weight;
	p_weight = stack[--top];
	p_style = stack[--top];
	p_family = stack[--top];

	sml->fields[p_family->ifield]->vtype = KEV_INT;
	sml->fields[p_style->ifield]->vtype = KEV_INT;
	sml->fields[p_weight->ifield]->vtype = KEV_INT;

	plgfont(
		(PLINT_NC_SCALAR)(&(sml->fields[p_family->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_style->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_weight->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plglevel(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_level;
	p_level = stack[--top];

	sml->fields[p_level->ifield]->vtype = KEV_INT;

	plglevel((PLINT_NC_SCALAR)(&(sml->fields[p_level->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgpage(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_xp, *p_yp, *p_xleng, *p_yleng, *p_xoﬀ, *p_yoﬀ;
	p_yoﬀ = stack[--top];
	p_xoﬀ = stack[--top];
	p_yleng = stack[--top];
	p_xleng = stack[--top];
	p_yp = stack[--top];
	p_xp = stack[--top];

	sml->fields[p_xp->ifield]->vtype = KEV_REAL;
	sml->fields[p_yp->ifield]->vtype = KEV_REAL;
	sml->fields[p_xleng->ifield]->vtype = KEV_INT;
	sml->fields[p_yleng->ifield]->vtype = KEV_INT;
	sml->fields[p_xoﬀ->ifield]->vtype = KEV_INT;
	sml->fields[p_yoﬀ->ifield]->vtype = KEV_INT;

	plgpage(
		(PLFLT_NC_SCALAR)(&(sml->fields[p_xp->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_yp->ifield]->r)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_xleng->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_yleng->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_xoﬀ->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_yoﬀ->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_function_plgra(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plgra();
	return top;
}

SML_EXPORT int __cdecl ke_plgradient(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *angle;
	angle = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];
	plgradient((PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT)angle->r);
	return top;
}

SML_EXPORT int __cdecl ke_plgriddata(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *npts, *xg, *nptsx, *yg, *nptsy, *zg, *type, *data;
	data = stack[--top];
	type = stack[--top];
	zg = stack[--top];
	nptsy = stack[--top];
	yg = stack[--top];
	nptsx = stack[--top];
	xg = stack[--top];
	npts = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	// must transform this matrix into a GSL matrix
	// JYV TODO
	PLFLT_NC_MATRIX return_matrix;

	plgriddata(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0),
		(PLINT)npts->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(xg->obj.vector, 0),
		(PLINT)nptsx->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(yg->obj.vector, 0),
		(PLINT)nptsy->i,
		(PLFLT_NC_MATRIX)&return_matrix,
		(PLINT)type->i,
		(PLFLT)data->r);
	return top;
}

SML_EXPORT int __cdecl ke_plgspa(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *xmax, *ymin, *ymax;
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];

	sml->fields[xmin->ifield]->vtype = KEV_REAL;
	sml->fields[xmax->ifield]->vtype = KEV_REAL;
	sml->fields[ymin->ifield]->vtype = KEV_REAL;
	sml->fields[ymax->ifield]->vtype = KEV_REAL;

	plgspa(
		(PLFLT_NC_SCALAR)(&(sml->fields[xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[ymax->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgstrm(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_strm;
	p_strm = stack[--top];

	sml->fields[p_strm->ifield]->vtype = KEV_INT;

	plgstrm((PLINT_NC_SCALAR)(&(sml->fields[p_strm->ifield]->i)));
		
	return top;
}

SML_EXPORT int __cdecl ke_plgver(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_ver;
	p_ver = stack[--top];

	sml->fields[p_ver->ifield]->vtype = KEV_STR;

	plgver((PLCHAR_NC_VECTOR)(&(sml->fields[p_ver->ifield]->obj.s)));

	return top;
}

SML_EXPORT int __cdecl ke_plgvpd(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_xmin, *p_xmax, *p_ymin, *p_ymax;
	p_ymax = stack[--top];
	p_ymin = stack[--top];
	p_xmax = stack[--top];
	p_xmin = stack[--top];

	sml->fields[p_xmin->ifield]->vtype = KEV_REAL;
	sml->fields[p_xmax->ifield]->vtype = KEV_REAL;
	sml->fields[p_ymin->ifield]->vtype = KEV_REAL;
	sml->fields[p_ymax->ifield]->vtype = KEV_REAL;

	plgvpd(
		(PLFLT_NC_SCALAR)(&(sml->fields[p_xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_ymax->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgvpw(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_xmin, *p_xmax, *p_ymin, *p_ymax;
	p_ymax = stack[--top];
	p_ymin = stack[--top];
	p_xmax = stack[--top];
	p_xmin = stack[--top];

	sml->fields[p_xmin->ifield]->vtype = KEV_REAL;
	sml->fields[p_xmax->ifield]->vtype = KEV_REAL;
	sml->fields[p_ymin->ifield]->vtype = KEV_REAL;
	sml->fields[p_ymax->ifield]->vtype = KEV_REAL;

	plgvpw(
		(PLFLT_NC_SCALAR)(&(sml->fields[p_xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_ymax->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plgxax(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_digmax, *p_digits;
	p_digits = stack[--top];
	p_digmax = stack[--top];

	sml->fields[p_digits->ifield]->vtype = KEV_INT;
	sml->fields[p_digits->ifield]->vtype = KEV_INT;

	plgxax(
		(PLINT_NC_SCALAR)(&(sml->fields[p_digits->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_digits->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgyax(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_digmax, *p_digits;
	p_digits = stack[--top];
	p_digmax = stack[--top];

	sml->fields[p_digits->ifield]->vtype = KEV_INT;
	sml->fields[p_digits->ifield]->vtype = KEV_INT;

	plgyax(
		(PLINT_NC_SCALAR)(&(sml->fields[p_digits->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_digits->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plgzax(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_digmax, *p_digits;
	p_digits = stack[--top];
	p_digmax = stack[--top];

	sml->fields[p_digits->ifield]->vtype = KEV_INT;
	sml->fields[p_digits->ifield]->vtype = KEV_INT;

	plgzax(
		(PLINT_NC_SCALAR)(&(sml->fields[p_digits->ifield]->i)),
		(PLINT_NC_SCALAR)(&(sml->fields[p_digits->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plhist(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *data, *datmin, *datmax, *nbin, *opt;
	opt = stack[--top];
	nbin = stack[--top];
	datmax = stack[--top];
	datmin = stack[--top];
	data = stack[--top];
	n = stack[--top];

	plhist(
		(PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(data->obj.vector, 0),
		(PLFLT)datmin->r, 
		(PLFLT)datmax->r, 
		(PLINT)nbin->i,
		(PLINT)opt->i);
	return top;
}

SML_EXPORT int __cdecl ke_plhlsrgb(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *h, *l, *s, *p_r, *p_g, *p_b;
	p_b = stack[--top];
	p_g = stack[--top];
	p_r = stack[--top];
	s = stack[--top];
	l = stack[--top];
	h = stack[--top];

	sml->fields[p_r->ifield]->vtype = KEV_REAL;
	sml->fields[p_g->ifield]->vtype = KEV_REAL;
	sml->fields[p_b->ifield]->vtype = KEV_REAL;

	plhlsrgb(
		(PLFLT)h->r, 
		(PLFLT)l->r, 
		(PLFLT)s->r,
		(PLFLT_NC_SCALAR)(&(sml->fields[p_r->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_g->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_b->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plimagefr(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *idata, *nx, *ny, *xmin, *xmax, *ymin, *ymax, *zmin, *zmax, *valuemin, *valuemax, *pltr, *pltr_data;
	pltr_data = stack[--top];
	pltr = stack[--top];
	valuemax = stack[--top];
	valuemin = stack[--top];
	zmax = stack[--top];
	zmin = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	idata = stack[--top];

	plimagefr(
		(PLFLT_MATRIX)gsl_matrix_const_ptr(idata->obj.matrix, 0, 0),
		(PLINT)nx->i, 
		(PLINT)ny->i, 
		(PLFLT)xmin->r, 
		(PLFLT)xmax->r, 
		(PLFLT)ymin->r, 
		(PLFLT)ymax->r, 
		(PLFLT)zmin->r, 
		(PLFLT)zmax->r, 
		(PLFLT)valuemin->r, 
		(PLFLT)valuemax->r, 
		(PLTRANSFORM_callback)pltr->obj.pltrcb,
		(PLPointer)pltr_data->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plimage(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *idata, *nx, *ny, *xmin, *xmax, *ymin, *ymax, *zmin, *zmax,  *Dxmin, *Dxmax, *Dymin, *Dymax;
	Dymax = stack[--top];
	Dymin = stack[--top];
	Dxmax = stack[--top];
	Dxmin = stack[--top];
	zmax = stack[--top];
	zmin = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	idata = stack[--top];

	plimage(
		(PLFLT_MATRIX)gsl_matrix_const_ptr(idata->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r,
		(PLFLT)zmin->r,
		(PLFLT)zmax->r,
		(PLFLT)Dxmin->r,
		(PLFLT)Dxmax->r,
		(PLFLT)Dymin->r,
		(PLFLT)Dymax->r);
	return top;
}

SML_EXPORT int __cdecl ke_pljoin(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x1, *y1, *x2, *y2;
	y2 = stack[--top];
	x2 = stack[--top];
	y1 = stack[--top];
	x1 = stack[--top];

	pljoin(
		(PLFLT)x1->r,
		(PLFLT)y1->r,
		(PLFLT)x2->r,
		(PLFLT)y2->r);
	return top;
}

SML_EXPORT int __cdecl ke_pllab(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xlabel, *ylabel, *tlabel;
	tlabel = stack[--top];
	ylabel = stack[--top];
	xlabel = stack[--top];

	pllab(
		(PLCHAR_VECTOR)xlabel->obj.s,
		(PLCHAR_VECTOR)ylabel->obj.s,
		(PLCHAR_VECTOR)tlabel->obj.s);
	return top;
}


SML_EXPORT int __cdecl ke_pllegend(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_legend_width, *p_legend_height, *opt, *position, *x, *y, 
		*plot_width, *bg_color, *bb_color, *bb_style, *nrow, *ncolumn,
		*nlegend, *opt_array, *text_oﬀset, *text_scale, *text_spacing, 
		*text_justiﬁcation, *text_colors, *text, *box_colors, *box_patterns, 
		*box_scales, *box_line_widths, *line_colors, *line_styles, *line_widths, 
		*symbol_colors, *symbol_scales, *symbol_numbers, *symbols;

	symbols = stack[--top];
	symbol_numbers = stack[--top];
	symbol_scales = stack[--top];
	symbol_colors = stack[--top];
	line_widths = stack[--top];
	line_styles = stack[--top];
	line_colors = stack[--top];
	box_line_widths = stack[--top];
	box_scales = stack[--top];
	box_patterns = stack[--top];
	box_colors = stack[--top];
	text = stack[--top];
	text_colors = stack[--top];
	text_justiﬁcation = stack[--top];
	text_spacing = stack[--top];
	text_scale = stack[--top];
	text_oﬀset = stack[--top];
	opt_array = stack[--top];
	nlegend = stack[--top];
	ncolumn = stack[--top];
	nrow = stack[--top];
	bb_style = stack[--top];
	bb_color = stack[--top];
	bg_color = stack[--top];
	plot_width = stack[--top];
	y = stack[--top];
	x = stack[--top];
	position = stack[--top];
	opt = stack[--top];
	p_legend_height = stack[--top];
	p_legend_width = stack[--top];

	pllegend(
		(PLFLT_NC_SCALAR)(&(sml->fields[p_legend_width->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_legend_height->ifield]->r)),
		(PLINT)opt->i, 
		(PLINT)position->i, 
		(PLFLT)x->r, 
		(PLFLT)y->r, 
		(PLFLT)plot_width->r, 
		(PLINT)bg_color->i,
		(PLINT)bb_color->i,
		(PLINT)bb_style->i,
		(PLINT)nrow->i,
		(PLINT)ncolumn->i,
		(PLINT)nlegend->i,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(opt_array->obj.vector_int, 0),
		(PLINT)text_oﬀset->i, 
		(PLFLT)text_scale->r, 
		(PLFLT)text_spacing->r, 
		(PLFLT)text_justiﬁcation->r, 
		(PLINT_VECTOR)gsl_vector_int_const_ptr(text_colors->obj.vector_int, 0),
		(PLCHAR_MATRIX)text->obj.s,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(box_colors->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(box_patterns->obj.vector_int, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(box_scales->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(box_line_widths->obj.vector, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(line_colors->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(line_styles->obj.vector_int, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(line_widths->obj.vector, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(symbol_colors->obj.vector_int, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(symbol_scales->obj.vector, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(symbol_numbers->obj.vector_int, 0),
		(PLCHAR_MATRIX)symbols->obj.s );
	return top;
}

SML_EXPORT int __cdecl ke_pllightsource(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z;
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	pllightsource(
		(PLFLT)x->r,
		(PLFLT)y->r,
		(PLFLT)z->r);
	return top;
}


SML_EXPORT int __cdecl ke_plline3(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *z;
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];

	plline3(
		(PLINT)n->r,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0));
	return top;
}

SML_EXPORT int __cdecl ke_pllsty(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *lin;
	lin = stack[--top];

	pllsty((PLINT)lin->r);
	return top;
}

// TODO gin
SML_EXPORT int __cdecl ke_plmap(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *mapform, *name, *minx, *maxx, *miny, *maxy;
	maxy = stack[--top];
	miny = stack[--top];
	maxx = stack[--top];
	minx = stack[--top];
	name = stack[--top];
	mapform = stack[--top];

	plmap(
		(PLMAPFORM_callback)mapform->obj.plmpcb, 
		(PLCHAR_VECTOR)name->obj.s, 
		(PLFLT)minx->r, 
		(PLFLT)maxx->r, 
		(PLFLT)miny->r, 
		(PLFLT)maxy->r);
	return top;
}

// TODO 
SML_EXPORT int __cdecl ke_plmapfill(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *mapform, *name, *minx, *maxx, *miny, *maxy, *plotentries, *nplotentries;
	nplotentries = stack[--top];
	plotentries = stack[--top];
	maxy = stack[--top];
	miny = stack[--top];
	maxx = stack[--top];
	minx = stack[--top];
	name = stack[--top];
	mapform = stack[--top];

	plmapfill(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLCHAR_VECTOR)name->obj.s,
		(PLFLT)minx->r,
		(PLFLT)maxx->r,
		(PLFLT)miny->r,
		(PLFLT)maxy->r,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(plotentries->obj.vector_int, 0),
		(PLINT)nplotentries->i);
	return top;
}

// TODO 
SML_EXPORT int __cdecl ke_plmapline(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *mapform, *name, *minx, *maxx, *miny, *maxy, *plotentries, *nplotentries;
	nplotentries = stack[--top];
	plotentries = stack[--top];
	maxy = stack[--top];
	miny = stack[--top];
	maxx = stack[--top];
	minx = stack[--top];
	name = stack[--top];
	mapform = stack[--top];

	plmapline(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLCHAR_VECTOR)name->obj.s,
		(PLFLT)minx->r,
		(PLFLT)maxx->r,
		(PLFLT)miny->r,
		(PLFLT)maxy->r,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(plotentries->obj.vector_int, 0),
		(PLINT)nplotentries->i);
	return top;
}

// TODO 
SML_EXPORT int __cdecl ke_plmapstring(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *mapform, *name, *string, *minx, *maxx, *miny, *maxy, *plotentries, *nplotentries;
	nplotentries = stack[--top];
	plotentries = stack[--top];
	maxy = stack[--top];
	miny = stack[--top];
	maxx = stack[--top];
	minx = stack[--top];
	string = stack[--top];
	name = stack[--top];
	mapform = stack[--top];

	plmapstring(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLCHAR_VECTOR)name->obj.s,
		(PLCHAR_VECTOR)string->obj.s,
		(PLFLT)minx->r,
		(PLFLT)maxx->r,
		(PLFLT)miny->r,
		(PLFLT)maxy->r,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(plotentries->obj.vector_int, 0),
		(PLINT)nplotentries->i);
	return top;
}

// TODO 
SML_EXPORT int __cdecl ke_plmaptex(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *mapform, *name, *dx, *dy, *just, *text, *minx, *maxx, *miny, *maxy, *plotentry;
	plotentry = stack[--top];
	maxy = stack[--top];
	miny = stack[--top];
	maxx = stack[--top];
	minx = stack[--top];
	text = stack[--top];
	just = stack[--top];
	dy = stack[--top];
	dx = stack[--top];
	name = stack[--top];
	mapform = stack[--top];

	plmaptex(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLCHAR_VECTOR)name->obj.s,
		(PLFLT)dx->r,
		(PLFLT)dy->r,
		(PLFLT)just->r,
		(PLCHAR_VECTOR)text->obj.s,
		(PLFLT)minx->r,
		(PLFLT)maxx->r,
		(PLFLT)miny->r,
		(PLFLT)maxy->r,
		(PLINT)plotentry->i);
	return top;
}


SML_EXPORT int __cdecl ke_plmeridians(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *mapform, *dlong, *dlat, *minlong, *maxlong, *minlat, *maxlat;
	maxlat = stack[--top];
	minlat = stack[--top];
	maxlong = stack[--top];
	minlong = stack[--top];
	dlat = stack[--top];
	dlong = stack[--top];
	mapform = stack[--top];

	plmeridians(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLFLT)dlong->r,
		(PLFLT)dlat->r,
		(PLFLT)minlong->r,
		(PLFLT)maxlong->r,
		(PLFLT)minlat->r,
		(PLFLT)maxlat->r);
	return top;
}

SML_EXPORT int __cdecl ke_plmesh(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *nx, *ny, *opt;
	opt = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plmesh(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i);
	return top;
}

SML_EXPORT int __cdecl ke_plmeshc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel;
	nlevel = stack[--top];
	clevel = stack[--top];
	opt = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plmeshc(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i);
	return top;
}

SML_EXPORT int __cdecl ke_plmkstrm(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_strm;
	p_strm = stack[--top];

	plmkstrm((PLINT_NC_SCALAR)(&(sml->fields[p_strm->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plmtex(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *side, *disp, *pos, *just, *text;
	text = stack[--top];
	just = stack[--top];
	pos = stack[--top];
	disp = stack[--top];
	side = stack[--top];

	plmtex(
		(PLCHAR_VECTOR)side->obj.s, 
		(PLFLT)disp->r, 
		(PLFLT)pos->r, 
		(PLFLT)just->r, 
		(PLCHAR_VECTOR)text->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plmtex3(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *side, *disp, *pos, *just, *text;
	text = stack[--top];
	just = stack[--top];
	pos = stack[--top];
	disp = stack[--top];
	side = stack[--top];

	plmtex3(
		(PLCHAR_VECTOR)side->obj.s,
		(PLFLT)disp->r,
		(PLFLT)pos->r,
		(PLFLT)just->r,
		(PLCHAR_VECTOR)text->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plot3d(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *nx, *ny, *opt, *side;
	side = stack[--top];
	opt = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plot3d(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i, 
		(PLINT)ny->i, 
		(PLINT)opt->i, 
		(PLBOOL)side->i);
	return top;
}

SML_EXPORT int __cdecl ke_plot3dc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel;
	nlevel = stack[--top];
	clevel = stack[--top];
	opt = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plot3dc(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i);
	return top;
}

SML_EXPORT int __cdecl ke_plot3dl(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel, *indexxmin, *indexxmax, *indexymin, *indexymax;

	indexymax = stack[--top];
	indexymin = stack[--top];
	indexxmax = stack[--top];
	indexxmin = stack[--top];

	nlevel = stack[--top];
	clevel = stack[--top];
	opt = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plot3dcl(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i,
		(PLINT)indexxmin->i,
		(PLINT)indexxmax->i,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(indexymin->obj.vector_int, 0), 
		(PLINT_VECTOR)gsl_vector_int_const_ptr(indexymax->obj.vector_int, 0));
	
	return top;
}


SML_EXPORT int __cdecl ke_plparseopts(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p_argc, *argv, *mode;
	mode = stack[--top];
	argv = stack[--top];
	p_argc = stack[top - 1];
	
	PLINT st = plparseopts(
		(int *)(&(sml->fields[p_argc->ifield]->i)),
		(PLCHAR_NC_MATRIX)argv->obj.s,
		(PLINT)mode->i);
	p_argc->i = st;
	p_argc->vtype = KEV_INT;
	p_argc->ttype = KET_VAL;
	return top;
}

SML_EXPORT int __cdecl ke_plpat(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *nlin, *inc, *del;
	del = stack[--top];
	inc = stack[--top];
	nlin = stack[--top];

	plpat((PLINT)nlin->i,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(inc->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(del->obj.vector_int, 0));

	return top;
}

SML_EXPORT int __cdecl ke_plpath(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x1, *y1, *x2, *y2;
	y2 = stack[--top];
	x2 = stack[--top];
	y1 = stack[--top];
	x1 = stack[--top];
	n = stack[--top];

	plpath(
		(PLINT)n->i, 
		(PLFLT)x1->r, 
		(PLFLT)y1->r,
		(PLFLT)x2->r,
		(PLFLT)y2->r);

	return top;
}

SML_EXPORT int __cdecl ke_plpoin(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *code;
	code = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];

	plpoin(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLINT)code->i);

	return top;
}

SML_EXPORT int __cdecl ke_plpoin3(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *z, *code;
	code = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];

	plpoin3(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0),
		(PLINT)code->i);

	return top;
}

SML_EXPORT int __cdecl ke_plpoly3(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *z, *draw, *ifcc;
	ifcc = stack[--top];
	draw = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];

	plpoly3(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0),
		(PLBOOL_VECTOR)gsl_vector_int_const_ptr(draw->obj.vector_int, 0),
		(PLBOOL)ifcc->i);

	return top;
}

SML_EXPORT int __cdecl ke_plprec(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *setp, *prec;
	prec = stack[--top];
	setp = stack[--top];

	plprec(
		(PLINT)setp->i,
		(PLINT)prec->i);

	return top;
}

SML_EXPORT int __cdecl ke_plpsty(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *patt;
	patt = stack[--top];

	plpsty((PLINT)patt->i);
	return top;
}

SML_EXPORT int __cdecl ke_plptex(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *dx, *dy, *just, *text;
	text = stack[--top];
	just = stack[--top];
	dy = stack[--top];
	dx = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plptex(
		(PLFLT)x->r,
		(PLFLT)y->r,
		(PLFLT)dx->r,
		(PLFLT)dy->r,
		(PLFLT)just->r,
		(PLCHAR_VECTOR)text->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plptex3(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *wx, *wy, *wz, *dx, *dy, *dz, *sx, *sy, *sz, *just, *text;
	text = stack[--top];
	just = stack[--top];
	sz = stack[--top];
	sy = stack[--top];
	sx = stack[--top];
	dz = stack[--top];
	dy = stack[--top];
	dx = stack[--top];
	wz = stack[--top];
	wy = stack[--top];
	wx = stack[--top];

	plptex3(
		(PLFLT)wx->r,
		(PLFLT)wy->r,
		(PLFLT)wz->r,
		(PLFLT)dx->r,
		(PLFLT)dy->r,
		(PLFLT)dz->r,
		(PLFLT)sx->r,
		(PLFLT)sy->r,
		(PLFLT)sz->r,
		(PLFLT)just->r,
		(PLCHAR_VECTOR)text->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plrandd(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *p;
	p = stack[top - 1];
	stack[top++] = *p;
	p = stack[top - 1];
	
	p->r = plrandd();
	p->vtype = KEV_REAL;
	p->ttype = KET_VAL;
	return top;
}

SML_EXPORT int __cdecl ke_plreplot(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plreplot();
	return top;
}

SML_EXPORT int __cdecl ke_plrgbhls(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b, *p_h, *p_l, *p_s;
	p_s = stack[--top];
	p_l = stack[--top];
	p_h = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	plrgbhls(
		(PLFLT)r->r,
		(PLFLT)g->r,
		(PLFLT)b->r,
		(PLFLT_NC_SCALAR)(&(sml->fields[p_h->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_l->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(sml->fields[p_s->ifield]->r)));
	return top;
}

SML_EXPORT int __cdecl ke_plschr(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *def, *scale;
	scale = stack[--top];
	def = stack[--top];

	plschr( (PLFLT)def->r, (PLFLT)scale->r);
	return top;
}

SML_EXPORT int __cdecl ke_plscmap0(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b, *ncol0;
	ncol0 = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	plscmap0(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLINT)ncol0->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscmap0a(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b, *alpha, *ncol0;
	ncol0 = stack[--top];
	alpha = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	plscmap0a(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLFLT_VECTOR)gsl_vector_int_const_ptr(alpha->obj.vector_int, 0),
		(PLINT)ncol0->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscmap0n(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *ncol0;
	ncol0 = stack[--top];

	plscmap0n((PLINT)ncol0->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscmap1_range(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *min_color, *max_color;
	max_color = stack[--top];
	min_color = stack[--top];

	plscmap1_range( (PLFLT)min_color->r, (PLFLT)max_color->r);
	return top;
}

SML_EXPORT int __cdecl ke_plscmap1(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b, *ncol1;
	ncol1 = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	plscmap1(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLINT)ncol1->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscmap1a(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b, *alpha, *ncol1;
	ncol1 = stack[--top];
	alpha = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	plscmap1a(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(alpha->obj.vector, 0),
		(PLINT)ncol1->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscmap1l(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *itype, *npts, *intensity, *coord1, *coord2, *coord3, *alt_hue_path;
	alt_hue_path = stack[--top];
	coord3 = stack[--top];
	coord2 = stack[--top];
	coord1 = stack[--top];
	intensity = stack[--top];
	npts = stack[--top];
	itype = stack[--top];

	plscmap1l( 
		(PLBOOL)itype->i, 
		(PLINT)npts->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(intensity->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord1->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord2->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord3->obj.vector, 0),
		(PLBOOL_VECTOR)gsl_vector_int_const_ptr(alt_hue_path->obj.vector_int, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plscmap1la(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *itype, *npts, *intensity, *coord1, *coord2, *coord3, *alpha, *alt_hue_path;
	alt_hue_path = stack[--top];
	alpha = stack[--top];
	coord3 = stack[--top];
	coord2 = stack[--top];
	coord1 = stack[--top];
	intensity = stack[--top];
	npts = stack[--top];
	itype = stack[--top];

	plscmap1la(
		(PLBOOL)itype->i,
		(PLINT)npts->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(intensity->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord1->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord2->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord3->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(alpha->obj.vector, 0),
		(PLBOOL_VECTOR)gsl_vector_int_const_ptr(alt_hue_path->obj.vector_int, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plscmap1n(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *ncol1;
	ncol1 = stack[--top];

	plscmap1n((PLINT)ncol1->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscol0(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *icol0, *r, *g, *b;
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];
	icol0 = stack[--top];

	plscol0(
		(PLINT)icol0->i, 
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscol0a(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *icol0, *r, *g, *b, *alpha;
	alpha = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];
	icol0 = stack[--top];

	plscol0a(
		(PLINT)icol0->i,
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i,
		(PLFLT)alpha->r);
	return top;
}

SML_EXPORT int __cdecl ke_plscolbg(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b;
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	plscolbg(
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscolbga(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *r, *g, *b, *alpha;
	alpha = stack[--top];
	b = stack[--top];
	g = stack[--top];
	r = stack[--top];

	plscolbga(
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i,
		(PLFLT)alpha->r);
	return top;
}

SML_EXPORT int __cdecl ke_plscolor(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *color;
	color = stack[--top];

	plscolor((PLINT)color->i);
	return top;
}

SML_EXPORT int __cdecl ke_plscompression(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *compression;
	compression = stack[--top];

	plscompression((PLINT)compression->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsdidev(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *mar, *aspect, *jx, *jy;
	jy = stack[--top];
	jx = stack[--top];
	aspect = stack[--top];
	mar = stack[--top];

	plsdidev(
		(PLFLT)mar->r, 
		(PLFLT)aspect->r,
		(PLFLT)jx->r,
		(PLFLT)jy->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsdimap(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *dimxmin, *dimxmax, *dimymin, *dimymax, *dimxpmm, *dimypmm;
	dimypmm = stack[--top];
	dimxpmm = stack[--top];
	dimymax = stack[--top];
	dimymin = stack[--top];
	dimxmax = stack[--top];
	dimxmin = stack[--top];

	plsdimap(
		(PLINT)dimxmin->i, 
		(PLINT)dimxmax->i,
		(PLINT)dimymin->i,
		(PLINT)dimymax->i,
		(PLFLT)dimxpmm->r,
		(PLFLT)dimypmm->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsdiori(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *rot;
	rot = stack[--top];

	plsdiori((PLFLT)rot->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsdiplt(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *ymin, *xmax, *ymax;
	ymax = stack[--top];
	xmax = stack[--top];
	ymin = stack[--top];
	xmin = stack[--top];

	plsdiplt(
		(PLFLT)xmin->r,
		(PLFLT)ymin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymax->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsdiplz(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *ymin, *xmax, *ymax;
	ymax = stack[--top];
	xmax = stack[--top];
	ymin = stack[--top];
	xmin = stack[--top];

	plsdiplz(
		(PLFLT)xmin->r,
		(PLFLT)ymin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymax->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsdrawmode(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *mode;
	mode = stack[--top];

	plsdrawmode(
		(PLINT)mode->i);
	return top;
}

SML_EXPORT int __cdecl ke_plseed(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *seed;
	seed = stack[--top];

	plseed(
		(unsigned int)seed->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsesc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *esc;
	esc = stack[--top];

	plsesc(
		(char)esc->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsetopt(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *opt, *optarg;
	optarg = stack[--top];
	opt = stack[top - 1];

	PLINT st = plsetopt( (PLCHAR_VECTOR)opt->obj.s, (PLCHAR_VECTOR)optarg->obj.s);
	opt->i = st;
	opt->vtype = KEV_INT;
	opt->ttype = KET_VAL;
	return top;
}

SML_EXPORT int __cdecl ke_plsfam(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *fam, *num, *bmax;
	bmax = stack[--top];
	num = stack[--top];
	fam = stack[--top];

	plsfam(
		(PLINT)fam->i, 
		(PLINT)num->i,
		(PLINT)bmax->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsfci(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *fci;
	fci = stack[--top];

	plsfci(	(PLUNICODE)fci->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsfnam(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *fnam;
	fnam = stack[--top];

	plsfnam(
		(PLCHAR_VECTOR)fnam->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plsfont(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *family, *style, *weight;
	weight = stack[--top];
	style = stack[--top];
	family = stack[--top];

	plsfont(
		(PLINT)family->i, 
		(PLINT)style->i,
		(PLINT)weight->i);
	return top;
}


SML_EXPORT int __cdecl ke_plshades(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *a, *nx, *ny, *defined, *xmin, *xmax, *ymin, *ymax, *clevel, *nlevel,
		*fill_width, *cont_color, *cont_width, *fill, *rectangular, *pltr, *pltr_data;
	pltr_data = stack[--top];
	pltr = stack[--top];
	rectangular = stack[--top];
	fill = stack[--top];
	cont_width = stack[--top];
	cont_color = stack[--top];
	fill_width = stack[--top];
	nlevel = stack[--top];
	clevel = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	defined = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	a = stack[--top];

	plshades(
		(PLFLT_MATRIX)gsl_matrix_const_ptr(a->obj.matrix, 0,0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLDEFINED_callback)defined->obj.pldefcb, 
		(PLFLT)xmin->r, 
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i,
		(PLFLT)fill_width->r,
		(PLINT)cont_color->i,
		(PLFLT)cont_width->r,
		(PLFILL_callback)fill->obj.plfillcb,
		(PLBOOL)rectangular->i, 
		(PLTRANSFORM_callback)pltr->obj.pltrcb, 
		(PLPointer)pltr_data->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plshade(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *a, *nx, *ny, *defined, *xmin, *xmax, *ymin, *ymax, *shade_min, *shade_max, *sh_cmap, *sh_color, *sh_width,
		*min_color, *min_width, *max_color, *max_width, *fill, *rectangular, *pltr, *pltr_data;
	pltr_data = stack[--top];
	pltr = stack[--top];
	rectangular = stack[--top];
	fill = stack[--top];
	max_width = stack[--top];
	max_color = stack[--top];
	min_width = stack[--top];
	min_color = stack[--top];
	sh_width = stack[--top];
	sh_color = stack[--top];
	sh_cmap = stack[--top];
	shade_max = stack[--top];
	shade_min = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	defined = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	a = stack[--top];

	plshade(
		(PLFLT_MATRIX)gsl_matrix_const_ptr(a->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLDEFINED_callback)defined->obj.pldefcb,
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r,
		(PLFLT)shade_min->r,
		(PLFLT)shade_max->r,
		(PLINT)sh_cmap->i,
		(PLFLT)sh_color->r,
		(PLFLT)sh_width->r,
		(PLINT)min_color->i,
		(PLFLT)min_width->r,
		(PLINT)max_color->i,
		(PLFLT)max_width->r,
		(PLFILL_callback)fill->obj.plfillcb,
		(PLBOOL)rectangular->i,
		(PLTRANSFORM_callback)pltr->obj.pltrcb,
		(PLPointer)pltr_data->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plslabelfunc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *label_func, *label_data;
	label_data = stack[--top];
	label_func = stack[--top];

	plslabelfunc(
		(PLLABEL_FUNC_callback)label_func->obj.pllblcb, 
		(PLPointer)label_data->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plsmaj(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *def, *scale;
	scale = stack[--top];
	def = stack[--top];

	plsmaj( (PLFLT)def->r, (PLFLT)scale->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsmem(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *maxx, *maxy, *plotmem;
	plotmem = stack[--top];
	maxy = stack[--top];
	maxx = stack[--top];

	plsmem( (PLINT)maxx->i, (PLINT)maxy->i, (PLPointer)plotmem->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plsmema(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *maxx, *maxy, *plotmem;
	plotmem = stack[--top];
	maxy = stack[--top];
	maxx = stack[--top];

	plsmema((PLINT)maxx->i, (PLINT)maxy->i, (PLPointer)plotmem->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plsmin(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *def, *scale;
	scale = stack[--top];
	def = stack[--top];

	plsmin( (PLFLT)def->r, (PLFLT)scale->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsori(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *ori;
	ori = stack[--top];

	plsori((PLINT)ori->i);
	return top;
}

SML_EXPORT int __cdecl ke_plspage(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *xp, *yp, *xleng, *yleng, *xoff, *yoff;
	yoff = stack[--top];
	xoff = stack[--top];
	yleng = stack[--top];
	xleng = stack[--top];
	yp = stack[--top];
	xp = stack[--top];

	plspage(
		(PLFLT)xp->r, 
		(PLFLT)yp->r,
		(PLINT)xleng->i, 
		(PLINT)yleng->i,
		(PLINT)xoff->i,
		(PLINT)yoff->i);
	return top;
}

SML_EXPORT int __cdecl ke_plspal0(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *filename;
	filename = stack[--top];

	plspal0((PLCHAR_VECTOR)filename->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plspal1(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *filename, *interpolate;
	interpolate = stack[--top];
	filename = stack[--top];

	plspal1((PLCHAR_VECTOR)filename->obj.s, (PLBOOL)interpolate->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsstrm(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *strm;
	strm = stack[--top];

	plsstrm((PLINT)strm->i);
	return top;
}

SML_EXPORT int __cdecl ke_plssub(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *nx, *ny;
	ny = stack[--top];
	nx = stack[--top];

	plssub((PLINT)nx->i, (PLINT)ny->i);
	return top;
}

SML_EXPORT int __cdecl ke_plssym(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *def, *scale;
	scale = stack[--top];
	def = stack[--top];

	plssym((PLFLT)def->r, (PLFLT)scale->r);
	return top;
}

SML_EXPORT int __cdecl ke_plstar(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *nx, *ny;
	ny = stack[--top];
	nx = stack[--top];

	plstar((PLINT)nx->i, (PLINT)ny->i);
	return top;
}

SML_EXPORT int __cdecl ke_plstart(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *devname, *nx, *ny;
	ny = stack[--top];
	nx = stack[--top];
	devname = stack[--top];

	plstart(
		(PLCHAR_VECTOR)devname->obj.s,
		(PLINT)nx->i, 
		(PLINT)ny->i);
	return top;
}

SML_EXPORT int __cdecl ke_plstransform(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *coordinate_transform, *coordinate_transform_data;
	coordinate_transform_data = stack[--top];
	coordinate_transform = stack[--top];

	plstransform(
		(PLTRANSFORM_callback)coordinate_transform->obj.pltrcb,
		(PLPointer)coordinate_transform_data->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plstring(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *string;
	string = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];

	plstring(
		(PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLCHAR_VECTOR)string->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plstring3(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *z, *string;
	string = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];

	plstring3(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0),
		(PLCHAR_VECTOR)string->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plstripa(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *id, *pen, *x, *y;
	y = stack[--top];
	x = stack[--top];
	pen = stack[--top];
	id = stack[--top];

	plstripa(
		(PLINT)id->i,
		(PLINT)pen->i,
		(PLFLT)x->r,
		(PLFLT)y->r);
	return top;
}

SML_EXPORT int __cdecl ke_plstripc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *id, *xspec, *yspec, *xmin, *xmax, *xjump, *ymin, *ymax, *xlpos, *ylpos, 
		*y_ascl, *acc, *colbox, *collab, *colline, *styline, *legline, *labx, *laby, *labtop;
	labtop = stack[--top];
	laby = stack[--top];
	labx = stack[--top];
	legline = stack[--top];
	styline = stack[--top];
	colline = stack[--top];
	collab = stack[--top];
	colbox = stack[--top];
	acc = stack[--top];
	y_ascl = stack[--top];
	ylpos = stack[--top];
	xlpos = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xjump = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	yspec = stack[--top];
	xspec = stack[--top];
	id = stack[--top];

	plstripc(
		(PLINT_NC_SCALAR)(&(sml->fields[id->ifield]->i)),
		(PLCHAR_VECTOR)xspec->obj.s,
		(PLCHAR_VECTOR)yspec->obj.s,
		(PLFLT)xmin->r, 
		(PLFLT)xmax->r,
		(PLFLT)xjump->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r,
		(PLFLT)xlpos->r,
		(PLFLT)ylpos->r,
		(PLBOOL)y_ascl->i, 
		(PLBOOL)acc->i,
		(PLINT)colbox->i, 
		(PLINT)collab->i,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(colline->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(styline->obj.vector_int, 0),
		(PLCHAR_MATRIX)legline->obj.s, 
		(PLCHAR_VECTOR)labx->obj.s,
		(PLCHAR_VECTOR)laby->obj.s,
		(PLCHAR_VECTOR)labtop->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plstripd(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *id;
	id = stack[--top];

	plstripd((PLINT)id->i);
	return top;
}

SML_EXPORT int __cdecl ke_plstyl(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *nms, *mark, *space;
	space = stack[--top];
	mark = stack[--top];
	nms = stack[--top];

	plstyl(
		(PLINT)nms->i, 
		(PLINT_VECTOR)gsl_vector_int_const_ptr(mark->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(space->obj.vector_int, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plsurf3d(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel;
	nlevel = stack[--top];
	clevel = stack[--top];
	opt = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plsurf3d(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), 
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0), 
		(PLINT)nx->i, 
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsurf3dl(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel, *indexxmin, *indexxmax, *indexymin, *indexymax;
	indexymax = stack[--top];
	indexymin = stack[--top];
	indexxmax = stack[--top];
	indexxmin = stack[--top];
	nlevel = stack[--top];
	clevel = stack[--top];
	opt = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	z = stack[--top];
	y = stack[--top];
	x = stack[--top];

	plsurf3dl(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i,
		(PLINT)indexxmin->i,
		(PLINT)indexxmax->i,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(indexymin->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(indexymax->obj.vector_int, 0));
	return top;
}

SML_EXPORT int __cdecl ke_plsvect(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *arrowx, *arrowy, *npts, *fill;
	fill = stack[--top];
	npts = stack[--top];
	arrowy = stack[--top];
	arrowx = stack[--top];

	plsvect(
		(PLFLT_VECTOR)gsl_vector_const_ptr(arrowx->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(arrowy->obj.vector, 0),
		(PLINT)npts->i, 
		(PLBOOL)fill->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsvpa(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *xmax, *ymin, *ymax;
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];

	plsvpa(
		(PLFLT)xmin->r, 
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r);
	return top;
}

SML_EXPORT int __cdecl ke_plsxax(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *digmax, *digits;
	digits = stack[--top];
	digmax = stack[--top];

	plsxax((PLINT)digmax->i, (PLINT)digits->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsyax(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *digmax, *digits;
	digits = stack[--top];
	digmax = stack[--top];

	plsyax((PLINT)digmax->i, (PLINT)digits->i);
	return top;
}

SML_EXPORT int __cdecl ke_plsym(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *n, *x, *y, *code;
	code = stack[--top];
	y = stack[--top];
	x = stack[--top];
	n = stack[--top];

	plsym(
		(PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), 
		(PLINT)code->i);
	return top;
}

SML_EXPORT int __cdecl ke_plszax(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *digmax, *digits;
	digits = stack[--top];
	digmax = stack[--top];

	plszax((PLINT)digmax->i, (PLINT)digits->i);
	return top;
}

SML_EXPORT int __cdecl ke_pltext(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	pltext();
	return top;
}

SML_EXPORT int __cdecl ke_pltimefmt(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *fmt;
	fmt = stack[--top];

	pltimefmt((PLCHAR_VECTOR)fmt->obj.s);
	return top;
}

SML_EXPORT int __cdecl ke_plvasp(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *aspect;
	aspect = stack[--top];

	plvasp((PLFLT)aspect->r);
	return top;
}

SML_EXPORT int __cdecl ke_plvect(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *u, *v, *nx, *ny, *scale, *pltr, *pltr_data;
	pltr_data = stack[--top];
	pltr = stack[--top];
	scale = stack[--top];
	ny = stack[--top];
	nx = stack[--top];
	v = stack[--top];
	u = stack[--top];

	plvect(
		(PLFLT_MATRIX)gsl_matrix_const_ptr(u->obj.matrix, 0,0), 
		(PLFLT_MATRIX)gsl_matrix_const_ptr(v->obj.matrix, 0, 0), 
		(PLINT)nx->i, 
		(PLINT)ny->i, 
		(PLFLT)scale->r,
		(PLTRANSFORM_callback)pltr->obj.pltrcb, 
		(PLPointer)pltr_data->obj.plptr);
	return top;
}

SML_EXPORT int __cdecl ke_plvpas(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *xmax, *ymin, *ymax, *aspect;
	aspect = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];

	plvpas(
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r,
		(PLFLT)aspect->r);
	return top;
}

SML_EXPORT int __cdecl ke_plvpor(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *xmax, *ymin, *ymax;
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];

	plvpor(
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r);
	return top;
}

SML_EXPORT int __cdecl ke_plvsta(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	plvsta();
	return top;
}

SML_EXPORT int __cdecl ke_plw3d(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *basex,* basey, *height, *xmin, *xmax, *ymin, *ymax, *zmin, *zmax, *alt, *az;
	az = stack[--top];
	alt = stack[--top];
	zmax = stack[--top];
	zmin = stack[--top];
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];
	height = stack[--top];
	basey = stack[--top];
	basex = stack[--top];

	plw3d(
		(PLFLT)basex->r, 
		(PLFLT)basey->r,
		(PLFLT)height->r,
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r,
		(PLFLT)zmin->r,
		(PLFLT)zmax->r,
		(PLFLT)alt->r,
		(PLFLT)az->r);
	return top;
}


SML_EXPORT int __cdecl ke_plwidth(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *width;
	width = stack[--top];

	plwidth(
		(PLFLT)width->r);
	return top;
}

SML_EXPORT int __cdecl ke_plwind(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *xmin, *xmax, *ymin, *ymax;
	ymax = stack[--top];
	ymin = stack[--top];
	xmax = stack[--top];
	xmin = stack[--top];

	plwind(
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r);
	return top;
}

SML_EXPORT int __cdecl ke_plxormod(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *mode, *status;
	status = stack[--top];
	mode = stack[--top];

	plxormod((PLBOOL)mode->i,
		(PLBOOL_NC_SCALAR)(&(sml->fields[status->ifield]->i)));
	return top;
}

SML_EXPORT int __cdecl ke_plabort(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *message;
	message = stack[--top];

	plabort((PLCHAR_VECTOR)message->obj.s);
	return top;
}

void dllke_plot_hash(sml_t* sml) {

	dllke_hash_add(sml, (fncp)&ke_plimload, PLOT_IMLOAD);
	dllke_hash_add(sml, (fncp)&ke_plimfree, PLOT_IMFREE);
	dllke_hash_add(sml, (fncp)&ke_plimwrite, PLOT_IMWRITE);
	dllke_hash_add(sml, (fncp)&ke_plimresize, PLOT_IMRESIZE);

	dllke_hash_add(sml, (fncp)&ke_plgcompression, PLOT_PLGCOMPRESSION);
	dllke_hash_add(sml, (fncp)&ke_plgdidev, PLOT_PLGDIDEV);
	dllke_hash_add(sml, (fncp)&ke_plgdiori, PLOT_PLGDIORI);
	dllke_hash_add(sml, (fncp)&ke_plgdiplt, PLOT_PLGDIPL);
	dllke_hash_add(sml, (fncp)&ke_function_plgdrawmode, PLOT_FUNCTION_PLGDRAWMODE);
	dllke_hash_add(sml, (fncp)&ke_plgfam, PLOT_PLGFAM);
	dllke_hash_add(sml, (fncp)&ke_plgfci, PLOT_PLGFCI);
	dllke_hash_add(sml, (fncp)&ke_plgfnam, PLOT_PLGFNAM);
	dllke_hash_add(sml, (fncp)&ke_plgfont, PLOT_PLGFONT);
	dllke_hash_add(sml, (fncp)&ke_plglevel, PLOT_PLGLEVEL);
	dllke_hash_add(sml, (fncp)&ke_plgpage, PLOT_PLGPAGE);
	dllke_hash_add(sml, (fncp)&ke_function_plgra, PLOT_FUNCTION_PLGRA);
	dllke_hash_add(sml, (fncp)&ke_plgradient, PLOT_PLGRADIENT);
	dllke_hash_add(sml, (fncp)&ke_plgriddata, PLOT_PLGRIDDATA);
	dllke_hash_add(sml, (fncp)&ke_plgspa, PLOT_PLGSPA);
	dllke_hash_add(sml, (fncp)&ke_plgstrm, PLOT_PLGSTRM);
	dllke_hash_add(sml, (fncp)&ke_plgver, PLOT_PLGVER);
	dllke_hash_add(sml, (fncp)&ke_plgvpd, PLOT_PLGVPD);
	dllke_hash_add(sml, (fncp)&ke_plgvpw, PLOT_PLGVPW);
	dllke_hash_add(sml, (fncp)&ke_plgxax, PLOT_PLGXAX);
	dllke_hash_add(sml, (fncp)&ke_plgyax, PLOT_PLGYAX);
	dllke_hash_add(sml, (fncp)&ke_plgzax, PLOT_PLGZAX);
	dllke_hash_add(sml, (fncp)&ke_plhist, PLOT_PLHIST);
	dllke_hash_add(sml, (fncp)&ke_plhlsrgb, PLOT_PLHLSRGB);
	dllke_hash_add(sml, (fncp)&ke_plimagefr, PLOT_PLIMAGEFR);
	dllke_hash_add(sml, (fncp)&ke_plimage, PLOT_PLIMAGE);
	dllke_hash_add(sml, (fncp)&ke_pljoin, PLOT_PLJOIN);
	dllke_hash_add(sml, (fncp)&ke_pllab, PLOT_PLLAB);
	dllke_hash_add(sml, (fncp)&ke_pllegend, PLOT_PLLEGEND);
	dllke_hash_add(sml, (fncp)&ke_pllightsource, PLOT_PLLIGHTSOURCE);
	dllke_hash_add(sml, (fncp)&ke_plline3, PLOT_PLLINE3);
	dllke_hash_add(sml, (fncp)&ke_pllsty, PLOT_PLLSTY);
	dllke_hash_add(sml, (fncp)&ke_plmap, PLOT_PLMAP);
	dllke_hash_add(sml, (fncp)&ke_plmapfill, PLOT_PLMAPFILL);
	dllke_hash_add(sml, (fncp)&ke_plmapline, PLOT_PLMAPLINE);
	dllke_hash_add(sml, (fncp)&ke_plmapstring, PLOT_PLMAPSTRING);
	dllke_hash_add(sml, (fncp)&ke_plmaptex, PLOT_PLMAPTEX);
	dllke_hash_add(sml, (fncp)&ke_plmeridians, PLOT_PLMERIDIANS);
	dllke_hash_add(sml, (fncp)&ke_plmesh, PLOT_PLMESH);
	dllke_hash_add(sml, (fncp)&ke_plmeshc, PLOT_PLMESHC);
	dllke_hash_add(sml, (fncp)&ke_plmkstrm, PLOT_PLMKSTRM);
	dllke_hash_add(sml, (fncp)&ke_plmtex, PLOT_PLMTEX);
	dllke_hash_add(sml, (fncp)&ke_plmtex3, PLOT_PLMTEX3);
	dllke_hash_add(sml, (fncp)&ke_plot3d, PLOT_PLOT3D);
	dllke_hash_add(sml, (fncp)&ke_plot3dc, PLOT_PLOT3DC);
	dllke_hash_add(sml, (fncp)&ke_plot3dl, PLOT_PLOT3DL);
	dllke_hash_add(sml, (fncp)&ke_plparseopts, PLOT_PLPARSEOPTS);
	dllke_hash_add(sml, (fncp)&ke_plpat, PLOT_PLPAT);
	dllke_hash_add(sml, (fncp)&ke_plpath, PLOT_PLPATH);
	dllke_hash_add(sml, (fncp)&ke_plpoin, PLOT_PLPOIN);
	dllke_hash_add(sml, (fncp)&ke_plpoin3, PLOT_PLPOIN3);
	dllke_hash_add(sml, (fncp)&ke_plpoly3, PLOT_PLPOLY3);
	dllke_hash_add(sml, (fncp)&ke_plprec, PLOT_PLPREC);
	dllke_hash_add(sml, (fncp)&ke_plpsty, PLOT_PLPSTY);
	dllke_hash_add(sml, (fncp)&ke_plptex, PLOT_PLPTEX);
	dllke_hash_add(sml, (fncp)&ke_plptex3, PLOT_PLPTEX3);
	dllke_hash_add(sml, (fncp)&ke_plrandd, PLOT_PLRANDD);
	dllke_hash_add(sml, (fncp)&ke_plreplot, PLOT_PLREPLOT);
	dllke_hash_add(sml, (fncp)&ke_plrgbhls, PLOT_PLRGBHLS);
	dllke_hash_add(sml, (fncp)&ke_plschr, PLOT_PLSCHR);
	dllke_hash_add(sml, (fncp)&ke_plscmap0, PLOT_PLSCMAP0);
	dllke_hash_add(sml, (fncp)&ke_plscmap0a, PLOT_PLSCMAP0A);
	dllke_hash_add(sml, (fncp)&ke_plscmap0n, PLOT_PLSCMAP0N);
	dllke_hash_add(sml, (fncp)&ke_plscmap1_range, PLOT_PLSCMAP1_RANGE);
	dllke_hash_add(sml, (fncp)&ke_plscmap1, PLOT_PLSCMAP1);
	dllke_hash_add(sml, (fncp)&ke_plscmap1a, PLOT_PLSCMAP1A);
	dllke_hash_add(sml, (fncp)&ke_plscmap1l, PLOT_PLSCMAP1L);
	dllke_hash_add(sml, (fncp)&ke_plscmap1la, PLOT_PLSCMAP1LA);
	dllke_hash_add(sml, (fncp)&ke_plscmap1n, PLOT_PLSCMAP1N);
	dllke_hash_add(sml, (fncp)&ke_plscol0, PLOT_PLSCOL0);
	dllke_hash_add(sml, (fncp)&ke_plscol0a, PLOT_PLSCOL0A);
	dllke_hash_add(sml, (fncp)&ke_plscolbg, PLOT_PLSCOLBG);
	dllke_hash_add(sml, (fncp)&ke_plscolbga, PLOT_PLSCOLBGA);
	dllke_hash_add(sml, (fncp)&ke_plscolor, PLOT_PLSCOLOR);
	dllke_hash_add(sml, (fncp)&ke_plscompression, PLOT_PLSCOMPRESSION);
	dllke_hash_add(sml, (fncp)&ke_plsdidev, PLOT_PLSDIDEV);
	dllke_hash_add(sml, (fncp)&ke_plsdimap, PLOT_PLSDIMAP);
	dllke_hash_add(sml, (fncp)&ke_plsdiori, PLOT_PLSDIORI);
	dllke_hash_add(sml, (fncp)&ke_plsdiplt, PLOT_PLSDIPLT);
	dllke_hash_add(sml, (fncp)&ke_plsdiplz, PLOT_PLSDIPLZ);
	dllke_hash_add(sml, (fncp)&ke_plsdrawmode, PLOT_PLSDRAWMODE);
	dllke_hash_add(sml, (fncp)&ke_plseed, PLOT_PLSEED);
	dllke_hash_add(sml, (fncp)&ke_plsesc, PLOT_PLSESC);
	dllke_hash_add(sml, (fncp)&ke_plsetopt, PLOT_PLSETOPT);
	dllke_hash_add(sml, (fncp)&ke_plsfam, PLOT_PLSFAM);
	dllke_hash_add(sml, (fncp)&ke_plsfci, PLOT_PLSFCI);
	dllke_hash_add(sml, (fncp)&ke_plsfnam, PLOT_PLSFNAM);
	dllke_hash_add(sml, (fncp)&ke_plsfont, PLOT_PLSFONT);
	dllke_hash_add(sml, (fncp)&ke_plshades, PLOT_PLSHADES);
	dllke_hash_add(sml, (fncp)&ke_plshade, PLOT_PLSHADE);
	dllke_hash_add(sml, (fncp)&ke_plslabelfunc, PLOT_PLSLABELFUNC);
	dllke_hash_add(sml, (fncp)&ke_plsmaj, PLOT_PLSMAJ);
	dllke_hash_add(sml, (fncp)&ke_plsmem, PLOT_PLSMEM);
	dllke_hash_add(sml, (fncp)&ke_plsmema, PLOT_PLSMEMA);
	dllke_hash_add(sml, (fncp)&ke_plsmin, PLOT_PLSMIN);
	dllke_hash_add(sml, (fncp)&ke_plsori, PLOT_PLSORI);
	dllke_hash_add(sml, (fncp)&ke_plspage, PLOT_PLSPAGE);
	dllke_hash_add(sml, (fncp)&ke_plspal0, PLOT_PLSPAL0);
	dllke_hash_add(sml, (fncp)&ke_plspal1, PLOT_PLSPAL1);
	dllke_hash_add(sml, (fncp)&ke_plsstrm, PLOT_PLSSTRM);
	dllke_hash_add(sml, (fncp)&ke_plssub, PLOT_PLSSUB);
	dllke_hash_add(sml, (fncp)&ke_plssym, PLOT_PLSSYM);
	dllke_hash_add(sml, (fncp)&ke_plstar, PLOT_PLSTAR);
	dllke_hash_add(sml, (fncp)&ke_plstart, PLOT_PLSTART);
	dllke_hash_add(sml, (fncp)&ke_plstransform, PLOT_PLSTRANSFORM);
	dllke_hash_add(sml, (fncp)&ke_plstring, PLOT_PLSTRING);
	dllke_hash_add(sml, (fncp)&ke_plstring3, PLOT_PLSTRING3);
	dllke_hash_add(sml, (fncp)&ke_plstripa, PLOT_PLSTRIPA);
	dllke_hash_add(sml, (fncp)&ke_plstripc, PLOT_PLSTRIPC);
	dllke_hash_add(sml, (fncp)&ke_plstripd, PLOT_PLSTRIPD);
	dllke_hash_add(sml, (fncp)&ke_plstyl, PLOT_PLSTYL);
	dllke_hash_add(sml, (fncp)&ke_plsurf3d, PLOT_PLSURF3D);
	dllke_hash_add(sml, (fncp)&ke_plsurf3dl, PLOT_PLSURF3DL);
	dllke_hash_add(sml, (fncp)&ke_plsvect, PLOT_PLSVECT);
	dllke_hash_add(sml, (fncp)&ke_plsvpa, PLOT_PLSVPA);
	dllke_hash_add(sml, (fncp)&ke_plsxax, PLOT_PLSXAX);
	dllke_hash_add(sml, (fncp)&ke_plsyax, PLOT_PLSYAX);
	dllke_hash_add(sml, (fncp)&ke_plsym, PLOT_PLSYM);
	dllke_hash_add(sml, (fncp)&ke_plszax, PLOT_PLSZAX);
	dllke_hash_add(sml, (fncp)&ke_pltext, PLOT_PLTEXT);
	dllke_hash_add(sml, (fncp)&ke_pltimefmt, PLOT_PLTIMEFMT);
	dllke_hash_add(sml, (fncp)&ke_plvasp, PLOT_PLVASP);
	dllke_hash_add(sml, (fncp)&ke_plvect, PLOT_PLVECT);
	dllke_hash_add(sml, (fncp)&ke_plvpas, PLOT_PLVPAS);
	dllke_hash_add(sml, (fncp)&ke_plvpor, PLOT_PLVPOR);
	dllke_hash_add(sml, (fncp)&ke_plvsta, PLOT_PLVSTA);
	dllke_hash_add(sml, (fncp)&ke_plwidth, PLOT_PLWIDTH);
	dllke_hash_add(sml, (fncp)&ke_plwind, PLOT_PLWIND);
	dllke_hash_add(sml, (fncp)&ke_plxormod, PLOT_PLXORMOD);
	dllke_hash_add(sml, (fncp)&ke_plabort, PLOT_PLABORT);

	dllke_hash_add(sml, (fncp)&ke_plgcolbg, PLOT_PLGCOLBG);
	dllke_hash_add(sml, (fncp)&ke_plgcolbga, PLOT_PLGCOLBGA);


	dllke_hash_add(sml, (fncp)&ke_plgcol0a, PLOT_PLGCOL0A);
	dllke_hash_add(sml, (fncp)&ke_plgcol0, PLOT_PLGCOL0);
	dllke_hash_add(sml, (fncp)&ke_plgcmap1_range, PLOT_PLGCMAP1RANGE);
	dllke_hash_add(sml, (fncp)&ke_plgchr, PLOT_PLGCHR);


	dllke_hash_add(sml, (fncp)&ke_plinit, PLOT);
	dllke_hash_add(sml, (fncp)&ke_plinit, PLOT_INIT);
	dllke_hash_add(sml, (fncp)&ke_plsdev, PLOT_PLSDEV);
	dllke_hash_add(sml, (fncp)&ke_plsdev, PLOT_PLDEV);
	dllke_hash_add(sml, (fncp)&ke_plenv, PLOT_PLENV);
	dllke_hash_add(sml, (fncp)&ke_plline, PLOT_PLLINE);
	dllke_hash_add(sml, (fncp)&ke_plend, PLOT_PLEND);
	dllke_hash_add(sml, (fncp)&ke_plpause, PLOT_PLSPAUSE);
	dllke_hash_add(sml, (fncp)&ke_plpause, PLOT_PLPAUSE);
	dllke_hash_add(sml, (fncp)&ke_pl_setcontlabelformat, PLOT_PLSETCONTLABELFORMAT);
	dllke_hash_add(sml, (fncp)&ke_pl_setcontlabelparam, PLOT_PLSETCONTLABELPARAM);
	dllke_hash_add(sml, (fncp)&ke_pladv, PLOT_PLADV);
	dllke_hash_add(sml, (fncp)&ke_plarc, PLOT_PLARC);
	dllke_hash_add(sml, (fncp)&ke_plaxes, PLOT_PLAXES);
	dllke_hash_add(sml, (fncp)&ke_plbin, PLOT_PLBIN);
	dllke_hash_add(sml, (fncp)&ke_plbop, PLOT_PLBOP);
	dllke_hash_add(sml, (fncp)&ke_plbox, PLOT_PLBOX);
	dllke_hash_add(sml, (fncp)&ke_plbox3, PLOT_PLBOX3);
	dllke_hash_add(sml, (fncp)&ke_plbtime, PLOT_PLBTIME);
	dllke_hash_add(sml, (fncp)&ke_plctime, PLOT_PLCTIME);
	dllke_hash_add(sml, (fncp)&ke_plcalc_world, PLOT_PLCALCWORLD);
	dllke_hash_add(sml, (fncp)&ke_plcol0, PLOT_PLCOL0);
	dllke_hash_add(sml, (fncp)&ke_plcol1, PLOT_PLCOL1);
	dllke_hash_add(sml, (fncp)&ke_plcolorbar, PLOT_PLCOLORBAR);
	dllke_hash_add(sml, (fncp)&ke_plconfigtime, PLOT_PLCONFIGTIME);
	dllke_hash_add(sml, (fncp)&ke_plcont, PLOT_PLCONT);
	dllke_hash_add(sml, (fncp)&ke_plcpstrm, PLOT_PLCPSTRM);

	dllke_hash_add(sml, (fncp)&ke_plend1, PLOT_PLEND1);
	dllke_hash_add(sml, (fncp)&ke_plenv0, PLOT_PLENV0);
	dllke_hash_add(sml, (fncp)&ke_pleop, PLOT_PLEOP);
	dllke_hash_add(sml, (fncp)&ke_plerrx, PLOT_PLERRX);
	dllke_hash_add(sml, (fncp)&ke_plerry, PLOT_PLERRY);
	dllke_hash_add(sml, (fncp)&ke_plfamadv, PLOT_PLFAMADV);
	dllke_hash_add(sml, (fncp)&ke_plfill, PLOT_PLFILL);
	dllke_hash_add(sml, (fncp)&ke_plfill3, PLOT_PLFILL3);
	dllke_hash_add(sml, (fncp)&ke_plflush, PLOT_PLFLUSH);
	dllke_hash_add(sml, (fncp)&ke_plfont, PLOT_PLFONT);
	dllke_hash_add(sml, (fncp)&ke_plfontld, PLOT_PLFONTLD);
	dllke_hash_add(sml, (fncp)&ke_plGetCursor, PLOT_PLGETCURSOR);
}

