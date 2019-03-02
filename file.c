#define STB_SPRINTF_IMPLEMENTATION
#include "./stb/stb_sprintf.h"
#include "api.h"
#include "file.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#define MAX_BUF 1023
#define MAX_SCAN_ARG 16

const char * valid_mode = ",r,w,a,rb,wb,ab,r+,w+,a+,r+b,w+b,rb+,wb+,a+b,ab+";
const char * cant_open_file = "can't open file for the mode parameter";
const char * cant_close_file = "can't close file";
/* Parameters
	IN
		1 : boolen expression
		2 : message
	OUT
		none
*/
static void ke_file_assert_true(sml_t* sml) {
	sml_assert_args(sml, 2, FILE_ASSERT_TRUE);
	sml_assert_type_int_or_real(sml, 1, FILE_ASSERT_TRUE);
	sml_assert_type(sml, 2, KEV_STR, FILE_ASSERT_TRUE);
	char * s = sml_pop_str(sml);
	int i = sml_pop_int(sml);
	if (i) {
		printf("Success: %s\n", s);
	} else {
		printf("Error: %s\n", s);
	}
}

/* Parameters
IN
1 : boolen expression
2 : message
OUT
none
*/
static void ke_file_assert_false(sml_t* sml) {
	sml_assert_args(sml, 2, FILE_ASSERT_FALSE);
	sml_assert_type_int_or_real(sml, 1, FILE_ASSERT_FALSE);
	sml_assert_type(sml, 2, KEV_STR, FILE_ASSERT_FALSE);
	char * s = sml_pop_str(sml);
	int i = sml_pop_int(sml);
	if (!i) {
		printf("Success: %s\n", s);
	}
	else {
		printf("Error: %s\n", s);
	}
}

/* Parameters
IN
	1 : KEV_INT : buffer len
OUT
	KEV_BUFFER	: allocated buffer
COMMENT
	buffer len is set
*/
static void ke_file_alloc_buffer(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_NEWBUFFER);
	sml_assert_type(sml, 1, KEV_INT, FILE_NEWBUFFER);
	int i = sml_pop_int(sml);
	void * ptr = sml_new_ptr(sml, i);
	sml_push_buffer(sml,ptr,i);
}

/* Parameters
IN
	1 : KEV_BUFFER	: allocated buffer
OUT
	none
COMMENT
	buffer len is set to zero
*/
static void ke_file_free_buffer(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_FREEBUFFER);
	sml_assert_type(sml, 1, KEV_BUFFER, FILE_FREEBUFFER);
	token_t * tokp = sml_pop_token(sml);
	void * ptr = sml_get_ptr(tokp);
	sml_assert_ptr(sml, ptr, 1, FILE_FREEBUFFER);
	sml_free_ptr(sml, ptr);
	sml_set_ptr_null(tokp);
	sml_set_int(tokp, 0);
	sml_set_ptr_null(tokp);
}

/* Parameters
IN
	1 : KEV_BUFFER	: allocated buffer
OUT
	KEV_INT	: buffer len
*/
static void ke_file_buffer_len(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_LENBUFFER);
	sml_assert_type(sml, 1, KEV_BUFFER, FILE_LENBUFFER);
	token_t *tokp = sml_pop_token(sml);
	int i = sml_get_len(tokp);
	sml_push_int(sml, i);
}

