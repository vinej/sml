#include "file.h"
#include "kexpr.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>

extern ke1_t *g_stack;    // stack for the evaluation of the program
extern ke1_t ** g_gbl_fields;

// alloc a buffer
static int ke_file_alloc_buffer(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	p->obj.buffer = ke_calloc_memory((size_t)p->i, (fpos_t)1);
	p->vtype = KEV_BUFFER;
	p->ttype = KET_VAL;
	return top;
}

// free a buffer
static int ke_file_free_buffer(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	ke_free_memory(p->obj.buffer);
	return top;
}

// Closes the stream.All buffers are flushed.
// int fclose(FILE *stream)
static int ke_file_fclose(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	fclose(p->obj.file);
	return top;
}

// Clears the end - of - file and error indicators for the given stream.
// void clearerr(FILE *stream)
static int ke_file_clearerr(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	clearerr(p->obj.file);
	return top;
}

// Tests the end - of - file indicator for the given stream.
// int feof(FILE *stream)
static int ke_file_feof(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[top - 1];
	int st = feof(p->obj.file);
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	p->i = st;
	p->r = st;
	return top;
}

// Tests the error indicator for the given stream.
// int ferror(FILE *stream)
static int ke_file_ferror(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[top - 1];
	int st = ferror(p->obj.file);
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	p->i = st;
	p->r = st;
	return top;
}

// Flushes the output buffer of a stream.
// int fflush(FILE *stream)
static int ke_file_fflush(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	fflush(p->obj.file);
	return top;
}

// Gets the current file position of the stream and writes it to pos.
// int fgetpos(FILE *stream, fpos_t *pos)
static int ke_file_fgetpos(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	fpos_t pos;
	fgetpos(p->obj.file, &pos);
	p->i = (int64_t)pos;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	p->r = (double)p->i;
	return top;
}

// Opens the filename pointed to by filename using the given mode.
// FILE *fopen(const char *filename, const char *mode)
static int ke_file_fopen(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *filename, *mode;
	mode = &stack[--top];
	filename = &stack[top - 1];
	filename->obj.file = fopen(filename->obj.s, mode->obj.s);
	filename->vtype = KEV_FILE;
	filename->ttype = KET_VAL;
	return top;
}

// Reads data from the given stream into the array pointed to by ptr.
// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
static int ke_file_fread(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *ptr, *size, *nmemb, *stream;
	stream = &stack[--top];
	nmemb = &stack[--top];
	size = &stack[--top];
	ptr = &stack[top - 1];

	ptr->i = fread(ptr->obj.buffer, (size_t)size->i, (size_t)nmemb->i, stream->obj.file);
	ptr->vtype = KEV_INT;
	ptr->ttype = KET_VAL;
	ptr->r = (double)ptr->i;
	return top;
}

// Associates a new filename with the given open stream and same time closing the old file in stream.
// FILE *freopen(const char *filename, const char *mode, FILE *stream)
static int ke_file_freopen(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *filename, *mode, *stream;
	stream = &stack[--top];
	mode = &stack[--top];
	filename = &stack[top - 1];

	FILE * tmp = freopen(filename->obj.s, mode->obj.s, stream->obj.file);
	filename->obj.file = tmp;
	filename->vtype = KEV_FILE;
	filename->ttype = KET_VAL;
	return top;
}

// Sets the file position of the stream to the given offset.The argument offset signifies the number of bytes to seek from the given whence position.
// int fseek(FILE *stream, long int offset, int whence)
static int ke_file_fseek(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *stream, *offset, *whence;
	whence = &stack[--top];
	offset = &stack[--top];
	stream = &stack[--top];
	fseek(stream->obj.file, (unsigned long)offset->i, (int)whence->i);
	return top;
}

