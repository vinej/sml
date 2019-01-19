#include "kexpr.h"
#include "plot.h"
#include <plplot/plplot.h>

extern ke1_t ** g_gbl_fields;

static int ke_plsdev(ke1_t *stack, int top) {
	ke1_t *p;
	p = &stack[--top];
	plsdev(p->obj.s);
	return top;
}

static int ke_plinit(ke1_t *stack, int top) {
	ke1_t *p;
	p = &stack[--top];
	plsdev(p->obj.s);
	plinit();
	return top;
}

static int ke_plenv(ke1_t *stack, int top) {
	ke1_t *xmin, *xmax, *ymin, *ymax, *just, *axis;
	axis = &stack[--top];
	just = &stack[--top];
	ymax = &stack[--top];
	ymin = &stack[--top];
	xmax = &stack[--top];
	xmin = &stack[--top];
	plenv((PLINT)xmin->i, (PLINT)xmax->i, (PLINT)ymin->i, (PLINT)ymax->i, (PLINT)just->i, (PLINT)axis->i);
	return top;
}

static int ke_plline(ke1_t *stack, int top) {
	ke1_t *n, *x, *y;
	y = &stack[--top];
	x = &stack[--top]; 
	n = &stack[--top];
	plline((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0));
	return top;
}

static int ke_plpause(ke1_t *stack, int top) {
	ke1_t *p;
	p = &stack[--top];
	plspause((PLBOOL)p->i);
	return top;
}

static int ke_plend(ke1_t *stack, int top) {
	plend();
	return --top;
}

static int ke_pl_setcontlabelformat(ke1_t *stack, int top) {
	ke1_t *lexp, *sigdig;
	sigdig = &stack[--top];
	lexp = &stack[--top];

	pl_setcontlabelformat((PLINT)lexp->i, (PLINT)sigdig->i);
	return top;
}

static int ke_pl_setcontlabelparam(ke1_t *stack, int top) {
	ke1_t *offset, *size, *spacing, *active;
	active = &stack[--top];
	spacing = &stack[--top];
	size = &stack[--top];
	offset = &stack[--top];

	pl_setcontlabelparam((PLFLT)offset->r, (PLFLT)size->r, (PLFLT)spacing->r, (PLINT)active->i);
	return top;
}

static int ke_pladv(ke1_t *stack, int top) {
	ke1_t *page;
	page = &stack[--top];
	pladv((PLINT)page->i);
	return top;
}

static int ke_plarc(ke1_t *stack, int top) {
	ke1_t *x, *y, *a, *b, *angle1, *angle2, *rotate, *fill;
	fill = &stack[--top];
	rotate = &stack[--top];
	angle2 = &stack[--top];
	angle1 = &stack[--top];
	b = &stack[--top];
	a = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];
	plarc((PLFLT)x->r, (PLFLT)y->r, (PLFLT)a->r, (PLFLT)b->r, (PLFLT)angle1->r, (PLFLT)angle2->r, (PLFLT)rotate->r,(PLBOOL)x->i);
	return top;
}

static int ke_plaxes(ke1_t *stack, int top) {
	ke1_t *x0, *y0, *xopt, *xtick, *nxsub, *yopt, *ytick, *nysub;
	nysub= &stack[--top];
	ytick = &stack[--top];
	yopt = &stack[--top];
	nxsub = &stack[--top];
	xtick = &stack[--top];
	xopt = &stack[--top];
	y0 = &stack[--top];
	x0 = &stack[--top];
	plaxes((PLFLT)x0->r, (PLFLT)y0->r, (PLCHAR_VECTOR)xopt->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i,
		(PLCHAR_VECTOR)yopt->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i);
	return top;
}

static int ke_plbin(ke1_t *stack, int top) {
	ke1_t *nbin, *x, *y, *opt;
	opt = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];
	nbin = &stack[--top];
	plbin((PLINT)nbin->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0), (PLINT)opt->i);
	return top;
}

static int ke_plbop(ke1_t *stack, int top) {
	plbop();
	return --top;
}

static int ke_plbox(ke1_t *stack, int top) {
	ke1_t *xopt, *xtick, *nxsub, *yopt, *ytick, *nysub;
	nysub = &stack[--top];
	ytick = &stack[--top];
	yopt = &stack[--top];
	nxsub = &stack[--top];
	xtick = &stack[--top];
	xopt = &stack[--top];
	plbox((PLCHAR_VECTOR)xopt->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i);
	return top;
}

