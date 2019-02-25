#include "../kexpr.h"
#include "plot.h"
#include <plplot/plplot.h>
#include <plplot/plplotP.h>
#include <stdio.h>
#include <Windows.h>
#include "../api.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "..\stb\stb_image.h"
#include "..\stb\stb_image_write.h"
#include "..\stb\stb_image_resize.h"

#define ke_get_out dllke_get_out

token_t * (__cdecl *dllke_get_out)(sml_t *sml) = 0;
int(__cdecl *dllke_hash_add)(sml_t *, fncp, char *) = 0;

SML_EXPORT void SML_CALL dllke_plot_hash(sml_t* sml);

SML_EXPORT void SML_CALL ke_dll_hash_add(sml_t* sml) {
	dllke_hash_add = sml->dllke_hash_add;
	dllke_get_out = sml->dllke_get_out;
	dllke_plot_hash(sml);
}

SML_EXPORT void SML_CALL ke_plimload(sml_t* sml) { 
	token_t *filename, *x, *y, *n, *zero;
	zero = sml_pop_token(sml);
	n = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	filename = sml_pop_token(sml);

	int xx, yy, nn;
	unsigned char *image = stbi_load(filename->obj.s, &xx, &yy, &nn, (int)zero->i);
	x->i = (int64_t)xx;
	y->i = (int64_t)yy;
	n->i = (int64_t)nn;
	x->r = (double)xx;
	y->r = (double)yy;
	n->r = (double)nn;
	x->vtype = KEV_INT;
	y->vtype = KEV_INT;
	n->vtype = KEV_INT;
	sml_push_image(sml, image);

}

SML_EXPORT void SML_CALL ke_plimfree(sml_t* sml) {
	token_t * tokp = sml_pop_token(sml);
	void * ptr = sml_get_ptr(tokp);
	free(ptr);
	sml_set_ptr_null(tokp);
}

SML_EXPORT void SML_CALL ke_plimwrite(sml_t* sml) {
	token_t *filename, *w, *h, *comp, *data, *quality, *type;
	type = sml_pop_token(sml);
	quality = sml_pop_token(sml);
	data = sml_pop_token(sml);
	comp = sml_pop_token(sml);
	h = sml_pop_token(sml);
	w = sml_pop_token(sml);
	filename = sml_pop_token(sml);
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
}

SML_EXPORT void SML_CALL ke_plimresize(sml_t* sml) {
	token_t *in, *in_w, *in_h, *in_stride, *out_w, *out_h, *out_stride, *nb_channel;

	nb_channel = sml_pop_token(sml);
	out_stride = sml_pop_token(sml);
	out_h = sml_pop_token(sml);
	out_w = sml_pop_token(sml);
	in_stride = sml_pop_token(sml);
	in_h = sml_pop_token(sml);
	in_w = sml_pop_token(sml);
	in = sml_pop_token(sml);
	char * image = malloc(out_h->i * out_w->i * nb_channel->i);
	stbir_resize_uint8(in->obj.image, (int)in_w->i, (int)in_h->i, (int)in_stride->i,
		image, (int)out_w->i, (int)out_h->i, (int)out_stride->i, (int)nb_channel->i);

	sml_push_image(sml, image);
}

SML_EXPORT void SML_CALL ke_plsdev(sml_t* sml) {
	token_t *p;
	p = sml_pop_token(sml);
	plsdev(p->obj.s);
}

SML_EXPORT void SML_CALL ke_plinit(sml_t* sml) {
	token_t *p;
	p = sml_pop_token(sml);
	plsdev(p->obj.s);
	plinit();
}

SML_EXPORT void SML_CALL ke_plenv(sml_t* sml) {
	token_t *xmin, *xmax, *ymin, *ymax, *just, *axis;
	axis = sml_pop_token(sml);
	just = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	plenv((PLFLT)xmin->r, (PLFLT)xmax->r, (PLFLT)ymin->r, (PLFLT)ymax->r, (PLINT)just->i, (PLINT)axis->i);
}

SML_EXPORT void SML_CALL ke_plline(sml_t* sml) {
	token_t *n, *x, *y;
	y = sml_pop_token(sml);
	x = sml_pop_token(sml); 
	n = sml_pop_token(sml);
	plline((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0));
}

SML_EXPORT void SML_CALL ke_plpause(sml_t* sml) {
	token_t *p;
	p = sml_pop_token(sml);
	plspause((PLBOOL)p->i);
}

SML_EXPORT void SML_CALL ke_plend(sml_t* sml) {
	plend();
}

SML_EXPORT void SML_CALL ke_pl_setcontlabelformat(sml_t* sml) {
	int sigdig = sml_pop_int(sml);
	int lexp = sml_pop_int(sml);
	pl_setcontlabelformat(lexp, sigdig);
}

SML_EXPORT void SML_CALL ke_pl_setcontlabelparam(sml_t* sml) { 
	int active = sml_pop_int(sml);
	double spacing = sml_pop_real(sml);
	double size = sml_pop_real(sml);
	double offset = sml_pop_real(sml);
	pl_setcontlabelparam((PLFLT)offset, (PLFLT)size, (PLFLT)spacing, (PLINT)active);
}

SML_EXPORT void SML_CALL ke_pladv(sml_t* sml) { 
	int page = sml_pop_int(sml);
	pladv((PLINT)page);
}

SML_EXPORT void SML_CALL ke_plarc(sml_t* sml) { 
	int fill = sml_pop_int(sml);
	double rotate = sml_pop_real(sml);
	double angle2 = sml_pop_real(sml);
	double angle1 = sml_pop_real(sml);
	double b = sml_pop_real(sml);
	double a = sml_pop_real(sml);
	double y = sml_pop_real(sml);
	double x = sml_pop_real(sml);
	plarc((PLFLT)x, (PLFLT)y, (PLFLT)a, (PLFLT)b, (PLFLT)angle1, (PLFLT)angle2, (PLFLT)rotate,(PLBOOL)fill);
}

SML_EXPORT void SML_CALL ke_plaxes(sml_t* sml) { 
	token_t *x0, *y0, *xopt, *xtick, *nxsub, *yopt, *ytick, *nysub;
	nysub= sml_pop_token(sml);
	ytick = sml_pop_token(sml);
	yopt = sml_pop_token(sml);
	nxsub = sml_pop_token(sml);
	xtick = sml_pop_token(sml);
	xopt = sml_pop_token(sml);
	y0 = sml_pop_token(sml);
	x0 = sml_pop_token(sml);
	plaxes((PLFLT)x0->r, (PLFLT)y0->r, (PLCHAR_VECTOR)xopt->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i,
		(PLCHAR_VECTOR)yopt->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i);
}

SML_EXPORT void SML_CALL ke_plbin(sml_t* sml) { 
	token_t *nbin, *x, *y, *opt;
	opt = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	nbin = sml_pop_token(sml);
	plbin((PLINT)nbin->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, (PLINT)0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, (PLINT)0), (PLINT)opt->i);
}

SML_EXPORT void SML_CALL ke_plbop(sml_t* sml) { 
	plbop();
}

SML_EXPORT void SML_CALL ke_plbox(sml_t* sml) { 
	token_t *xopt, *xtick, *nxsub, *yopt, *ytick, *nysub;
	nysub = sml_pop_token(sml);
	ytick = sml_pop_token(sml);
	yopt = sml_pop_token(sml);
	nxsub = sml_pop_token(sml);
	xtick = sml_pop_token(sml);
	xopt = sml_pop_token(sml);
	plbox((PLCHAR_VECTOR)xopt->obj.s, (PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLFLT)ytick->r, (PLINT)nysub->i);
}

SML_EXPORT void SML_CALL ke_plbox3(sml_t* sml) { 
	token_t *xopt, *xlabel, *xtick, *nxsub, *yopt, *ylabel, *ytick, *nysub, *zopt, *zlabel, *ztick, *nzsub;
	nzsub = sml_pop_token(sml);
	ztick = sml_pop_token(sml);
	zlabel = sml_pop_token(sml);
	zopt = sml_pop_token(sml);
	nysub = sml_pop_token(sml);
	ytick = sml_pop_token(sml);
	ylabel = sml_pop_token(sml);
	yopt = sml_pop_token(sml);
	nxsub = sml_pop_token(sml);
	xtick = sml_pop_token(sml);
	xlabel = sml_pop_token(sml);
	xopt = sml_pop_token(sml);
	plbox3((PLCHAR_VECTOR)xopt->obj.s, (PLCHAR_VECTOR)xlabel->obj.s, 
		(PLFLT)xtick->r, (PLINT)nxsub->i, (PLCHAR_VECTOR)yopt->obj.s, (PLCHAR_VECTOR)ylabel->obj.s, 
		(PLFLT)ytick->r, (PLINT)nysub->i, (PLCHAR_VECTOR)zopt->obj.s, (PLCHAR_VECTOR)zlabel->obj.s, 
		(PLFLT)ztick->r, (PLINT)nzsub->i);
}

SML_EXPORT void SML_CALL ke_plbtime(sml_t* sml) { 
	token_t *year, *month, *day, *hour, *min, *sec, *ctime;
	ctime = sml_pop_token(sml);
	sec = sml_pop_token(sml);
	min = sml_pop_token(sml);
	hour = sml_pop_token(sml);
	day = sml_pop_token(sml);
	month = sml_pop_token(sml);
	year = sml_pop_token(sml);


	ctime = sml_pop_token(sml);
	year->vtype = KEV_INT;
	month->vtype = KEV_INT;
	day->vtype = KEV_INT;
	hour->vtype = KEV_INT;
	min->vtype = KEV_INT;
	sec->vtype = KEV_REAL;
	plbtime((PLINT_NC_SCALAR)(&(year->i)),
			(PLINT_NC_SCALAR)(&(month->i)),
			(PLINT_NC_SCALAR)(&(day->i)),
			(PLINT_NC_SCALAR)(&(hour->i)),
			(PLINT_NC_SCALAR)(&(min->i)),
			(PLFLT_NC_SCALAR)(&(sec->r)),
			(PLFLT)ctime->r);
}

