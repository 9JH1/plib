#include "plib.h"
#include <stdio.h>

int main(const int a, const char *b[]){
	pl_arg *help = PL_A("--help","basic help call",.shorthand="-h");
	pl_arg *test = PL_A("--test","basic take-value",.takes_value=1,.shorthand="-t");
	pl_arg *extr = PL_A("--extr","extra arg for tests");
	pl_return_type ret; 

	if((ret = pl_proc(a, b)) == PL_SUCCESS){
		if(pl_arg_run(test) == PL_SUCCESS){
			printf("--test: '%s'\n", pl_arg_value(test));	
		}
	} else {  
		printf("error '%s' occured from arg '%s'\n",
				PL_E(ret), // error code as a string
				PL_LAST_ARG); // last arg parsed from pl_proc 
		
		// run builtin help func
		pl_help();
	}
	pl_exit();
	return 0;
}