static int ke_plbox3(ke1_t *stack, int top) {
	ke1_t *xopt, *xlabel, *xtick, *nxsub, *yopt, *ylabel, *ytick, *nysub, *zopt, *zlabel, *ztick, *nzsub;
	nzsub = &stack[--top];
	ztick = &stack[--top];
	zlabel = &stack[--top];
	zopt = &stack[--top];
	nysub = &stack[--top];
	ytick = &stack[--top];
	ylabel = &stack[--top];
	yopt = &stack[--top];
	nxsub = &stack[--top];
	xtick = &stack[--top];
	xlabel = &stack[--top];
	xopt = &stack[--top];
	plbox3((PLCHAR_VECTOR)xopt->obj.s, (PLCHAR_VECTOR)xlabel->obj.s, 
		(PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLCHAR_VECTOR)ylabel->obj.s, 
		(PLFLT)ytick->r, (PLINT)nysub->i, (PLCHAR_VECTOR)zopt->obj.s, (PLCHAR_VECTOR)zlabel->obj.s, 
		(PLFLT)ztick->r, (PLINT)nzsub->i);
	return top;
}

static int ke_plbtime(ke1_t *stack, int top) {
	ke1_t *year, *month, *day, *hour, *min, *sec, *ctime;
	ctime = &stack[--top];
	sec = &stack[--top];
	min = &stack[--top];
	hour = &stack[--top];
	day = &stack[--top];
	month = &stack[--top];
	year = &stack[--top];


	ctime = &stack[--top];
	g_gbl_fields[year->ifield]->vtype = KEV_INT;
	g_gbl_fields[month->ifield]->vtype = KEV_INT;
	g_gbl_fields[day->ifield]->vtype = KEV_INT;
	g_gbl_fields[hour->ifield]->vtype = KEV_INT;
	g_gbl_fields[min->ifield]->vtype = KEV_INT;
	g_gbl_fields[sec->ifield]->vtype = KEV_REAL;
	plbtime((PLINT_NC_SCALAR)(&(g_gbl_fields[year->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[month->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[day->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[hour->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[min->ifield]->i)),
			(PLFLT_NC_SCALAR)(&(g_gbl_fields[sec->ifield]->r)),
			(PLFLT)ctime->r);
	return top;
}

static int ke_plctime(ke1_t *stack, int top) {
	ke1_t *year, *month, *day, *hour, *min, *sec, *ctime;
	ctime = &stack[--top];
	sec = &stack[--top];
	min = &stack[--top];
	hour = &stack[--top];
	day = &stack[--top];
	month = &stack[--top];
	year = &stack[--top];
	g_gbl_fields[ctime->ifield]->vtype = KEV_REAL;
	plctime((PLINT)year->i, (PLINT)month->i-1, (PLINT)day->i, (PLINT)hour->i, (PLINT)min->i, (PLFLT)sec->r, (PLFLT_NC_SCALAR)(&(g_gbl_fields[ctime->ifield]->r)));
	return top;
}

static int ke_plcalc_world(ke1_t *stack, int top) {
	ke1_t *rx, *ry, *wx, *wy, *window;
	window = &stack[--top];
	wy = &stack[--top];
	wx = &stack[--top];
	ry = &stack[--top];
	rx = &stack[--top];
	g_gbl_fields[wx->ifield]->vtype = KEV_REAL;
	g_gbl_fields[wy->ifield]->vtype = KEV_REAL;
	g_gbl_fields[window->ifield]->vtype = KEV_INT;
	plcalc_world((PLFLT)rx->r, (PLFLT)ry->r,
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[wx->ifield]->r)), 
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[wy->ifield]->r)), 
		(PLINT_NC_SCALAR)(&(g_gbl_fields[window->ifield]->i)));
	return top;
}

static int ke_plclear(ke1_t *stack, int top) {
	plclear();
	return top;
}

static int ke_plcol0(ke1_t *stack, int top) {
	ke1_t *col0;
	col0 = &stack[--top];
	plcol0((PLINT)col0->i);
	return top;
}

