#define STB_SPRINTF_IMPLEMENTATION
#include "./stb/stb_sprintf.h"
#include "kexpr.h"
#include "file.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#define MAX_BUF 1023
#define MAX_SCAN_ARG 16

// alloc a buffer
static int ke_file_alloc_buffer(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_NEWBUFFER);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_INT, "buffer_size", FILE_NEWBUFFER);
	ke_validate_parameter_int_gt_zero(p, "buffer_size", FILE_NEWBUFFER);
#endif // SML_VALIDATION
	out->obj.buffer = ke_calloc_memory(sml, (size_t)p->i, 1);
	out->vtype = KEV_BUFFER;
	out->ttype = KET_VAL;
	return top;
}

// free a buffer
static int ke_file_free_buffer(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FREBUFFER);
#endif // SML_VALIDATION
	ke1_t *p;
	p = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_BUFFER, "buffer_name", FILE_FREBUFFER);
	ke_validate_parameter_not_null(p, p->obj.buffer, "buffer_name", FILE_FREBUFFER);
#endif // SML_VALIDATION
	ke_free_memory(sml, p->obj.buffer);
	return top;
}

// Closes the stream.All buffers are flushed.
// int fclose(FILE *stream)
static int ke_file_fclose(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FCLOSE);
#endif // SML_VALIDATION
	ke1_t *p;
	p = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_FILE, "file", FILE_FCLOSE);
#endif // SML_VALIDATION
	fclose(p->obj.file);
	return top;
}

// Clears the end - of - file and error indicators for the given stream.
// void clearerr(FILE *stream)
static int ke_file_clearerr(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_CLEARERR);
#endif // SML_VALIDATION
	ke1_t *p;
	p = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_FILE, "file",FILE_CLEARERR);
#endif // SML_VALIDATION
	clearerr(p->obj.file);
	return top;
}

// Tests the end - of - file indicator for the given stream.
// int feof(FILE *stream)
static int ke_file_feof(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FEOF);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_FILE, "file", FILE_FEOF);
#endif // SML_VALIDATION
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->i = feof(p->obj.file);
	out->r = (double)p->i;
	return top;
}

// Tests the error indicator for the given stream.
// int ferror(FILE *stream)
static int ke_file_ferror(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FERROR);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_FILE, "file", FILE_FERROR);
#endif // SML_VALIDATION
	int st = ferror(p->obj.file);
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->i = st;
	out->r = st;
	return top;
}

// Flushes the output buffer of a stream.
// int fflush(FILE *stream)
static int ke_file_fflush(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FFLUSH);
#endif // SML_VALIDATION
	ke1_t *p;
	p = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_FILE, "file", FILE_FFLUSH);
#endif // SML_VALIDATION
	fflush(p->obj.file);
	return top;
}

// Gets the current file position of the stream and writes it to pos.
// int fgetpos(FILE *stream) //, fpos_t *pos)
static int ke_file_fgetpos(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FGETPOS);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->r = (double)p->i;
	return top;
}

// Opens the filename pointed to by filename using the given mode.
// FILE *fopen(const char *filename, const char *mode)
static int ke_file_fopen(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_FOPEN);
#endif // SML_VALIDATION
	ke1_t *out, *p, *mode;
	mode = stack[--top];
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_STR, "filename", FILE_FOPEN);
	ke_validate_parameter_vtype(mode, KEV_STR, "mode", FILE_FOPEN);
#endif // SML_VALIDATION
	out->obj.file = fopen(p->obj.s, mode->obj.s);
	out->vtype = KEV_FILE;
	out->ttype = KET_VAL;
	return top;
}

// Reads data from the given stream into the array pointed to by ptr.
// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
static int ke_file_fread(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 4, FILE_FREAD);
#endif // SML_VALIDATION
	ke1_t *out, *ptr, *size, *nmemb, *stream;
	stream = stack[--top];
	nmemb = stack[--top];
	size = stack[--top];
	ptr = stack[--top ];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(ptr, KEV_BUFFER, "buffer_name", FILE_FREAD);
	ke_validate_parameter_vtype(size, KEV_INT, "size", FILE_FREAD);
	ke_validate_parameter_vtype(nmemb, KEV_INT, "nmemb", FILE_FREAD);
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_FREAD);
#endif // SML_VALIDATION

	out->i = fread(ptr->obj.buffer, (size_t)size->i, (size_t)nmemb->i, stream->obj.file);
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->r = (double)out->i;
	return top;
}

