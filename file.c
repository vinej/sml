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
const char * cant_seek_file = "can't seek file";
const char * cant_setpos_file = "can't setpos file";
const char * cant_flush_file = "can't flush file";
const char * cant_remove_file = "can't remove file";
const char * cant_rename_file = "can't rename file";
const char * cant_tmp_file = "can't temp file";
const char * cant_sprintf = "can't sprintf";
const char * cant_fgets = "can't fgets";

// TESTED
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

// TESTED
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


static void ke_command_print_nonl(sml_t* sml) {
	token_t * tokp = sml_get_tokp(sml);
	int ntmp = sml_get_args(sml);
	int n = ntmp;
	--n;
	sml_set_args(sml, n);
	token_t *p = sml_pop_token(sml);
	if (n) {
		ke_command_print_nonl(sml);
	}
	ke_print_one(sml, p);
	sml_set_args(sml, ntmp);
}

static void ke_command_print(sml_t* sml) {
	int top = sml_get_top(sml);
	int n = sml_get_args(sml);

	ke_command_print_nonl(sml);
	printf("\n");

	top -= n;
	sml_set_top(sml, top);
}

// TESTED
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
	int i = sml_pop_int(sml) + 1;  // add  an extra 0 byte at the end
	void * ptr = sml_new_ptr(sml, i);
	sml_push_buffer(sml,ptr,i);
}

// TESTED
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

// TESTED
/* Parameters
IN
	1 : KEV_BUFFER	: allocated buffer
OUT
	KEV_INT	: buffer len
*/
static void ke_file_buffer_len(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_LENBUFFER);
	sml_assert_type(sml, 1, KEV_BUFFER, FILE_LENBUFFER);
	int i = sml_pop_int(sml);
	if (i > 0) --i; // remove the extra byte at the end
	sml_push_int(sml, i);
}

// TESTED
/* Parameters
IN
1 : KEV_BUFFER	: allocated buffer
OUT
	none
*/
static void ke_file_buffer_clr(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_CLRBUFFER);
	sml_assert_type(sml, 1, KEV_BUFFER, FILE_CLRBUFFER);
	token_t *tokp = sml_pop_token(sml);
	int i = sml_get_len(tokp);
	void * buffer = sml_get_buffer(tokp);
	sml_assert_ptr(sml, buffer, 1, FILE_CLRBUFFER);
	memset(buffer, 0, i);
}

// TESTED
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
	sml_assert_size(sml, strlen(name), 1, FILE_FREOPEN);
	FILE* file = fopen(name, mode);
	if (file == NULL) {
		sml_fatal_error(sml, cant_open_file, FILE_FOPEN);
	}
	sml_push_file(sml, file);
}

// TESTED
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

// TESTED
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

// TESTED
// Reads data from the given stream into the array pointed to by ptr.
// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
/* Parameters
IN
	1 : KEV_PTR		: buffer, could be a STR or a BUFFER
	2:  KEV_INT		: size
	3:  KEV_INT     : nmemb
	4 : KEV_FILE	: file ptr
OUT
	none

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
	if (i != nmemb) {
		sml_fatal_error(sml, "fread_mismatch", FILE_FWRITE);
	}
}

// TESTED
// Writes data from the array pointed to by ptr to the given stream.
// size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
/* Parameters
IN
	1 : KEV_PRT		: buffer, could be a STR or a BUFFER
	2:  KEV_INT		: element size
	3:  KEV_INT     : element count
	4 : KEV_FILE	: file ptr
OUT
	none
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
	if (i != nmemb) {
		sml_fatal_error(sml, "write_mismatch", FILE_FWRITE);
	}
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
	int st = fsetpos(file, fposp);
	if (st != 0) {
		sml_fatal_error(sml, cant_setpos_file, FILE_FSEEK);
	}
}

// TESTED
// Sets the file position of the stream to the given offset.The argument offset signifies the number of bytes to seek from the given whence position.
// int fseek(FILE *stream, long int offset, int whence)
/* Parameters
IN
	1 : KEV_FILE:	FILE *
	2 : KEV_INT	:	offset :  from the next param
	3 : KEV_INT	:   whence :  1=SEEK_SET Beginning of file, 2=SEEK_CUR Current position of the file pointer, 3=SEEK_END
	OUT
none
*/
static void ke_file_fseek(sml_t* sml) {
	sml_assert_args(sml, 3, FILE_FSEEK);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FSEEK);
	sml_assert_type_int_or_real(sml, 2, FILE_FSEEK);
	sml_assert_type_int_or_real(sml, 3, FILE_FSEEK);
	
	int whence = sml_pop_int(sml);
	sml_assert_range(sml, 1, whence, SEEK_SET, SEEK_END, FILE_FSEEK);

	int offset = sml_pop_int(sml);
	sml_assert_size(sml, offset+1, 1, FILE_FSEEK);

	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_FSEEK);
	int st = fseek(file, offset, whence);
	if (st != 0) {
		sml_fatal_error(sml, cant_seek_file, FILE_FSEEK);
	}
}