static int ke_plcol1(ke1_t *stack, int top) {
	ke1_t *col1;
	col1 = &stack[--top];
	plcol1((PLFLT)col1->r);
	return top;
}

static int ke_plcolorbar(ke1_t *stack, int top) {
	ke1_t *p_colorbar_width, *p_colorbar_height, *opt, *position, *x, *y, *x_length, *y_length,
		*bg_color, *bb_color, *bb_style, *low_cap_color, *high_cap_color, *cont_color, *cont_width,
		*n_labels, *label_opts, *labels, *naxes, *axis_opts, *ticks, *sub_ticks, *n_values, *values;

	values = &stack[--top];
	n_values = &stack[--top];
	sub_ticks = &stack[--top];
	ticks = &stack[--top];
	axis_opts = &stack[--top];
	naxes = &stack[--top];
	labels = &stack[--top];
	label_opts = &stack[--top];
	n_labels = &stack[--top];
	cont_width = &stack[--top];
	cont_color = &stack[--top];
	high_cap_color = &stack[--top];
	low_cap_color = &stack[--top];
	bb_style = &stack[--top];
	bb_color = &stack[--top];
	bg_color = &stack[--top];
	y_length = &stack[--top];
	x_length = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];
	position = &stack[--top];
	opt = &stack[--top];
	p_colorbar_height = &stack[--top];
	p_colorbar_width = &stack[--top];

	g_gbl_fields[p_colorbar_width->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_colorbar_height->ifield]->vtype = KEV_REAL;
	plcolorbar(
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_colorbar_width->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_colorbar_height->ifield]->r)),
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
		(PLINT_VECTOR)gsl_vector_const_ptr(label_opts->obj.vector,0),
		(PLCHAR_MATRIX)labels->obj.s,
		(PLINT)naxes->i,
		(PLCHAR_MATRIX)axis_opts->obj.s,
		(PLFLT_VECTOR)gsl_vector_const_ptr(ticks->obj.vector,0),
		(PLINT_VECTOR)sub_ticks->obj.vector, 
		(PLINT_VECTOR)n_values->obj.vector,
		(PLFLT_MATRIX)gsl_matrix_const_ptr(label_opts->obj.matrix, 0,0));
	g_gbl_fields[p_colorbar_width->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_colorbar_height->ifield]->vtype = KEV_REAL;
	return top;
}

static int ke_plconfigtime(ke1_t *stack, int top) {
	ke1_t *scale, *oﬀset1, *oﬀset2, *ccontrol, *ifbtime_oﬀset, *year, *month, *day, *hour, *min, *sec;
	sec = &stack[--top];
	min = &stack[--top];
	hour = &stack[--top];
	day = &stack[--top];
	month = &stack[--top];
	year = &stack[--top];
	ifbtime_oﬀset = &stack[--top];
	ccontrol = &stack[--top];
	oﬀset2 = &stack[--top];
	oﬀset1 = &stack[--top];
	scale = &stack[--top];

	plconfigtime((PLFLT)scale->r, (PLFLT)oﬀset1->r, (PLFLT)oﬀset2->r, (PLINT)ccontrol->i, 
		(PLINT)ifbtime_oﬀset->i, (PLINT)year->i, (PLINT)month->i, (PLINT)day->i, 
		(PLINT)hour->i, (PLINT)min->i, (PLFLT)sec->r);
	return top;
}

static int ke_plcont(ke1_t *stack, int top) {
	ke1_t *f, *nx, *ny, *kx, *lx, *ky, *ly, *clevel, *nlevel, *pltr, *pltr_data;

	pltr_data = &stack[--top];
	pltr = &stack[--top];
	nlevel = &stack[--top];
	clevel = &stack[--top];
	ly = &stack[--top];
	ky = &stack[--top];
	lx = &stack[--top];
	kx = &stack[--top];
	ny = &stack[--top];
	nx = &stack[--top];
	f = &stack[--top];

	plcont((PLFLT_MATRIX)gsl_matrix_ptr(f->obj.matrix,0,0), (PLINT)nx->i, (PLINT)ny->i, (PLINT)kx->i, (PLINT)lx->i,
		(PLINT)ky->i, (PLINT)ly->i, (PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0), (PLINT)nlevel->i,
		(PLTRANSFORM_callback)pltr->obj.tokp, (PLPointer)pltr_data->obj.tokp);
	return top;
}