// Associates a new filename with the given open stream and same time closing the old file in stream.
// FILE *freopen(const char *filename, const char *mode, FILE *stream)
static int ke_file_freopen(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 3, FILE_FREOPEN);
#endif // SML_VALIDATION
	ke1_t *out, *filename, *mode, *stream;
	stream = stack[--top];
	mode = stack[--top];
	filename = stack[--top];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(filename, KEV_STR, "filename", FILE_FREOPEN);
	ke_validate_parameter_vtype(mode, KEV_STR, "mode", FILE_FREOPEN);
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_FREOPEN);
#endif // SML_VALIDATION
	FILE * tmp = freopen(filename->obj.s, mode->obj.s, stream->obj.file);
	out->obj.file = tmp;
	out->vtype = KEV_FILE;
	out->ttype = KET_VAL;
	return top;
}

// Sets the file position of the stream to the given offset.The argument offset signifies the number of bytes to seek from the given whence position.
// int fseek(FILE *stream, long int offset, int whence)
static int ke_file_fseek(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 3, FILE_FSEEK);
#endif // SML_VALIDATION
	ke1_t *stream, *offset, *whence;
	whence = stack[--top];
	offset = stack[--top];
	stream = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(whence, KEV_INT, "whence", FILE_FSEEK);
	ke_validate_parameter_vtype(offset, KEV_INT, "offset", FILE_FSEEK);
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_FSEEK);
#endif // SML_VALIDATION

	fseek(stream->obj.file, (unsigned long)offset->i, (int)whence->i);
	return top;
}

// Sets the file position of the given stream to the given position.The argument pos is a position given by the function fgetpos.
// int fsetpos(FILE *stream, const fpos_t *pos)
static int ke_file_fsetpos(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_FSETPOS);
#endif // SML_VALIDATION
	ke1_t *p, *pos;
	pos = stack[--top];
	p = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(pos, KEV_INT, "file", FILE_FSETPOS);
	ke_validate_parameter_vtype(p, KEV_FILE, "pos",  FILE_FSETPOS);
#endif // SML_VALIDATION

	//fsetpos(p->obj.file, pos->i);
	return top;
}

// Returns the current file position of the given stream.
// long int ftell(FILE *stream)
static int ke_file_ftell(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FTELL);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(p, KEV_FILE, "file", FILE_FSETPOS);
#endif // SML_VALIDATION
	out->i = ftell(p->obj.file);
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->r = (double)out->i;
	return top;
}

// Writes data from the array pointed to by ptr to the given stream.
// size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
static int ke_file_fwrite(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 4, FILE_FWRITE);
#endif // SML_VALIDATION
	ke1_t *out, *ptr, *size, *nmemb, *stream;
	stream = stack[--top];
	nmemb = stack[--top];
	size = stack[--top];
	ptr = stack[--top];
	stack[top++] = sml->out; out = sml->out;
#ifdef _DEBUG
	ke_validate_parameter_vtype(ptr, KEV_BUFFER, "buffer_name", FILE_FWRITE);
	ke_validate_parameter_vtype(nmemb, KEV_INT, "nmemb", FILE_FWRITE);
	ke_validate_parameter_vtype(size, KEV_INT, "size", FILE_FWRITE);
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_FWRITE);
#endif // SML_VALIDATION
	out->i = fwrite(ptr->obj.buffer, (size_t)size->i, (size_t)nmemb->i, stream->obj.file);
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->r = (double)out->i;
	return top;
}

// Deletes the given filename so that it is no longer accessible.
// int remove(const char *filename)
static int ke_file_remove(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_REMOVE);
#endif // SML_VALIDATION
	ke1_t *filename;
	filename = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(filename, KEV_STR, "filename", FILE_REMOVE);
#endif // SML_VALIDATION
	remove(filename->obj.s);
	return top;
}


// Causes the filename referred to, by old_filename to be changed to new_filename.
// int rename(const char *old_filename, const char *new_filename)
static int ke_file_rename(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_RENAME);
#endif // SML_VALIDATION
	ke1_t *old_filename, *new_filename;
	new_filename = stack[--top];
	old_filename = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(new_filename, KEV_STR, "new_filename", FILE_RENAME);
	ke_validate_parameter_vtype(old_filename, KEV_STR, "old_filename", FILE_RENAME);
#endif // SML_VALIDATION
	rename(old_filename->obj.s, new_filename->obj.s);
	return top;
}

// Sets the file position to the beginning of the file of the given stream.
// void rewind(FILE *stream)
static int ke_file_rewind(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_REWIND);
#endif // SML_VALIDATION
	ke1_t *stream;
	--top;  // don't need out
#ifdef _DEBUG
	ke_validate_parameter_vtype(stream, KEV_STR, "file", FILE_REWIND);
#endif // SML_VALIDATION
	rewind(stream->obj.file);
	return top;
}

// Defines how a stream should be buffered.
// void setbuf(FILE *stream, char *buffer)
static int ke_file_setbuf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_SETBUF);
#endif // SML_VALIDATION
	ke1_t *stream, *buffer;
	buffer = stack[--top];
	stream = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_SETBUF);
	ke_validate_parameter_vtype(buffer, KEV_BUFFER, "buffer_name", FILE_SETBUF);