SML_EXPORT void SML_CALL ke_plctime(sml_t* sml) { 
	token_t *year, *month, *day, *hour, *min, *sec, *ctime;
	ctime = sml_pop_token(sml);
	sec = sml_pop_token(sml);
	min = sml_pop_token(sml);
	hour = sml_pop_token(sml);
	day = sml_pop_token(sml);
	month = sml_pop_token(sml);
	year = sml_pop_token(sml);
	ctime->vtype = KEV_REAL;
	plctime((PLINT)year->i, (PLINT)month->i-1, (PLINT)day->i, (PLINT)hour->i, (PLINT)min->i, (PLFLT)sec->r, (PLFLT_NC_SCALAR)(&(ctime->r)));
	ctime->i = (int64_t)ctime->r;
}

SML_EXPORT void SML_CALL ke_plcalc_world(sml_t* sml) { 
	token_t *rx, *ry, *wx, *wy, *window;
	window = sml_pop_token(sml);
	wy = sml_pop_token(sml);
	wx = sml_pop_token(sml);
	ry = sml_pop_token(sml);
	rx = sml_pop_token(sml);
	wx->vtype = KEV_REAL;
	wy->vtype = KEV_REAL;
	window->vtype = KEV_INT;
	plcalc_world((PLFLT)rx->r, (PLFLT)ry->r,
		(PLFLT_NC_SCALAR)(&(wx->r)), 
		(PLFLT_NC_SCALAR)(&(wy->r)), 
		(PLINT_NC_SCALAR)(&(window->i)));
	wx->i = (int64_t)wx->r;
	wy->i = (int64_t)wy->r;
	window->r = (double)window->i;
}

SML_EXPORT void SML_CALL ke_plclear(sml_t* sml) { 
	plclear();
}

SML_EXPORT void SML_CALL ke_plcol0(sml_t* sml) { 
	token_t *col0;
	col0 = sml_pop_token(sml);
	plcol0((PLINT)col0->i);
}

SML_EXPORT void SML_CALL ke_plcol1(sml_t* sml) { 
	token_t *col1;
	col1 = sml_pop_token(sml);
	plcol1((PLFLT)col1->r);
}


