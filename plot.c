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
	plline((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0));
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
	plaxes((PLFLT)x0->r, (PLFLT)y0->r, (PLCHAR_VECTOR)xopt->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i);
	return top;
}

static int ke_plbin(ke1_t *stack, int top) {
	ke1_t *nbin, *x, *y, *opt;
	opt = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];
	nbin = &stack[--top];
	plbin((PLINT)nbin->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0), (PLINT)opt->i);
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
	plbox3((PLCHAR_VECTOR)xopt->obj.s, (PLCHAR_VECTOR)xlabel->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLCHAR_VECTOR)ylabel->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i, (PLCHAR_VECTOR)zopt->obj.s, (PLCHAR_VECTOR)zlabel->obj.s, (PLFLT)ztick->r, (PLINT)nzsub->i);
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
	plbtime((PLINT_NC_SCALAR)(&(g_gbl_fields[year->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[month->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[day->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[hour->ifield]->i)),
			(PLINT_NC_SCALAR)(&(g_gbl_fields[min->ifield]->i)),
			(PLFLT_NC_SCALAR)(&(g_gbl_fields[sec->ifield]->r)),
			(PLFLT)ctime->r);
	g_gbl_fields[year->ifield]->vtype = KEV_INT;
	g_gbl_fields[month->ifield]->vtype = KEV_INT;
	g_gbl_fields[day->ifield]->vtype = KEV_INT;
	g_gbl_fields[hour->ifield]->vtype = KEV_INT;
	g_gbl_fields[min->ifield]->vtype = KEV_INT;
	g_gbl_fields[sec->ifield]->vtype = KEV_REAL;
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
	plfill3((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), (PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0));
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


void ke_plot_hash() {
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
}