#endif // SML_VALIDATION

	setbuf(stream->obj.file, buffer->obj.buffer);
	return top;
}

// Another function to define how a stream should be buffered.
// int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
static int ke_file_setvbuf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 4, FILE_SETVBUF);
#endif // SML_VALIDATION
	ke1_t *stream, *buffer, *mode, *size;
	size = stack[--top];
	mode = stack[--top];
	buffer = stack[--top];
	stream = stack[--top];
#ifdef _DEBUG
	ke_validate_parameter_vtype(size, KEV_INT, "size", FILE_SETVBUF);
	ke_validate_parameter_vtype(mode, KEV_STR, "mode", FILE_SETVBUF);
	ke_validate_parameter_vtype(buffer, KEV_BUFFER, "buffer_name", FILE_SETVBUF);
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_SETVBUF);
#endif // SML_VALIDATION

	setvbuf(stream->obj.file, buffer->obj.buffer, (int)mode->i, (size_t)size->i);
	return top;
}


// Creates a temporary file in binary update mode(wb + ).
// FILE *tmpfile(void)
static int ke_file_tmpfile(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 0, FILE_TMPFILE);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
	out->obj.file = tmpfile();
	out->vtype = KEV_FILE;
	out->ttype = KET_VAL;
	return top;
}

// Generates and returns a valid temporary filename which does not exist.
// char *tmpnam(char *str)
static int ke_file_tmpnam(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_TMPNAME);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
	char * buf = ke_calloc_memory(sml, MAX_BUF + 1, 1);
	//if (mkstemp(buf) == 0) {
	//	printf("Error: ke_file_tmpnam");
	//}
	if (p->vtype == KEV_STR) {
		ke_free_memory(sml, sml->fields[p->ifield]->obj.s);
	}
	else {
		sml->fields[p->ifield]->vtype = KEV_STR;
	}
	size_t len = strlen(buf);
	sml->fields[p->ifield]->obj.s = ke_malloc_memory(sml,len + 1);
	memcpy(sml->fields[p->ifield]->obj.s, buf, len + 1);
	out->vtype = KEV_STR;
	out->obj.s = buf;
	return top;
}

void* gen_valist(sml_t* sml, size_t n_args, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *q;
	char * m = (char*)ke_calloc_memory(sml,MAX_BUF+1, 1); /* prepare enough memory*/
	void * va = m; /* copies the pointer */
	for (int i = top - (int)n_args + 1; i < top; i++) {
		q = stack[i];
		if (q->vtype == KEV_STR) {
			(*(char**)m) = q->obj.s; /* puts the next value */
			m += sizeof(char*); /* move forward again*/
		}
		else if (q->vtype == KEV_INT) {
			(*(int*)m) = (int)q->i; /* puts the third element */
			m += sizeof(int); /* unneeded, but here for clarity. */
		}
		else if (q->vtype == KEV_REAL) {
			(*(double*)m) = q->r; /* puts the third element */
			m += sizeof(double); /* unneeded, but here for clarity. */
		}
	}
	return va;
}

void gen_freelist(sml_t* sml, size_t n_args, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *q;
	for (int i = top - (int)n_args + 1; i < top; i++) {
		q = stack[i];
		if (q->vtype == KEV_STR && q->tofree == 1) {
			ke_free_memory(sml,q->obj.s);
			q->tofree = 0;
		}
	}
}

void strrepl(char *str, const char *a, const char *b) {
	for (char *cursor = str; (cursor = strstr(cursor, a)) != NULL;) {
		memmove(cursor + strlen(b), cursor + strlen(a), strlen(cursor) - strlen(a) + 1);
		for (int i = 0; b[i] != '\0'; i++)
			cursor[i] = b[i];
		cursor += strlen(b);
	}
}

// OUT HERE

// Sends formatted output to a stream using an argument list.
// int vfprintf(FILE *stream, const char *format, char * arg)
static int ke_file_vfprintf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *stream, *format;
	format = stack[top - tokp->n_args + 1];
	stream = stack[top - tokp->n_args];
#ifdef _DEBUG
	ke_validate_parameter_vtype(format, KEV_STR, "format", FILE_TMPNAME);
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_TMPNAME);
#endif // SML_VALIDATION

	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args-1, top);
		char* buf = ke_calloc_memory(sml,MAX_BUF+1, 1);
		stbsp_vsprintf(buf, format->obj.s, va);
		strrepl(buf, "\\n", "\n");
		fputs(buf, stream->obj.file);
		ke_free_memory(sml,buf);
		ke_free_memory(sml,va);
	}
	else {
		strrepl(format->obj.s, "\\n", "\n");
		fputs(format->obj.s, stream->obj.file);
	}
	gen_freelist(sml, (size_t)tokp->n_args - 1, top);
	return top - tokp->n_args;  
}