SML_EXPORT void SML_CALL ke_plcolorbar(sml_t* sml) { 
	token_t *p_colorbar_width, *p_colorbar_height, *opt, *position, *x, *y, *x_length, *y_length;
	token_t *bg_color, *bb_color, *bb_style, *low_cap_color, *high_cap_color, *cont_color, *cont_width;
	token_t *n_labels, *label_opts, *labels, *naxes, *axis_opts, *ticks, *sub_ticks, *n_values, *values;

	values = sml_pop_token(sml);
	n_values = sml_pop_token(sml);
	sub_ticks = sml_pop_token(sml);
	ticks = sml_pop_token(sml);
	axis_opts = sml_pop_token(sml);
	naxes = sml_pop_token(sml);
	labels = sml_pop_token(sml);
	label_opts = sml_pop_token(sml);
	n_labels = sml_pop_token(sml);
	cont_width = sml_pop_token(sml);
	cont_color = sml_pop_token(sml);
	high_cap_color = sml_pop_token(sml);
	low_cap_color = sml_pop_token(sml);
	bb_style = sml_pop_token(sml);
	bb_color = sml_pop_token(sml);
	bg_color = sml_pop_token(sml);
	y_length = sml_pop_token(sml);
	x_length = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	position = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	p_colorbar_height = sml_pop_token(sml);
	p_colorbar_width = sml_pop_token(sml);

	p_colorbar_width->vtype = KEV_REAL;
	p_colorbar_height->vtype = KEV_REAL;
	plcolorbar(
		(PLFLT_NC_SCALAR)(&p_colorbar_width->r),
		(PLFLT_NC_SCALAR)(&p_colorbar_height->r),
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
	p_colorbar_width->i = (int64_t)p_colorbar_width->r;
	p_colorbar_height->i = (int64_t)p_colorbar_height->r;
}

SML_EXPORT void SML_CALL ke_plcont(sml_t* sml) { 
	token_t *f, *nx, *ny, *kx, *lx, *ky, *ly, *clevel, *nlevel, *pltr, *pltr_data;
	pltr_data = sml_pop_token(sml);
	pltr = sml_pop_token(sml);
	nlevel = sml_pop_token(sml);
	clevel = sml_pop_token(sml);
	ly = sml_pop_token(sml);
	ky = sml_pop_token(sml);
	lx = sml_pop_token(sml);
	kx = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	f = sml_pop_token(sml);

	plcont((PLFLT_MATRIX)gsl_matrix_ptr(f->obj.matrix,0,0), (PLINT)nx->i, (PLINT)ny->i, (PLINT)kx->i, (PLINT)lx->i,
		(PLINT)ky->i, (PLINT)ly->i, (PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0), (PLINT)nlevel->i,
		(PLTRANSFORM_callback)pltr->obj.tokp, (PLPointer)pltr_data->obj.tokp);
}

SML_EXPORT void SML_CALL ke_plcpstrm(sml_t* sml) { 
	token_t *iplsr, *flags;
	flags = sml_pop_token(sml);
	iplsr = sml_pop_token(sml);
	plcpstrm((PLINT)iplsr->i, (PLBOOL)flags->i);
}

SML_EXPORT void SML_CALL ke_plend1(sml_t* sml) { 
	plend1();
}

SML_EXPORT void SML_CALL ke_plenv0(sml_t* sml) { 
	token_t *xmin, *xmax, *ymin, *ymax, *just, *axis;
	axis = sml_pop_token(sml);
	just = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	plenv0((PLFLT)xmin->r, (PLFLT)xmax->r, (PLFLT)ymin->r, (PLFLT)ymax->r, (PLINT)just->i, (PLINT)axis->i);
}

SML_EXPORT void SML_CALL ke_pleop(sml_t* sml) { 
	pleop();
}

SML_EXPORT void SML_CALL ke_plerrx(sml_t* sml) { 
	token_t *n, *xmin, *xmax, *y;
	y = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	n = sml_pop_token(sml);
	plerrx((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(xmin->obj.vector, 0),
						(PLFLT_VECTOR)gsl_vector_const_ptr(xmax->obj.vector, 0),
						(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
}

SML_EXPORT void SML_CALL ke_plerry(sml_t* sml) { 
	token_t *n, *ymin, *ymax, *y;
	y = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	n = sml_pop_token(sml);
	plerry((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(ymin->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(ymax->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
}

SML_EXPORT void SML_CALL ke_plfamadv(sml_t* sml) { 
	plfamadv();
}

SML_EXPORT void SML_CALL ke_plfill(sml_t* sml) { 
	token_t *n, *x, *y;
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);
	plfill((PLINT)n->i, (PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), (PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0));
}

SML_EXPORT void SML_CALL ke_plfill3(sml_t* sml) { 
	token_t *n, *x, *y, *z;
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);
	plfill3((PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0));
}

SML_EXPORT void SML_CALL ke_plflush(sml_t* sml) { 
	plflush();
}

SML_EXPORT void SML_CALL ke_plfont(sml_t* sml) { 
	token_t *ifont;
	ifont = sml_pop_token(sml);
	plfont((PLINT)ifont->i);
}

SML_EXPORT void SML_CALL ke_plfontld(sml_t* sml) { 
	token_t *fnt;
	fnt = sml_pop_token(sml);
	plfontld((PLINT)fnt->i);
}

SML_EXPORT void SML_CALL ke_plGetCursor(sml_t* sml) { 
	token_t *gin;
	gin = sml_pop_token(sml);
	gin->i = plGetCursor((PLGraphicsIn *)gin->obj.plgrin);
	gin->vtype = KEV_INT;
	gin->ttype = KET_VAL;
}

SML_EXPORT void SML_CALL ke_plgchr(sml_t* sml) { 
	token_t *p_def, *p_ht;
	p_ht = sml_pop_token(sml);
	p_def = sml_pop_token(sml);

	plgchr((PLFLT_NC_SCALAR)(&(p_def->r)), (PLFLT_NC_SCALAR)(&(p_ht->r)));
	p_def->i = (int64_t)p_def->r;
	p_ht->i = (int64_t)p_ht->r;
}

SML_EXPORT void SML_CALL ke_plgcmap1_range(sml_t* sml) { 
	token_t *min_color, *max_color;
	max_color = sml_pop_token(sml);
	min_color = sml_pop_token(sml);

	min_color->vtype = KEV_REAL;
	max_color->vtype = KEV_REAL;
	plgcmap1_range((PLFLT_NC_SCALAR)(&(min_color->r)), (PLFLT_NC_SCALAR)(&(max_color->r)));
	min_color->i = (int64_t)min_color->r;
	max_color->i = (int64_t)max_color->r;
}

SML_EXPORT void SML_CALL ke_plgcol0(sml_t* sml) { 
	token_t *icol0, *r, *g, *b;
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);
	icol0 = sml_pop_token(sml);

	r->vtype = KEV_INT;
	g->vtype = KEV_INT;
	b->vtype = KEV_INT;
	plgcol0((PLINT)icol0->i, 
		(PLINT_NC_SCALAR)(&(r->i)), 
		(PLINT_NC_SCALAR)(&(g->i)),
		(PLINT_NC_SCALAR)(&(b->i)));
	r->r = (double)r->i;
	g->r = (double)g->i;
	b->r = (double)b->i;
}

SML_EXPORT void SML_CALL ke_plgcol0a(sml_t* sml) { 
	token_t *icol0, *r, *g, *b, *alpha;
	alpha = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);
	icol0 = sml_pop_token(sml);
	r->vtype = KEV_INT;
	g->vtype = KEV_INT;
	b->vtype = KEV_INT;
	alpha->vtype = KEV_REAL;

	plgcol0a((PLINT)icol0->i,
		(PLINT_NC_SCALAR)(&(r->i)),
		(PLINT_NC_SCALAR)(&(g->i)),
		(PLINT_NC_SCALAR)(&(b->i)),
		(PLFLT_NC_SCALAR)(&(alpha->r)));
	r->r = (double)r->i;
	g->r = (double)g->i;
	b->r = (double)b->i;
	alpha->i = (int64_t)alpha->r;
}

SML_EXPORT void SML_CALL ke_plgcolbg(sml_t* sml) {
	token_t *r, *g, *b;
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);
	r->vtype = KEV_INT;
	g->vtype = KEV_INT;
	b->vtype = KEV_INT;

	plgcolbg(
		(PLINT_NC_SCALAR)(&(r->i)),
		(PLINT_NC_SCALAR)(&(g->i)),
		(PLINT_NC_SCALAR)(&(b->i)));
	r->r = (double)r->i;
	g->r = (double)g->i;
	b->r = (double)b->i;
}

SML_EXPORT void SML_CALL ke_plgcolbga(sml_t* sml) { 
	token_t *r, *g, *b, *alpha;
	alpha = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	r->vtype = KEV_INT;
	g->vtype = KEV_INT;
	b->vtype = KEV_INT;
	alpha->vtype = KEV_REAL;

	plgcolbga(
		(PLINT_NC_SCALAR)(&(r->i)),
		(PLINT_NC_SCALAR)(&(g->i)),
		(PLINT_NC_SCALAR)(&(b->i)),
		(PLFLT_NC_SCALAR)(&(alpha->r)));
	r->r = (double)r->i;
	g->r = (double)g->i;
	b->r = (double)b->i;
	alpha->i = (int64_t)alpha->r;
}

SML_EXPORT void SML_CALL ke_plgcompression(sml_t* sml) { 
	token_t *compression;
	compression = sml_pop_token(sml);

	compression->vtype = KEV_INT;
	plgcompression((PLINT_NC_SCALAR)(&(compression->i)));
	compression->r = (double)compression->i;
}

SML_EXPORT void SML_CALL ke_plgdidev(sml_t* sml) { 
	token_t *p_mar, *p_aspect, *p_jx, *p_jy;
	p_jy = sml_pop_token(sml);
	p_jx = sml_pop_token(sml);
	p_aspect = sml_pop_token(sml);
	p_mar = sml_pop_token(sml);

	p_mar->vtype = KEV_REAL;
	p_aspect->vtype = KEV_REAL;
	p_jx->vtype = KEV_REAL;
	p_jy->vtype = KEV_REAL;

	plgdidev(
		(PLFLT_NC_SCALAR)(&(p_mar->r)),
		(PLFLT_NC_SCALAR)(&(p_aspect->r)),
		(PLFLT_NC_SCALAR)(&(p_jx->r)),
		(PLFLT_NC_SCALAR)(&(p_jy->r)));
	p_mar->i = (int64_t)p_mar->r;
	p_aspect->i = (int64_t)p_aspect->r;
	p_jx->i = (int64_t)p_jx->r;
	p_jy->i = (int64_t)p_jy->r;
}

SML_EXPORT void SML_CALL ke_plgdiori(sml_t* sml) { 
	token_t *p_rot;
	p_rot = sml_pop_token(sml);

	p_rot->vtype = KEV_REAL;

	plgdiori((PLFLT_NC_SCALAR)(&(p_rot->r)));
	p_rot->i = (int64_t)p_rot->r;
}

SML_EXPORT void SML_CALL ke_plgdiplt(sml_t* sml) { 
	token_t *p_xmin, *p_ymin, *p_xmax, *p_ymax;
	p_ymax = sml_pop_token(sml);
	p_xmax = sml_pop_token(sml);
	p_ymin = sml_pop_token(sml);
	p_xmin = sml_pop_token(sml);

	p_xmin->vtype = KEV_REAL;
	p_ymin->vtype = KEV_REAL;
	p_xmax->vtype = KEV_REAL;
	p_ymax->vtype = KEV_REAL;

	plgdiplt(
		(PLFLT_NC_SCALAR)(&(p_xmin->r)),
		(PLFLT_NC_SCALAR)(&(p_ymin->r)),
		(PLFLT_NC_SCALAR)(&(p_xmax->r)),
		(PLFLT_NC_SCALAR)(&(p_ymax->r)));
	p_xmin->i = (int64_t)p_xmin->r;
	p_ymin->i = (int64_t)p_ymin->r;
	p_xmax->i = (int64_t)p_xmax->r;
	p_ymax->i = (int64_t)p_ymax->r;
}

SML_EXPORT void SML_CALL ke_function_plgdrawmode(sml_t* sml) { 
	token_t *p;
	p = sml_pop_token(sml);
	int i = (PLINT)plgdrawmode();
	sml_push_int(sml,i);
}

SML_EXPORT void SML_CALL ke_plgfam(sml_t* sml) { 
	token_t *p_fam, *p_num, *p_bmax;
	p_bmax = sml_pop_token(sml);
	p_num = sml_pop_token(sml);
	p_fam = sml_pop_token(sml);

	p_fam->vtype = KEV_INT;
	p_num->vtype = KEV_INT;
	p_bmax->vtype = KEV_INT;

	plgfam(
		(PLINT_NC_SCALAR)(&(p_fam->i)),
		(PLINT_NC_SCALAR)(&(p_num->i)),
		(PLINT_NC_SCALAR)(&(p_bmax->i)));
	p_fam->r = (double)p_fam->i;
	p_num->r = (double)p_num->i;
	p_bmax->r = (double)p_bmax->i;
}

SML_EXPORT void SML_CALL ke_plgfci(sml_t* sml) { 
	token_t *p_fci;
	p_fci = sml_pop_token(sml);

	p_fci->vtype = KEV_INT;

	plgfci((PLUNICODE_NC_SCALAR)(&(p_fci->i)));
	p_fci->r = (double)p_fci->i;
}

SML_EXPORT void SML_CALL ke_plgfnam(sml_t* sml) { 
	token_t *fnam;
	fnam = sml_pop_token(sml);

	fnam->vtype = KEV_STR;
	plgfnam((PLCHAR_NC_VECTOR)(&(fnam->obj.s)));
}

SML_EXPORT void SML_CALL ke_plgfont(sml_t* sml) { 
	token_t *p_family, *p_style, *p_weight;
	p_weight = sml_pop_token(sml);
	p_style = sml_pop_token(sml);
	p_family = sml_pop_token(sml);

	p_family->vtype = KEV_INT;
	p_style->vtype = KEV_INT;
	p_weight->vtype = KEV_INT;

	plgfont(
		(PLINT_NC_SCALAR)(&(p_family->i)),
		(PLINT_NC_SCALAR)(&(p_style->i)),
		(PLINT_NC_SCALAR)(&(p_weight->i)));
	p_family->r = (double)p_family->i;
	p_style->r = (double)p_style->i;
	p_weight->r = (double)p_weight->i;
}

SML_EXPORT void SML_CALL ke_plglevel(sml_t* sml) { 
	token_t *p_level;
	p_level = sml_pop_token(sml);

	p_level->vtype = KEV_INT;

	plglevel((PLINT_NC_SCALAR)(&(p_level->i)));
	p_level->r = (double)p_level->i;
}

SML_EXPORT void SML_CALL ke_plgpage(sml_t* sml) { 
	token_t *p_xp, *p_yp, *p_xleng, *p_yleng, *p_xoff, *p_yoff;
	p_yoff= sml_pop_token(sml);
	p_xoff = sml_pop_token(sml);
	p_yleng = sml_pop_token(sml);
	p_xleng = sml_pop_token(sml);
	p_yp = sml_pop_token(sml);
	p_xp = sml_pop_token(sml);

	p_xp->vtype = KEV_REAL;
	p_yp->vtype = KEV_REAL;
	p_xleng->vtype = KEV_INT;
	p_yleng->vtype = KEV_INT;
	p_xoff->vtype = KEV_INT;
	p_yoff->vtype = KEV_INT;

	plgpage(
		(PLFLT_NC_SCALAR)(&(p_xp->r)),
		(PLFLT_NC_SCALAR)(&(p_yp->r)),
		(PLINT_NC_SCALAR)(&(p_xleng->i)),
		(PLINT_NC_SCALAR)(&(p_yleng->i)),
		(PLINT_NC_SCALAR)(&(p_xoff->i)),
		(PLINT_NC_SCALAR)(&(p_yoff->i)));
	p_xp->i = (int64_t)p_xp->r;
	p_yp->i = (int64_t)p_yp->r;
	p_xleng->r = (double)p_xleng->i;
	p_yleng->r = (double)p_yleng->i;
	p_xoff->r = (double)p_xoff->i;
	p_yoff->r = (double)p_yoff->i;
}

SML_EXPORT void SML_CALL ke_function_plgra(sml_t* sml) { 
	plgra();
}

SML_EXPORT void SML_CALL ke_plgradient(sml_t* sml) { 
	token_t *n, *x, *y, *angle;
	angle = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);
	plgradient((PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT)angle->r);
}

SML_EXPORT void SML_CALL ke_plgriddata(sml_t* sml) {
	token_t *x, *y, *z, *npts, *xg, *nptsx, *yg, *nptsy, *zg, *type, *data;
	data = sml_pop_token(sml);
	type = sml_pop_token(sml);
	zg = sml_pop_token(sml);
	nptsy = sml_pop_token(sml);
	yg = sml_pop_token(sml);
	nptsx = sml_pop_token(sml);
	xg = sml_pop_token(sml);
	npts = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_plgspa(sml_t* sml) { 
	token_t *xmin, *xmax, *ymin, *ymax;
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);

	xmin->vtype = KEV_REAL;
	xmax->vtype = KEV_REAL;
	ymin->vtype = KEV_REAL;
	ymax->vtype = KEV_REAL;

	plgspa(
		(PLFLT_NC_SCALAR)(&(xmin->r)),
		(PLFLT_NC_SCALAR)(&(ymin->r)),
		(PLFLT_NC_SCALAR)(&(xmax->r)),
		(PLFLT_NC_SCALAR)(&(ymax->r)));

	xmin->i = (int64_t)xmin->r;
	ymin->i = (int64_t)ymin->r;
	xmax->i = (int64_t)xmax->r;
	ymax->i = (int64_t)ymax->r;
}

SML_EXPORT void SML_CALL ke_plgstrm(sml_t* sml) { 
	token_t *p_strm;
	p_strm = sml_pop_token(sml);

	p_strm->vtype = KEV_INT;

	plgstrm((PLINT_NC_SCALAR)(&(p_strm->i)));
	p_strm->r = (double)p_strm->i;
}

SML_EXPORT void SML_CALL ke_plgver(sml_t* sml) { 
	token_t *p_ver;
	p_ver = sml_pop_token(sml);

	p_ver->vtype = KEV_STR;

	plgver((PLCHAR_NC_VECTOR)(&(p_ver->obj.s)));
}

SML_EXPORT void SML_CALL ke_plgvpd(sml_t* sml) { 
	token_t *p_xmin, *p_xmax, *p_ymin, *p_ymax;
	p_ymax = sml_pop_token(sml);
	p_ymin = sml_pop_token(sml);
	p_xmax = sml_pop_token(sml);
	p_xmin = sml_pop_token(sml);

	p_xmin->vtype = KEV_REAL;
	p_xmax->vtype = KEV_REAL;
	p_ymin->vtype = KEV_REAL;
	p_ymax->vtype = KEV_REAL;

	plgvpd(
		(PLFLT_NC_SCALAR)(&(p_xmin->r)),
		(PLFLT_NC_SCALAR)(&(p_xmax->r)),
		(PLFLT_NC_SCALAR)(&(p_ymin->r)),
		(PLFLT_NC_SCALAR)(&(p_ymax->r)));
	p_xmin->i = (int64_t)p_xmin->r;
	p_xmax->i = (int64_t)p_xmax->r;
	p_ymin->i = (int64_t)p_ymin->r;
	p_ymax->i = (int64_t)p_ymax->r;
}

SML_EXPORT void SML_CALL ke_plgvpw(sml_t* sml) { 
	token_t *p_xmin, *p_xmax, *p_ymin, *p_ymax;
	p_ymax = sml_pop_token(sml);
	p_ymin = sml_pop_token(sml);
	p_xmax = sml_pop_token(sml);
	p_xmin = sml_pop_token(sml);

	p_xmin->vtype = KEV_REAL;
	p_xmax->vtype = KEV_REAL;
	p_ymin->vtype = KEV_REAL;
	p_ymax->vtype = KEV_REAL;

	plgvpw(
		(PLFLT_NC_SCALAR)(&(p_xmin->r)),
		(PLFLT_NC_SCALAR)(&(p_xmax->r)),
		(PLFLT_NC_SCALAR)(&(p_ymin->r)),
		(PLFLT_NC_SCALAR)(&(p_ymax->r)));
	p_xmin->i = (int64_t)p_xmin->r;
	p_xmax->i = (int64_t)p_xmax->r;
	p_ymin->i = (int64_t)p_ymin->r;
	p_ymax->i = (int64_t)p_ymax->r;
}

SML_EXPORT void SML_CALL ke_plgxax(sml_t* sml) { 
	token_t *p_digmax, *p_digits;
	p_digits = sml_pop_token(sml);
	p_digmax = sml_pop_token(sml);

	p_digits->vtype = KEV_INT;
	p_digits->vtype = KEV_INT;

	plgxax(
		(PLINT_NC_SCALAR)(&(p_digits->i)),
		(PLINT_NC_SCALAR)(&(p_digits->i)));
	p_digits->r = (double)p_digits->i;
	p_digits->r = (double)p_digits->i;
}

SML_EXPORT void SML_CALL ke_plgyax(sml_t* sml) { 
	token_t *p_digmax, *p_digits;
	p_digits = sml_pop_token(sml);
	p_digmax = sml_pop_token(sml);

	p_digits->vtype = KEV_INT;
	p_digits->vtype = KEV_INT;

	plgyax(
		(PLINT_NC_SCALAR)(&(p_digits->i)),
		(PLINT_NC_SCALAR)(&(p_digits->i)));
	p_digits->r = (double)p_digits->i;
	p_digits->r = (double)p_digits->i;
}

SML_EXPORT void SML_CALL ke_plgzax(sml_t* sml) { 
	token_t *p_digmax, *p_digits;
	p_digits = sml_pop_token(sml);
	p_digmax = sml_pop_token(sml);

	p_digits->vtype = KEV_INT;
	p_digits->vtype = KEV_INT;

	plgzax(
		(PLINT_NC_SCALAR)(&(p_digits->i)),
		(PLINT_NC_SCALAR)(&(p_digits->i)));
	p_digits->r = (double)p_digits->i;
	p_digits->r = (double)p_digits->i;
}

SML_EXPORT void SML_CALL ke_plhist(sml_t* sml) { 
	token_t *n, *data, *datmin, *datmax, *nbin, *opt;
	opt = sml_pop_token(sml);
	nbin = sml_pop_token(sml);
	datmax = sml_pop_token(sml);
	datmin = sml_pop_token(sml);
	data = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plhist(
		(PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(data->obj.vector, 0),
		(PLFLT)datmin->r, 
		(PLFLT)datmax->r, 
		(PLINT)nbin->i,
		(PLINT)opt->i);
}

SML_EXPORT void SML_CALL ke_plhlsrgb(sml_t* sml) { 
	token_t *h, *l, *s, *p_r, *p_g, *p_b;
	p_b = sml_pop_token(sml);
	p_g = sml_pop_token(sml);
	p_r = sml_pop_token(sml);
	s = sml_pop_token(sml);
	l = sml_pop_token(sml);
	h = sml_pop_token(sml);

	p_r->vtype = KEV_REAL;
	p_g->vtype = KEV_REAL;
	p_b->vtype = KEV_REAL;

	plhlsrgb(
		(PLFLT)h->r, 
		(PLFLT)l->r, 
		(PLFLT)s->r,
		(PLFLT_NC_SCALAR)(&(p_r->r)),
		(PLFLT_NC_SCALAR)(&(p_g->r)),
		(PLFLT_NC_SCALAR)(&(p_b->r)));
	p_r->i = (int64_t)p_r->r;
	p_g->i = (int64_t)p_g->r;
	p_b->i = (int64_t)p_b->r;
}

SML_EXPORT void SML_CALL ke_plimagefr(sml_t* sml) { 
	token_t *idata, *nx, *ny, *xmin, *xmax, *ymin, *ymax, *zmin, *zmax, *valuemin, *valuemax, *pltr, *pltr_data;
	pltr_data = sml_pop_token(sml);
	pltr = sml_pop_token(sml);
	valuemax = sml_pop_token(sml);
	valuemin = sml_pop_token(sml);
	zmax = sml_pop_token(sml);
	zmin = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	idata = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_plimage(sml_t* sml) { 
	token_t *idata, *nx, *ny, *xmin, *xmax, *ymin, *ymax, *zmin, *zmax,  *Dxmin, *Dxmax, *Dymin, *Dymax;
	Dymax = sml_pop_token(sml);
	Dymin = sml_pop_token(sml);
	Dxmax = sml_pop_token(sml);
	Dxmin = sml_pop_token(sml);
	zmax = sml_pop_token(sml);
	zmin = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	idata = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_pljoin(sml_t* sml) { 
	token_t *x1, *y1, *x2, *y2;
	y2 = sml_pop_token(sml);
	x2 = sml_pop_token(sml);
	y1 = sml_pop_token(sml);
	x1 = sml_pop_token(sml);

	pljoin(
		(PLFLT)x1->r,
		(PLFLT)y1->r,
		(PLFLT)x2->r,
		(PLFLT)y2->r);
}

SML_EXPORT void SML_CALL ke_pllab(sml_t* sml) { 
	token_t *xlabel, *ylabel, *tlabel;
	tlabel = sml_pop_token(sml);
	ylabel = sml_pop_token(sml);
	xlabel = sml_pop_token(sml);

	pllab(
		(PLCHAR_VECTOR)xlabel->obj.s,
		(PLCHAR_VECTOR)ylabel->obj.s,
		(PLCHAR_VECTOR)tlabel->obj.s);
}


SML_EXPORT void SML_CALL ke_pllegend(sml_t* sml) { 
	token_t *p_legend_width, *p_legend_height, *opt, *position, *x, *y, 
		*plot_width, *bg_color, *bb_color, *bb_style, *nrow, *ncolumn,
		*nlegend, *opt_array, *text_offset, *text_scale, *text_spacing, 
		*text_justification, *text_colors, *text, *box_colors, *box_patterns, 
		*box_scales, *box_line_widths, *line_colors, *line_styles, *line_widths, 
		*symbol_colors, *symbol_scales, *symbol_numbers, *symbols;

	symbols = sml_pop_token(sml);
	symbol_numbers = sml_pop_token(sml);
	symbol_scales = sml_pop_token(sml);
	symbol_colors = sml_pop_token(sml);
	line_widths = sml_pop_token(sml);
	line_styles = sml_pop_token(sml);
	line_colors = sml_pop_token(sml);
	box_line_widths = sml_pop_token(sml);
	box_scales = sml_pop_token(sml);
	box_patterns = sml_pop_token(sml);
	box_colors = sml_pop_token(sml);
	text = sml_pop_token(sml);
	text_colors = sml_pop_token(sml);
	text_justification = sml_pop_token(sml);
	text_spacing = sml_pop_token(sml);
	text_scale = sml_pop_token(sml);
	text_offset = sml_pop_token(sml);
	opt_array = sml_pop_token(sml);
	nlegend = sml_pop_token(sml);
	ncolumn = sml_pop_token(sml);
	nrow = sml_pop_token(sml);
	bb_style = sml_pop_token(sml);
	bb_color = sml_pop_token(sml);
	bg_color = sml_pop_token(sml);
	plot_width = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	position = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	p_legend_height = sml_pop_token(sml);
	p_legend_width = sml_pop_token(sml);

	pllegend(
		(PLFLT_NC_SCALAR)(&(p_legend_width->r)),
		(PLFLT_NC_SCALAR)(&(p_legend_height->r)),
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
		(PLINT)text_offset->i, 
		(PLFLT)text_scale->r, 
		(PLFLT)text_spacing->r, 
		(PLFLT)text_justification->r, 
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

	p_legend_width->i = (int64_t)p_legend_width->r;
	p_legend_height->i = (int64_t)p_legend_height->r;
}

SML_EXPORT void SML_CALL ke_pllightsource(sml_t* sml) { 
	token_t *x, *y, *z;
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

	pllightsource(
		(PLFLT)x->r,
		(PLFLT)y->r,
		(PLFLT)z->r);
}


SML_EXPORT void SML_CALL ke_plline3(sml_t* sml) { 
	token_t *n, *x, *y, *z;
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plline3(
		(PLINT)n->r,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0));
}

SML_EXPORT void SML_CALL ke_pllsty(sml_t* sml) {
	token_t *lin;
	lin = sml_pop_token(sml);

	pllsty((PLINT)lin->r);
}

// TODO gin
SML_EXPORT void SML_CALL ke_plmap(sml_t* sml) { 
	token_t *mapform, *name, *minx, *maxx, *miny, *maxy;
	maxy = sml_pop_token(sml);
	miny = sml_pop_token(sml);
	maxx = sml_pop_token(sml);
	minx = sml_pop_token(sml);
	name = sml_pop_token(sml);
	mapform = sml_pop_token(sml);

	plmap(
		(PLMAPFORM_callback)mapform->obj.plmpcb, 
		(PLCHAR_VECTOR)name->obj.s, 
		(PLFLT)minx->r, 
		(PLFLT)maxx->r, 
		(PLFLT)miny->r, 
		(PLFLT)maxy->r);
}

// TODO 
SML_EXPORT void SML_CALL ke_plmapfill(sml_t* sml) { 
	token_t *mapform, *name, *minx, *maxx, *miny, *maxy, *plotentries, *nplotentries;
	nplotentries = sml_pop_token(sml);
	plotentries = sml_pop_token(sml);
	maxy = sml_pop_token(sml);
	miny = sml_pop_token(sml);
	maxx = sml_pop_token(sml);
	minx = sml_pop_token(sml);
	name = sml_pop_token(sml);
	mapform = sml_pop_token(sml);

	plmapfill(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLCHAR_VECTOR)name->obj.s,
		(PLFLT)minx->r,
		(PLFLT)maxx->r,
		(PLFLT)miny->r,
		(PLFLT)maxy->r,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(plotentries->obj.vector_int, 0),
		(PLINT)nplotentries->i);
}

// TODO 
SML_EXPORT void SML_CALL ke_plmapline(sml_t* sml) { 
	token_t *mapform, *name, *minx, *maxx, *miny, *maxy, *plotentries, *nplotentries;
	nplotentries = sml_pop_token(sml);
	plotentries = sml_pop_token(sml);
	maxy = sml_pop_token(sml);
	miny = sml_pop_token(sml);
	maxx = sml_pop_token(sml);
	minx = sml_pop_token(sml);
	name = sml_pop_token(sml);
	mapform = sml_pop_token(sml);

	plmapline(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLCHAR_VECTOR)name->obj.s,
		(PLFLT)minx->r,
		(PLFLT)maxx->r,
		(PLFLT)miny->r,
		(PLFLT)maxy->r,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(plotentries->obj.vector_int, 0),
		(PLINT)nplotentries->i);
}

// TODO 
SML_EXPORT void SML_CALL ke_plmapstring(sml_t* sml) { 
	token_t *mapform, *name, *string, *minx, *maxx, *miny, *maxy, *plotentries, *nplotentries;
	nplotentries = sml_pop_token(sml);
	plotentries = sml_pop_token(sml);
	maxy = sml_pop_token(sml);
	miny = sml_pop_token(sml);
	maxx = sml_pop_token(sml);
	minx = sml_pop_token(sml);
	string = sml_pop_token(sml);
	name = sml_pop_token(sml);
	mapform = sml_pop_token(sml);

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
}

// TODO 
SML_EXPORT void SML_CALL ke_plmaptex(sml_t* sml) { 
	token_t *mapform, *name, *dx, *dy, *just, *text, *minx, *maxx, *miny, *maxy, *plotentry;
	plotentry = sml_pop_token(sml);
	maxy = sml_pop_token(sml);
	miny = sml_pop_token(sml);
	maxx = sml_pop_token(sml);
	minx = sml_pop_token(sml);
	text = sml_pop_token(sml);
	just = sml_pop_token(sml);
	dy = sml_pop_token(sml);
	dx = sml_pop_token(sml);
	name = sml_pop_token(sml);
	mapform = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_plmeridians(sml_t* sml) { 
	token_t *mapform, *dlong, *dlat, *minlong, *maxlong, *minlat, *maxlat;
	maxlat = sml_pop_token(sml);
	minlat = sml_pop_token(sml);
	maxlong = sml_pop_token(sml);
	minlong = sml_pop_token(sml);
	dlat = sml_pop_token(sml);
	dlong = sml_pop_token(sml);
	mapform = sml_pop_token(sml);

	plmeridians(
		(PLMAPFORM_callback)mapform->obj.plmpcb,
		(PLFLT)dlong->r,
		(PLFLT)dlat->r,
		(PLFLT)minlong->r,
		(PLFLT)maxlong->r,
		(PLFLT)minlat->r,
		(PLFLT)maxlat->r);
}

SML_EXPORT void SML_CALL ke_plmesh(sml_t* sml) { 
	token_t *x, *y, *z, *nx, *ny, *opt;
	opt = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

	plmesh(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i);
}

SML_EXPORT void SML_CALL ke_plmeshc(sml_t* sml) { 
	token_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel;
	nlevel = sml_pop_token(sml);
	clevel = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

	plmeshc(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i);
}

SML_EXPORT void SML_CALL ke_plmkstrm(sml_t* sml) { 
	token_t *p_strm;
	p_strm = sml_pop_token(sml);
	p_strm->vtype = KEV_INT;
	plmkstrm((PLINT_NC_SCALAR)(&(p_strm->i)));
	p_strm->r = (double)p_strm->i;
}

SML_EXPORT void SML_CALL ke_plmtex(sml_t* sml) { 
	token_t *side, *disp, *pos, *just, *text;
	text = sml_pop_token(sml);
	just = sml_pop_token(sml);
	pos = sml_pop_token(sml);
	disp = sml_pop_token(sml);
	side = sml_pop_token(sml);

	plmtex(
		(PLCHAR_VECTOR)side->obj.s, 
		(PLFLT)disp->r, 
		(PLFLT)pos->r, 
		(PLFLT)just->r, 
		(PLCHAR_VECTOR)text->obj.s);
}

SML_EXPORT void SML_CALL ke_plmtex3(sml_t* sml) { 
	token_t *side, *disp, *pos, *just, *text;
	text = sml_pop_token(sml);
	just = sml_pop_token(sml);
	pos = sml_pop_token(sml);
	disp = sml_pop_token(sml);
	side = sml_pop_token(sml);

	plmtex3(
		(PLCHAR_VECTOR)side->obj.s,
		(PLFLT)disp->r,
		(PLFLT)pos->r,
		(PLFLT)just->r,
		(PLCHAR_VECTOR)text->obj.s);
}

SML_EXPORT void SML_CALL ke_plot3d(sml_t* sml) { 
	token_t *x, *y, *z, *nx, *ny, *opt, *side;
	side = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

	plot3d(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i, 
		(PLINT)ny->i, 
		(PLINT)opt->i, 
		(PLBOOL)side->i);
}

SML_EXPORT void SML_CALL ke_plot3dc(sml_t* sml) { 
	token_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel;
	nlevel = sml_pop_token(sml);
	clevel = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

	plot3dc(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0),
		(PLINT)nx->i,
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i);
}

SML_EXPORT void SML_CALL ke_plot3dl(sml_t* sml) {
	token_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel, *indexxmin, *indexxmax, *indexymin, *indexymax;

	indexymax = sml_pop_token(sml);
	indexymin = sml_pop_token(sml);
	indexxmax = sml_pop_token(sml);
	indexxmin = sml_pop_token(sml);

	nlevel = sml_pop_token(sml);
	clevel = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

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
}


SML_EXPORT void SML_CALL ke_plparseopts(sml_t* sml) { 
	token_t *p_argc, *argv, *mode;
	mode = sml_pop_token(sml);
	argv = sml_pop_token(sml);
	p_argc = sml_pop_token(sml);
	PLINT st = plparseopts(
		(int *)(&(p_argc->i)),
		(PLCHAR_NC_MATRIX)argv->obj.s,
		(PLINT)mode->i);
	int i = st;
	sml_push_int(sml, i);
	p_argc->r = (double)p_argc->i;
}

SML_EXPORT void SML_CALL ke_plpat(sml_t* sml) { 
	token_t *nlin, *inc, *del;
	del = sml_pop_token(sml);
	inc = sml_pop_token(sml);
	nlin = sml_pop_token(sml);

	plpat((PLINT)nlin->i,
		(PLINT_VECTOR)gsl_vector_int_const_ptr(inc->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(del->obj.vector_int, 0));
}

SML_EXPORT void SML_CALL ke_plpath(sml_t* sml) { 
	token_t *n, *x1, *y1, *x2, *y2;
	y2 = sml_pop_token(sml);
	x2 = sml_pop_token(sml);
	y1 = sml_pop_token(sml);
	x1 = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plpath(
		(PLINT)n->i, 
		(PLFLT)x1->r, 
		(PLFLT)y1->r,
		(PLFLT)x2->r,
		(PLFLT)y2->r);
}

SML_EXPORT void SML_CALL ke_plpoin(sml_t* sml) {
	token_t *n, *x, *y, *code;
	code = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plpoin(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLINT)code->i);
}

SML_EXPORT void SML_CALL ke_plpoin3(sml_t* sml) { 
	token_t *n, *x, *y, *z, *code;
	code = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plpoin3(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0),
		(PLINT)code->i);
}

SML_EXPORT void SML_CALL ke_plpoly3(sml_t* sml) {
	token_t *n, *x, *y, *z, *draw, *ifcc;
	ifcc = sml_pop_token(sml);
	draw = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plpoly3(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0),
		(PLBOOL_VECTOR)gsl_vector_int_const_ptr(draw->obj.vector_int, 0),
		(PLBOOL)ifcc->i);
}

SML_EXPORT void SML_CALL ke_plprec(sml_t* sml) { 
	token_t *setp, *prec;
	prec = sml_pop_token(sml);
	setp = sml_pop_token(sml);

	plprec(
		(PLINT)setp->i,
		(PLINT)prec->i);
}

SML_EXPORT void SML_CALL ke_plpsty(sml_t* sml) { 
	token_t *patt;
	patt = sml_pop_token(sml);

	plpsty((PLINT)patt->i);
}

SML_EXPORT void SML_CALL ke_plptex(sml_t* sml) { 
	token_t *x, *y, *dx, *dy, *just, *text;
	text = sml_pop_token(sml);
	just = sml_pop_token(sml);
	dy = sml_pop_token(sml);
	dx = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

	plptex(
		(PLFLT)x->r,
		(PLFLT)y->r,
		(PLFLT)dx->r,
		(PLFLT)dy->r,
		(PLFLT)just->r,
		(PLCHAR_VECTOR)text->obj.s);
}

SML_EXPORT void SML_CALL ke_plptex3(sml_t* sml) { 
	token_t *wx, *wy, *wz, *dx, *dy, *dz, *sx, *sy, *sz, *just, *text;
	text = sml_pop_token(sml);
	just = sml_pop_token(sml);
	sz = sml_pop_token(sml);
	sy = sml_pop_token(sml);
	sx = sml_pop_token(sml);
	dz = sml_pop_token(sml);
	dy = sml_pop_token(sml);
	dx = sml_pop_token(sml);
	wz = sml_pop_token(sml);
	wy = sml_pop_token(sml);
	wx = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_plrandd(sml_t* sml) { 
	token_t *p;
	p = sml_pop_token(sml);
	double r = plrandd();
	sml_push_real(sml, r);
}

SML_EXPORT void SML_CALL ke_plreplot(sml_t* sml) { 
	plreplot();
}

SML_EXPORT void SML_CALL ke_plrgbhls(sml_t* sml) { 
	token_t *r, *g, *b, *p_h, *p_l, *p_s;
	p_s = sml_pop_token(sml);
	p_l = sml_pop_token(sml);
	p_h = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	p_h->vtype = KEV_REAL;
	p_l->vtype = KEV_REAL;
	p_s->vtype = KEV_REAL;

	plrgbhls(
		(PLFLT)r->r,
		(PLFLT)g->r,
		(PLFLT)b->r,
		(PLFLT_NC_SCALAR)(&(p_h->r)),
		(PLFLT_NC_SCALAR)(&(p_l->r)),
		(PLFLT_NC_SCALAR)(&(p_s->r)));

	p_h->i = (int64_t)p_h->r;
	p_l->i = (int64_t)p_l->r;
	p_s->i = (int64_t)p_s->r;
}

SML_EXPORT void SML_CALL ke_plschr(sml_t* sml) {
	token_t *def, *scale;
	scale = sml_pop_token(sml);
	def = sml_pop_token(sml);

	plschr( (PLFLT)def->r, (PLFLT)scale->r);
}

SML_EXPORT void SML_CALL ke_plscmap0(sml_t* sml) { 
	token_t *r, *g, *b, *ncol0;
	ncol0 = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	plscmap0(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLINT)ncol0->i);
}

SML_EXPORT void SML_CALL ke_plscmap0a(sml_t* sml) {
	token_t *r, *g, *b, *alpha, *ncol0;
	ncol0 = sml_pop_token(sml);
	alpha = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	plscmap0a(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLFLT_VECTOR)gsl_vector_int_const_ptr(alpha->obj.vector_int, 0),
		(PLINT)ncol0->i);
}

