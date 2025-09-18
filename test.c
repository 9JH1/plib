#include "plib.h"
#include <stdbool.h>
#include <stdio.h> 

void help(){
	printf("PLib example program\nOptions:\n");
	pl_help();
}


int main(const int argc, const char *argv[]){
	const pl_arg *test = pl_a((pl_arg){
			.name = "--test",
			.description = "basic test flag",
			.takes_value = true,
			});

	const pl_return_type ret = pl_proc_at_i(argc, argv);

	// on error
	if(ret != PL_SUCCESS){
		printf("error: %s\n",pl_return_type_string[ret]);
		printf("from arg: %s\n",PL_LAST_ARG);
		help();
	}

	printf("\ndone\n");
	return 1;
}