static int ke_file_xvfprintf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *buf, *stream, *format;
	format = stack[top - tokp->n_args + 2];
	stream = stack[top - tokp->n_args + 1];
	buf = stack[top - tokp->n_args];
#ifdef _DEBUG
	ke_validate_parameter_vtype(format, KEV_STR, "format", FILE_FPRINTF);
	ke_validate_parameter_vtype(stream, KEV_FILE, "file", FILE_FPRINTF);
	ke_validate_parameter_vtype(buf, KEV_STR, "str_buffer", FILE_FPRINTF);
#endif // SML_VALIDATION
	if (tokp->n_args > 3) {
		char * va = gen_valist(sml, (size_t)tokp->n_args - 2, top);
		stbsp_vsprintf(sml->fields[buf->ifield]->obj.s, format->obj.s, va);
		strrepl(sml->fields[buf->ifield]->obj.s, "\\n", "\n");
		fputs(sml->fields[buf->ifield]->obj.s, stream->obj.file);
		ke_free_memory(sml,va);
	}
	else {
		strrepl(format->obj.s, "\\n", "\n");
		fputs(format->obj.s, stream->obj.file);
	}
	gen_freelist(sml,(size_t)tokp->n_args - 2, top);
	return top - tokp->n_args;
}

// Sends formatted output to stdout using an argument list.
// int vprintf(const char *format, char * arg)
static int ke_file_vprintf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t  *format;
	format = stack[top - tokp->n_args];
#ifdef _DEBUG
	ke_validate_parameter_vtype(format, KEV_STR, "format", FILE_PRINTF);
#endif // SML_VALIDATION
	if (tokp->n_args > 1) {
		char * va = gen_valist(sml,(size_t)tokp->n_args, top);
		char * buf = ke_calloc_memory(sml,MAX_BUF+1, 1);
		stbsp_vsprintf(buf, format->obj.s, va);
		strrepl(buf, "\\n", "\n");
		fputs(buf, stdout);
		ke_free_memory(sml, buf);
		ke_free_memory(sml, va);
		gen_freelist(sml, (size_t)tokp->n_args - 1, top);
	}
	else {
		strrepl(format->obj.s, "\\n", "\n");
		fputs(format->obj.s, stdout);
	}
	gen_freelist(sml, (size_t)tokp->n_args, top);
	return top - tokp->n_args;
}

static int ke_file_xvprintf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t  *buf, *format;
	format = stack[top - tokp->n_args + 1];
	buf = stack[top - tokp->n_args];
#ifdef _DEBUG
	ke_validate_parameter_vtype(format, KEV_STR, "format", FILE_XPRINTF);
	ke_validate_parameter_vtype(buf, KEV_STR, "str_buffer", FILE_XPRINTF);
#endif // SML_VALIDATION

	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args - 1, top);
		stbsp_vsprintf(sml->fields[buf->ifield]->obj.s, format->obj.s, (char *)va);
		strrepl(sml->fields[buf->ifield]->obj.s, "\\n", "\n");
		fputs(sml->fields[buf->ifield]->obj.s, stdout);
		ke_free_memory(sml,va);
		gen_freelist(sml,(size_t)tokp->n_args - 1, top);
	}
	else {
		strrepl(format->obj.s, "\\n", "\n");
		fputs(format->obj.s, stdout);
	}
	gen_freelist(sml, (size_t)tokp->n_args, top);
	return top - tokp->n_args;
}

// Sends formatted output to a string using an argument list.
// int vsprintf(char *str, const char *format, char * arg)
static int ke_file_vsprintf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t  *format, *str;
	format = stack[top - tokp->n_args + 1];
	str = stack[top - tokp->n_args];
#ifdef _DEBUG
	ke_validate_parameter_vtype(format, KEV_STR, "format", FILE_SPRINTF);
#endif // SML_VALIDATION
	char * buf = ke_calloc_memory(sml,MAX_BUF+1, 1);
	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args - 1, top);
		stbsp_vsprintf(buf, format->obj.s, va);
		ke_free_memory(sml,va);
	}
	else {
		memcpy(buf, format->obj.s, strlen(format->obj.s) + 1);
	}
	size_t len = strlen(buf);
	char * str2 = ke_malloc_memory(sml,len + 1);
	memcpy(str2, buf, len + 1);
	ke_free_memory(sml,buf);
	sml->fields[str->ifield]->obj.s = str2;
	sml->fields[str->ifield]->vtype = KEV_STR;
	gen_freelist(sml, (size_t)tokp->n_args, top);
	return top - tokp->n_args;
}

static int ke_file_xvsprintf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t  *buf, *format;
	format = stack[top - tokp->n_args + 1];
	buf = stack[top - tokp->n_args];
