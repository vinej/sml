#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "kexpr.h"
#include "utf8.h"
#include <setjmp.h>

char * gets(char *str);
utf8* read_utf8_file(utf8* filename);

void ht_timing(void (*f)(void))
{
	clock_t t = clock();
	(*f)();
	printf("[ht_timing] %.3lf sec\n", (double)((double)clock() - t) / CLOCKS_PER_SEC);
}

int main(int argc, char *argv[])
{

#if defined(_M_X64) || defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif
	printf("%d\n", (int)sizeof(struct ke1_s));
	int MAX = 1;
	int c = 0, err = 0, to_print = 0, is_console = 0, is_one_expr = 0, is_file = 1; //, is_int = 0;
	kexpr_t *ke;

	if (argc == 1) {
		is_console = 1;
		is_file = 0;
	}
	//while ((c = getopt(argc, argv, "pcef")) >= 0) {
	//	if (c == 'p') to_print = 1;
	//	if (c == 'c') is_console = 1;
	//	if (c == 'e') is_one_expr = 1;
	//	if (c == 'f') is_file = 1;
	//}
	//if (2 == argc && !is_console) {
	//	fprintf(stderr, "\nUsage: %s [-pi] <expr>\n", argv[0]);
	//	return 1;
	//}
	sml_t * sml = ke_create_sml();
	ke_init_memory_count(sml);
	utf8 *str = NULL;
	if (is_file) {
		str = read_utf8_file(argv[1]);
		if (!str) {
			exit(1);
		}
	}
	else if (is_one_expr) {
		str = argv[1];
	}

	if (!is_console) {
		ke_fill_hash(sml);
		ke = ke_parse(sml, str, &err);
		int status = 0;
		if (ke != NULL) {
			status = ke_fill_list(sml, ke);
		} else {
			status = -1;
		}
		if (status == -1 || err || ke == NULL) {
			fprintf(stderr, "\nParse error: 0x%x\n", err);
			ke_free_val(sml);
			ke_free_hash(sml);
			return 1;
		}

		sml->val = setjmp(sml->env_buffer);
		if (sml->val != 0) {
			printf("\nSML: Error found in code: %d\n", sml->val);
		}
		else {
			if (!to_print) {
				int64_t vi;
				double vr;
				char *vs;
				int ret_type;
				clock_t start_t = clock();
				err |= ke_eval(sml, ke, &vi, &vr, &vs, &ret_type);
				clock_t end_t = clock();
				double total_t = (double)((double)end_t - start_t) / CLOCKS_PER_SEC;
				printf("Total time taken by CPU: %.6lf second\n", total_t);
			}
			else ke_print(sml, ke);
		}

		if (is_file) {
			free(str);
		}
		ke_free_tokens(sml);
		ke_free_val(sml);
		ke_destroy(sml, ke);

		ke_free_sml(sml);
		if (err) {
			fprintf(stderr, "\nParse error: 0x%x\n", err);
		}
	} else {
        ke_fill_hash(sml);
        char str[1000];

        while(1) {
            printf( "jyv>>> : ");
            fgets( str,999,stdin );
            char * p = str;
            while(*p) {
                if (*p < 32) *p = 32;
                if (*p == '[') *p=32, ++p, *p=32;
                ++p;
            }
            if (strcmp(str, "exit") == 0) {
                break;
            } else {
				int64_t vi;
                double vr;
                char *vs;
                int ret_type;
                ke = ke_parse(sml,str, &err);
				if (err || ke == NULL) {
                    fprintf(stderr, "\nParse error: 0x%x\n", err);
                } else {
					int status = ke_fill_list(sml,ke);
					if (status != -1) {
						err |= ke_eval(sml,ke, &vi, &vr, &vs, &ret_type);
					}
					ke_free_tokens(sml);
					if (err) {
                        fprintf(stderr, "\nEval error: 0x%x\n", err);
                    }
                    ke_free(sml,ke);
                }
                err = 0;
            }
            printf( "\n");
        }
		ke_free_val(sml);
		ke_free_hash(sml);
    }
	return 0;

}