/* Parameters
IN
	1 : KEV_STR	: file name
	2 : KEV_STR	: open mode "r", "rw", ect..
		"r" 	ouverture d'un fichier texte en lecture.
		"w" 	ouverture d'un fichier texte en écriture.
		"a" 	ouverture d'un fichier texte en écriture à la fin.
		"rb" 	ouverture d'un fichier binaire en lecture.
		"wb"	ouverture d'un fichier binaire en écriture.
		"ab"	ouverture d'un fichier binaire en écriture à la fin.
		"r+"	ouverture d'un fichier texte en lecture/écriture.
		"w+"	ouverture d'un fichier texte en lecture/écriture.
		"a+"	ouverture d'un fichier texte en lecture/écriture à la fin.
		"r+b" ou "rb+	ouverture d'un fichier binaire en lecture/écriture.
		"w+b" ou "wb+	ouverture d'un fichier binaire en lecture/écriture.
		"a+b" ou "ab+	ouverture d'un fichier binaire en lecture/écriture à la fin.
OUT
	KEV_FILE	: FILE *
*/
static void ke_file_fopen(sml_t* sml) {
	sml_assert_args(sml, 2, FILE_FOPEN);
	sml_assert_type(sml, 1, KEV_STR, FILE_FOPEN);
	sml_assert_str(sml, 1, FILE_FOPEN);
	sml_assert_type(sml, 2, KEV_STR, FILE_FOPEN);
	sml_assert_str(sml, 2, FILE_FOPEN);
	sml_assert_str_in(sml, 2, valid_mode, FILE_FOPEN);
	char * mode = sml_pop_str(sml);
	char * name = sml_pop_str(sml);
	FILE* file = fopen(name, mode);
	if (file == NULL) {
		sml_fatal_error(sml, cant_open_file, FILE_FOPEN);
	}
	sml_push_file(sml, file);
}

/* Parameters
IN
1 : KEV_FILE	: file ptr
OUT
	KEV_INT	:	1 == OPEN, 0 == CLOSE
*/
static void ke_file_fisopen(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_FISOPEN);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FISOPEN);
	token_t * tokp = sml_pop_token(sml);
	sml_push_int(sml, sml_get_int(tokp));
}

/* Parameters
IN
1 : KEV_FILE	: file ptr
OUT
none
*/
static void ke_file_fclose(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_FCLOSE);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FCLOSE);
	token_t * tokp = sml_pop_token(sml);
	FILE* file = sml_get_file(tokp);
	sml_assert_ptr(sml, file, 1, FILE_FCLOSE);
	int st = fclose(file);
	if (st != 0) {
		sml_fatal_error(sml, cant_close_file, FILE_FCLOSE);
	}
	sml_set_ptr_null(tokp);
	sml_set_int(tokp, 0);
}

// Reads data from the given stream into the array pointed to by ptr.
// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
/* Parameters
IN
	1 : KEV_PTR		: buffer, could be a STR or a BUFFER
	2:  KEV_INT		: size
	3:  KEV_INT     : ememb
	4 : KEV_FILE	: file ptr
OUT
	KEV_INT	:	number read

*/
static void ke_file_fread(sml_t* sml) {
	sml_assert_args(sml, 4, FILE_FREAD);

	sml_assert_type(sml, 1, KEV_BUFFER, FILE_FREAD);
	sml_assert_type_int_or_real(sml, 2, FILE_FREAD);
	sml_assert_type_int_or_real(sml, 3, FILE_FREAD);
	sml_assert_type(sml, 4, KEV_FILE, FILE_FREAD);
	
	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 4, FILE_FREAD);

	int nmemb = sml_pop_int(sml);
	sml_assert_size(sml, nmemb, 3, FILE_FREAD);

	int size = sml_pop_int(sml);
	sml_assert_size(sml, size, 2, FILE_FREAD);

	token_t* tokp = sml_pop_token(sml);
	sml_assert_gz(sml, sml_get_int(tokp)-(size*nmemb), FILE_FREAD)
	char * ptr = sml_get_ptr(tokp);
	sml_assert_ptr(sml, ptr, 1, FILE_FREAD);

	int i = fread(ptr, size, nmemb, file);
	sml_push_int(sml, i);
}

