#include "file.h"
#include "kexpr.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>


// Closes the stream.All buffers are flushed.
// int fclose(FILE *stream)
static int ke_file_fclose(ke1_t *stack, int top) {
	return top;
}

// Clears the end - of - file and error indicators for the given stream.
// void clearerr(FILE *stream)
static int ke_file_clearerr(ke1_t *stack, int top) {
	return top;
}

// Tests the end - of - file indicator for the given stream.
// int feof(FILE *stream)
static int ke_file_feof(ke1_t *stack, int top) {
	return top;
}

// Tests the error indicator for the given stream.
// int ferror(FILE *stream)
static int ke_file_ferror(ke1_t *stack, int top) {
	return top;
}

// Flushes the output buffer of a stream.
// int fflush(FILE *stream)
static int ke_file_fflush(ke1_t *stack, int top) {
	return top;
}

// Gets the current file position of the stream and writes it to pos.
// int fgetpos(FILE *stream, fpos_t *pos)
static int ke_file_fgetpos(ke1_t *stack, int top) {
	return top;
}

// Opens the filename pointed to by filename using the given mode.
// FILE *fopen(const char *filename, const char *mode)
static int ke_file_fopen(ke1_t *stack, int top) {
	return top;
}

// Reads data from the given stream into the array pointed to by ptr.
// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
static int ke_file_fread(ke1_t *stack, int top) {
	return top;
}

// Associates a new filename with the given open stream and same time closing the old file in stream.
// FILE *freopen(const char *filename, const char *mode, FILE *stream)
static int ke_file_freopen(ke1_t *stack, int top) {
	return top;
}

// Sets the file position of the stream to the given offset.The argument offset signifies the number of bytes to seek from the given whence position.
// int fseek(FILE *stream, long int offset, int whence)
static int ke_file_fseek(ke1_t *stack, int top) {
	return top;
}

// Sets the file position of the given stream to the given position.The argument pos is a position given by the function fgetpos.
// int fsetpos(FILE *stream, const fpos_t *pos)
static int ke_file_fsetpos(ke1_t *stack, int top) {
	return top;
}

// Returns the current file position of the given stream.
// long int ftell(FILE *stream)
static int ke_file_ftell(ke1_t *stack, int top) {
	return top;
}

// Writes data from the array pointed to by ptr to the given stream.
// size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
static int ke_file_fwrtie(ke1_t *stack, int top) {
	return top;
}

// Deletes the given filename so that it is no longer accessible.
// int remove(const char *filename)
static int ke_file_remove(ke1_t *stack, int top) {
	return top;
}


// Causes the filename referred to, by old_filename to be changed to new_filename.
// int rename(const char *old_filename, const char *new_filename)
static int ke_file_rename(ke1_t *stack, int top) {
	return top;
}

// Sets the file position to the beginning of the file of the given stream.
// void rewind(FILE *stream)
static int ke_file_rewind(ke1_t *stack, int top) {
	return top;
}

// Defines how a stream should be buffered.
// void setbuf(FILE *stream, char *buffer)
static int ke_file_setbuf(ke1_t *stack, int top) {
	return top;
}

// Another function to define how a stream should be buffered.
// int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
static int ke_file_setvbuf(ke1_t *stack, int top) {
	return top;
}


// Creates a temporary file in binary update mode(wb + ).
// FILE *tmpfile(void)
static int ke_file_tmpfile(ke1_t *stack, int top) {
	return top;
}

// Generates and returns a valid temporary filename which does not exist.
// char *tmpnam(char *str)
static int ke_file_tmpnam(ke1_t *stack, int top) {
	return top;
}

// Sends formatted output to a stream.
// int fprintf(FILE *stream, const char *format, ...)
static int ke_file_fprintf(ke1_t *stack, int top) {
	return top;
}

// Sends formatted output to stdout.
// int printf(const char *format, ...)
static int ke_file_printf(ke1_t *stack, int top) {
	return top;
}

