#include "kexpr.h"
#include "vector.h"

extern ke1_t ** g_gbl_fields;

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

int ke_poperty_get(struct ke1_s* stack, struct ke1_s* prop, int top) {
	// get the real name
	ke1_t* tokp = g_gbl_fields[prop->ifield];
	switch (tokp->vtype) {
	case KEV_MAT:
		break;
	case KEV_VEC:
		return ke_poperty_vector_get(stack, prop, top);
		break;
	case KEV_VEC_INT:
		break;
	case KEV_STR:
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
		break;
	case KEV_VEC:
		return ke_poperty_vector_set(stack, prop, top);
		break;
	case KEV_VEC_INT:
		break;
	case KEV_STR:
		break;
	default:
		printf("Property not implemented for type %d\n", prop->vtype);
		break;
	}
	return top;
}
