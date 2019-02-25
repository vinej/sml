#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

void ke_command_hash(sml_t* sml);
cmdp ke_command(sml_t* sml, char * name);
void ke_command_destroy(sml_t* sml);
//int ke_command_get_rtn();
vcmdp ke_command_val(sml_t* sml, char * name);
int ke_command_icmd(sml_t* sml, char * name);
int ke_set_ijmp(sml_t* sml,kexpr_t *kexpr, token_t ** tokens);


#define CMD_FOR "for"
#define CMD_NEXT "next"
#define CMD_DEF "def"
#define CMD_EXE "exe"
#define CMD_RTN "enddef"
#define CMD_IF "if"
#define CMD_ELSE "else"
#define CMD_END "end"
#define CMD_BRK "break"
#define CMD_CNT "continue"
#define CMD_PRINT "print"
#define CMD_PRINTF "printf"
#define CMD_SPRINTF "sprintf"
#define CMD_PRINTNOLN "printn"
#define CMD_IMPORT "import"
#define CMD_CONSTANTS "M_"

#endif // COMMAND_H_INCLUDED
