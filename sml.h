#ifndef SML_H
#define SML_H

#include "kexpr.h"

int ke_sml(sml_t *sml, kexpr_t *kexpr, int64_t *_i, double *_r, char **_p, int *ret_type);

#endif