// TESTED
// Returns the current file position of the given stream.
// long int ftell(FILE *stream)
/* Parameters
IN
	1 : KEV_FILE:	FILE *
OUT
	none
*/
static void ke_file_ftell(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_FSEEK);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FSEEK);
	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_FSEEK);
	int i = ftell(file);
	sml_push_int(sml, i);
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


// TESTED
// Clears the end - of - file and error indicators for the given stream.
// void clearerr(FILE *stream)
/* Parameters
IN
	1 : KEV_FILE:	FILE *
OUT
	none
*/
static void ke_file_clearerr(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_CLEARERR);
	sml_assert_type(sml, 1, KEV_FILE, FILE_CLEARERR);
	FILE* file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_CLEARERR);
	clearerr(file);
}

// TESTED
// Tests the end - of - file indicator for the given stream.
// int feof(FILE *stream)
/* Parameters
IN
	1 : KEV_FILE:	FILE *
OUT
	none
*/
static void ke_file_feof(sml_t* sml) { 
	sml_assert_args(sml, 1, FILE_FEOF);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FEOF);
	FILE* file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_FEOF);
	int i = feof(file);
	sml_push_int(sml, i);
}

// TESTED
// Tests the error indicator for the given stream.
// int ferror(FILE *stream)
/* Parameters
IN
	1 : KEV_FILE:	FILE *
OUT
	none
*/

static void ke_file_ferror(sml_t* sml) { 
	sml_assert_args(sml, 1, FILE_FERROR);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FERROR);
	FILE* file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_FERROR);
	int i = ferror(file);
	sml_push_int(sml, i);
}

// TESTED
// Flushes the output buffer of a stream.
// int fflush(FILE *stream)
/* Parameters
IN
	1 : KEV_FILE:	FILE *
OUT
	none
*/
static void ke_file_fflush(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_FFLUSH);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FFLUSH);
	FILE* file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_FFLUSH);
	int st = fflush(file);
	if (st != 0) {
		sml_fatal_error(sml, cant_flush_file, FILE_FFLUSH);
	}
}



// Associates a new filename with the given open stream and same time closing the old file in stream.
// FILE *freopen(const char *filename, const char *mode, FILE *stream)
/* Parameters
IN
	1 : KEV_STR	:	filename
	2 : KEV_STR	:	mode
	3 : KEV_FILE:   FILE *
OUT
	KEV_FILE:   FILE *
*/
static void ke_file_freopen(sml_t* sml) { 
	sml_assert_args(sml, 3, FILE_FREOPEN);
	sml_assert_type(sml, 1, KEV_STR, FILE_FREOPEN);
	sml_assert_type(sml, 2, KEV_STR, FILE_FREOPEN);
	sml_assert_type(sml, 3, KEV_FILE, FILE_FREOPEN);

	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 3, FILE_FREOPEN);

	char * mode = sml_pop_str(sml);
	sml_assert_str_in(sml, 2, valid_mode, FILE_FREOPEN);

	char * filename = sml_pop_str(sml);
	sml_assert_size(sml, strlen(filename), 1, FILE_FREOPEN);

	FILE * ptr = freopen(filename, mode, file);
	if (ptr == NULL) {
		sml_fatal_error(sml, cant_open_file, FILE_FREOPEN);
	}
	sml_push_file(sml, ptr);
}

// TESTED
// Deletes the given filename so that it is no longer accessible.
// int exist(const char *filename)
/* Parameters
IN
	1 : KEV_STR	:	filename
OUT
	KEV_INT:   0 false, 1 true
*/
static void ke_file_exist(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_FEXIST);
	sml_assert_type(sml, 1, KEV_STR, FILE_FEXIST);
	sml_assert_str(sml, 1, FILE_FEXIST);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "r");
	if (f) {
		fclose(f);
		sml_push_int(sml, 1);
	}
	else {
		sml_push_int(sml, 0);
	}
}