static int ke_plcpstrm(ke1_t *stack, int top) {
	ke1_t *iplsr, *ﬂags;
	ﬂags = &stack[--top];
	iplsr = &stack[--top];

	plcpstrm((PLINT)iplsr->i, (PLBOOL)ﬂags->i);
	return top;
}

static int ke_plend1(ke1_t *stack, int top) {
	plend1();
	return top;
}

static int ke_plenv0(ke1_t *stack, int top) {
	ke1_t *xmin, *xmax, *ymin, *ymax, *just, *axis;
	axis = &stack[--top];
	just = &stack[--top];
	ymax = &stack[--top];
	ymin = &stack[--top];
	xmax = &stack[--top];
	xmin = &stack[--top];
	plenv0((PLFLT)xmin->r, (PLFLT)xmax->r, (PLFLT)ymin->r, (PLFLT)ymax->r, (PLINT)just->i, (PLINT)axis->i);
	return top;
}

static int ke_pleop(ke1_t *stack, int top) {
	pleop();
	return top;
}

static int ke_plerrx(ke1_t *stack, int top) {
	ke1_t *n, *xmin, *xmax, *y;
	y = &stack[--top];
	xmax = &stack[--top];
	xmin = &stack[--top];
	n = &stack[--top];
	plerrx((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(xmin->obj.vector, 0),
						(PLFLT_VECTOR)gsl_vector_const_ptr(xmax->obj.vector, 0),
						(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
	return top;
}

static int ke_plerry(ke1_t *stack, int top) {
	ke1_t *n, *ymin, *ymax, *y;
	y = &stack[--top];
	ymax = &stack[--top];
	ymin = &stack[--top];
	n = &stack[--top];
	plerry((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(ymin->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(ymax->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
	return top;
}

static int ke_plfamadv(ke1_t *stack, int top) {
	plfamadv();
	return top;
}

static int ke_plfill(ke1_t *stack, int top) {
	ke1_t *n, *x, *y;
	y = &stack[--top];
	x = &stack[--top];
	n = &stack[--top];
	plfill((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
	return top;
}

static int ke_plfill3(ke1_t *stack, int top) {
	ke1_t *n, *x, *y, *z;
	z = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];
	n = &stack[--top];
	plfill3((PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0));
	return top;
}

static int ke_plflush(ke1_t *stack, int top) {
	plflush();
	return top;
}

static int ke_plfont(ke1_t *stack, int top) {
	ke1_t *ifont;
	ifont = &stack[--top];
	plfont((PLINT)ifont->i);
	return top;
}

static int ke_plfontld(ke1_t *stack, int top) {
	ke1_t *fnt;
	fnt = &stack[--top];
	plfontld((PLINT)fnt->i);
	return top;
}

static int ke_plGetCursor(ke1_t *stack, int top) {
	ke1_t *gin;
	gin = &stack[--top];
	gin->i = plGetCursor((PLGraphicsIn *)gin->obj.gin);
	gin->vtype = KEV_INT;
	gin->ttype = KET_VAL;
	return top;
}

static int ke_plgchr(ke1_t *stack, int top) {
	ke1_t *p_def, *p_ht;
	p_ht = &stack[--top];
	p_def = &stack[--top];

	plgchr((PLFLT_NC_SCALAR)(&(g_gbl_fields[p_def->ifield]->r)), (PLFLT_NC_SCALAR)(&(g_gbl_fields[p_ht->ifield]->r)));
	return top;
}

static int ke_plgcmap1_range(ke1_t *stack, int top) {
	ke1_t *min_color, *max_color;
	max_color = &stack[--top];
	min_color = &stack[--top];

	g_gbl_fields[min_color->ifield]->vtype = KEV_REAL;
	g_gbl_fields[max_color->ifield]->vtype = KEV_REAL;
	plgcmap1_range((PLFLT_NC_SCALAR)(&(g_gbl_fields[min_color->ifield]->r)), (PLFLT_NC_SCALAR)(&(g_gbl_fields[max_color->ifield]->r)));
	return top;
}

static int ke_plgcol0(ke1_t *stack, int top) {
	ke1_t *icol0, *r, *g, *b;
	b = &stack[--top];
	g = &stack[--top];
	r = &stack[--top];
	icol0 = &stack[--top];

	g_gbl_fields[r->ifield]->vtype = KEV_INT;
	g_gbl_fields[g->ifield]->vtype = KEV_INT;
	g_gbl_fields[b->ifield]->vtype = KEV_INT;
	plgcol0((PLINT)icol0->i, 
		(PLINT_NC_SCALAR)(&(g_gbl_fields[r->ifield]->i)), 
		(PLINT_NC_SCALAR)(&(g_gbl_fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[b->ifield]->i)));
	return top;
}

static int ke_plgcol0a(ke1_t *stack, int top) {
	ke1_t *icol0, *r, *g, *b, *alpha;
	alpha = &stack[--top];
	b = &stack[--top];
	g = &stack[--top];
	r = &stack[--top];
	icol0 = &stack[--top];
	g_gbl_fields[r->ifield]->vtype = KEV_INT;
	g_gbl_fields[g->ifield]->vtype = KEV_INT;
	g_gbl_fields[b->ifield]->vtype = KEV_INT;
	g_gbl_fields[alpha->ifield]->vtype = KEV_REAL;

	plgcol0a((PLINT)icol0->i,
		(PLINT_NC_SCALAR)(&(g_gbl_fields[r->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[b->ifield]->i)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[alpha->ifield]->r)));
	return top;
}

static int ke_plgcolbg(ke1_t *stack, int top) {
	ke1_t *r, *g, *b;
	b = &stack[--top];
	g = &stack[--top];
	r = &stack[--top];
	g_gbl_fields[r->ifield]->vtype = KEV_INT;
	g_gbl_fields[g->ifield]->vtype = KEV_INT;
	g_gbl_fields[b->ifield]->vtype = KEV_INT;

	plgcolbg(
		(PLINT_NC_SCALAR)(&(g_gbl_fields[r->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[b->ifield]->i)));
	return top;
}

static int ke_plgcolbga(ke1_t *stack, int top) {
	ke1_t *r, *g, *b, *alpha;
	alpha = &stack[--top];
	b = &stack[--top];
	g = &stack[--top];
	r = &stack[--top];

	g_gbl_fields[r->ifield]->vtype = KEV_INT;
	g_gbl_fields[g->ifield]->vtype = KEV_INT;
	g_gbl_fields[b->ifield]->vtype = KEV_INT;
	g_gbl_fields[alpha->ifield]->vtype = KEV_REAL;

	plgcolbga(
		(PLINT_NC_SCALAR)(&(g_gbl_fields[r->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[g->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[b->ifield]->i)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[alpha->ifield]->r)));
	return top;
}

static int ke_plgcompression(ke1_t *stack, int top) {
	ke1_t *compression;
	compression = &stack[--top];

	g_gbl_fields[compression->ifield]->vtype = KEV_INT;
	plgcompression((PLINT_NC_SCALAR)(&(g_gbl_fields[compression->ifield]->i)));
	return top;
}

static int ke_plgdidev(ke1_t *stack, int top) {
	ke1_t *p_mar, *p_aspect, *p_jx, *p_jy;
	p_jy = &stack[--top];
	p_jx = &stack[--top];
	p_aspect = &stack[--top];
	p_mar = &stack[--top];

	g_gbl_fields[p_mar->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_aspect->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_jx->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_jy->ifield]->vtype = KEV_REAL;

	plgdidev(
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_mar->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_aspect->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_jx->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_jy->ifield]->r)));
	return top;
}

static int ke_plgdiori(ke1_t *stack, int top) {
	ke1_t *p_rot;
	p_rot = &stack[--top];

	g_gbl_fields[p_rot->ifield]->vtype = KEV_REAL;

	plgdiori((PLFLT_NC_SCALAR)(&(g_gbl_fields[p_rot->ifield]->r)));
	return top;
}

static int ke_plgdiplt(ke1_t *stack, int top) {
	ke1_t *p_xmin, *p_ymin, *p_xmax, *p_ymax;
	p_ymax = &stack[--top];
	p_xmax = &stack[--top];
	p_ymin = &stack[--top];
	p_xmin = &stack[--top];

	g_gbl_fields[p_xmin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_ymin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_xmax->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_ymax->ifield]->vtype = KEV_REAL;

	plgdiplt(
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_ymax->ifield]->r)));
	return top;
}

static int ke_function_plgdrawmode(ke1_t *stack, int top) {
	ke1_t *p;
	p = &stack[top - 1];
	stack[top++] = *p;
	p = &stack[top - 1];
	p->i = (PLINT)plgdrawmode();
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_plgfam(ke1_t *stack, int top) {
	ke1_t *p_fam, *p_num, *p_bmax;
	p_bmax = &stack[--top];
	p_num = &stack[--top];
	p_fam = &stack[--top];

	g_gbl_fields[p_fam->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_num->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_bmax->ifield]->vtype = KEV_INT;

	plgfam(
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_fam->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_num->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_bmax->ifield]->i)));
	return top;
}

static int ke_plgfci(ke1_t *stack, int top) {
	ke1_t *p_fci;
	p_fci = &stack[--top];

	g_gbl_fields[p_fci->ifield]->vtype = KEV_INT;

	plgfci((PLUNICODE_NC_SCALAR)(&(g_gbl_fields[p_fci->ifield]->i)));
	return top;
}

static int ke_plgfnam(ke1_t *stack, int top) {
	ke1_t *fnam;
	fnam = &stack[--top];

	g_gbl_fields[fnam->ifield]->vtype = KEV_STR;

	plgfnam((PLCHAR_NC_VECTOR)(&(g_gbl_fields[fnam->ifield]->obj.s)));
	return top;
}

static int ke_plgfont(ke1_t *stack, int top) {
	ke1_t *p_family, *p_style, *p_weight;
	p_weight = &stack[--top];
	p_style = &stack[--top];
	p_family = &stack[--top];

	g_gbl_fields[p_family->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_style->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_weight->ifield]->vtype = KEV_INT;

	plgfont(
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_family->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_style->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_weight->ifield]->i)));
	return top;
}