// Sets the file position of the given stream to the given position.The argument pos is a position given by the function fgetpos.
// int fsetpos(FILE *stream, const fpos_t *pos)
static int ke_file_fsetpos(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[top - 1];
	fpos_t pos;
	fsetpos(p->obj.file, &pos);
	p->i = (int64_t)pos;
	p->r = (double)p->i;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

// Returns the current file position of the given stream.
// long int ftell(FILE *stream)
static int ke_file_ftell(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[top - 1];
	p->i = ftell(p->obj.file);
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	p->r = (double)p->i;
	return top;
}

// Writes data from the array pointed to by ptr to the given stream.
// size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
static int ke_file_fwrite(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *ptr, *size, *nmemb, *stream;
	stream = &stack[--top];
	nmemb = &stack[--top];
	size = &stack[--top];
	ptr = &stack[top - 1];

	ptr->i = fwrite(ptr->obj.buffer, (size_t)size->i, (size_t)nmemb->i, stream->obj.file);
	ptr->vtype = KEV_INT;
	ptr->ttype = KET_VAL;
	ptr->r = (double)ptr->i;
	return top;
}

// Deletes the given filename so that it is no longer accessible.
// int remove(const char *filename)
static int ke_file_remove(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *filename;
	filename = &stack[--top];
	remove(filename->obj.s);
	return top;
}


// Causes the filename referred to, by old_filename to be changed to new_filename.
// int rename(const char *old_filename, const char *new_filename)
static int ke_file_rename(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *old_filename, *new_filename;
	new_filename = &stack[--top];
	old_filename = &stack[--top];
	rename(old_filename->obj.s, new_filename->obj.s);
	return top;
}

// Sets the file position to the beginning of the file of the given stream.
// void rewind(FILE *stream)
static int ke_file_rewind(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *stream;
	stream = &stack[--top];
	rewind(stream->obj.file);
	return top;
}

// Defines how a stream should be buffered.
// void setbuf(FILE *stream, char *buffer)
static int ke_file_setbuf(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *stream, *buffer;
	buffer = &stack[--top];
	stream = &stack[--top];
	setbuf(stream->obj.file, buffer->obj.buffer);
	return top;
}

// Another function to define how a stream should be buffered.
// int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
static int ke_file_setvbuf(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *stream, *buffer, *mode, *size;
	size = &stack[--top];
	mode = &stack[--top];
	buffer = &stack[--top];
	stream = &stack[--top];
	setvbuf(stream->obj.file, buffer->obj.buffer, (int)mode->i, (size_t)size->i);
	return top;
}


// Creates a temporary file in binary update mode(wb + ).
// FILE *tmpfile(void)
static int ke_file_tmpfile(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	p->obj.file = tmpfile();
	p->vtype = KEV_FILE;
	p->ttype = KET_VAL;
	return top;
}

// Generates and returns a valid temporary filename which does not exist.
// char *tmpnam(char *str)
static int ke_file_tmpnam(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	p->obj.s = tmpnam(p->obj.s);
	p->vtype = KEV_STR;
	p->ttype = KET_VAL;
	return top;
}

va_list gen_valist(size_t n_args, int len, int top, int * size) {
	ke1_t *q;
	va_list m = (char*)ke_calloc_memory(1000, 1); /* prepare enough memory*/
	void* va = m; /* copies the pointer */
	size_t total_size = len + 1;
	for (int i = top - n_args + 1; i < top; i++) {
		q = &g_stack[i];
		if (q->vtype == KEV_STR) {
			(*(char**)m) = q->obj.s; /* puts the next value */
			m += sizeof(char*); /* move forward again*/
			total_size += strlen(q->obj.s) + 1;
		}
		else if (q->vtype == KEV_INT) {
			(*(int*)m) = (int)q->i; /* puts the third element */
			m += sizeof(int); /* unneeded, but here for clarity. */
			total_size += (size_t)20;
		}
		else if (q->vtype == KEV_REAL) {
			(*(double*)m) = q->r; /* puts the third element */
			m += sizeof(double); /* unneeded, but here for clarity. */
			total_size += (size_t)20;
		}
	}
	*size = total_size;
	return va;
}


// Sends formatted output to a stream using an argument list.
// int vfprintf(FILE *stream, const char *format, va_list arg)
static int ke_file_vfprintf(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *stream, *format;
	format = &stack[top - tokp->n_args + 1];
	stream = &stack[top - tokp->n_args];
	if (tokp->n_args > 2) {
		int total_size = 0;
		va_list va = gen_valist(tokp->n_args, strlen(format->obj.s), top, &total_size);
		vfprintf(stream->obj.file, format->obj.s, va);
		ke_free_memory(va);
	}
	else {
		vfprintf(stream->obj.file, format->obj.s, NULL);
	}
	return top - tokp->n_args + 1;
}


// Sends formatted output to stdout using an argument list.
// int vprintf(const char *format, va_list arg)
static int ke_file_vprintf(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t  *format;
	format = &stack[top - tokp->n_args];
	if (tokp->n_args > 1) {
		int total_size = 0;
		va_list va = gen_valist(tokp->n_args, strlen(format->obj.s), top, &total_size);
		vprintf(format->obj.s, va);
		ke_free_memory(va);
	}
	else {
		vprintf(format->obj.s, NULL);
	}
	return top - tokp->n_args + 1;
}

// Sends formatted output to a string using an argument list.
// int vsprintf(char *str, const char *format, va_list arg)
static int ke_file_vsprintf(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t  *str, *format;
	format = &stack[top - tokp->n_args + 1];
	str = &stack[top - tokp->n_args];
	if (tokp->n_args > 1) {
		int total_size = 0;
		va_list va = gen_valist(tokp->n_args, strlen(format->obj.s), top, &total_size);
		vsprintf(str->obj.s, format->obj.s, va);
		ke_free_memory(va);
	}
	else {
		vsprintf(str->obj.s, format->obj.s, NULL);
	}
	return top - tokp->n_args + 1;
}

// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int fgetc(FILE *stream)
static int ke_file_fgetc(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *stream;
	stream = &stack[--top];
	stream->i = fgetc(stream->obj.file);
	stream->vtype = KEV_INT;
	stream->ttype = KET_VAL;
	stream->r = (double)stream->i;
	return top;
}

// Reads a line from the specified stream and stores it into the string pointed to by str.It stops when either(n - 1) characters are read, the newline character is read, or the end - of - file is reached, whichever comes first.
// char *fgets(char *str, int n, FILE *stream)
static int ke_file_fgets(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *str, *n, *stream;
	stream = &stack[--top];
	n = &stack[--top];
	str = &stack[--top];
	//memset(g_gbl_fields[str->ifield]->obj.s, 0, n->i);
	fgets(g_gbl_fields[str->ifield]->obj.s, (int)n->i, stream->obj.file);
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// int fputc(int char, FILE *stream)
static int ke_file_fputc(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *c, *stream;
	stream = &stack[--top];
	c = &stack[--top];
	fputc((char)c->i, stream->obj.file);
	return top;
}

// Writes a string to the specified stream up to but not including the null character.
// int fputs(const char *str, FILE *stream)
static int ke_file_fputs(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *str, *stream;
	stream = &stack[--top];
	str = &stack[--top];
	fputs(str->obj.s, stream->obj.file);
	return top;
}

// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int getc(FILE *stream)
static int ke_file_getc(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	p->i = fgetc(p->obj.file);
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	p->r = (double)p->i;
	return top;
}

// Gets a character(an unsigned char) from stdin.
// int getchar(void)
static int ke_file_getchar(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *p;
	p = &stack[--top];
	p->i = getchar();
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	p->r = (double)p->i;
	return top;
}

// Reads a line from stdin and stores it into the string pointed to by, str.It stops when either the newline character is read or when the end - of - file is reached, whichever comes first.
// char *gets(char *str)
static int ke_file_gets(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *str;
	str = &stack[top - 1];
	gets(str->obj.s);
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// 37 int putc(int char, FILE *stream)
static int ke_file_putc(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *c, *stream;
	c = &stack[--top];
	stream = &stack[--top];
	putc((int)c->i, stream->obj.file);
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to stdout.
// int putchar(int char)
static int ke_file_putchar(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *c;
	c = &stack[--top];
	putchar((int)c->i);
	return top;
}

// Writes a string to stdout up to but not including the null character.A newline character is appended to the output.
// int puts(const char *str)
static int ke_file_puts(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *str;
	str = &stack[--top];
	puts(str->obj.s);
	return top;
}

// Pushes the character char(an unsigned char) onto the specified stream so that the next character is read.
// int ungetc(int char, FILE *stream)
static int ke_file_ungetc(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *c, *stream;
	stream = &stack[--top];
	c = &stack[--top];
	ungetc((int)c->i, stream->obj.file);
	return top;
}


// Prints a descriptive error message to stderr.First the string str is printed followed by a colon and then a space.
// void perror(const char *str)
static int ke_file_perror(ke1_t *stack, ke1_t *tokp, int top) {
	ke1_t *str;
	str = &stack[--top];
	perror(str->obj.s);
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
	ke_hash_add((fncp)&ke_file_alloc_buffer, FILE_NEWBUFFER);
	ke_hash_add((fncp)&ke_file_free_buffer, FILE_FREBUFFER);
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
	ke_hash_add((fncp)&ke_file_remove, FILE_REMOVE);
	ke_hash_add((fncp)&ke_file_rename, FILE_RENAME);
	ke_hash_add((fncp)&ke_file_rewind, FILE_REWIND);
	ke_hash_add((fncp)&ke_file_setbuf, FILE_SETBUF);
	ke_hash_add((fncp)&ke_file_setvbuf, FILE_SETVBUF);
	ke_hash_add((fncp)&ke_file_tmpfile, FILE_TMPFILE);
	ke_hash_add((fncp)&ke_file_tmpnam, FILE_TMPNAME);
	ke_hash_add((fncp)&ke_file_vfprintf, FILE_VFPRINTF);
	ke_hash_add((fncp)&ke_file_vsprintf, FILE_VSPRINTF);
	//ke_hash_add((fncp)&ke_file_fscanf, FILE_FSCANF);
	//ke_hash_add((fncp)&ke_file_scanf, FILE_SCANF);
	//ke_hash_add((fncp)&ke_file_sscanf, FILE_SSCANF);
	ke_hash_add((fncp)&ke_file_fgetc, FILE_FGETC);
	ke_hash_add((fncp)&ke_file_fgets, FILE_FGETS);
	ke_hash_add((fncp)&ke_file_fputc, FILE_FPUTC);
	ke_hash_add((fncp)&ke_file_fputs, FILE_FPUTS);
	ke_hash_add((fncp)&ke_file_getc, FILE_GETC);
	ke_hash_add((fncp)&ke_file_getchar, FILE_GETCHAR);
	ke_hash_add((fncp)&ke_file_gets, FILE_GETS);
	ke_hash_add((fncp)&ke_file_putc, FILE_PUTC);
	ke_hash_add((fncp)&ke_file_putchar, FILE_PUTCHAR);
	ke_hash_add((fncp)&ke_file_puts, FILE_PUTS);
	ke_hash_add((fncp)&ke_file_ungetc, FILE_UNGETC);
	ke_hash_add((fncp)&ke_file_perror, FILE_PERROR);

}

void ke_file_print(ke1_t *k) {
	printf("File: %d\n", k->icmd);
	printf("%g\n", k->r);
}