#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include "api.h"

void ke_file_hash(sml_t* sml);

#define FILE_ASSERT_TRUE "assert.true"
#define FILE_ASSERT_FALSE "assert.false"

#define FILE_NEWBUFFER "buf.new"
#define FILE_FREEBUFFER "buf.free"
#define FILE_LENBUFFER "buf.len"
#define FILE_CLRBUFFER "buf.clr"

#define FILE_FEXIST "file.exist"
#define FILE_FCLOSE "file.close"
#define FILE_CLEARERR "file.clearerr"
#define FILE_FEOF "file.eof"
#define FILE_FERROR "file.error"
#define FILE_FFLUSH "file.flush"
#define FILE_FGETPOS "file.getpos"
#define FILE_FOPEN "file.open"
#define FILE_FISOPEN "file.isopen"
#define FILE_FREAD "file.read"
#define FILE_FREOPEN "file.reopen"
#define FILE_FSEEK "file.seek"
#define FILE_FSETPOS "file.setpos"
#define FILE_FTELL "file.tell"
#define FILE_FWRITE "file.write"
#define FILE_REMOVE "file.remove"
#define FILE_RENAME "file.rename"
#define FILE_REWIND "file.rewind"
#define FILE_SETBUF "file.setbuf"
#define FILE_SETVBUF "fsetvbuf"
#define FILE_TMPFILE "file.tmp"
#define FILE_TMPNAME "file.tmpname"

#define FILE_FPRINTF "file.fprintf"
#define FILE_PRINT "print"
#define FILE_PRINTF "file.printf"
#define FILE_SPRINTF "file.sprintf"
#define FILE_FSCANF "file.fscanf"
#define FILE_SCANF "file.scanf"
#define FILE_SSCANF "file.sscanf"
#define FILE_FGETC "file.fgetc"
#define FILE_FGETS "file.fgets"
#define FILE_XFGETS "file.xfgets"
#define FILE_FPUTC "file.fputc"
#define FILE_FPUTS "file.fputs"
#define FILE_GETC "file.getc"
#define FILE_GETCHAR "file.getchar"
#define FILE_XGETS "file.xgets"
#define FILE_GETS "file.gets"
#define FILE_PUTC "file.putc"
#define FILE_PUTCHAR "file.putchar"
#define FILE_PUTS "file.puts"
#define FILE_UNGETC "file.ungetc"
#define FILE_PERROR "file.perror"


#endif // FILE_H_INCLUDED