static int ke_plglevel(ke1_t *stack, int top) {
	ke1_t *p_level;
	p_level = &stack[--top];

	g_gbl_fields[p_level->ifield]->vtype = KEV_INT;

	plglevel((PLINT_NC_SCALAR)(&(g_gbl_fields[p_level->ifield]->i)));
	return top;
}

static int ke_plgpage(ke1_t *stack, int top) {
	ke1_t *p_xp, *p_yp, *p_xleng, *p_yleng, *p_xoﬀ, *p_yoﬀ;
	p_yoﬀ = &stack[--top];
	p_xoﬀ = &stack[--top];
	p_yleng = &stack[--top];
	p_xleng = &stack[--top];
	p_yp = &stack[--top];
	p_xp = &stack[--top];

	g_gbl_fields[p_xp->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_yp->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_xleng->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_yleng->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_xoﬀ->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_yoﬀ->ifield]->vtype = KEV_INT;

	plgpage(
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_xp->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_yp->ifield]->r)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_xleng->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_yleng->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_xoﬀ->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_yoﬀ->ifield]->i)));
	return top;
}

static int ke_function_plgra(ke1_t *stack, int top) {
	plgra();
	return top;
}

static int ke_plgradient(ke1_t *stack, int top) {
	ke1_t *n, *x, *y, *angle;
	angle = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];
	n = &stack[--top];
	plgradient((PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT)angle->r);
	return top;
}

