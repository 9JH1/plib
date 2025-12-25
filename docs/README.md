## `Plib` Documentation
> `Plib 6.x.x Only`

### Introduction
> What is an argument and Creating an argument list 

`Plib` stores all arguments in a structure called `plib_Argument`:
```c
struct plib_Argument {
	char *flag;
	char *shrt;
	char *desc;
	
	uint8_t opt; 
	int idx;
	int  vals[PL_MAX_ARG_V];
};
```

All argument structs are held in a user defined array which is then passed into the many macros. The main parsing function `plib_Parse` requires the following values:
- Count of system arguments.
- Array of system arguments.
- Array of `Plib` arguments. 
- Delimiter for arguments.

The first two values are a given simply have `argc` and `argv` defined in your main function like so:
```c
int 
main (int argc, char *argv[])
{
    ...
}
```
And hey presto you have the first two values needed to run `plib_Parse`. The next value you need is an array of `Plib` arguments like mentioned previously. You can define such an array like so:
```c

struct plib_Argument pl[2] = {
    /* flag,      short, description */
    { "--test",  "-t",  "Basic testing flag"},
    { "--test2", "-t",  "description here!"}
};
```
Every item in this array of course has to of type `(plib_Argument)`. View the `plib_Argument` structure for every possible value you could use but understand that the `.name`, `.shrt` and `.desc` values are the only non-meta properties in an argument. The `.opt` property holds 8 toggle-able bits that represent different settings for an argument, and the `.idx` and `.vals` properties are for value indexing. DO NOT change the non-meta properties unless you know what you are doing!

### Initialization
> Editing argument properties and looping

`Plib` uses a large amount of loop based macros to alter arguments in a range. The first thing you should do after creating an argument list is initializing the arguments in the argument list. Currently the arguments will have a flag, short flag and description (if all values provided like in above example). Two other values must be initialized to prevent errors later on, those being the `,idx` and `.opt` values. To do this for every argument simply call the `plib_CreateArgCount(<pl>)` function with `<pl>` being your argument list. Once you have done this your code may look like the following:
```c
#include "plib6.c"

int 
main(int argc, char *argv[])
{
    // Create argument array
	struct plib_Argument pl[1] = {
		{"--test",  "-t",  "Basic testing flag"},
	};

    // Initialize arguments
    plib_CreateArgCount(pl);
    ...
}
```
---

Now `Plib` comes with four built in settings, these let you change the way an argument behaves. Before I explain these though you must first understand how plib lets you interact with arguments. Plib uses a loop and range based system for interactions, the following functions can be used as a scope header:
- `plib_forEach(n1, n2, ar)`
- `plib_CreateArgCount(ar)`
- `plib_ForAll(ar)`
- `plib_For(arg)`
- `plib_InitArgForAll(ar)`

In all of these macros `ar` is a type of `struct plib_Argument *` so this is where you would put the `pl` array from earlier. All of thse macros perform similar tasks usually looping over 1 a range or all arguments. As stated they can be used as a scope header eg:
```c
plib_ForAll(pl){
    ...
}
```

When inside a `Plib` scoped function you can reference the `plib_Arg` value which will be a pointer to current iterated argument. If you do not want to iterate through arguments you can use the provided `plib_For` macro which takes in a `struct plib_Argument` for example `pl[0]` and will act just like the other scoped functions where `plib_Arg` is a pointer to the argument parsed into the macro. `plib_Arg` is a global argument that is assigned the pointer value of an array on each iteration of a scoped loop function, because of this global variable, `Plib` IS NOT thread safe. Simply use initialize and parse your arguments using `Plib` BEFORE creating threads, simple fix. If for some reason you cannot stand doing that un-written ideology then you will have to customize the previous functions to use a mutex variable.
<br>
<br>
Now that you know how basic argument management is handled I'll tell you how to change the settings of an argument. There are 8 possible toggle-able values within the 8 bits of each arguments `.opt` value. `Plib` only uses 4 of these 8 bits so feel free to use any bits after index `3`. The following settings are available e.g.
- Enabled
- Required 
- Takes Multiple Values
- Takes Value 

By default when using the builtin `plibCreateArgCount` macro to initialize arguments each argument will have the enabled setting already activated. The following functions will TOGGLE their respective settings:
- `plib_SetEnabled(arg)`
- `plib_SetRequired(arg)`
- `plib_SetTakesMultipleValues(arg)`
- `plib_SetTakesValue(arg)`

