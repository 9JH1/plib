#include <stdio.h>
#include "plib.h"


int main(const int c, const char *v[]){
	pl_arg *test = PL_P("--test",.takes_value = 1);
	pl_r ret = PL_FAILURE;
	
	if((ret = pl_proc(c,v)) == PL_SUCCESS){
		if(PL_R(test)){

			// iterate through every use of --test
			for(int i = 0 ; i < pl_arg_count(test) ; i++){
				printf("%02d --test: %s\n",
						i+1,
						test->value[i]);
			}
		}
	} else {
		// handle errors 
		printf("Error: '%s' from '%s'\n",
				PL_E(ret),
				PL_LAST_ARG);

		pl_help();
		return 1;
	}

	return 0;
}
