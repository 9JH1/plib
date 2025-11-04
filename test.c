#include <stdio.h>
#include "plib.h"

int main(int argc, char *argv[]){
	pl_arg *help = PL_A(
			"--help",
			"Show this dialog",
			.short_flag = "-h");

	pl_arg *value = PL_A(
			"--value","take a value",
			.takes_value = 1,
			.cat = "Extra");

	pl_arg *required = PL_A(
			"--required",
			"basic required flag",
			.short_flag = "-r",
			.required = 1,
			.cat = "Extra");
	
	// process arguments (auto gets argv argc)
	pl_r return_code = PL_PROC();

	// catch errors 
	if(return_code != PL_SUCCESS){
		PL_E_INFO(return_code)
		printf("use --help for more information\n");
		return 1;
	}

	if(PL_R(help))
		pl_help();

	if(PL_R(value)){
		printf("--value run with value '%s'\n",PL_G(value));	
	}

	return 0;
}
