#include "../../plib6.h"
#include <stdio.h>

static char *get_byte_string(uint32_t word){ 
	static char out[9];
	for (int n = 7, p = 0; n >= 0; n--, p++) 
		snprintf(out+p,9, "%u", (word >> n) & 1);
	return out;
}


int
main(int argc, char *argv[])
{
	enum { test, test2, arg_end};
	
	// Create argument list 
	struct plib_Argument pl[arg_end] = {
		/* enum tag, flag, short, description */
		[test] = {"--test",  "-t",  "Basic testing flag"},
		[test2] = {"--test2", "-t", "Basic no arg flag"}
	};
	
	// Initialize values and set all arguments to take a value:
	plib_InitArgForAll(pl)
		plib_SetTakesValue(plib_Arg);

	// reference individual argument 
	plib_For(pl[test2])
		plib_SetRequired(plib_Arg);

	// Parse arguments and catch errors
	if(!plib_Parse_ez(pl)){
		printf("Error occured with argument: %s (error code: %d)\n",
			argv[PL_RETURN.index], PL_RETURN.code);
	}

	// loop through each argument
	plib_ForAll(pl){
		if(plib_ArgEnabled(plib_Arg))
			printf("%s - %d - %s\n", plib_Arg->flag, plib_ArgValueCount(plib_Arg), get_byte_string(plib_Arg->opt));
		
		// Handle if argument was run... 
		if(plib_WasRun(plib_Arg)){
			char * val = plib_GetArgValue(plib_Arg, 0, argv);
			printf("Argument --test was run with value: %s\n", val);
		}
	}

	// Done!

	return 0;
}
