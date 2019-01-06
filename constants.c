#include "stdafx.h"
#include <math.h>

double ke_constants(const char * name) {
	if (strcmp(name, "m_pi") == 0) return	M_PI;
	if (strcmp(name, "m_e") == 0) return M_E;
	return 0;
}