// Writes data from the array pointed to by ptr to the given stream.
// size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
/* Parameters
IN
	1 : KEV_PRT		: buffer, could be a STR or a BUFFER
	2:  KEV_INT		: element size
	3:  KEV_INT     : element count
	4 : KEV_FILE	: file ptr
OUT
KEV_INT	:	number read

*/
static void ke_file_fwrite(sml_t* sml) {
	sml_assert_args(sml, 4, FILE_FWRITE);

	sml_assert_type_int_or_real(sml, 2, FILE_FWRITE);
	sml_assert_type_int_or_real(sml, 3, FILE_FWRITE);
	sml_assert_type(sml, 4, KEV_FILE, FILE_FWRITE);
	
	token_t* filep = sml_pop_token(sml);
	sml_assert_size(sml, sml_get_int(filep), 4, FILE_FWRITE);
	FILE * file = sml_get_file(filep);
	sml_assert_ptr(sml, file, 4, FILE_FWRITE);

	int nmemb = sml_pop_int(sml);
	sml_assert_size(sml, nmemb, 3, FILE_FREAD);

	int size = sml_pop_int(sml);
	sml_assert_size(sml, size, 2, FILE_FREAD);

	token_t* tokp = sml_pop_token(sml);
	void * ptr = sml_get_ptr(tokp);
	sml_assert_ptr(sml, ptr, 1, FILE_FWRITE);
	sml_assert_gz(sml, sml_get_int(tokp) - (size*nmemb), FILE_FWRITE)

	int i = fwrite(ptr, size, nmemb, file);
	sml_push_int(sml, i);
}

// Sets the file position of the given stream to the given position.The argument pos is a position given by the function fgetpos.
// int fsetpos(FILE *stream, const fpos_t *pos)
/* Parameters
IN
1 : KEV_FILE	: file *
2 : KEV_FPOS	: fpos *
OUT
	none
*/
static void ke_file_fsetpos(sml_t* sml) {
	sml_assert_args(sml, 2, FILE_FSETPOS);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FSETPOS);
	sml_assert_type(sml, 2, KEV_FPOS, FILE_FSETPOS);

	fpos_t* fposp = sml_pop_fpos(sml);
	//sml_assert_size(sml, sml_peek_int(sml), 2, FILE_FSETPOS);
	sml_assert_ptr(sml, fposp, 2, FILE_FSETPOS);

	token_t* tokp = sml_pop_token(sml);
	FILE * file = sml_get_file(tokp);
	sml_assert_ptr(sml, file, 1, FILE_FSETPOS);
	fsetpos(file, fposp);
}

// Gets the current file position of the stream and writes it to pos.
// int fgetpos(FILE *stream) //, fpos_t *pos)
// JYV: TODO
/* Parameters
IN
	1 : KEV_FILE	: file *
OUT
	KEV_FPOS	: f_pos_t *

*/
static void ke_file_fgetpos(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_FGETPOS);
	FILE* file = sml_pop_file(sml);
	fpos_t * fposp = ke_calloc_memory(sml, sizeof(fpos_t), 1);
	int st = fgetpos(file, fposp);
	if (st != 0) {
		sml_fatal_error(sml, "fgetpos error", FILE_FGETPOS);
	}
	sml_push_fpos(sml, fposp);
}



// Clears the end - of - file and error indicators for the given stream.
// void clearerr(FILE *stream)
static void ke_file_clearerr(sml_t* sml) { 
	FILE* file = sml_pop_file(sml);
	clearerr(file);
}

// Tests the end - of - file indicator for the given stream.
// int feof(FILE *stream)
static void ke_file_feof(sml_t* sml) { 
	FILE* file = sml_pop_file(sml);
	int i = feof(file);
	sml_push_int(sml, i);
}

// Tests the error indicator for the given stream.
// int ferror(FILE *stream)
static void ke_file_ferror(sml_t* sml) { 
	FILE* file = sml_pop_file(sml);
	int i = ferror(file);
	sml_push_int(sml, i);
}

// Flushes the output buffer of a stream.
// int fflush(FILE *stream)
static void ke_file_fflush(sml_t* sml) { 
	FILE* file = sml_pop_file(sml);
	fflush(file);
}



// Associates a new filename with the given open stream and same time closing the old file in stream.
// FILE *freopen(const char *filename, const char *mode, FILE *stream)
static void ke_file_freopen(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	char * mode = sml_pop_str(sml);
	char * filename = sml_pop_str(sml);
	FILE * ptr = freopen(filename, mode, file);
	sml_push_file(sml, file);
}