In this case `arg` is a pointer to an index in your argument list e.g. `&pl[0]` the only safe method to use the functions above is inside a loop scope as previously discussed. The `plib_Arg` can be used in place of `arg` like so:
```c
plib_ForAll(pl){
    plib_SetTakesValue(plib_Arg);
}
```
The above code would loop through every argument and set the options to expect this argument with a value when parsing. Again I'll mention that using the `plib_For` macro will let you change the properties of a single argument without a loop. All other options are pretty straightforward. If you are unfamiliar with bitwise operations you could implement a basic macro to get the value of a given property.
```c
#define uint8_t_read(arg, idx) (arg->opt & (1u << idx))

plib_For(pl[0]){
    plib_SetTakesValue(plib_Arg);

    // plib_SetTakesValue uses the 0th bit to store 
    // its value. read plib6.h for the other indexes.
    printf("%d\n", uint8_t_read(plib_Arg, 0));
}
```

The output of this would be: 
```bash
$ ./myProgram  
1
```
Removing the line `plib_SetTakesValue...` would result in an output of `0` as the `plib_SetTakesValue` bit would not be toggled. 

### Parsing 
> Error catching, Parsing, and post parse setup.

Like mentioned `plib_Parse` takes in four values, you can alternativly use `plib_Parse_ez(<pl>)` which only requires the argument list, this works because it automatically takes in argc and argv so ensure that they are actually defined. now the only other parameter for `plib_Parse` that I have not talked about is the delimiter, this is the character at which an argument is split. `Plib` parses arguments like so by splitting arguments at the delimiter, for example with a delimiter of `=` and an argument of `--test=123` is parsed then the function will cut the string at the equal sign and use the first half for the key and the second half for the value. Note that `Plib` will split at the LAST occurance of an equal sign so arguments like this: `--test=val=123` would be split into `--test=val` and `123` which could allow for some really funky argument names aha.
<br>
<br>
`Plib` has an extended error handling system. when an error occurs a code will be returned from the following enumerator:
```c
enum {
    PL_SUCCESS, PL_RETURN_TYPE_END, PL_ARG_NONE,
    PL_ARG_NOT_FOUND, PL_ARG_VALUE, PL_ARG_NO_VALUE,
    PL_TO_MANY_VALUES
};
```

if `PL_SUCCESS` is returned then all arguments where parsed successfully. if not then something went wrong. Now on each iteration a global variable is updated on the last index of argument that was parsed. and you can reference both its `.index` and `.code` values to get the index and return code values. this can be used for some basic error checking like so:
```c
...

if(plib_Parse(argc, argv, pl, '=') != PL_SUCCESS){
	printf("Error occured with argument: %s (error code: %d)\n",
		argv[PL_RETURN.index], PL_RETURN.code);
}
...
```

The previous code will print out the error and the argument that caused this error. As you can see this is incredibly simple to do. NOW a side note is that in the argument parsing function the actual value index in `argv` is used to return the value of the argument which means that changes are the index will be useless unless you store a backup of `argv`.
<br>
<br>
Final topic is clean up and exit, this is the biggest and most important part when using `Plib`.. Just kidding! `Plib` uses ZERO memory allocation which means that there is absolutly nothing that needs to occur for clean up! you can use plib without worrying about memory leaks!

---

Enjoy! Feel free to open an issue for additional information on how plib works, but before doing so please read over the plib6.h source code, its under 200 lines and only contains one real function but I understand it gets very compact.

### Tips and tricks
My first word of advise would be to partion your arguments properly, that way you can properly utilize the loop functions. 

If you want to keep track of arguments easier you can set up an `enum` which can be used to call a certain index of the argument array using a keyword name like so:
```c
enum { test, test2, arg_end};
	
// Create argument list 
struct plib_Argument pl[arg_end] = {
	/* enum tag, flag, short, description */
	[test] = {"--test",  "-t",  "Basic testing flag"},
	[test2] = {"--test2", "-t", "Basic no arg flag"}
};
	
// reference individual argument 
plib_For(pl[test2])
	plib_SetTakesValue(plib_Arg);
```


Instead of using the `plib_CreateArgCount` and then using a `ForAll` loop right after you can instead use `plib_InitArgForAll` which runs `plib_CreateArgCount` and then uses the `plib_ForAll` macro right afterwards which removes the need for multiple macro calls.

You can change the amount of values each argument can hold by changing increasing the `PL_MAX_ARG_V` constant, alternativly you could pass `-DPL_MAX_ARG_V=<size>` into your compiler where `<size>` is the amount of values each argument can hold at a maximum, default is 64.

---
End of file
