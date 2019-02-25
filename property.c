#include "kexpr.h"
#include "vector.h"
#include "matrix.h"
#include "str.h"
#include "api.c"

static void ke_poperty_str_get(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 0) {
		return ke_str_prop_get_0par(sml);
	}
	else if (narg == 1) {
		return ke_str_prop_get_1par(sml);
	}
	else if (narg == 2) {
		return ke_str_prop_get_2par(sml);
	} else {
		printf("ke_poperty_str_get :Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

static void ke_poperty_matrix_get(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 2) {
		ke_matrix_prop_get(sml);
	}
	else {
		printf("ke_poperty_matrix_get : Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

static void ke_poperty_vector_get(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 1) {
		ke_vector_get(sml);
	}
	else {
		printf("ke_poperty_vector_get : Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

static int ke_poperty_matrix_set(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 2) {
		ke_matrix_prop_set(sml);
	}
	else {
		printf("ke_poperty_matrix_set : Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

static int ke_poperty_str_set(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 1) {
		return ke_str_prop_set_1par(sml);
	}
	else if (narg == 2) {
		return ke_str_prop_set_2par(sml);
	}
	else {
		printf("ke_poperty_str_set :Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

static void ke_poperty_vector_set(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 1) {
		ke_vector_prop_set(sml);
	}
	else {
		printf("ke_poperty_vector_set : Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

static void ke_poperty_vector_int_get(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 1) {
		ke_vector_int_get(sml);
	}
	else {
		printf("ke_poperty_vector_int_get : Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

static void ke_poperty_vector_int_set(sml_t* sml) {
	int narg = sml_get_args(sml);
	if (narg == 1) {
		ke_vector_prop_int_set(sml);
	}
	else {
		printf("ke_poperty_vector_int_set : Property not implemented for %d\n", sml_get_vtype(sml));
	}
}

void ke_poperty_get(sml_t* sml) {
	// get the real name
	int top = sml_get_top(sml);
	token_t * tokp = sml_peek_token(sml, (top - 1));  //  the real field to manage
	switch (sml_get_type(tokp)) {
	case KEV_MAT:
		return ke_poperty_matrix_get(sml);
		break;
	case KEV_VEC:
		return ke_poperty_vector_get(sml);
		break;
	case KEV_VEC_INT:
		return ke_poperty_vector_int_get(sml);
		break;
	case KEV_STR:
		return ke_poperty_str_get(sml);
		break;
	default:
		printf("ke_poperty_get :Property not implemented for %d\n", sml_get_vtype(sml));
		break;
	}
}

void ke_poperty_set(sml_t* sml) {
	// get the real name
	int top = sml_get_top(sml);
	token_t * tokp = sml_peek_token(sml, (top - 2));
	switch (sml_get_type(tokp)) {
	case KEV_MAT:
		return ke_poperty_matrix_set(sml);
		break;
	case KEV_VEC:
		return ke_poperty_vector_set(sml);
		break;
	case KEV_VEC_INT:
		return ke_poperty_vector_int_set(sml);
		break;
	case KEV_STR:
		return ke_poperty_str_set(sml);
		break;
	default:
		printf("ke_poperty_set :Property not implemented for type %d\n", sml_get_vtype(sml));
		break;
	}
}