// Sets the file position of the stream to the given offset.The argument offset signifies the number of bytes to seek from the given whence position.
// int fseek(FILE *stream, long int offset, int whence)
static void ke_file_fseek(sml_t* sml) { 
	int whence = sml_pop_int(sml);
	int offset = sml_pop_int(sml);
	FILE * file = sml_pop_file(sml);
	fseek(file, offset, whence);
}


// Returns the current file position of the given stream.
// long int ftell(FILE *stream)
static void ke_file_ftell(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	int i = ftell(file);
	sml_push_int(sml, i);
}


// Deletes the given filename so that it is no longer accessible.
// int remove(const char *filename)
static void ke_file_remove(sml_t* sml) { 
	char * filename = sml_pop_str(sml);
	remove(filename);
}

// Causes the filename referred to, by old_filename to be changed to new_filename.
// int rename(const char *old_filename, const char *new_filename)
static void ke_file_rename(sml_t* sml) { 
	char * new_filename = sml_pop_str(sml);
	char * old_filename = sml_pop_str(sml);
	rename(old_filename, new_filename);
}

// Sets the file position to the beginning of the file of the given stream.
// void rewind(FILE *stream)
static void ke_file_rewind(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	rewind(file);
}

// Defines how a stream should be buffered.
// void setbuf(FILE *stream, char *buffer)
static void ke_file_setbuf(sml_t* sml) { 
	void * ptr = sml_pop_ptr(sml);
	FILE * file = sml_pop_file(sml);
	setbuf(file, ptr);
}

// Another function to define how a stream should be buffered.
// int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
static void ke_file_setvbuf(sml_t* sml) { 
	size_t size = sml_pop_int(sml);
	int mode = sml_pop_int(sml);
	void * ptr = sml_pop_ptr(sml);
	FILE * file = sml_pop_file(sml);
	setvbuf(file, ptr, mode, size);
}


// Creates a temporary file in binary update mode(wb + ).
// FILE *tmpfile(void)
static void ke_file_tmpfile(sml_t* sml) { 
	FILE * file = tmpfile();
	sml_push_file(sml, file);
}

// Generates and returns a valid temporary filename which does not exist.
// char *tmpnam(char *str)
// JYV TODO
static void ke_file_tmpnam(sml_t* sml) { 
	char * str = sml_new_ptr(sml, MAX_BUF + 1);
	sml_push_str(sml, str);
}

void* gen_valist(sml_t* sml, size_t n_args, int top) {
	token_t **stack = sml_get_stack(sml);
	token_t *q;
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
	token_t **stack = sml_get_stack(sml);
	token_t *q;
	for (int i = top - (int)n_args + 1; i < top; i++) {
		q = stack[i];
		if (q->vtype == KEV_STR && q->tofree == 1) {
			sml_free_ptr(sml,q->obj.s);
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
static void ke_file_vfprintf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml,(tokp->n_args+1));
	FILE * file = sml_peek_file(sml,(top-tokp->n_args));

	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args-1, top);
		char* ptr = sml_new_ptr(sml, MAX_BUF+1);
		stbsp_vsprintf(ptr, format, va);
		strrepl(ptr, "\\n", "\n");
		fputs(ptr, file);
		sml_free_ptr(sml,ptr);
		sml_free_ptr(sml,va);
	}
	else {
		strrepl(format, "\\n", "\n");
		fputs(format, file);
	}
	gen_freelist(sml, (size_t)tokp->n_args - 1, top);
	top = top - tokp->n_args;
	sml_set_top(sml,top);  
}

static void ke_file_xvfprintf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top-tokp->n_args+2));
	FILE * file = sml_peek_file(sml, (top-tokp->n_args+1));
	void * ptr = sml_peek_ptr(sml, (top-tokp->n_args));
	if (tokp->n_args > 3) {
		char * va = gen_valist(sml, (size_t)tokp->n_args - 2, top);
		stbsp_vsprintf(ptr, format, va);
		strrepl(ptr, "\\n", "\n");
		fputs(ptr, file);
		sml_free_ptr(sml,va);
	}
	else {
		strrepl(format, "\\n", "\n");
		fputs(format, file);
	}
	gen_freelist(sml,(size_t)tokp->n_args - 2, top);
	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