// Sends formatted output to a string.
// int sprintf(char *str, const char *format, ...)
static int ke_file_sprintf(ke1_t *stack, int top) {
	return top;
}

// Sends formatted output to a stream using an argument list.
// int vfprintf(FILE *stream, const char *format, va_list arg)
static int ke_file_vfprintf(ke1_t *stack, int top) {
	return top;
}


// Sends formatted output to stdout using an argument list.
// int vprintf(const char *format, va_list arg)
static int ke_file_vprintf(ke1_t *stack, int top) {
	return top;
}

// Sends formatted output to a string using an argument list.
// int vsprintf(char *str, const char *format, va_list arg)
static int ke_file_vsprintf(ke1_t *stack, int top) {
	return top;
}


// Reads formatted input from a stream.
// int fscanf(FILE *stream, const char *format, ...)
static int ke_file_fscanf(ke1_t *stack, int top) {
	return top;
}


// Reads formatted input from stdin.
// int scanf(const char *format, ...)
static int ke_file_scanf(ke1_t *stack, int top) {
	return top;
}

// Reads formatted input from a string.
// int sscanf(const char *str, const char *format, ...)
static int ke_file_sscanf(ke1_t *stack, int top) {
	return top;
}


// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int fgetc(FILE *stream)
static int ke_file_fgetc(ke1_t *stack, int top) {
	return top;
}