// TESTED
// Deletes the given filename so that it is no longer accessible.
// int remove(const char *filename)
/* Parameters
IN
	1 : KEV_STR	:	filename
OUT
	none
*/
static void ke_file_remove(sml_t* sml) { 
	sml_assert_args(sml, 1, FILE_REMOVE);
	sml_assert_type(sml, 1, KEV_STR, FILE_REMOVE);
	sml_assert_str(sml, 1, FILE_REMOVE);
	char * filename = sml_pop_str(sml);

	int st = remove(filename);
	if (st != 0) {
		sml_fatal_error(sml, cant_remove_file, FILE_REMOVE);
	}
}

// TESTED
// Causes the filename referred to, by old_filename to be changed to new_filename.
// int rename(const char *old_filename, const char *new_filename)
/* Parameters
IN
	1 : KEV_STR	:	filename
	2 : KEV_STR	:	filename
OUT
	none
*/
static void ke_file_rename(sml_t* sml) {
	sml_assert_args(sml, 2, FILE_REMOVE);
	sml_assert_type(sml, 1, KEV_STR, FILE_REMOVE);
	sml_assert_str(sml, 1, FILE_REMOVE);
	sml_assert_type(sml, 2, KEV_STR, FILE_REMOVE);
	sml_assert_str(sml, 2, FILE_REMOVE);
	char * new_filename = sml_pop_str(sml);
	char * old_filename = sml_pop_str(sml);
	int st = rename(old_filename, new_filename);
	if (st != 0) {
		sml_fatal_error(sml, cant_rename_file, FILE_REMOVE);
	}
}

// TESTED
// Sets the file position to the beginning of the file of the given stream.
// void rewind(FILE *stream)
/* Parameters
IN
	1 : KEV_FILE	:	file *
OUT
	none
*/
static void ke_file_rewind(sml_t* sml) {
	sml_assert_args(sml, 1, FILE_REWIND);
	sml_assert_type(sml, 1, KEV_FILE, FILE_REWIND);
	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_REWIND)
	rewind(file);
}

// Defines how a stream should be buffered.
// void setbuf(FILE *stream, char *buffer)
static void ke_file_setbuf(sml_t* sml) { 
	sml_assert_args(sml, 2, FILE_SETBUF);
	sml_assert_type(sml, 1, KEV_FILE, FILE_SETBUF);
	sml_assert_type(sml, 1, KEV_BUFFER, FILE_SETBUF);
	
	void * ptr = sml_pop_ptr(sml);
	sml_assert_ptr(sml, ptr, 2, FILE_SETBUF)

	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_SETBUF)

	setbuf(file, ptr);
}

// Another function to define how a stream should be buffered.
// int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
static void ke_file_setvbuf(sml_t* sml) { 
	sml_assert_args(sml, 4, FILE_SETVBUF);
	sml_assert_type(sml, 1, KEV_FILE, FILE_SETVBUF);
	sml_assert_type(sml, 2, KEV_BUFFER, FILE_SETVBUF);
	sml_assert_type_int_or_real(sml, 3, FILE_SETVBUF);
	sml_assert_type_int_or_real(sml, 4, FILE_SETVBUF);
	size_t size = sml_pop_int(sml);
	sml_assert_size(sml, size, 4, FILE_SETVBUF);
	int mode = sml_pop_int(sml);
	sml_assert_size(sml, size, 3, FILE_SETVBUF);
	void * ptr = sml_pop_ptr(sml);
	sml_assert_ptr(sml, ptr, 2, FILE_SETVBUF);
	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, ptr, 1, FILE_SETVBUF);

	setvbuf(file, ptr, mode, size);
}

// TESTED
// Creates a temporary file in binary update mode(wb + ).
// FILE *tmpfile(void)
/* Parameters
IN
	none
OUT
	KEV_FILE	: file *
*/
static void ke_file_tmpfile(sml_t* sml) { 
	sml_assert_args(sml, 0, FILE_TMPFILE);
	FILE * file = tmpfile();
	if (file == NULL) {
		sml_fatal_error(sml, cant_tmp_file, FILE_TMPFILE);
	}
	sml_push_file(sml, file);
}

