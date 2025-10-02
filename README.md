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
# Changelog
> started 02/10/2025
1.  
