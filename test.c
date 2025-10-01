#include "plib.h"
#include <stdio.h>

int main(const int a, const char *b[]){
	
	// init arguments 
	pl_arg *help = PL_P("--help","basic help call",.shorthand="-h");
	pl_arg *test = PL_P("--test","basic take-value",.takes_value=1,.shorthand="-t");
	pl_return_type ret; 

	if((ret = pl_proc(a, b)) == PL_SUCCESS){
		if(pl_arg_run(test) == PL_SUCCESS)
			printf("'%s'\n", pl_arg_value(test));

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
