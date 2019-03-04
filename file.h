#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include "api.h"

void ke_file_hash(sml_t* sml);

#define FILE_ASSERT_TRUE "assert_true"
#define FILE_ASSERT_FALSE "assert_false"

#define FILE_NEWBUFFER "newbuffer"
#define FILE_FREEBUFFER "freebuffer"
#define FILE_LENBUFFER "lenbuffer"
#define FILE_CLRBUFFER "clrbuffer"

#define FILE_FEXIST "fexist"
#define FILE_FCLOSE "fclose"
#define FILE_FCLOSE "fclose"
#define FILE_CLEARERR "fclearerr"
#define FILE_FEOF "feof"
#define FILE_FERROR "ferror"
#define FILE_FFLUSH "fflush"
#define FILE_FGETPOS "fgetpos"
#define FILE_FOPEN "fopen"
#define FILE_FISOPEN "fisopen"
#define FILE_FREAD "fread"
#define FILE_FREOPEN "freopen"
#define FILE_FSEEK "fseek"
#define FILE_FSETPOS "fsetpos"
#define FILE_FTELL "ftell"
#define FILE_FWRITE "fwrite"
#define FILE_REMOVE "fremove"
#define FILE_RENAME "frename"
#define FILE_REWIND "frewind"
#define FILE_SETBUF "fsetbuf"
#define FILE_SETVBUF "fsetvbuf"
#define FILE_TMPFILE "ftmpfile"
#define FILE_TMPNAME "ftmpnam"

#define FILE_FPRINTF "fprintf"
#define FILE_PRINTF "printf"
#define FILE_SPRINTF "sprintf"
#define FILE_FSCANF "fscanf"
#define FILE_SCANF "scanf"
#define FILE_SSCANF "sscanf"

#define FILE_FGETC "fgetc"
#define FILE_FGETS "fgets"
#define FILE_XFGETS "xfgets"
#define FILE_FPUTC "fputc"
#define FILE_FPUTS "fputs"
#define FILE_GETC "getc"
#define FILE_GETCHAR "getchar"
#define FILE_XGETS "xgets"
#define FILE_GETS "gets"
#define FILE_PUTC "putc"
#define FILE_PUTCHAR "putchar"
#define FILE_PUTS "puts"
#define FILE_UNGETC "ungetc"
#define FILE_PERROR "perror"


#endif // FILE_H_INCLUDED