SML_EXPORT void SML_CALL ke_plscmap0n(sml_t* sml) { 
	token_t *ncol0;
	ncol0 = sml_pop_token(sml);

	plscmap0n((PLINT)ncol0->i);
}

SML_EXPORT void SML_CALL ke_plscmap1_range(sml_t* sml) { 
	token_t *min_color, *max_color;
	max_color = sml_pop_token(sml);
	min_color = sml_pop_token(sml);

	plscmap1_range( (PLFLT)min_color->r, (PLFLT)max_color->r);
}

SML_EXPORT void SML_CALL ke_plscmap1(sml_t* sml) { 
	token_t *r, *g, *b, *ncol1;
	ncol1 = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	plscmap1(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLINT)ncol1->i);
}

SML_EXPORT void SML_CALL ke_plscmap1a(sml_t* sml) { 
	token_t *r, *g, *b, *alpha, *ncol1;
	ncol1 = sml_pop_token(sml);
	alpha = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	plscmap1a(
		(PLINT_VECTOR)gsl_vector_int_const_ptr(r->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(g->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(b->obj.vector_int, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(alpha->obj.vector, 0),
		(PLINT)ncol1->i);
}

SML_EXPORT void SML_CALL ke_plscmap1l(sml_t* sml) { 
	token_t *itype, *npts, *intensity, *coord1, *coord2, *coord3, *alt_hue_path;
	alt_hue_path = sml_pop_token(sml);
	coord3 = sml_pop_token(sml);
	coord2 = sml_pop_token(sml);
	coord1 = sml_pop_token(sml);
	intensity = sml_pop_token(sml);
	npts = sml_pop_token(sml);
	itype = sml_pop_token(sml);

	plscmap1l( 
		(PLBOOL)itype->i, 
		(PLINT)npts->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(intensity->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord1->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord2->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord3->obj.vector, 0),
		(PLBOOL_VECTOR)gsl_vector_int_const_ptr(alt_hue_path->obj.vector_int, 0));
}

SML_EXPORT void SML_CALL ke_plscmap1la(sml_t* sml) { 
	token_t *itype, *npts, *intensity, *coord1, *coord2, *coord3, *alpha, *alt_hue_path;
	alt_hue_path = sml_pop_token(sml);
	alpha = sml_pop_token(sml);
	coord3 = sml_pop_token(sml);
	coord2 = sml_pop_token(sml);
	coord1 = sml_pop_token(sml);
	intensity = sml_pop_token(sml);
	npts = sml_pop_token(sml);
	itype = sml_pop_token(sml);

	plscmap1la(
		(PLBOOL)itype->i,
		(PLINT)npts->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(intensity->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord1->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord2->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(coord3->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(alpha->obj.vector, 0),
		(PLBOOL_VECTOR)gsl_vector_int_const_ptr(alt_hue_path->obj.vector_int, 0));
}

SML_EXPORT void SML_CALL ke_plscmap1n(sml_t* sml) { 
	token_t *ncol1;
	ncol1 = sml_pop_token(sml);

	plscmap1n((PLINT)ncol1->i);
}

SML_EXPORT void SML_CALL ke_plscol0(sml_t* sml) {
	token_t *icol0, *r, *g, *b;
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);
	icol0 = sml_pop_token(sml);

	plscol0(
		(PLINT)icol0->i, 
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i);
}

SML_EXPORT void SML_CALL ke_plscol0a(sml_t* sml) { 
	token_t *icol0, *r, *g, *b, *alpha;
	alpha = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);
	icol0 = sml_pop_token(sml);

	plscol0a(
		(PLINT)icol0->i,
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i,
		(PLFLT)alpha->r);
}

SML_EXPORT void SML_CALL ke_plscolbg(sml_t* sml) { 
	token_t *r, *g, *b;
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	plscolbg(
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i);
}

SML_EXPORT void SML_CALL ke_plscolbga(sml_t* sml) { 
	token_t *r, *g, *b, *alpha;
	alpha = sml_pop_token(sml);
	b = sml_pop_token(sml);
	g = sml_pop_token(sml);
	r = sml_pop_token(sml);

	plscolbga(
		(PLINT)r->i,
		(PLINT)g->i,
		(PLINT)b->i,
		(PLFLT)alpha->r);
}

SML_EXPORT void SML_CALL ke_plscolor(sml_t* sml) { 
	token_t *color;
	color = sml_pop_token(sml);

	plscolor((PLINT)color->i);
}

SML_EXPORT void SML_CALL ke_plscompression(sml_t* sml) { 
	token_t *compression;
	compression = sml_pop_token(sml);

	plscompression((PLINT)compression->i);
}

SML_EXPORT void SML_CALL ke_plsdidev(sml_t* sml) { 
	token_t *mar, *aspect, *jx, *jy;
	jy = sml_pop_token(sml);
	jx = sml_pop_token(sml);
	aspect = sml_pop_token(sml);
	mar = sml_pop_token(sml);

	plsdidev(
		(PLFLT)mar->r, 
		(PLFLT)aspect->r,
		(PLFLT)jx->r,
		(PLFLT)jy->r);
}

SML_EXPORT void SML_CALL ke_plsdimap(sml_t* sml) {
	token_t *dimxmin, *dimxmax, *dimymin, *dimymax, *dimxpmm, *dimypmm;
	dimypmm = sml_pop_token(sml);
	dimxpmm = sml_pop_token(sml);
	dimymax = sml_pop_token(sml);
	dimymin = sml_pop_token(sml);
	dimxmax = sml_pop_token(sml);
	dimxmin = sml_pop_token(sml);

	plsdimap(
		(PLINT)dimxmin->i, 
		(PLINT)dimxmax->i,
		(PLINT)dimymin->i,
		(PLINT)dimymax->i,
		(PLFLT)dimxpmm->r,
		(PLFLT)dimypmm->r);
}

SML_EXPORT void SML_CALL ke_plsdiori(sml_t* sml) {
	token_t *rot;
	rot = sml_pop_token(sml);

	plsdiori((PLFLT)rot->r);
}

SML_EXPORT void SML_CALL ke_plsdiplt(sml_t* sml) { 
	token_t *xmin, *ymin, *xmax, *ymax;
	ymax = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmin = sml_pop_token(sml);

	plsdiplt(
		(PLFLT)xmin->r,
		(PLFLT)ymin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymax->r);
}

SML_EXPORT void SML_CALL ke_plsdiplz(sml_t* sml) { 
	token_t *xmin, *ymin, *xmax, *ymax;
	ymax = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmin = sml_pop_token(sml);

	plsdiplz(
		(PLFLT)xmin->r,
		(PLFLT)ymin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymax->r);
}

SML_EXPORT void SML_CALL ke_plsdrawmode(sml_t* sml) {
	token_t *mode;
	mode = sml_pop_token(sml);

	plsdrawmode(
		(PLINT)mode->i);
}

SML_EXPORT void SML_CALL ke_plseed(sml_t* sml) {
	token_t *seed;
	seed = sml_pop_token(sml);

	plseed(
		(unsigned int)seed->i);
}

SML_EXPORT void SML_CALL ke_plsesc(sml_t* sml) { 
	token_t *esc;
	esc = sml_pop_token(sml);

	plsesc(
		(char)esc->i);
}

SML_EXPORT void SML_CALL ke_plsetopt(sml_t* sml) { 
	token_t *opt, *optarg;
	optarg = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	PLINT st = plsetopt( (PLCHAR_VECTOR)opt->obj.s, (PLCHAR_VECTOR)optarg->obj.s);
	int i = st;
	sml_push_int(sml,i)
}

SML_EXPORT void SML_CALL ke_plsfam(sml_t* sml) { 
	token_t *fam, *num, *bmax;
	bmax = sml_pop_token(sml);
	num = sml_pop_token(sml);
	fam = sml_pop_token(sml);

	plsfam(
		(PLINT)fam->i, 
		(PLINT)num->i,
		(PLINT)bmax->i);
}

SML_EXPORT void SML_CALL ke_plsfci(sml_t* sml) { 
	token_t *fci;
	fci = sml_pop_token(sml);

	plsfci(	(PLUNICODE)fci->i);
}

SML_EXPORT void SML_CALL ke_plsfnam(sml_t* sml) { 
	token_t *fnam;
	fnam = sml_pop_token(sml);

	plsfnam(
		(PLCHAR_VECTOR)fnam->obj.s);
}

SML_EXPORT void SML_CALL ke_plsfont(sml_t* sml) { 
	token_t *family, *style, *weight;
	weight = sml_pop_token(sml);
	style = sml_pop_token(sml);
	family = sml_pop_token(sml);

	plsfont(
		(PLINT)family->i, 
		(PLINT)style->i,
		(PLINT)weight->i);
}


SML_EXPORT void SML_CALL ke_plshades(sml_t* sml) {
	
	token_t *a, *nx, *ny, *defined, *xmin, *xmax, *ymin, *ymax, *clevel, *nlevel,
		*fill_width, *cont_color, *cont_width, *fill, *rectangular, *pltr, *pltr_data;
	pltr_data = sml_pop_token(sml);
	pltr = sml_pop_token(sml);
	rectangular = sml_pop_token(sml);
	fill = sml_pop_token(sml);
	cont_width = sml_pop_token(sml);
	cont_color = sml_pop_token(sml);
	fill_width = sml_pop_token(sml);
	nlevel = sml_pop_token(sml);
	clevel = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	defined = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	a = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_plshade(sml_t* sml) {
	token_t *a, *nx, *ny, *defined, *xmin, *xmax, *ymin, *ymax, *shade_min, *shade_max, *sh_cmap, *sh_color, *sh_width,
		*min_color, *min_width, *max_color, *max_width, *fill, *rectangular, *pltr, *pltr_data;
	pltr_data = sml_pop_token(sml);
	pltr = sml_pop_token(sml);
	rectangular = sml_pop_token(sml);
	fill = sml_pop_token(sml);
	max_width = sml_pop_token(sml);
	max_color = sml_pop_token(sml);
	min_width = sml_pop_token(sml);
	min_color = sml_pop_token(sml);
	sh_width = sml_pop_token(sml);
	sh_color = sml_pop_token(sml);
	sh_cmap = sml_pop_token(sml);
	shade_max = sml_pop_token(sml);
	shade_min = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	defined = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	a = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_plslabelfunc(sml_t* sml) { 
	token_t *label_func, *label_data;
	label_data = sml_pop_token(sml);
	label_func = sml_pop_token(sml);

	plslabelfunc(
		(PLLABEL_FUNC_callback)label_func->obj.pllblcb, 
		(PLPointer)label_data->obj.plptr);
}

SML_EXPORT void SML_CALL ke_plsmaj(sml_t* sml) {
	token_t *def, *scale;
	scale = sml_pop_token(sml);
	def = sml_pop_token(sml);

	plsmaj( (PLFLT)def->r, (PLFLT)scale->r);
}

SML_EXPORT void SML_CALL ke_plsmem(sml_t* sml) { 
	token_t *maxx, *maxy, *plotmem;
	plotmem = sml_pop_token(sml);
	maxy = sml_pop_token(sml);
	maxx = sml_pop_token(sml);

	plsmem( (PLINT)maxx->i, (PLINT)maxy->i, (PLPointer)plotmem->obj.plptr);
}

SML_EXPORT void SML_CALL ke_plsmema(sml_t* sml) {
	token_t *maxx, *maxy, *plotmem;
	plotmem = sml_pop_token(sml);
	maxy = sml_pop_token(sml);
	maxx = sml_pop_token(sml);

	plsmema((PLINT)maxx->i, (PLINT)maxy->i, (PLPointer)plotmem->obj.plptr);
}

SML_EXPORT void SML_CALL ke_plsmin(sml_t* sml) {
	token_t *def, *scale;
	scale = sml_pop_token(sml);
	def = sml_pop_token(sml);

	plsmin( (PLFLT)def->r, (PLFLT)scale->r);
}

SML_EXPORT void SML_CALL ke_plsori(sml_t* sml) {
	token_t *ori;
	ori = sml_pop_token(sml);

	plsori((PLINT)ori->i);
}

SML_EXPORT void SML_CALL ke_plspage(sml_t* sml) {
	token_t *xp, *yp, *xleng, *yleng, *xoff, *yoff;
	yoff = sml_pop_token(sml);
	xoff = sml_pop_token(sml);
	yleng = sml_pop_token(sml);
	xleng = sml_pop_token(sml);
	yp = sml_pop_token(sml);
	xp = sml_pop_token(sml);

	plspage(
		(PLFLT)xp->r, 
		(PLFLT)yp->r,
		(PLINT)xleng->i, 
		(PLINT)yleng->i,
		(PLINT)xoff->i,
		(PLINT)yoff->i);
}

SML_EXPORT void SML_CALL ke_plspal0(sml_t* sml) { 
	token_t *filename;
	filename = sml_pop_token(sml);

	plspal0((PLCHAR_VECTOR)filename->obj.s);
}

SML_EXPORT void SML_CALL ke_plspal1(sml_t* sml) { 
	token_t *filename, *interpolate;
	interpolate = sml_pop_token(sml);
	filename = sml_pop_token(sml);

	plspal1((PLCHAR_VECTOR)filename->obj.s, (PLBOOL)interpolate->i);
}

SML_EXPORT void SML_CALL ke_plsstrm(sml_t* sml) { 
	token_t *strm;
	strm = sml_pop_token(sml);

	plsstrm((PLINT)strm->i);
}

SML_EXPORT void SML_CALL ke_plssub(sml_t* sml) { 
	token_t *nx, *ny;
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);

	plssub((PLINT)nx->i, (PLINT)ny->i);
}

SML_EXPORT void SML_CALL ke_plssym(sml_t* sml) { 
	token_t *def, *scale;
	scale = sml_pop_token(sml);
	def = sml_pop_token(sml);

	plssym((PLFLT)def->r, (PLFLT)scale->r);
}

SML_EXPORT void SML_CALL ke_plstar(sml_t* sml) { 
	token_t *nx, *ny;
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);

	plstar((PLINT)nx->i, (PLINT)ny->i);
}

SML_EXPORT void SML_CALL ke_plstart(sml_t* sml) { 
	token_t *devname, *nx, *ny;
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	devname = sml_pop_token(sml);

	plstart(
		(PLCHAR_VECTOR)devname->obj.s,
		(PLINT)nx->i, 
		(PLINT)ny->i);
}

SML_EXPORT void SML_CALL ke_plstransform(sml_t* sml) {
	token_t *coordinate_transform, *coordinate_transform_data;
	coordinate_transform_data = sml_pop_token(sml);
	coordinate_transform = sml_pop_token(sml);

	plstransform(
		(PLTRANSFORM_callback)coordinate_transform->obj.pltrcb,
		(PLPointer)coordinate_transform_data->obj.plptr);
}

SML_EXPORT void SML_CALL ke_plstring(sml_t* sml) { 
	token_t *n, *x, *y, *string;
	string = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plstring(
		(PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLCHAR_VECTOR)string->obj.s);
}

SML_EXPORT void SML_CALL ke_plstring3(sml_t* sml) { 
	token_t *n, *x, *y, *z, *string;
	string = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plstring3(
		(PLINT)n->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(z->obj.vector, 0),
		(PLCHAR_VECTOR)string->obj.s);
}

SML_EXPORT void SML_CALL ke_plstripa(sml_t* sml) {
	token_t *id, *pen, *x, *y;
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	pen = sml_pop_token(sml);
	id = sml_pop_token(sml);

	plstripa(
		(PLINT)id->i,
		(PLINT)pen->i,
		(PLFLT)x->r,
		(PLFLT)y->r);
}

SML_EXPORT void SML_CALL ke_plstripc(sml_t* sml) { 
	token_t *id, *xspec, *yspec, *xmin, *xmax, *xjump, *ymin, *ymax, *xlpos, *ylpos, 
		*y_ascl, *acc, *colbox, *collab, *colline, *styline, *legline, *labx, *laby, *labtop;
	labtop = sml_pop_token(sml);
	laby = sml_pop_token(sml);
	labx = sml_pop_token(sml);
	legline = sml_pop_token(sml);
	styline = sml_pop_token(sml);
	colline = sml_pop_token(sml);
	collab = sml_pop_token(sml);
	colbox = sml_pop_token(sml);
	acc = sml_pop_token(sml);
	y_ascl = sml_pop_token(sml);
	ylpos = sml_pop_token(sml);
	xlpos = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xjump = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	yspec = sml_pop_token(sml);
	xspec = sml_pop_token(sml);
	id = sml_pop_token(sml);

	id->vtype = KEV_INT;
	plstripc(
		(PLINT_NC_SCALAR)(&(id->i)),
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

	id->r = (double)id->i;
}

SML_EXPORT void SML_CALL ke_plstripd(sml_t* sml) { 
	token_t *id;
	id = sml_pop_token(sml);

	plstripd((PLINT)id->i);
}

SML_EXPORT void SML_CALL ke_plstyl(sml_t* sml) { 
	token_t *nms, *mark, *space;
	space = sml_pop_token(sml);
	mark = sml_pop_token(sml);
	nms = sml_pop_token(sml);

	plstyl(
		(PLINT)nms->i, 
		(PLINT_VECTOR)gsl_vector_int_const_ptr(mark->obj.vector_int, 0),
		(PLINT_VECTOR)gsl_vector_int_const_ptr(space->obj.vector_int, 0));
}

SML_EXPORT void SML_CALL ke_plsurf3d(sml_t* sml) { 
	token_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel;
	nlevel = sml_pop_token(sml);
	clevel = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

	plsurf3d(
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), 
		(PLFLT_MATRIX)gsl_matrix_const_ptr(z->obj.matrix, 0, 0), 
		(PLINT)nx->i, 
		(PLINT)ny->i,
		(PLINT)opt->i,
		(PLFLT_VECTOR)gsl_vector_const_ptr(clevel->obj.vector, 0),
		(PLINT)nlevel->i);
}

SML_EXPORT void SML_CALL ke_plsurf3dl(sml_t* sml) { 

	token_t *x, *y, *z, *nx, *ny, *opt, *clevel, *nlevel, *indexxmin, *indexxmax, *indexymin, *indexymax;
	indexymax = sml_pop_token(sml);
	indexymin = sml_pop_token(sml);
	indexxmax = sml_pop_token(sml);
	indexxmin = sml_pop_token(sml);
	nlevel = sml_pop_token(sml);
	clevel = sml_pop_token(sml);
	opt = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	z = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);

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
}

SML_EXPORT void SML_CALL ke_plsvect(sml_t* sml) { 
	token_t *arrowx, *arrowy, *npts, *fill;
	fill = sml_pop_token(sml);
	npts = sml_pop_token(sml);
	arrowy = sml_pop_token(sml);
	arrowx = sml_pop_token(sml);

	plsvect(
		(PLFLT_VECTOR)gsl_vector_const_ptr(arrowx->obj.vector, 0),
		(PLFLT_VECTOR)gsl_vector_const_ptr(arrowy->obj.vector, 0),
		(PLINT)npts->i, 
		(PLBOOL)fill->i);
}

SML_EXPORT void SML_CALL ke_plsvpa(sml_t* sml) { 
	token_t *xmin, *xmax, *ymin, *ymax;
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);

	plsvpa(
		(PLFLT)xmin->r, 
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r);
}

SML_EXPORT void SML_CALL ke_plsxax(sml_t* sml) { 
	token_t *digmax, *digits;
	digits = sml_pop_token(sml);
	digmax = sml_pop_token(sml);

	plsxax((PLINT)digmax->i, (PLINT)digits->i);
}

SML_EXPORT void SML_CALL ke_plsyax(sml_t* sml) { 
	token_t *digmax, *digits;
	digits = sml_pop_token(sml);
	digmax = sml_pop_token(sml);

	plsyax((PLINT)digmax->i, (PLINT)digits->i);
}

SML_EXPORT void SML_CALL ke_plsym(sml_t* sml) { 
	token_t *n, *x, *y, *code;
	code = sml_pop_token(sml);
	y = sml_pop_token(sml);
	x = sml_pop_token(sml);
	n = sml_pop_token(sml);

	plsym(
		(PLINT)n->i, 
		(PLFLT_VECTOR)gsl_vector_const_ptr(x->obj.vector, 0), 
		(PLFLT_VECTOR)gsl_vector_const_ptr(y->obj.vector, 0), 
		(PLINT)code->i);
}

SML_EXPORT void SML_CALL ke_plszax(sml_t* sml) { 
	token_t *digmax, *digits;
	digits = sml_pop_token(sml);
	digmax = sml_pop_token(sml);

	plszax((PLINT)digmax->i, (PLINT)digits->i);
}

SML_EXPORT void SML_CALL ke_pltext(sml_t* sml) { 
	pltext();
}

SML_EXPORT void SML_CALL ke_pltimefmt(sml_t* sml) { 
	token_t *fmt;
	fmt = sml_pop_token(sml);

	pltimefmt((PLCHAR_VECTOR)fmt->obj.s);
}

SML_EXPORT void SML_CALL ke_plvasp(sml_t* sml) { 
	token_t *aspect;
	aspect = sml_pop_token(sml);

	plvasp((PLFLT)aspect->r);
}

SML_EXPORT void SML_CALL ke_plvect(sml_t* sml) { 
	token_t *u, *v, *nx, *ny, *scale, *pltr, *pltr_data;
	pltr_data = sml_pop_token(sml);
	pltr = sml_pop_token(sml);
	scale = sml_pop_token(sml);
	ny = sml_pop_token(sml);
	nx = sml_pop_token(sml);
	v = sml_pop_token(sml);
	u = sml_pop_token(sml);

	plvect(
		(PLFLT_MATRIX)gsl_matrix_const_ptr(u->obj.matrix, 0,0), 
		(PLFLT_MATRIX)gsl_matrix_const_ptr(v->obj.matrix, 0, 0), 
		(PLINT)nx->i, 
		(PLINT)ny->i, 
		(PLFLT)scale->r,
		(PLTRANSFORM_callback)pltr->obj.pltrcb, 
		(PLPointer)pltr_data->obj.plptr);
}

SML_EXPORT void SML_CALL ke_plvpas(sml_t* sml) { 
	token_t *xmin, *xmax, *ymin, *ymax, *aspect;
	aspect = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);

	plvpas(
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r,
		(PLFLT)aspect->r);
}

