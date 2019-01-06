#include "kexpr.h"
#include "plot.h"
#include "plplot.h"


static int ke_plsdev(ke1_t *stack, int top) {
	ke1_t *p;
	p = &stack[--top];
	plsdev(p->s);
	return top;
}

static int ke_plinit(ke1_t *stack, int top) {
	ke1_t *p;
	p = &stack[--top];
	plsdev(p->s);
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
	plline((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->gsl.vector, (PLINT)0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->gsl.vector, (PLINT)0));
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
	plaxes((PLFLT)x0->r, (PLFLT)y0->r, (PLCHAR_VECTOR)xopt->s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->s, (PLFLT)ytick->r, (PLINT)nysub->i);
	return top;
}

static int ke_plbin(ke1_t *stack, int top) {
	ke1_t *nbin, *x, *y, *opt;
	opt = &stack[--top];
	y = &stack[--top];
	x = &stack[--top];
	nbin = &stack[--top];
	plbin((PLINT)nbin->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->gsl.vector, (PLINT)0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->gsl.vector, (PLINT)0), (PLINT)opt->i);
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
	plbox((PLCHAR_VECTOR)xopt->s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->s, (PLFLT)ytick->r, (PLINT)nysub->i);
	return --top;
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
	plbox3((PLCHAR_VECTOR)xopt->s, (PLCHAR_VECTOR)xlabel->s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->s, (PLCHAR_VECTOR)ylabel->s, (PLFLT)ytick->r, (PLINT)nysub->i, (PLCHAR_VECTOR)zopt->s, (PLCHAR_VECTOR)zlabel->s, (PLFLT)ztick->r, (PLINT)nzsub->i);
	return --top;
}

void ke_plot_hash() {
	ke_hash_add((fncp)&ke_plinit, PLOT);
	ke_hash_add((fncp)&ke_plinit, PLOT_INIT);
	ke_hash_add((fncp)&ke_plsdev, PLOT_PLSDEV);
	ke_hash_add((fncp)&ke_plenv, PLOT_PLENV);
	ke_hash_add((fncp)&ke_plline, PLOT_PLLINE);
	ke_hash_add((fncp)&ke_plend, PLOT_PLEND);
	ke_hash_add((fncp)&ke_plpause, PLOT_PLSPAUSE);
	ke_hash_add((fncp)&ke_pl_setcontlabelformat, PLOT_PLSETCONTLABELFORMAT);
	ke_hash_add((fncp)&ke_pl_setcontlabelparam, PLOT_PLSETCONTLABELPARAM);
	ke_hash_add((fncp)&ke_pladv, PLOT_PLADV);
	ke_hash_add((fncp)&ke_plarc, PLOT_PLARC);
	ke_hash_add((fncp)&ke_plaxes, PLOT_PLAXES);
	ke_hash_add((fncp)&ke_plbin, PLOT_PLBIN);
	ke_hash_add((fncp)&ke_plbop, PLOT_PLBOP);
	ke_hash_add((fncp)&ke_plbox, PLOT_PLBOX);
	ke_hash_add((fncp)&ke_plbox3, PLOT_PLBOX3);
}

