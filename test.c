#include <stdio.h>
#include "plib6.h"
/* Arguments */
enum {
	help, template, template_file, json_file,
	delimiter_char, output_file, no_print,
	end_arg
};

int
main(int argc, char *argv[])
{
	// Set argument array
	static struct plib_Argument pl[end_arg] = {
		[help]           = {"--help",           "-h", "Display this dialog"},
		[template]       = {"--template",       "-t", "Set HTML template string"},
		[template_file]  = {"--template-file",  "-i", "Set path to HTML template file"},
		[json_file]      = {"--json-file",      "-j", "Set JSON data file"},
		[delimiter_char] = {"--delimiter_char", "",   "Set custom delimiter"},
		[output_file]    = {"--output-file",    "-o", "Output file (defaults to out.htm)"},
		[no_print]       = {"--no-print",       "",   "Disable final print to stdout"}
	};

	// Initialize arguments
	plib_CreateArgCount(pl);

	// Set value arguments
	plib_ForEach(1,4, pl)
		plib_ToggleProperty(plib_Arg, PLIB_SETTAKESVALUE);

	ifnot_plib_Parse(pl){
		printf("Options: \n");
		plib_HelpMenu(pl, printf);
		if(PL_RETURN.code != PL_ARG_NONE){
			printf("\nArgument parsing error occured:\n");
			printf("Code: %d\n", PL_RETURN.code);
			printf("Idx: %d (%s)\n", PL_RETURN.index, argv[PL_RETURN.index]);
		} else printf("No arguments provided.\n");
	}

	return 0;
}