// Generates and returns a valid temporary filename which does not exist.
// char *tmpnam(char *str)
/* Parameters
IN
	none
OUT
	KEV_STR		:	unique file name
*/
static void ke_file_tmpnam(sml_t* sml) {
	sml_assert_args(sml, 0, FILE_TMPNAME);
	char * buffer = ke_calloc_memory(sml, 261, 1);
	buffer = tmpnam(NULL);
	sml_push_str(sml,buffer);
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

// TESTED
/* Parameters
IN
	KEV_FILE	:	file *
	KEV_STR		:	format
	...			:	variable number of arguments
OUT
	none
*/
static void ke_file_fprintf(sml_t* sml) { 
	sml_assert_args_min(sml, 2, FILE_FPRINTF);
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);

	sml_assert_type(sml, 1, KEV_FILE, FILE_FPRINTF);
	sml_assert_type(sml, 2, KEV_STR, FILE_FPRINTF);

	char * format = sml_peek_str(sml,(top-tokp->n_args+1));
	sml_assert_str(sml, 2, FILE_FPRINTF);

	FILE * file = sml_peek_file(sml,(top-tokp->n_args));
	sml_assert_ptr(sml, file, 1, FILE_FPRINTF)

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

// TESTED
/* Parameters
IN
	KEV_BUFFER	:	void * buffer
	KEV_STR		:	format
	...			:	variable number of arguments
OUT
	none
*/
static void ke_file_sprintf(sml_t* sml) { 
	sml_assert_args_min(sml, 2, FILE_SPRINTF);

	sml_assert_type(sml, 1, KEV_BUFFER, FILE_SPRINTF);
	sml_assert_type(sml, 2, KEV_STR, FILE_SPRINTF);

	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	
	char * format = sml_peek_str(sml, (top-tokp->n_args+1));
	sml_assert_str(sml, 2, FILE_SPRINTF);

	void * ptr = sml_peek_ptr(sml, (top-tokp->n_args));
	sml_assert_ptr(sml, ptr, 1, FILE_SPRINTF);

	if (tokp->n_args > 2) {
		char * va = gen_valist(sml, (size_t)tokp->n_args - 1, top);
		int cnt = stbsp_vsprintf(ptr, format, va);
		if (cnt <= 0) {
			sml_fatal_error(sml, cant_sprintf, FILE_SPRINTF)
		}
		strrepl(ptr, "\\n", "\n");
		sml_free_ptr(sml,va);
	}
	else {
		strrepl(format, "\\n", "\n");
		memcpy(ptr, format, strlen(format));
	}
	gen_freelist(sml,(size_t)tokp->n_args - 2, top);
	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

// TESTED
/* Parameters
IN
	KEV_STR		:	format
...			:	variable number of arguments
OUT
	none
*/
static void ke_file_printf(sml_t* sml) { 
	sml_assert_args_min(sml, 1, FILE_PRINTF);
	sml_assert_type(sml, 1, KEV_STR, FILE_PRINTF);

	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);

	char * format = sml_peek_str(sml, (top - tokp->n_args));
	//sml_assert_str(sml, 1, FILE_PRINTF);

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

// TESTED
/* Parameters
IN
	KEV_STR		:	format   (with double lf, lg, 
	...			:	variable number of arguments for output
OUT
	none
*/
static void ke_file_scanf(sml_t* sml) { 
	sml_assert_args_min(sml, 2, FILE_SCANF);
	sml_assert_type(sml, 1, KEV_STR, FILE_SCANF);

	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
	char * format = sml_peek_str(sml, (top - tokp->n_args));
	sml_assert_ptr(sml, format, 1, FILE_SCANF);

	void **va = gen_scan_valist(sml, (size_t) tokp->n_args, top);
	int count = 0;
	if (tokp->n_args <= MAX_SCAN_ARG + 1) {
		count = scanf(format, va[0], va[1], va[2], va[3], va[4], va[5], va[6], va[7],
						va[8], va[9], va[10], va[11], va[12], va[13], va[14], va[15]);
	} else {
		printf("Error: ke_file_vscanf :  max 16 arguments");
	}
	if (count != tokp->n_args - 1 ) {
		printf("Error: ke_file_scanf");
	}

	set_i_r(sml, (size_t)tokp->n_args, top);
	sml_free_ptr(sml, va);

	top = top - tokp->n_args;
	sml_set_top(sml, top);
}

// TESTED
/* Parameters
IN
	KEV_FILE	:	file *
	KEV_STR		:	format   (with double lf, lg,
	...			:	variable number of arguments for output
OUT
none
*/
static void ke_file_fscanf(sml_t* sml) { 
	sml_assert_args_min(sml, 3, FILE_FSCANF);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FSCANF);
	sml_assert_type(sml, 2, KEV_STR, FILE_FSCANF);

	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);

	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	sml_assert_str(sml, 2, FILE_FSCANF);

	FILE * file = sml_peek_file(sml, (top - tokp->n_args));
	sml_assert_ptr(sml, file, 1, FILE_FSCANF);

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

// TESTED
/* Parameters
IN
	KEV_FILE	:	source str
	KEV_STR		:	format   (with double lf, lg,
	...			:	variable number of arguments for output
OUT
	none
*/
static void ke_file_sscanf(sml_t* sml) {
	sml_assert_args_min(sml, 3, FILE_SSCANF);
	//sml_assert_type(sml, 1, KEV_BUFFER, FILE_SSCANF);
	sml_assert_type(sml, 2, KEV_STR, FILE_SSCANF);

	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);

	char * format = sml_peek_str(sml, (top - tokp->n_args + 1));
	sml_assert_str(sml, 1, FILE_SSCANF);

	char * str = sml_peek_str(sml, (top - tokp->n_args));
	sml_assert_str(sml, 2, FILE_SSCANF);

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

// TESTED
/* Parameters
IN
	KEV_FILE	:	source str
OUT
	KEV_INT		:	ascii number of the char
*/
static void ke_file_fgetc(sml_t* sml) { token_t **stack = sml->stack;
	sml_assert_args(sml, 1, FILE_FGETC);
	sml_assert_type(sml, 1, KEV_FILE, FILE_FGETC);

	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_FGETC);

	int i = fgetc(file);
	sml_push_int(sml, i);
}