#ifdef _DEBUG
	ke_validate_parameter_vtype(format, KEV_STR, "format", FILE_SPRINTF);
	ke_validate_parameter_vtype(buf, KEV_STR, "str_buffer", FILE_SPRINTF);
#endif // SML_VALIDATION
	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args - 1, top);
		stbsp_vsprintf(sml->fields[buf->ifield]->obj.s, format->obj.s, va);
		ke_free_memory(sml,va);
	}
	else {
		memcpy(buf, format->obj.s, strlen(format->obj.s) + 1);
	}
	gen_freelist(sml,(size_t)tokp->n_args - 1, top);
	return top - tokp->n_args;
}

void** gen_scan_valist(sml_t*sml, size_t n_args, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *q;
	void** va = ke_calloc_memory(sml,128, sizeof(void*)); /* prepare enough memory*/
	for (int i = top - (int)n_args + 1, j = 0; i < top; i++, j++) {
		q = stack[i];
		if (q->vtype == KEV_STR) {
			ke_free_memory(sml,sml->fields[q->ifield]->obj.s);
			sml->fields[q->ifield]->obj.s = ke_calloc_memory(sml,MAX_BUF+1, 1);
			va[j] = (char*)sml->fields[q->ifield]->obj.s; /* puts the next value */
		}
		else if (q->vtype == KEV_INT) {
			va[j] = (int64_t *)&(sml->fields[q->ifield]->i);
		}
		else if (q->vtype == KEV_REAL) {
			va[j] = (double *)&(sml->fields[q->ifield]->r);
		}
	}
	return va;
}

void ** gen_xscan_valist(sml_t*sml, size_t n_args, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *q;
	void** va = ke_calloc_memory(sml,100, sizeof(void*)); /* prepare enough memory*/
	for (int i = top - (int)n_args + 1, j = 0; i < top; i++, j++) {
		q = stack[i];
		if (q->vtype == KEV_STR) {
			va[j] = sml->fields[q->ifield]->obj.s; /* puts the next value */
		}
		else if (q->vtype == KEV_INT) {
			va[j] = (int*)&(sml->fields[q->ifield]->i);
		}
		else if (q->vtype == KEV_REAL) {
			va[j] = (double*)&(sml->fields[q->ifield]->r);
		}
	}
	return va;
}

void set_i_r(sml_t*sml, size_t n_args, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *q;
	for (int i = top - (int)n_args + 1; i < top; i++) {
		q = stack[i];
		if (q->vtype == KEV_STR) {
			size_t len = strlen(sml->fields[q->ifield]->obj.s);
			char * tmp = ke_calloc_memory(sml,len + 1,1);
			memcpy(tmp, sml->fields[q->ifield]->obj.s, len + 1);
			ke_free_memory(sml,sml->fields[q->ifield]->obj.s);
			sml->fields[q->ifield]->obj.s = tmp;
		} else if (q->vtype == KEV_INT) {
			sml->fields[q->ifield]->r = (double)sml->fields[q->ifield]->i;
		}
		else if (q->vtype == KEV_REAL) {
			sml->fields[q->ifield]->i = (int64_t)sml->fields[q->ifield]->r;
		}
	}
}

void set_xi_r(sml_t*sml, size_t n_args, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *q;
	for (int i = top - (int)n_args + 1; i < top; i++) {
		q = stack[i];
		if (q->vtype == KEV_INT) {
			sml->fields[q->ifield]->r = (double)sml->fields[q->ifield]->i;
		}
		else if (q->vtype == KEV_REAL) {
			sml->fields[q->ifield]->i = (int64_t)sml->fields[q->ifield]->r;
		}
	}
}

