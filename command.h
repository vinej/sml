#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

void ke_command_hash();
cmdp ke_command(char * name);
void ke_command_destroy();
int ke_command_get_rtn();
vcmdp ke_command_val(char * name);
int ke_command_icmd(char * name);
void ke_set_ijmp(kexpr_t *kexpr, ke1_t ** tokens);


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
#define CMD_PRINTNOLN "printn"
#define CMD_CONSTANTS "M_"

#define CMD_IFOR 1
#define CMD_INEXT 2
#define CMD_IBRK 3
#define CMD_ICNT 4
#define CMD_IDEF 5
#define CMD_IRTN 6
#define CMD_IIF 7
#define CMD_IELSE 8
#define CMD_IEND 9
#define CMD_IEXE 10

#endif // COMMAND_H_INCLUDED