static int ke_plgriddata(ke1_t *stack, int top) {
	ke1_t *x, *y, *z, *npts, *xg, *nptsx, *yg, *nptsy, *zg, *type, *data;
	data = &stack[--top];
	type = &stack[--top];
	zg = &stack[--top];
	nptsy = &stack[--top];
	yg = &stack[--top];
	nptsx = &stack[--top];
	xg = &stack[--top];
	npts = &stack[--top];
	z = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];

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

static int ke_plgspa(ke1_t *stack, int top) {
	ke1_t *xmin, *xmax, *ymin, *ymax;
	ymax = &stack[--top];
	ymin = &stack[--top];
	xmax = &stack[--top];
	xmin = &stack[--top];

	g_gbl_fields[xmin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[xmax->ifield]->vtype = KEV_REAL;
	g_gbl_fields[ymin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[ymax->ifield]->vtype = KEV_REAL;

	plgspa(
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[ymax->ifield]->r)));
	return top;
}

static int ke_plgstrm(ke1_t *stack, int top) {
	ke1_t *p_strm;
	p_strm = &stack[--top];

	g_gbl_fields[p_strm->ifield]->vtype = KEV_INT;

	plgstrm((PLINT_NC_SCALAR)(&(g_gbl_fields[p_strm->ifield]->i)));
		
	return top;
}