/* Parameters
IN
	KEV_STR		:	str output
	KEV_FILE	:	FILE *
OUT
	none
*/
static void ke_file_fgets(sml_t* sml) {
	sml_assert_args(sml, 2, FILE_FGETS);
	sml_assert_type(sml, 1, KEV_STR, FILE_FGETS);
	sml_assert_type(sml, 2, KEV_FILE, FILE_FGETS);

	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 1, FILE_FGETS);

	token_t * token = sml_pop_token(sml);
	char ** str = sml_adr_str(token);
	char * ptr = sml_new_ptr(sml,MAX_BUF+1);
	if (fgets(ptr, MAX_BUF, file) == NULL) {
		//sml_fatal_error(sml,cant_fgets, FILE_FGETS);
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
	sml_free_ptr(sml, ptr);
}

// gets(char *buf, int size, FILE)
static void ke_file_xfgets(sml_t* sml) { 
	sml_assert_args(sml, 3, FILE_XFGETS);
	sml_assert_type(sml, 1, KEV_BUFFER, FILE_XFGETS);
	sml_assert_type(sml, 2, KEV_INT, FILE_XFGETS);
	sml_assert_type(sml, 3, KEV_FILE, FILE_XFGETS);
	
	FILE * file = sml_pop_file(sml);
	sml_assert_ptr(sml, file, 3, FILE_XFGETS);

	int size = sml_pop_int(sml);
	sml_assert_size(sml, size, 2, FILE_XFGETS);

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
	ke_hash_add(sml, (fncp)&ke_command_print, FILE_PRINT);

	ke_hash_add(sml, (fncp)&ke_file_assert_true, FILE_ASSERT_TRUE);
	ke_hash_add(sml, (fncp)&ke_file_assert_false, FILE_ASSERT_FALSE);

	ke_hash_add(sml, (fncp)&ke_file_alloc_buffer, FILE_NEWBUFFER);
	ke_hash_add(sml, (fncp)&ke_file_free_buffer, FILE_FREEBUFFER);
	ke_hash_add(sml, (fncp)&ke_file_buffer_len, FILE_LENBUFFER);
	ke_hash_add(sml, (fncp)&ke_file_buffer_clr, FILE_CLRBUFFER);

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
	ke_hash_add(sml, (fncp)&ke_file_exist, FILE_FEXIST);
	ke_hash_add(sml, (fncp)&ke_file_remove, FILE_REMOVE);
	ke_hash_add(sml, (fncp)&ke_file_rename, FILE_RENAME);
	ke_hash_add(sml, (fncp)&ke_file_rewind, FILE_REWIND);
	ke_hash_add(sml, (fncp)&ke_file_setbuf, FILE_SETBUF);
	ke_hash_add(sml, (fncp)&ke_file_setvbuf, FILE_SETVBUF);
	ke_hash_add(sml, (fncp)&ke_file_tmpfile, FILE_TMPFILE);
	ke_hash_add(sml, (fncp)&ke_file_tmpnam, FILE_TMPNAME);

	ke_hash_add(sml, (fncp)&ke_file_printf, FILE_PRINTF);
	ke_hash_add(sml, (fncp)&ke_file_sprintf, FILE_SPRINTF);
	ke_hash_add(sml, (fncp)&ke_file_fprintf, FILE_FPRINTF);

	ke_hash_add(sml, (fncp)&ke_file_scanf, FILE_SCANF);
	ke_hash_add(sml, (fncp)&ke_file_sscanf, FILE_SSCANF);
	ke_hash_add(sml, (fncp)&ke_file_fscanf, FILE_FSCANF);

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