// Sends formatted output to stdout using an argument list.
// int vprintf(const char *format, char * arg)
static void ke_file_vprintf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args));
	if (tokp->n_args > 1) {
		char * va = gen_valist(sml,(size_t)tokp->n_args, top);
		char * ptr = sml_new_ptr(sml,MAX_BUF+1);
		stbsp_vsprintf(ptr, format, va);
		strrepl(ptr, "\\n", "\n");
		fputs(ptr, stdout);
		sml_free_ptr(sml, ptr);
		sml_free_ptr(sml, va);
		gen_freelist(sml, (size_t)tokp->n_args - 1, top);
	}
	else {
		strrepl(format, "\\n", "\n");
		fputs(format, stdout);
	}
	gen_freelist(sml, (size_t)tokp->n_args, top);
	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

static void ke_file_xvprintf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	void * ptr = sml_peek_ptr(sml, (top - tokp->n_args));

	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args - 1, top);
		stbsp_vsprintf(ptr, format, (char *)va);
		strrepl(ptr, "\\n", "\n");
		fputs(ptr, stdout);
		sml_free_ptr(sml,va);
		gen_freelist(sml,(size_t)tokp->n_args - 1, top);
	}
	else {
		strrepl(format, "\\n", "\n");
		fputs(format, stdout);
	}
	gen_freelist(sml, (size_t)tokp->n_args, top);
	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

