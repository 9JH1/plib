```
plib V3.4
-----------------------
PLib is a tiny (13.4KB) but powerful argument library 
written in C. Using this library you can very easily 
set up full argument parsing with many features.
Here is a quickstart:
```
```c
// test.c
#include <stdio.h> 
#include "plib.h"

int main(const int argc, const char argv[]){
    // create the --test flag
    pl_arg test_flag = pl_a((pl_arg){
        .name = "--test",
        .takes_value = NO_VALUE,
        });

    // parse the argc and argv 
    if(pl_proc_i(argc,argv) == 0){

        // if argument was run
        if(pl_arg_run(test_flag) == 0){
            printf("Hello World!\n");
        }
    }
        
    // exit plib
    pl_exit();
    return 1;
}
```
```
in this example running the program like so: 
gcc test.c plib.c -o test  
./test --test 

would result in the following output:
Hello World!

note that the pl_arg value has these other parameters: 
```
```c
pl_arg my_flag pl_a((pl_arg){
    .name = "--flag-name",
    .description = "description for my flag",
    .catagory = "metadata flag catagory (for help dialog)",
    .takes_value = NO_VALUE,
});
```
```
Keep in mind the pl_arg struct has other values that are used
for other various functions, regardless if they are set or not 
the values not listed above will be overwritten on pl_a() call.

Also, when creating an argument the MINIMUM required values are 
just to include the name, if you do not define catagory it will 
fallback to "base" and if you leave takes_value undefined it will
fallback to NO_VALUE.

some other examples of outputs may include: 

TAKING VALUES: 
```
```c 
// values.c 
#include <stdio.h>
#include "plib.h"

int main(const int argc, const char argv[]){
    // create the --value flag
    pl_arg test_flag = pl_a((pl_arg){
        .name = "--value"
        .takes_value = TAKES_VALUE
        });

    // parse the argc and argv 
    if(pl_proc_i(argc,argv) == 0){

        // check if argument has been run 
        if(pl_arg_run(test_flag) != 0)
            return 2;

        // print argument value 
        char* test_flag_value = pl_arg_value(&test_flag);
        printf("value: %s\n",test_flag_value);

        // alternativly (no validation or error check)
        // char* test_flag_value = test_flag->value;
    }
        
    // exit plib
    pl_exit();
    return 1;
}
```
```
OUTPUT:
$ ./values --value="123" --print-value 
>> Value: 123

GLOBAL END FLAG:
```
```c
// command_shower.c
#include <stdio.h>
#include "plib.h"

int main(const int argc, const char argv[]){
    // create the -- flag
    pl_arg commands = pl_a((pl_arg){
        .name = "--",
        .takes_value = TAKES_VALUE,
        });

    // --run-command flag
    pl_arg enable = pl_a((pl_arg){
        .name = "--run-command",
        .takes_value = TAKES_VALUE,
        });
 
    // parse the argc and argv 
    if(pl_proc_i(argc,argv) == 0){

        // check if argument has been run 
        if(pl_arg_run(enable) != 0)
            return 2;

        // print argument value 
        char* value = pl_arg_value(&commands);
        printf("Running Command: %s\n",value);
    }

    // exit plib
    pl_exit();
    return 1;
}
```
```
(all text after the -- is attributed to the -- flag value)
$ ./command_shower --run-command=true -- cat myFile.txt | grep "phrase"
>> Running command: cat myFile.txt | grep "phrase"

for more information read plib.h
```
