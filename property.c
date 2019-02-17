#include "kexpr.h"
#include "vector.h"
#include "matrix.h"
#include "str.h"

static int ke_poperty_str_get(sml_t* sml, struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 0) {
		return ke_str_prop_get_0par(sml, prop, top);
	}
	else if (narg == 1) {
		return ke_str_prop_get_1par(sml, prop, top);
	}
	else if (narg == 2) {
		return ke_str_prop_get_2par(sml, prop, top);
	} else {
		printf("ke_poperty_str_get :Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_matrix_get(sml_t* sml, struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 2) {
		top = ke_matrix_prop_get(sml, prop, top);
	}
	else {
		printf("ke_poperty_matrix_get : Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_vector_get(sml_t* sml,struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_get(sml, prop, top);
	}
	else {
		printf("ke_poperty_vector_get : Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_matrix_set(sml_t* sml,struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 2) {
		top = ke_matrix_prop_set(sml, prop, top);
	}
	else {
		printf("ke_poperty_matrix_set : Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_str_set(sml_t* sml,struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		return ke_str_prop_set_1par(sml, prop, top);
	}
	else if (narg == 2) {
		return ke_str_prop_set_2par(sml, prop, top);
	}
	else {
		printf("ke_poperty_str_set :Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_vector_set(sml_t* sml, struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_prop_set(sml, prop, top);
	}
	else {
		printf("ke_poperty_vector_set : Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_vector_int_get(sml_t* sml, struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_int_get(sml, prop, top);
	}
	else {
		printf("ke_poperty_vector_int_get : Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_vector_int_set(sml_t* sml, struct ke1_s** stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_prop_int_set(sml, prop, top);
	}
	else {
		printf("ke_poperty_vector_int_set : Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

int ke_poperty_get(sml_t* sml,struct ke1_s** stack, struct ke1_s* prop, int top) {
	// get the real name
	ke1_t* tokp = stack[top - 1];  //  the real field to manage
	switch (tokp->vtype) {
	case KEV_MAT:
		return ke_poperty_matrix_get(sml, stack, prop, top);
		break;
	case KEV_VEC:
		return ke_poperty_vector_get(sml, stack, prop, top);
		break;
	case KEV_VEC_INT:
		return ke_poperty_vector_int_get(sml, stack, prop, top);
		break;
	case KEV_STR:
		return ke_poperty_str_get(sml, stack, prop, top);
		break;
	default:
		printf("ke_poperty_get :Property not implemented for %d\n", prop->vtype);
		break;
	}
	return top;
}

int ke_poperty_set(sml_t* sml,struct ke1_s** stack, struct ke1_s* prop, int top) {
	// get the real name
	ke1_t* tokp = stack[top - 2];
	switch (tokp->vtype) {
	case KEV_MAT:
		return ke_poperty_matrix_set(sml, stack, prop, top);
		break;
	case KEV_VEC:
		return ke_poperty_vector_set(sml, stack, prop, top);
		break;
	case KEV_VEC_INT:
		return ke_poperty_vector_int_set(sml, stack, prop, top);
		break;
	case KEV_STR:
		return ke_poperty_str_set(sml, stack, prop, top);
		break;
	default:
		printf("ke_poperty_set :Property not implemented for type %d\n", prop->vtype);
		break;
	}
	return top;
}