SML_EXPORT void SML_CALL ke_plvpor(sml_t* sml) { 
	token_t *xmin, *xmax, *ymin, *ymax;
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);

	plvpor(
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r);
}

SML_EXPORT void SML_CALL ke_plvsta(sml_t* sml) { 
	plvsta();
}

SML_EXPORT void SML_CALL ke_plw3d(sml_t* sml) { 
	token_t *basex,* basey, *height, *xmin, *xmax, *ymin, *ymax, *zmin, *zmax, *alt, *az;
	az = sml_pop_token(sml);
	alt = sml_pop_token(sml);
	zmax = sml_pop_token(sml);
	zmin = sml_pop_token(sml);
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);
	height = sml_pop_token(sml);
	basey = sml_pop_token(sml);
	basex = sml_pop_token(sml);

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
}


SML_EXPORT void SML_CALL ke_plwidth(sml_t* sml) {
	token_t *width;
	width = sml_pop_token(sml);

	plwidth(
		(PLFLT)width->r);
}

SML_EXPORT void SML_CALL ke_plwind(sml_t* sml) {
	token_t *xmin, *xmax, *ymin, *ymax;
	ymax = sml_pop_token(sml);
	ymin = sml_pop_token(sml);
	xmax = sml_pop_token(sml);
	xmin = sml_pop_token(sml);

	plwind(
		(PLFLT)xmin->r,
		(PLFLT)xmax->r,
		(PLFLT)ymin->r,
		(PLFLT)ymax->r);
}

