#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "kexpr.h"

void ht_timing(void (*f)(void))
{
	clock_t t = clock();
	(*f)();
	printf("[ht_timing] %.3lf sec\n", (double)(clock() - t) / CLOCKS_PER_SEC);
}

int main(int argc, char *argv[])
{
	printf("%d\n", sizeof(struct ke1_s));
    int MAX = 1;
    ke_init_memory_count();
	int c = 0 , err = 0 , to_print = 0 , is_console = 0 , is_one_expr = 0, is_file = 1; //, is_int = 0;
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

    char *str = NULL;
	if (is_file) {
        FILE *fp;
        fp = fopen(argv[1], "r");
        if (fp == NULL){
            printf("\nCould not open file %s",argv[1]);
            return 1;
        }
        fseek(fp, 0L, SEEK_END);
        int sz = ftell(fp);
        str = calloc(sz+1,1);
        rewind(fp);
        fread (str, 1, sz, fp);
        fclose(fp);
	} else if (is_one_expr) {
        str = argv[1];
	}

	if (!is_console) {
        ke_fill_hash();
        ke = ke_parse(str, &err);
		int status = ke_fill_list(ke);
		if (status == -1 || err || ke == NULL) {
            fprintf(stderr, "\nParse error: 0x%x\n", err);
            ke_free_val();
            ke_free_hash();
            return 1;
        }

        for(int i = 0; i < MAX; ++i) {
            if (!to_print) {
				int64_t vi;
                double vr;
                char *vs;
                int ret_type;
                clock_t start_t = clock();
                err |= ke_eval(ke, &vi, &vr, &vs, &ret_type);
                clock_t end_t = clock();
                double total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
                printf("Total time taken by CPU: %.6lf second\n", total_t );
                //printf("\n[ht_timing] %.6lf sec\n", (double)(clock() - t) / CLOCKS_PER_SEC);
                if (err) {
                    fprintf(stderr, "\nParse error: 0x%x\n", err);
                    ke_free(ke);
					ke_free_tokens();
					ke_free_val();
                    ke_free_hash();
                    exit(1);
                }
            } else ke_print(ke);
        }
        //printf("\n[ht_timing] %.6lf sec\n", (double)(clock() - t) / CLOCKS_PER_SEC);
        if (is_file) {
            free(str);
        }
		ke_free_tokens();
		ke_free_val();
        ke_destroy(ke);
	}
    else {
        ke_fill_hash();
        char str[1000];

        while(1) {
            printf( "jyv>>> : ");
            gets( str );
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
                ke = ke_parse(str, &err);
				if (err || ke == NULL) {
                    fprintf(stderr, "\nParse error: 0x%x\n", err);
                } else {
					int status = ke_fill_list(ke);
					if (status != -1) {
						err |= ke_eval(ke, &vi, &vr, &vs, &ret_type);
					}
					ke_free_tokens();
					if (err) {
                        fprintf(stderr, "\nEval error: 0x%x\n", err);
                    }
                    ke_free(ke);
                }
                err = 0;
            }
            printf( "\n");
        }
		ke_free_val();
		ke_free_hash();
    }
	return 0;

}

