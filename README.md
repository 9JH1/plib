```
plib v5.0.11
--------------------
Plib is a basic argument parsing library that 
lets you simply define command line arguments 
and parse them quickly.
```
---
<br>

## Example of argument parsing:
```c
#include <main.c>
#include <string.h>

int main(const int argc, const char *argv[]){
    if(argc <= 1){
        printf("not enough arguments\n");
        return 1; 
    }

    // loop go through arguments 
    for(int i = 1; i < argc; i++){

        // store current argument 
        char *cur_arg = argv[i];

        // comparisons 
        if(strcmp(cur_arg,"--help") == 0){
            printf("help!");

        // print arg with value    
        } else if (strcmp(cur_arg,"--test") == 0){
            if(argv[i+1] == NULL){
                printf("--test needs a value!\n");
            }
            printf("--test run with value '%s'\n",argv[i+1]);
            i++;
        }
    }

    return 0;
}
```

This code can parse arguments like so (`program` being the compiled code from above)
```
./program --help
help!

./program --test 
--test needs a value! 

./program --test 123
--test run with value '123'
```

Now if we did the same thing using plib: 
```c 
#include "plib.h"
#include <stdio.h>

int main(const int argc, char *argv[]){
    pl_arg *help = PL_A("--help");
    pl_arg *test = PL_A("--test",.takes_value = 1);
    pl_r return_code = 0;

    // parse the arguments 
    if((return_code = pl_proc(argc,argv)) == PL_SUCCESS){

        // if help is run  
        if(PL_R(help))
            printf("help!");

        // if test is run 
        if(PL_R(test))
            printf("--test run with value '%s'\n",test);

    // error handling
    } else {
        printf("error occured!\nCode: %s\nArgument: %s\n",
            PL_E(return_code), // get a string of the return code 
            PL_LAST_ARG, // get the last string argument parsed
        )
    }
    return 0;
}
```
which outputs
```
./plib_program --help 
help! 

./plib_program --test 
error occured!
Code: ARG_REQUIRES_VALUE
Argument: --test 

./plib_program --test=123
--test run with value '123'
```
---
<br>

## Features
Even though both snippets are roughly the same size the plib option offers much more readable code as well as having indepth builtin error detection.
plib comes out of the box with a custom help function that prints out all of the arguments in a nice format like so:
```
Options: 
  --help | NO VALUE
  --test | VALUE 
```

This help menu prints out all information available for an argument, other options that can be added to an argument include,
- Catagory: 
    > defaults to Options: and is used as the argument header.
- Type: 
    > type of argument, used only in the plib help dialog
- Description:
    > description of argument 
- Takes Value:
    > if the argument takes a value or not.

<br>

Search the documentation for `pl_arg` to view all options 

---
As shown above plib comes with extensive error checking, when plib encounters an issue plib will return a specific enumerator (see `pl_r` plib-return & `pl_s` plib-string, in the documentation) which can be stringified using the `PL_E` (plib-error) function. Also shown is the `PL_LAST_ARG` value which retrives the 
argument that caused the issue.
<br>

## Full Documentation
This project uses the doxygen documentation system, in order to view the documentation you must install the `doxygen` program to your system. PLib does not come with its own DoxyFile so you need to create one, this can be done like so:
```
doxygen -g .plibDoxy
doxygen .plibDoxy
```

To view the newly generated documentation cd into the `./html` folder and open the `index.html` file in a browser of your choice.
> Roughly seven pages of documentation.