SML_EXPORT void SML_CALL ke_plxormod(sml_t* sml) {
	token_t *mode, *status;
	status = sml_pop_token(sml);
	mode = sml_pop_token(sml);
	status->vtype = KEV_INT;
	plxormod((PLBOOL)mode->i,
		(PLBOOL_NC_SCALAR)(&(status->i)));
	status->r = (double)status->i;
}

SML_EXPORT void SML_CALL ke_plabort(sml_t* sml) {
	token_t *message;
	message = sml_pop_token(sml);

	plabort((PLCHAR_VECTOR)message->obj.s);
}

SML_EXPORT void SML_CALL ke_plconfigtime(sml_t* sml) {
	token_t *lscale;	token_t *offset1;
	token_t *offset2;
	token_t *ccontrol;

	token_t *ifbtime_offset, *year, *month, *day, *hour, *min, *sec;
	sec = sml_pop_token(sml);
	min = sml_pop_token(sml);
	hour = sml_pop_token(sml);
	day = sml_pop_token(sml);
	month = sml_pop_token(sml);
	year = sml_pop_token(sml);
	ifbtime_offset = sml_pop_token(sml);
	ccontrol = sml_pop_token(sml);
	offset2 = sml_pop_token(sml);
	offset1 = sml_pop_token(sml);
	lscale = sml_pop_token(sml);

	plconfigtime((PLFLT)lscale->r, (PLFLT)offset1->r, (PLFLT)offset2->r, (PLINT)ccontrol->i,
		(PLINT)ifbtime_offset->i, (PLINT)year->i, (PLINT)month->i, (PLINT)day->i,
		(PLINT)hour->i, (PLINT)min->i, (PLFLT)sec->r);
}

SML_EXPORT void SML_CALL dllke_plot_hash(sml_t* sml) {

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

