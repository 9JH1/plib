```
plib V4.5
-----------------------
PLib is a tiny (13.4KB) but powerful argument library 
written in C. Using this library you can very easily 
set up full argument parsing with many features.

included in the lib folder you can find many other util librarys
```
```c
#include "plib.h"
#include <stdio.h>

int main(const int a, const char *b[]){
    // create arguments 
	pl_arg *help = PL_P("--help","basic help call",.type="void",.shorthand="-h");
	pl_arg *test = PL_P("--test","basic value arg",1,.type="char *",.shorthand="-t");

    // error code var 
	pl_return_type ret; 

    // process the system arguments 
	if((ret = pl_proc(a, b)) == PL_SUCCESS){

        // check if arguments have been run 
		if(PL_R(test))
			printf("'%s'\n", pl_arg_value(test));
		
		if(PL_R(help))
			pl_help(); 
	
	} else {
        // handle errors
		printf("error '%s' ('%s')\n", PL_E(ret), PL_LAST_ARG);
		pl_help();
	}
    
    // exit
	return 0;
}
```
# Usage librarys
> what the lib files provide 
- plib.h - allows for advanced and fleixable argument parsing/handling.
- lib/draw.h - provides functions to draw boxes borders and titles, useful if making a TUI interface.
- lib/ansi.h - defines a bunch of ansi codes that do things eg move caret to pos or change term buffer.
- lib/gc.h - basic garbage collector.
- input.h - a input loop that collects mouse and keyboard information.
All of this is done using standard librarys, so no curses or ncurses.
# Changelog
> started 02/10/2025
1. added multi argument support 
    before, when using the same arg (that takes a value) twice the second run would overwrite the first runs 
    value, now ive changed `arg.value` to be a `char **` array and ive added `arg.value_idx` and `arg.value_cap` meta
    for use in dma in `pl_proc`. you can now use `pl_arg_count` as a macro that returns the `arg.value_idx` value, below 
    is a basic usage:
```c
#include <stdio.h>
#include "plib.h"


int main(const int c, const char *v[]){
	pl_arg *test = PL_P("--test",.takes_value = 1);
	pl_return_type ret = PL_FAILURE;
	
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
```
2.