static int ke_plgver(ke1_t *stack, int top) {
	ke1_t *p_ver;
	p_ver = &stack[--top];

	g_gbl_fields[p_ver->ifield]->vtype = KEV_STR;

	plgver((PLCHAR_NC_VECTOR)(&(g_gbl_fields[p_ver->ifield]->obj.s)));

	return top;
}

static int ke_plgvpd(ke1_t *stack, int top) {
	ke1_t *p_xmin, *p_xmax, *p_ymin, *p_ymax;
	p_ymax = &stack[--top];
	p_ymin = &stack[--top];
	p_xmax = &stack[--top];
	p_xmin = &stack[--top];

	g_gbl_fields[p_xmin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_xmax->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_ymin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_ymax->ifield]->vtype = KEV_REAL;

	plgvpd(
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_ymax->ifield]->r)));
	return top;
}

static int ke_plgvpw(ke1_t *stack, int top) {
	ke1_t *p_xmin, *p_xmax, *p_ymin, *p_ymax;
	p_ymax = &stack[--top];
	p_ymin = &stack[--top];
	p_xmax = &stack[--top];
	p_xmin = &stack[--top];

	g_gbl_fields[p_xmin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_xmax->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_ymin->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_ymax->ifield]->vtype = KEV_REAL;

	plgvpw(
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_xmin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_xmax->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_ymin->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_ymax->ifield]->r)));
	return top;
}

static int ke_plgxax(ke1_t *stack, int top) {
	ke1_t *p_digmax, *p_digits;
	p_digits = &stack[--top];
	p_digmax = &stack[--top];

	g_gbl_fields[p_digits->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_digits->ifield]->vtype = KEV_INT;

	plgxax(
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_digits->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_digits->ifield]->i)));
	return top;
}

static int ke_plgyax(ke1_t *stack, int top) {
	ke1_t *p_digmax, *p_digits;
	p_digits = &stack[--top];
	p_digmax = &stack[--top];

	g_gbl_fields[p_digits->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_digits->ifield]->vtype = KEV_INT;

	plgyax(
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_digits->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_digits->ifield]->i)));
	return top;
}

static int ke_plgzax(ke1_t *stack, int top) {
	ke1_t *p_digmax, *p_digits;
	p_digits = &stack[--top];
	p_digmax = &stack[--top];

	g_gbl_fields[p_digits->ifield]->vtype = KEV_INT;
	g_gbl_fields[p_digits->ifield]->vtype = KEV_INT;

	plgzax(
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_digits->ifield]->i)),
		(PLINT_NC_SCALAR)(&(g_gbl_fields[p_digits->ifield]->i)));
	return top;
}

static int ke_plhist(ke1_t *stack, int top) {
	ke1_t *n, *data, *datmin, *datmax, *nbin, *opt;
	opt = &stack[--top];
	nbin = &stack[--top];
	datmax = &stack[--top];
	datmin = &stack[--top];
	data = &stack[--top];
	n = &stack[--top];

	plhist(
		(PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(data->obj.vector, 0),
		(PLFLT)datmin->r, 
		(PLFLT)datmax->r, 
		(PLINT)nbin->i,
		(PLINT)opt->i);
	return top;
}

static int ke_plhlsrgb(ke1_t *stack, int top) {
	ke1_t *h, *l, *s, *p_r, *p_g, *p_b;
	p_b = &stack[--top];
	p_g = &stack[--top];
	p_r = &stack[--top];
	s = &stack[--top];
	l = &stack[--top];
	h = &stack[--top];

	g_gbl_fields[p_r->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_g->ifield]->vtype = KEV_REAL;
	g_gbl_fields[p_b->ifield]->vtype = KEV_REAL;

	plhlsrgb(
		(PLFLT)h->r, 
		(PLFLT)l->r, 
		(PLFLT)s->r,
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_r->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_g->ifield]->r)),
		(PLFLT_NC_SCALAR)(&(g_gbl_fields[p_b->ifield]->r)));
	return top;
}