// Sends formatted output to a string using an argument list.
// int vsprintf(char *str, const char *format, char * arg)
static void ke_file_vsprintf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	char * str = sml_peek_str(sml, (top - tokp->n_args));
	char * ptr = sml_new_ptr(sml,MAX_BUF+1);
	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args - 1, top);
		stbsp_vsprintf(ptr, format, va);
		sml_free_ptr(sml,va);
	}
	else {
		memcpy(ptr, format, strlen(format) + 1);
	}
	size_t len = strlen(ptr);
	char * str2 = sml_new_ptr(sml,len + 1);
	memcpy(str2, ptr, len + 1);
	sml_free_ptr(sml,ptr);
	str = str2;
	//str->vtype = KEV_STR;
	gen_freelist(sml, (size_t)tokp->n_args, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

static void ke_file_xvsprintf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	char * ptr = sml_new_ptr(sml, MAX_BUF + 1);
	if (tokp->n_args > 2) {
		char * va = gen_valist(sml,(size_t)tokp->n_args - 1, top);
		stbsp_vsprintf(ptr, format, va);
		sml_free_ptr(sml,va);
	}
	else {
		memcpy(ptr, format, strlen(format) + 1);
	}
	gen_freelist(sml,(size_t)tokp->n_args - 1, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

void** gen_scan_valist(sml_t*sml, size_t n_args, int top) {
	token_t **stack = sml->stack;
	token_t *q;
	void** va = ke_calloc_memory(sml,128, sizeof(void*)); /* prepare enough memory*/
	for (int i = top - (int)n_args + 1, j = 0; i < top; i++, j++) {
		q = stack[i];
		if (q->vtype == KEV_STR) {
			ke_free_memory(sml, q->obj.s);
			q->obj.s = ke_calloc_memory(sml,MAX_BUF+1, 1);
			va[j] = (char*)q->obj.s; /* puts the next value */
		}
		else if (q->vtype == KEV_INT) {
			va[j] = (int64_t *)&(q->i);
		}
		else if (q->vtype == KEV_REAL) {
			va[j] = (double *)&(q->r);
		}
	}
	return va;
}

void ** gen_xscan_valist(sml_t*sml, size_t n_args, int top) {
	token_t **stack = sml->stack;
	token_t *q;
	void** va = ke_calloc_memory(sml,100, sizeof(void*)); /* prepare enough memory*/
	for (int i = top - (int)n_args + 1, j = 0; i < top; i++, j++) {
		q = stack[i];
		if (q->vtype == KEV_STR) {
			va[j] = q->obj.s; /* puts the next value */
		}
		else if (q->vtype == KEV_INT) {
			va[j] = (int*)&(q->i);
		}
		else if (q->vtype == KEV_REAL) {
			va[j] = (double*)&(q->r);
		}
	}
	return va;
}

void set_i_r(sml_t*sml, size_t n_args, int top) {
	token_t **stack = sml->stack;
	token_t *q;
	for (int i = top - (int)n_args + 1; i < top; i++) {
		q = stack[i];
		if (q->vtype == KEV_STR) {
			size_t len = strlen(q->obj.s);
			char * tmp = ke_calloc_memory(sml,len + 1,1);
			memcpy(tmp, q->obj.s, len + 1);
			ke_free_memory(sml,q->obj.s);
			q->obj.s = tmp;
		} else if (q->vtype == KEV_INT) {
			q->r = (double)q->i;
		}
		else if (q->vtype == KEV_REAL) {
			q->i = (int64_t)q->r;
		}
	}
}

void set_xi_r(sml_t*sml, size_t n_args, int top) {
	token_t **stack = sml->stack;
	token_t *q;
	for (int i = top - (int)n_args + 1; i < top; i++) {
		q = stack[i];
		if (q->vtype == KEV_INT) {
			q->r = (double)q->i;
		}
		else if (q->vtype == KEV_REAL) {
			q->i = (int64_t)q->r;
		}
	}
}

// Sends formatted output to stdout using an argument list.
// int vscanf(char * restrict format, char * arg_ptr); 
static void ke_file_vscanf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args));
	if (tokp->n_args > 1) {
		void **va = gen_scan_valist(sml, (size_t) tokp->n_args, top);
		int count = 0;
		if (tokp->n_args <= MAX_SCAN_ARG + 1) {
		   count = scanf(format, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
						  va[8], va[9], va[10], va[11], va[12], va[13], va[14], va[15]);
		} else {
			printf("Error: ke_file_vscanf :  max 16 arguments");
		}
		if (count != tokp->n_args - 1 ) {
			printf("Error: ke_file_xvfscanf");
		}
		sml_free_ptr(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_i_r(sml, (size_t)tokp->n_args - 1, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

static void ke_file_xvscanf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args));
	if (tokp->n_args > 1) {
		void** va = gen_xscan_valist(sml,(size_t)tokp->n_args, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 1) {
			if (scanf(format, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
					  va[8], va[9], va[10], va[11], va[12], va[13], va[14], va[15]) != tokp->n_args - 1) {
				printf("Error : ke_file_xvscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		sml_free_ptr(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_xi_r(sml,(size_t)tokp->n_args, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

// Sends formatted output to a string using an argument list.
// int vfscanf(FILE * restrict stream, const char * restrict format,char * arg_ptr); 
static void ke_file_vfscanf(sml_t* sml) { 
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	FILE * file = sml_peek_file(sml, (top - tokp->n_args));
	if (tokp->n_args > 2) {
		void** va = gen_scan_valist(sml,(size_t)tokp->n_args - 1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			int count = fscanf(file, format,
				va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
				va[8],va[9], va[10], va[11], va[12], va[13], va[14], va[15] );
			if (count != tokp->n_args - 2 ) {
				printf("Error: ke_file_vfscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}

		sml_free_ptr(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_i_r(sml,(size_t)tokp->n_args - 1, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

static void ke_file_xvfscanf(sml_t* sml) {
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	FILE * file = sml_peek_file(sml, (top - tokp->n_args));

	if (tokp->n_args > 2) {
		void** va = gen_xscan_valist(sml,(size_t)tokp->n_args - 1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			if (fscanf(file, format, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
					   va[8],va[9], va[10], va[11], va[12], va[13], va[14], va[15] ) != tokp->n_args - 2) {
				printf("error : ke_file_xvfscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		sml_free_ptr(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_xi_r(sml,(size_t)tokp->n_args - 1, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

// read formatted output to a string using an argument list.
// int sscanf(char * restrict str, const char * restrict format, char* buffer, char * arg_ptr);
static void ke_file_vsscanf(sml_t* sml) {
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	char * str = sml_peek_str(sml, (top - tokp->n_args));

	if (tokp->n_args > 2) {
		void** va = gen_scan_valist(sml,(size_t)tokp->n_args-1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			int count = sscanf(str, format, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
				va[8], va[9], va[10], va[11], va[12], va[13], va[14], va[15]);
			if (count != tokp->n_args - 2) {
				printf("Error: ke_file_vsscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		sml_free_ptr(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_i_r(sml,(size_t)tokp->n_args-1, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

static void ke_file_xvsscanf(sml_t* sml) { token_t **stack = sml->stack;
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	char * ptr = sml_peek_ptr(sml, (top - tokp->n_args));
	if (tokp->n_args > 2) {
		void** va = gen_xscan_valist(sml,(size_t)tokp->n_args - 1, top);
		if (tokp->n_args <= MAX_SCAN_ARG + 2) {
			if (sscanf(ptr, format, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
					   va[8],va[9], va[10], va[11], va[12], va[13], va[14], va[15]) != -tokp->n_args-2){
				printf("Error: ke_file_xvsscanf");
			}
		} else {
			printf("Error: ke_file_xvscanf :  max 16 arguments");
		}
		sml_free_ptr(sml,va);
	}
	else {
		printf("error of parameter sscanf");
	}
	set_xi_r(sml,(size_t)tokp->n_args - 1, top);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int fgetc(FILE *stream)
static void ke_file_fgetc(sml_t* sml) { token_t **stack = sml->stack;
	FILE * file = sml_pop_file(sml);
	int i = fgetc(file);
	sml_push_int(sml, i);
}

// Reads a line from the specified stream and stores it into the string pointed to by str.It stops when either(n - 1) characters are read, the newline character is read, or the end - of - file is reached, whichever comes first.
// char *fgets(char *str, FILE *stream)
// n not used
static void ke_file_fgets(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	token_t * token = sml_pop_token(sml);
	char ** str = sml_adr_str(token);
	char * ptr = sml_new_ptr(sml,MAX_BUF+1);
	if (fgets(ptr, MAX_BUF, file) == NULL) {
		printf("Error: ke_file_fgets");
	}
	if (token->vtype == KEV_STR) {
		sml_free_ptr(sml, *str);
	}
	else {
		token->vtype = KEV_STR;
	}
	size_t len = strlen(ptr);
	*str = sml_new_ptr(sml,len + 1);
	memcpy(*str, ptr, len + 1);
}

// gets(char *buf, int size, FILE)
static void ke_file_xfgets(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	int size = sml_pop_int(sml);
	token_t* token = sml_pop_token(sml);
	char ** str = sml_adr_str(token);
	if ( fgets(*str, size, file) == NULL) {
		printf("Error: ke_file_xfgets");
	}
}


// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// int fputc(int char, FILE *stream)
static void ke_file_fputc(sml_t* sml) {
	FILE * file = sml_pop_file(sml);
	int c = sml_pop_int(sml);
	fputc(c, file);
}

// Writes a string to the specified stream up to but not including the null character.
// int fputs(const char *str, FILE *stream)
static void ke_file_fputs(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	char * str = sml_pop_str(sml);
	fputs(str, file);
}

// Gets the next character(an unsigned char) from the specified stream and advances the position indicator for the stream.
// int getc(FILE *stream)
static void ke_file_getc(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	int i = fgetc(file);
	sml_push_int(sml, i);
}

// Gets a character(an unsigned char) from stdin.
// int getchar(void)
static void ke_file_getchar(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	int i = getchar();
	sml_push_int(sml, i);
}

// Reads a line from stdin and stores it into the string pointed to by, str.It stops when either the newline character is read or when the end - of - file is reached, whichever comes first.
// char *gets(char *str)
static void ke_file_gets(sml_t* sml) { 
	token_t* token = sml_pop_token(sml);
	char ** str = sml_adr_str(token);

	char * ptr = sml_new_ptr(sml,MAX_BUF+1);
	if (fgets(ptr, MAX_BUF, stdin) == 0) {
		printf("Error: ke_file_gets");
	}
	if (token->vtype == KEV_STR) {
		sml_free_ptr(sml,*str);
	}
	else {
		token->vtype = KEV_STR;
	}
	size_t len = strlen(ptr);
	*str = sml_new_ptr(sml, len + 1);
	memcpy(*str, ptr, len + 1);
}

// gets(char *buf, int size)
static void ke_file_xgets(sml_t* sml) { 
	int size = sml_pop_int(sml);
	token_t * token = sml_pop_token(sml);
	char ** str = sml_adr_str(token);
	if ( fgets(*str, (int)size, stdin) == 0) {
		printf("Error ke_file_xgets");
	}
}

// Writes a character(an unsigned char) specified by the argument char to the specified stream and advances the position indicator for the stream.
// 37 int putc(int char, FILE *stream)
static void ke_file_putc(sml_t* sml) { 
	int c = sml_pop_int(sml);
	FILE * file = sml_pop_file(sml);
	putc(c, file);
}

// Writes a character(an unsigned char) specified by the argument char to stdout.
// int putchar(int char)
static void ke_file_putchar(sml_t* sml) { 
	int c = sml_pop_int(sml);
	putchar(c);
}

// Writes a string to stdout up to but not including the null character.A newline character is appended to the output.
// int puts(const char *str)
static void ke_file_puts(sml_t* sml) { 
	char * str = sml_pop_str(sml);
	puts(str);
}

// Pushes the character char(an unsigned char) onto the specified stream so that the next character is read.
// int ungetc(int char, FILE *stream)
static void ke_file_ungetc(sml_t* sml) { 
	FILE * file = sml_pop_file(sml);
	int c = sml_pop_int(sml);
	ungetc(c, file);
}


// Prints a descriptive error message to stderr.First the string str is printed followed by a colon and then a space.
// void perror(const char *str)
static void ke_file_perror(sml_t* sml) { 
	token_t* token = sml_pop_token(sml);
	char ** str = sml_adr_str(token);
	char * ptr = sml_new_ptr(sml,MAX_BUF+1);
	perror(ptr);
	if (token->vtype == KEV_STR) {
		sml_free_ptr(sml, *str);
	}
	else {
		token->vtype = KEV_STR;
	}

	size_t len = strlen(ptr);
	*str = sml_new_ptr(sml,len + 1);
	memcpy(*str, ptr, len + 1);
}


void ke_file_hash(sml_t* sml) {
	ke_hash_add(sml, (fncp)&ke_file_assert_true, FILE_ASSERT_TRUE);
	ke_hash_add(sml, (fncp)&ke_file_assert_false, FILE_ASSERT_FALSE);

	ke_hash_add(sml, (fncp)&ke_file_alloc_buffer, FILE_NEWBUFFER);
	ke_hash_add(sml, (fncp)&ke_file_free_buffer, FILE_FREEBUFFER);
	ke_hash_add(sml, (fncp)&ke_file_buffer_len, FILE_LENBUFFER);

	ke_hash_add(sml, (fncp)&ke_file_fclose, FILE_FCLOSE);
	ke_hash_add(sml, (fncp)&ke_file_clearerr, FILE_CLEARERR);
	ke_hash_add(sml, (fncp)&ke_file_feof, FILE_FEOF);
	ke_hash_add(sml, (fncp)&ke_file_ferror, FILE_FERROR);
	ke_hash_add(sml, (fncp)&ke_file_fflush, FILE_FFLUSH);
	ke_hash_add(sml, (fncp)&ke_file_fgetpos, FILE_FGETPOS);
	ke_hash_add(sml, (fncp)&ke_file_fisopen, FILE_FISOPEN);
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

void ke_file_print(token_t *k) {
	printf("File: %d\n", k->icmd);
	printf("%g\n", k->r);
}
