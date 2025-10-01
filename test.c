#include "plib.h"
#include <stdio.h>

int main(const int a, const char *b[]){
	pl_arg *help = PL_P("--help","basic help call",.type="void",.shorthand="-h");
	pl_arg *test = PL_P("--test","basic value arg",1,.type="char *",.shorthand="-t");

	pl_return_type ret; 

	if((ret = pl_proc(a, b)) == PL_SUCCESS){
		if(PL_R(test))
			printf("'%s'\n", pl_arg_value(test));
		
		if(PL_R(help))
			pl_help(); 
	
	} else {
		printf("error '%s' ('%s')\n", PL_E(ret), PL_LAST_ARG);
		pl_help();
	}

	return 0;
}
