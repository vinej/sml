#include "stdafx.h"
#include <math.h>

double ke_constants(const char * name) {
	if (strcmp(name, "m_pi") == 0) return	M_PI;
	else if (strcmp(name, "m_e") == 0) return M_E;
	else if (strcmp(name, "m_radian") == 0) return 1;
	else if (strcmp(name, "m_degree") == 0) return 2;
	else if (strcmp(name, "m_gradian") == 0) return 3;
	return 0;
}


