#include "plib.h"
#include <stdio.h> 

int main(const int argc, const char *argv[]){
	// CREATE --test FLAG 
	pl_arg *test = PL_A(
			.name = "--test",
			.description = "basic test flag",
			.takes_value = TAKES_VALUE, // | NO_VALUE
			);

	// PARSE ARGUMENTS 
	const pl_return_type ret = PL_PROC();

	if(ret != PL_SUCCESS){
		printf("error: '%s' on argument '%s'\n",
				pl_return_type_string[ret], // STRINGIFYED RETURN CODE
				PL_LAST_ARG // LAST RUN COMMAND
				);

		printf("\nHere are the available flags:\n");
		pl_help(); // LIST ALL CREATED ARGUMENTS
	
	} else if(pl_arg_run(test) == PL_SUCCESS){
		printf("%s was run with '%s' value\n",
				test->name,
				pl_arg_value(test) // argument value
				);
	}

	// EXIT PLib (required)
	pl_exit();
	return 1;
}

// PLib showcase code 
// 2025-09-19 18:14