// Reads a line from the specified stream and stores it into the string pointed to by str.It stops when either(n - 1) characters are read, the newline character is read, or the end - of - file is reached, whichever comes first.
// char *fgets(char *str, int n, FILE *stream)
static int ke_file_fgets(ke1_t *stack, int top) {
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// int fputc(int char, FILE *stream)
static int ke_file_fputc(ke1_t *stack, int top) {
	return top;
}

// Writes a string to the specified stream up to but not including the null character.
// int fputs(const char *str, FILE *stream)
static int ke_file_fputs(ke1_t *stack, int top) {
	return top;
}

// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int getc(FILE *stream)
static int ke_file_getc(ke1_t *stack, int top) {
	return top;
}

// Gets a character(an unsigned char) from stdin.
// int getchar(void)
static int ke_file_getchar(ke1_t *stack, int top) {
	return top;
}

// Reads a line from stdin and stores it into the string pointed to by, str.It stops when either the newline character is read or when the end - of - file is reached, whichever comes first.
// char *gets(char *str)
static int ke_file_gets(ke1_t *stack, int top) {
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// 37 int putc(int char, FILE *stream)
static int ke_file_putc(ke1_t *stack, int top) {
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to stdout.
// int putchar(int char)
static int ke_file_putchar(ke1_t *stack, int top) {
	return top;
}

// Writes a string to stdout up to but not including the null character.A newline character is appended to the output.
// int puts(const char *str)
static int ke_file_puts(ke1_t *stack, int top) {
	return top;
}

// Pushes the character char(an unsigned char) onto the specified stream so that the next character is read.
// int ungetc(int char, FILE *stream)
static int ke_file_ungetc(ke1_t *stack, int top) {
	return top;
}


// Prints a descriptive error message to stderr.First the string str is printed followed by a colon and then a space.
// void perror(const char *str)
static int ke_file_perror(ke1_t *stack, int top) {
	return top;
}

/*
int getint(void)
{
	int x = 0;
	register int c = getchar_unlocked();
	for (; c > 47 && c < 58; c = getchar_unlocked())
	{
		x = (x << 1) + (x << 3) + c&15;
	}
	return x;
}


double getdouble(void) {
	register int ch = getchar_unlocked();
	int sign = ch;
	if (sign == '-' || sign == '+') {
		ch = getchar_unlocked();
	}
	long long sum = 0;
	int digit_count = 0;
	int dp_offset = -1;
	int power10 = 0;
	int dp = '.';
	for (;;) {
		while (ch >= '0' && ch <= '9') {
			digit_count++;
			if (sum < LLONG_MAX / 10) {
				sum = sum * 10 + ch - '0';
			}
			else {
				power10++;
			}
			ch = getchar_unlocked();
		}
		if (ch == dp) {
			dp = '0';
			dp_offset = digit_count;
			ch = getchar_unlocked();
		}
		else {
			break;
		}
	}
	if (dp_offset >= 0) {
		power10 -= digit_count - dp_offset;
	}
	if (ch == 'e' || ch == 'E') {
		ch = getchar_unlocked();
		int esign = ch;
		if (esign == '-' || esign == '+') {
			ch = getchar_unlocked();
		}
		int expo = 0;
		while (ch >= '0' && ch <= '9') {
			expo = expo * 10 + ch - '0';
			ch = getchar_unlocked();
		}
		if (esign == '-') expo = -expo;
		power10 += expo;
	}
	double y = sum;
	if (power10) {
		// combine these 2 steps for speed with `pow(10,power10)`.
		// leave separate for better handling of very large/very tiny numbers 
		y *= pow(5, power10);  // 
		y = ldexp(y, power10);
	}
	if (sign == '-') y = -y;
	return y;
}
*/

void ke_file_hash() {
	ke_hash_add((fncp)&ke_file_fclose, FILE_FCLOSE);
	ke_hash_add((fncp)&ke_file_clearerr, FILE_CLEARERR);
	ke_hash_add((fncp)&ke_file_feof, FILE_FEOF);
	ke_hash_add((fncp)&ke_file_ferror, FILE_FERROR);
	ke_hash_add((fncp)&ke_file_fflush, FILE_FFLUSH);
	ke_hash_add((fncp)&ke_file_fgetpos, FILE_FGETPOS);
	ke_hash_add((fncp)&ke_file_fopen, FILE_FOPEN);
	ke_hash_add((fncp)&ke_file_fread, FILE_FREAD);
	ke_hash_add((fncp)&ke_file_freopen, FILE_FREOPEN);
	ke_hash_add((fncp)&ke_file_fseek, FILE_FSEEK);
	ke_hash_add((fncp)&ke_file_fsetpos, FILE_FSETPOS);
	ke_hash_add((fncp)&ke_file_ftell, FILE_FTELL);
	ke_hash_add((fncp)&ke_file_fwrite, FILE_FWRITE);
	ke_hash_add((fncp)&ke_file_fwrite, FILE_FWRITE);

#define FILE_REMOVE "remove"
#define FILE_RENAME "rename"
#define FILE_REWIND "rewind"
#define FILE_SETBUF "setbuf"
#define FILE_SETVBUF "setvbuf"
#define FILE_TMPFILE "tmpfile"
#define FILE_SETNAME "setnam"
#define FILE_FPRINTF "fprintf"
#define FILE_FPRINT "fprint"
#define FILE_SPRINTF "sprintf"
#define FILE_VFPRINTF "vfprintf"
#define FILE_VPRINTF "vprintf"
#define FILE_VSPRINTF "vsprintf"
#define FILE_FSCANF "fscanf"
#define FILE_SCANF "scanf"
#define FILE_SSCANF "sscanf"
#define FILE_FGETC "fgetc"
#define FILE_FGETS "fgets"
#define FILE_FPUTC "fputc"
#define FILE_FPUTS "fputs"
#define FILE_GETC "getc"
#define FILE_GETCHAR "getchar"
#define FILE_GETS "gets"
#define FILE_PUTC "putc"
#define FILE_PUTCHAR "putchar"
#define FILE_PUTS "puts"
#define FILE_UNGETC "ungetc"
#define FILE_PERROR "perror"


}

void ke_file_print(ke1_t *k) {
	printf("Function: %d\n", k->icmd);
	printf("%g\n", k->r);
}
