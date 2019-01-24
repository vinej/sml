#include "kexpr.h"
#include "vector.h"
#include "matrix.h"
#include "str.h"

extern ke1_t ** g_gbl_fields;
static int ke_poperty_str_get(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 0) {
		return ke_str_prop_get_0par(stack, top);
	}
	else if (narg == 1) {
		return ke_str_prop_get_1par(stack, top);
	}
	else if (narg == 2) {
		return ke_str_prop_get_2par(stack, top);
	} else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_matrix_get(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 2) {
		top = ke_matrix_prop_get(stack, top);
	}
	else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_vector_get(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_get(stack, top);
	}
	else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_matrix_set(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 2) {
		top = ke_matrix_prop_set(stack, top);
	}
	else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_str_set(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		return ke_str_prop_set_1par(stack, top);
	}
	else if (narg == 2) {
		return ke_str_prop_set_2par(stack, top);
	}
	else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
}

static int ke_poperty_vector_set(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_prop_set(stack, top);
	}
	else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_vector_int_get(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_int_get(stack, top);
	}
	else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

static int ke_poperty_vector_int_set(struct ke1_s* stack, struct ke1_s* prop, int top) {
	int narg = prop->n_args;
	if (narg == 1) {
		top = ke_vector_prop_int_set(stack, top);
	}
	else {
		printf("Property not implemented for %d\n", prop->vtype);
	}
	return top;
}

int ke_poperty_get(struct ke1_s* stack, struct ke1_s* prop, int top) {
	// get the real name
	ke1_t* tokp = g_gbl_fields[prop->ifield];
	switch (tokp->vtype) {
	case KEV_MAT:
		return ke_poperty_matrix_get(stack, prop, top);
		break;
	case KEV_VEC:
		return ke_poperty_vector_get(stack, prop, top);
		break;
	case KEV_VEC_INT:
		return ke_poperty_vector_int_get(stack, prop, top);
		break;
	case KEV_STR:
		return ke_poperty_str_get(stack, prop, top);
		break;
	default:
		printf("Property not implemented for %d\n", prop->vtype);
		break;
	}
	return top;
}

int ke_poperty_set(struct ke1_s* stack, struct ke1_s* prop, int top) {
	// get the real name
	ke1_t* tokp = g_gbl_fields[prop->ifield];
	switch (tokp->vtype) {
	case KEV_MAT:
		return ke_poperty_matrix_set(stack, prop, top);
		break;
	case KEV_VEC:
		return ke_poperty_vector_set(stack, prop, top);
		break;
	case KEV_VEC_INT:
		return ke_poperty_vector_int_set(stack, prop, top);
		break;
	case KEV_STR:
		return ke_poperty_str_set(stack, prop, top);
		break;
	default:
		printf("Property not implemented for type %d\n", prop->vtype);
		break;
	}
	return top;
}