void ke_plot_hash() {
	ke_hash_add((fncp)&ke_plgcolbg, PLOT_PLGCOLBG);
	ke_hash_add((fncp)&ke_plgcolbga, PLOT_PLGCOLBGA);


	ke_hash_add((fncp)&ke_plgcol0a, PLOT_PLGCOL0A);
	ke_hash_add((fncp)&ke_plgcol0, PLOT_PLGCOL0);
	ke_hash_add((fncp)&ke_plgcmap1_range, PLOT_PLGCMAP1RANGE);
	ke_hash_add((fncp)&ke_plgchr, PLOT_PLGCHR);


	ke_hash_add((fncp)&ke_plinit, PLOT);
	ke_hash_add((fncp)&ke_plinit, PLOT_INIT);
	ke_hash_add((fncp)&ke_plsdev, PLOT_PLSDEV);
	ke_hash_add((fncp)&ke_plsdev, PLOT_PLDEV);
	ke_hash_add((fncp)&ke_plenv, PLOT_PLENV);
	ke_hash_add((fncp)&ke_plline, PLOT_PLLINE);
	ke_hash_add((fncp)&ke_plend, PLOT_PLEND);
	ke_hash_add((fncp)&ke_plpause, PLOT_PLSPAUSE);
	ke_hash_add((fncp)&ke_plpause, PLOT_PLPAUSE);
	ke_hash_add((fncp)&ke_pl_setcontlabelformat, PLOT_PLSETCONTLABELFORMAT);
	ke_hash_add((fncp)&ke_pl_setcontlabelparam, PLOT_PLSETCONTLABELPARAM);
	ke_hash_add((fncp)&ke_pladv, PLOT_PLADV);
	ke_hash_add((fncp)&ke_plarc, PLOT_PLARC);
	ke_hash_add((fncp)&ke_plaxes, PLOT_PLAXES);
	ke_hash_add((fncp)&ke_plbin, PLOT_PLBIN);
	ke_hash_add((fncp)&ke_plbop, PLOT_PLBOP);
	ke_hash_add((fncp)&ke_plbox, PLOT_PLBOX);
	ke_hash_add((fncp)&ke_plbox3, PLOT_PLBOX3);
	ke_hash_add((fncp)&ke_plbtime, PLOT_PLBTIME);
	ke_hash_add((fncp)&ke_plctime, PLOT_PLCTIME);
	ke_hash_add((fncp)&ke_plcalc_world, PLOT_PLCALCWORLD);
	ke_hash_add((fncp)&ke_plcol0, PLOT_PLCOL0);
	ke_hash_add((fncp)&ke_plcol1, PLOT_PLCOL1);
	ke_hash_add((fncp)&ke_plcolorbar, PLOT_PLCOLORBAR);
	ke_hash_add((fncp)&ke_plconfigtime, PLOT_PLCONFIGTIME);
	ke_hash_add((fncp)&ke_plcont, PLOT_PLCONT);
	ke_hash_add((fncp)&ke_plcpstrm, PLOT_PLCPSTRM);

	ke_hash_add((fncp)&ke_plend1, PLOT_PLEND1);
	ke_hash_add((fncp)&ke_plenv0, PLOT_PLENV0);
	ke_hash_add((fncp)&ke_pleop, PLOT_PLEOP);
	ke_hash_add((fncp)&ke_plerrx, PLOT_PLERRX);
	ke_hash_add((fncp)&ke_plerry, PLOT_PLERRY);
	ke_hash_add((fncp)&ke_plfamadv, PLOT_PLFAMADV);
	ke_hash_add((fncp)&ke_plfill, PLOT_PLFILL);
	ke_hash_add((fncp)&ke_plfill3, PLOT_PLFILL3);
	ke_hash_add((fncp)&ke_plflush, PLOT_PLFLUSH);
	ke_hash_add((fncp)&ke_plfont, PLOT_PLFONT);
	ke_hash_add((fncp)&ke_plfontld, PLOT_PLFONTLD);
	ke_hash_add((fncp)&ke_plGetCursor, PLOT_PLGETCURSOR);


}

