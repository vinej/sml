#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

#include "api.h"

void ke_str_hash(sml_t* sml);
void ke_str_prop_get_0par(sml_t* sml);
void ke_str_prop_get_1par(sml_t* sml);
void ke_str_prop_set_1par(sml_t* sml);
void ke_str_prop_get_2par(sml_t* sml);
void ke_str_prop_set_2par(sml_t* sml);

#define STR_STRBUF "strbuf"
#define STR_STRFREE "strfree"
#define STR_STRCPY "strcpy"
#define STR_STRCAT "strcat"
#define STR_STRLEN "strlen"
#define STR_STRCMP "strcmp"
//#define STR_STRCHR "strchr"
//#define STR_STRSTR "strstr"
//#define STR_STRFMT "strfmt"


#endif // STR_H_INCLUDED