// Sends formatted output to stdout using an argument list.
// int vscanf(char * restrict format, char * arg_ptr); 
static int ke_file_vscanf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t  *format;  
	format = stack[top - tokp->n_args];
	if (tokp->n_args > 1) {
		void **va = gen_scan_valist(sml, (size_t) tokp->n_args, top);
		int count = 0;
		if (tokp->n_args <= MAX_SCAN_ARG + 1) {
		   count = scanf(format->obj.s, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
						  va[8], va[9], va[10], va[11], va[12], va[13], va[14], va[15]);
		} else {
			printf("Error: ke_file_vscanf :  max 16 arguments");
		}
		if (count != tokp->n_args - 1 ) {
			printf("Error: ke_file_xvfscanf");
		}
		ke_free_memory(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_i_r(sml, (size_t)tokp->n_args - 1, top);
	return top - tokp->n_args;
}

static int ke_file_xvscanf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t  *format;
	format = stack[top - tokp->n_args];
	if (tokp->n_args > 1) {
		void** va = gen_xscan_valist(sml,(size_t)tokp->n_args, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 1) {
			if (scanf(format->obj.s, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
					  va[8], va[9], va[10], va[11], va[12], va[13], va[14], va[15]) != tokp->n_args - 1) {
				printf("Error : ke_file_xvscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		ke_free_memory(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_xi_r(sml,(size_t)tokp->n_args, top);
	return top - tokp->n_args;
}

// Sends formatted output to a string using an argument list.
// int vfscanf(FILE * restrict stream, const char * restrict format,char * arg_ptr); 
static int ke_file_vfscanf(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t  *format, *stream;
	format = stack[top - tokp->n_args + 1];
	stream = stack[top - tokp->n_args];
	char name[] = "les amis ratons ";
	if (tokp->n_args > 2) {
		void** va = gen_scan_valist(sml,(size_t)tokp->n_args - 1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			int count = fscanf(stream->obj.file, format->obj.s,
				va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
				va[8],va[9], va[10], va[11], va[12], va[13], va[14], va[15] );
			if (count != tokp->n_args - 2 ) {
				printf("Error: ke_file_vfscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}

		ke_free_memory(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_i_r(sml,(size_t)tokp->n_args - 1, top);
	return top - tokp->n_args;
}

static int ke_file_xvfscanf(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t  *format, *stream;
	format = stack[top - tokp->n_args + 1];
	stream = stack[top - tokp->n_args];
	if (tokp->n_args > 2) {
		void** va = gen_xscan_valist(sml,(size_t)tokp->n_args - 1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			if (fscanf(stream->obj.file, format->obj.s, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
					   va[8],va[9], va[10], va[11], va[12], va[13], va[14], va[15] ) != tokp->n_args - 2) {
				printf("error : ke_file_xvfscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		ke_free_memory(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_xi_r(sml,(size_t)tokp->n_args - 1, top);
	return top - tokp->n_args;
}

// read formatted output to a string using an argument list.
// int sscanf(char * restrict str, const char * restrict format, char* buffer, char * arg_ptr);
static int ke_file_vsscanf(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t  *format, *str;
	format = stack[top - tokp->n_args + 1];
	str = stack[top - tokp->n_args];
	if (tokp->n_args > 2) {
		void** va = gen_scan_valist(sml,(size_t)tokp->n_args-1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			int count = sscanf(str->obj.s, format->obj.s, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
				va[8], va[9], va[10], va[11], va[12], va[13], va[14], va[15]);
			if (count != tokp->n_args - 2) {
				printf("Error: ke_file_vsscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		ke_free_memory(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_i_r(sml,(size_t)tokp->n_args-1, top);
	return top - tokp->n_args;
}

static int ke_file_xvsscanf(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t  *format, *buf;
	format = stack[top - tokp->n_args + 1];
	buf = stack[top - tokp->n_args];
	if (tokp->n_args > 2) {
		void** va = gen_xscan_valist(sml,(size_t)tokp->n_args - 1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			if (sscanf(buf->obj.s, format->obj.s, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
					   va[8],va[9], va[10], va[11], va[12], va[13], va[14], va[15]) != -tokp->n_args-2){
				printf("Error: ke_file_xvsscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		ke_free_memory(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_xi_r(sml,(size_t)tokp->n_args - 1, top);
	return top - tokp->n_args;
}

// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int fgetc(FILE *stream)
static int ke_file_fgetc(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FGETC);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
	out->i = fgetc(p->obj.file);
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->r = (double)out->i;
	return top;
}

// Reads a line from the specified stream and stores it into the string pointed to by str.It stops when either(n - 1) characters are read, the newline character is read, or the end - of - file is reached, whichever comes first.
// char *fgets(char *str, FILE *stream)
// n not used
static int ke_file_fgets(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_FGETS);
#endif // SML_VALIDATION
	ke1_t *str, *stream;
	stream = stack[--top];
	str = stack[--top];

	char * buf = ke_calloc_memory(sml,MAX_BUF+1, 1);
	if (fgets(buf, MAX_BUF, stream->obj.file) == NULL) {
		//printf("Error: ke_file_fgets");
	}
	if (str->vtype == KEV_STR) {
		ke_free_memory(sml, sml->fields[str->ifield]->obj.s);
	}
	else {
		sml->fields[str->ifield]->vtype = KEV_STR;
	}
	size_t len = strlen(buf);
	sml->fields[str->ifield]->obj.s = ke_calloc_memory(sml,len + 1,1);
	memcpy(sml->fields[str->ifield]->obj.s, buf, len + 1);
	return top;
}

// gets(char *buf, int size, FILE)
static int ke_file_xfgets(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 3, FILE_XFGETS);
#endif // SML_VALIDATION
	ke1_t *buf, *size, *file;
	file = stack[--top];
	size = stack[--top];
	buf = stack[--top];
	if ( fgets(sml->fields[buf->ifield]->obj.s, (int)size->i, file->obj.file) == NULL) {
		//printf("Error: ke_file_xfgets");
	}
	return top;
}


// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// int fputc(int char, FILE *stream)
static int ke_file_fputc(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_FPUTS);
#endif // SML_VALIDATION
	ke1_t *c, *stream;
	stream = stack[--top];
	c = stack[--top];
	fputc((char)c->i, stream->obj.file);
	return top;
}

// Writes a string to the specified stream up to but not including the null character.
// int fputs(const char *str, FILE *stream)
static int ke_file_fputs(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_FPUTS);
#endif // SML_VALIDATION
	ke1_t *str, *stream;
	stream = stack[--top];
	str = stack[--top];
	fputs(str->obj.s, stream->obj.file);
	return top;
}

// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int getc(FILE *stream)
static int ke_file_getc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_FGETC);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top ];
	stack[top++] = sml->out; out = sml->out;
	out->i = fgetc(p->obj.file);
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->r = (double)out->i;
	return top;
}

// Gets a character(an unsigned char) from stdin.
// int getchar(void)
static int ke_file_getchar(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_GETCHAR);
#endif // SML_VALIDATION
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
	out->i = getchar();
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	out->r = (double)out->i;
	return top;
}

// Reads a line from stdin and stores it into the string pointed to by, str.It stops when either the newline character is read or when the end - of - file is reached, whichever comes first.
// char *gets(char *str)
static int ke_file_gets(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_GETS);
#endif // SML_VALIDATION
	ke1_t *str;
	str = stack[--top];
	char * buf = ke_calloc_memory(sml,MAX_BUF+1, 1);
	if (fgets(buf, MAX_BUF, stdin) == 0) {
		printf("Error: ke_file_gets");
	}
	if (str->vtype == KEV_STR) {
		ke_free_memory(sml,sml->fields[str->ifield]->obj.s);
	}
	else {
		sml->fields[str->ifield]->vtype = KEV_STR;
	}
	size_t len = strlen(buf);
	sml->fields[str->ifield]->obj.s = ke_calloc_memory(sml, len + 1,1);
	memcpy(sml->fields[str->ifield]->obj.s, buf, len + 1);
	return top;
}

// gets(char *buf, int size)
static int ke_file_xgets(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_XGETS);
#endif // SML_VALIDATION
	ke1_t *buf, *size;
	size = stack[--top];
	buf = stack[--top];
	if ( fgets(sml->fields[buf->ifield]->obj.s, (int)size->i, stdin) == 0) {
		printf("Error ke_file_xgets");
	}
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// 37 int putc(int char, FILE *stream)
static int ke_file_putc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_FPUTC);
#endif // SML_VALIDATION
	ke1_t *c, *stream;
	c = stack[--top];
	stream = stack[--top];
	putc((int)c->i, stream->obj.file);
	return top;
}

// Writes a character(an unsigned char) specified by the argument char to stdout.
// int putchar(int char)
static int ke_file_putchar(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_PUTCHAR);
#endif // SML_VALIDATION
	ke1_t *c;
	c = stack[--top];
	putchar((int)c->i);
	return top;
}

// Writes a string to stdout up to but not including the null character.A newline character is appended to the output.
// int puts(const char *str)
static int ke_file_puts(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_PUTS);
#endif // SML_VALIDATION
	ke1_t *str;
	str = stack[--top];
	puts(str->obj.s);
	return top;
}

// Pushes the character char(an unsigned char) onto the specified stream so that the next character is read.
// int ungetc(int char, FILE *stream)
static int ke_file_ungetc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 2, FILE_UNGETC);
#endif // SML_VALIDATION
	ke1_t *c, *stream;
	stream = stack[--top];
	c = stack[--top];
	ungetc((int)c->i, stream->obj.file);
	return top;
}


// Prints a descriptive error message to stderr.First the string str is printed followed by a colon and then a space.
// void perror(const char *str)
static int ke_file_perror(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
#ifdef _DEBUG
	ke_validate_parameter_qte(tokp, 1, FILE_PERROR);
#endif // SML_VALIDATION
	ke1_t *str;
	str = stack[--top];
	char * buf = ke_calloc_memory(sml,MAX_BUF+1, 1);
	perror(buf);
	if (str->vtype == KEV_STR) {
		ke_free_memory(sml, sml->fields[str->ifield]->obj.s);
	}
	else {
		sml->fields[str->ifield]->vtype = KEV_STR;
	}

	size_t len = strlen(buf);
	sml->fields[str->ifield]->obj.s = ke_calloc_memory(sml,len + 1,1);
	memcpy(sml->fields[str->ifield]->obj.s, buf, len + 1);
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

void ke_file_hash(sml_t* sml) {
	ke_hash_add(sml, (fncp)&ke_file_alloc_buffer, FILE_NEWBUFFER);
	ke_hash_add(sml, (fncp)&ke_file_free_buffer, FILE_FREBUFFER);
	ke_hash_add(sml, (fncp)&ke_file_fclose, FILE_FCLOSE);
	ke_hash_add(sml, (fncp)&ke_file_clearerr, FILE_CLEARERR);
	ke_hash_add(sml, (fncp)&ke_file_feof, FILE_FEOF);
	ke_hash_add(sml, (fncp)&ke_file_ferror, FILE_FERROR);
	ke_hash_add(sml, (fncp)&ke_file_fflush, FILE_FFLUSH);
	ke_hash_add(sml, (fncp)&ke_file_fgetpos, FILE_FGETPOS);
	ke_hash_add(sml, (fncp)&ke_file_fopen, FILE_FOPEN);
	ke_hash_add(sml, (fncp)&ke_file_fread, FILE_FREAD);
	ke_hash_add(sml, (fncp)&ke_file_freopen, FILE_FREOPEN);
	ke_hash_add(sml, (fncp)&ke_file_fseek, FILE_FSEEK);
	ke_hash_add(sml, (fncp)&ke_file_fsetpos, FILE_FSETPOS);
	ke_hash_add(sml, (fncp)&ke_file_ftell, FILE_FTELL);
	ke_hash_add(sml, (fncp)&ke_file_fwrite, FILE_FWRITE);
	ke_hash_add(sml, (fncp)&ke_file_remove, FILE_REMOVE);
	ke_hash_add(sml, (fncp)&ke_file_rename, FILE_RENAME);
	ke_hash_add(sml, (fncp)&ke_file_rewind, FILE_REWIND);
	ke_hash_add(sml, (fncp)&ke_file_setbuf, FILE_SETBUF);
	ke_hash_add(sml, (fncp)&ke_file_setvbuf, FILE_SETVBUF);
	ke_hash_add(sml, (fncp)&ke_file_tmpfile, FILE_TMPFILE);
	ke_hash_add(sml, (fncp)&ke_file_tmpnam, FILE_TMPNAME);

	ke_hash_add(sml, (fncp)&ke_file_vfprintf, FILE_FPRINTF);
	ke_hash_add(sml, (fncp)&ke_file_vsprintf, FILE_SPRINTF);
	ke_hash_add(sml, (fncp)&ke_file_vprintf, FILE_PRINTF);
	ke_hash_add(sml, (fncp)&ke_file_vfscanf, FILE_FSCANF);
	ke_hash_add(sml, (fncp)&ke_file_vscanf, FILE_SCANF);
	ke_hash_add(sml, (fncp)&ke_file_vsscanf, FILE_SSCANF);

	ke_hash_add(sml, (fncp)&ke_file_xvfprintf, FILE_XFPRINTF);
	ke_hash_add(sml, (fncp)&ke_file_xvsprintf, FILE_XSPRINTF);
	ke_hash_add(sml, (fncp)&ke_file_xvprintf, FILE_XPRINTF);
	ke_hash_add(sml, (fncp)&ke_file_xvfscanf, FILE_XFSCANF);
	ke_hash_add(sml, (fncp)&ke_file_xvscanf, FILE_XSCANF);
	ke_hash_add(sml, (fncp)&ke_file_xvsscanf, FILE_XSSCANF);

	ke_hash_add(sml, (fncp)&ke_file_fgetc, FILE_FGETC);
	ke_hash_add(sml, (fncp)&ke_file_fgets, FILE_FGETS);
	ke_hash_add(sml, (fncp)&ke_file_xfgets, FILE_XFGETS);
	ke_hash_add(sml, (fncp)&ke_file_fputc, FILE_FPUTC);
	ke_hash_add(sml, (fncp)&ke_file_fputs, FILE_FPUTS);
	ke_hash_add(sml, (fncp)&ke_file_getc, FILE_GETC);
	ke_hash_add(sml, (fncp)&ke_file_getchar, FILE_GETCHAR);
	ke_hash_add(sml, (fncp)&ke_file_gets, FILE_GETS);
	ke_hash_add(sml, (fncp)&ke_file_xgets, FILE_XGETS);
	ke_hash_add(sml, (fncp)&ke_file_putc, FILE_PUTC);
	ke_hash_add(sml, (fncp)&ke_file_putchar, FILE_PUTCHAR);
	ke_hash_add(sml, (fncp)&ke_file_puts, FILE_PUTS);
	ke_hash_add(sml, (fncp)&ke_file_ungetc, FILE_UNGETC);
	ke_hash_add(sml, (fncp)&ke_file_perror, FILE_PERROR);

}

void ke_file_print(ke1_t *k) {
	printf("File: %d\n", k->icmd);
	printf("%g\n", k->r);
}
