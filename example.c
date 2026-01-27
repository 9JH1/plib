#include <stdio.h>
#include "plib6.h"

// Arguments
enum {
	test_arg_1,
	test_arg_2,
	test_arg_3,
	end_arg
};

int
main (int argc, char *argv[])
{
	// Setup argument array
	static struct plib_Argument pl[end_arg] = {
		[test_arg_1] = {"--test1", "-t1", "test argument 1"},
		[test_arg_2] = {"--test2", "-t2", "test argument 2"},
		[test_arg_3] = {"--test3", "-t3", "test argument 3"}
	};

	// Initialize arguments
	plib_CreateArgAndForAll(pl)
	  {
		plib_ToggleProperty(plib_Arg, PLIB_ENABLED);
		plib_ToggleProperty (plib_Arg, PLIB_TAKESVALUE);
	  }

	// Handle errors
	ifnot_plib_Parse (pl)
	  {
		printf ("Options: \n");
		plib_HelpMenu (pl);
		
		if (PL_RETURN.code != PL_ARG_NONE)
		  {
			// Print error data
			printf( "\nArgument parsing error occured:\n");
			printf ("%s (%s)\n", plib_Error, plib_ErrorArgument);
			return 1;
		  }
		
		printf ("No arguments provided.\n");
		return 0;
	  }

	// Print all values given:
	for (int i = 0; i < end_arg; i++)
	  {
		struct plib_Argument local = pl[i];
		for (int j = 0; j < local.idx; j++)
		  {
		  	printf("%s - %s(%d)\n", 
					local.flag,          // flag name
					argv[local.vals[j]], // argument value
					local.vals[j]);      // value index in argv
		  }
	  }

	// Exit (no cleanup procedure)
	return 0;
}
