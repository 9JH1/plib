/**
 * @file plib.h 
 * @brief Basic argument parsing library
 * 
 * Plib gives functions to create and manage the system 
 * arguments of any given function, these arguments can 
 * be defined with many options for example shorthand 
 * and long-hand versions of the same ag eg 
 * `--help` and `-h`. 
 *
 * Arguments are dynamically allocated and free'd 
 * automatically on exit so you dont have to worry 
 * about any memory leaks. Along with this plib provides 
 * a builtin function called @ref pl_help which prints 
 * out all of the arguments in a standard format. 
 * 
 * In order to get started with plib first you must include 
 * the system arguments into your main function, for reference
 * look here: 
 * @ref "https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-main-function"
 * 
 * To create an argument use @ref pl_a @ref PL_A or @ref PL_P,
 * read the structure declaration @ref pl_arg to learn all of 
 * the options.
 * @code 
 * #include "plib.h"
 *
 * int main(const int c, const char *v[]){
 *   pl_arg * my_arg = PL_P("--my-arg", "basic first argument!");
 *   pl_proc(c,v);
 *   pl_help();
 *	 return 0;
 * }
 * @endcode
 * As you can see we defined an argument called my_arg with 
 * the flag name of `--my-arg` and a description of "basic first argument".
 * running this code outputs the following:
 * @code 
 * Options:
 *   --my-arg | basic first argument!
 * @endcode
 * To check if an argument is run use @ref PL_R or @ref pl_arg_run like so 
 * @code 
 * #include <stdio.h>
 * // ...
 * if(PL_R(my_arg))
 *   printf("Hello World!\n");
 * // ... 
 * @endcode
 * Now if we run our program it will print back "Hello World!" To take an value
 * with an argument you simply put .takes_value = 1 in the argument definition 
 * like so:
 * @code 
 * // ...
 * pl_a * my_arg = PL_P("--my-arg","basic first argument!", .takes_value = 1);
 * // ...
 * @endcode 
 * if the code is run the same as before you will now notice we do not get the 
 * "Hello World!" output anymore , this is because plib failed to parse the 
 * `--my-arg argument` and instead threw an error because it was missing a value. 
 * simple error checking can be done like so 
 * @code 
 * // ...
 * pl_r return_code;
 * if((return_code = PL_PROC(c,v)) == PL_SUCCESS){
 *   // plib parsed all argument sucessfully.
 *
 * } else {
 *   // error occured
 *   printf("Error occured %s from %s\n",
 *     PL_E(return_code),
 *     PL_LAST_ARG);
 * }
 * // ...
 * @endcode
 * Now if we run the program with the same arguments we will get an output like 
 * so: 
 * @code 
 * Error occured ARG_REQUIRES_VALUE from --my-arg
 * @endcode 
 * PLib uses '--key=val' argument value style, so to resolve this plib error run 
 * the program with --my-arg="value". 
 *
 * To get the value of an arg either use @ref pl_arg_value like so:
 * @code 
 * // ... 
 * if(PL_R(my_arg))
 *   printf("Argument run with value %s\n",
 *     pl_arg_value(my_arg) ); // get argument value 
 * @endcode 
 * Now running the program will print out whatever value has been parsed.
 * here is the full example of what we just went over: 
 * @code 
 * #include <stdio.h>
 * #include "plib.h"
 *
 * int main(const int c, const char *v[]){
 *   pl_arg * my_arg = PL_P("--my-arg","my argument!", .takes_value = 1);
 *   pl_r return_code;
 *   
 *   if((return_code = pl_proc(c,v)) == PL_SUCCESS){
 *     if(PL_R(my_arg))
 *      printf("Argument run with value %s\n", 
 *      pl_arg_value(my_arg) ); // get argument value 
 *   } else {
 *     
 *     // error occured
 *     printf("Error occured %s from %s\n",
 *       PL_E(return_code),
 *       PL_LAST_ARG);
 *   }
 *
 *   return 0;
 * }
 * @endcode
 **/
#ifndef PLIB_H
#define PLIB_H
#ifndef PL_INIT_ARG_ALLOC 
/**
 * @brief initial argument alloc amount 
 *
 * PLib adds each new argument to an array of pl_a structs.
 * this value is the initial start value of plib, note that 
 * if you want to optimize your code and use @ref PL_A instead 
 * of @ref PL_P then increase this value to the same amount of 
 * arguments you intend on defining.
 **/
#define PL_INIT_ARG_ALLOC 2
#endif

#ifdef PL_VERBOSE_ENABLE

 /**
 * @brief enables verbose logs 
 *
 * To enable verbose logs use the `-DPL_VERBOSE_ENABLE` 
 * flag when running `gcc`
 **/
#define PL_VERBOSE 1 
#else 
/**
 * @brief enables verbose logs 
 * To enable verbose logs use the `-DPL_VERBOSE_ENABLE`
 * flag when running `gcc`
 **/
#define PL_VERBOSE 0
#endif
/**
 * @brief Plib version 
 **/
#define PL_VERSION "4.7" 

/** 
 * @brief argument options 
 * @param name flag name eg `--help`
 * @param description flag description (used in @ref pl_help)
 * @param takes_value set whether or not the argument should expect a value 
 * @param category set a category for an argument, arguments of the same category are printed 
 *        togeather in @ref pl_help
 * @param type type of value, not actuall enforced but used in @ref pl_help instead. eg VOID or INT.
 * @param value if `.takes_value = 1` then the value(s) parsed in will be allocated here.
 * @param value_cap Value is dynamically allocated this is a variable used in the DMA.
 * @param value_idx same as value_cap but this value also is the amount of arguments parsed 
 *        into the flag, this doubles as the run count of any arguments that take a value.
 * @param triggered this value is `>1` if the argument was run and is `0` if the arg was not run.
 *        this doubles as the run count for all arguments
 **/
typedef struct pl_arg {
	const char *name;       
  const char *description;
	int takes_value; 
	const char *category; 
	const char *shorthand; 
	const char *type;	
  char **value;
	int value_cap;
	int value_idx;
  int triggered;
	int shorthand_triggered;
} pl_arg;

/**
 * @brief PLibs local array of arguments.
 * @see PL_ARGS_IDX
 * @see PL_ARGS_CAP
 **/
extern pl_arg *PL_ARGS;

/** 
 * @brief PLibs local count of arguments 
 * @see PL_ARGS_IDX
 * @see PL_ARGV
 **/
extern int PL_ARGS_IDX;

/**
 * @brief PL_ARGS capacity used for DMA.
 * @see PL_ARGS
 * @see PL_ARGS_IDX
 **/
extern int PL_ARGS_CAP;

/**
 * @brief local copy of argument array parsed into @ref pl_proc 
 */
extern char** PL_ARGV;

/** 
 * @breif local copy of argument count parsed into @ref pl_proc 
 **/
extern int PL_ARGC;

/** 
 * @brief index of PL_ARGC that was last parsed by @ref pl_proc 
 *        used in error checking if pl_proc throws an error this 
 *        value is the index of argument that caused the error.
 **/
extern int PL_PROC_END_ARGC; 


/** 
 * @brief PLib return codes 
 * 
 * These codes are used by most plib functions a code can be turned
 * into a string using the @ref PL_E function. The strings used for 
 * error codes can be read in the @ref pl_r array.
 * @see PL_E 
 * @see pl_s
 **/
typedef enum {
	PL_ARG_NOT_FOUND = -1,
	PL_ARG_REQUIRES_VALUE = -2,
	PL_ARG_NO_REQUIRES_VALUE = -3,
	PL_NO_ARGUMENTS_GIVEN = -4,
	PL_MEM_ALLOC_ERROR = -5,
	PL_SUCCESS = -7,
	PL_ARG_INVALID_FORMAT = -8,
	PL_ARG_IS_NULL = -9,
	PL_FAILURE = -10,
	PL_ARG_PRE_EXISTS = -11,
	PL_ARG_MISSING_PARAM = -12,
} pl_r;


/** 
 * @brief Convert -n to n-1, used for converting @ref pl_r enum to valid array index in @ref pl_s 
 * @param n int to invert - 1 
 * @see pl_s
 **/
#define PL_INVERT(n) ((n * -1) - 1)


/** 
 * @brief list of error code strings 
 * @see PL_E 
 * @see pl_r 
 **/
static const char *pl_s[12] = {
    [PL_INVERT(PL_ARG_NOT_FOUND)] = "ARG_NOT_FOUND",
    [PL_INVERT(PL_ARG_REQUIRES_VALUE)] = "ARG_REQUIRES_VALUE",
    [PL_INVERT(PL_ARG_NO_REQUIRES_VALUE)] = "ARG_NO_REQUIRES_VALUE",
		[PL_INVERT(PL_NO_ARGUMENTS_GIVEN)] = "NO_ARGUMENTS_GIVEN",
		[PL_INVERT(PL_MEM_ALLOC_ERROR)] = "MEM_ALLOC_ERROR",
		[PL_INVERT(PL_SUCCESS)] = "SUCCESS",
		[PL_INVERT(PL_ARG_INVALID_FORMAT)] = "ARG_INVALID_FORMAT",
		[PL_INVERT(PL_ARG_IS_NULL)] = "ARG_IS_NULL",
		[PL_INVERT(PL_FAILURE)] = "FAILURE",
		[PL_INVERT(PL_ARG_PRE_EXISTS)] = "ARG_PRE_EXISTS",
		[PL_INVERT(PL_ARG_MISSING_PARAM)] =  "ARG_MISSING_PARAM"
};


/** 
 * @breif check if argument exists using flag name 
 * 
 * takes in a string name and searches PL_ARGS to check if 
 * the parsed string exists as a argument flag in the args 
 * array. if it does return PL_SUCCESS, if not return 
 * PL_FALIURE 
 *
 * @param name String .name of pl_arg to search for.
 * */
pl_r pl_arg_exist(const char *name);


/**
 * @brief takes in an argc and argv and parse them 
 *        in comparison to the arguments in PL_ARGS. 
 * @param argc count of command line arguments 
 * @param argv char array of command line arguments
 * @see PL_PROC 
 **/ 
pl_r pl_proc(const int argc, const char *argv[]);

/** 
 * @brief macro for removing the PL_SUCCESS check and argv and argc params.
 * 
 * This macro will automatically include argc and argv, this will work as long 
 * as `argc` and `argv` are defined and are of the correct types. 
 *
 * examples before:
 * @code 
 * // ... 
 * if(pl_proc(argc, argv) == PL_SUCCESS){
 *   // ... 
 * }
 * // ...
 * @endcode 
 *
 * and after
 * @code 
 * // ... 
 * if(PL_PROC()){
 *   // ...
 * }
 * // ...
 * @endcode 
 **/
#define PL_PROC() (pl_proc(argc,argv) == PL_SUCCESS)


/** 
 * @brief check if argument has been run 
 * 
 * You can instead use the @ref PL_R macro 
 * which similar to the @ref PL_PROC macro 
 * just removes the comparison to PL_SUCCESS 
 * which cuts back on the line size and general 
 * clutter.
 *
 * @param local pl_arg * argument to check 
 * @see PL_P 
 * @see PL_R 
 **/
pl_r pl_arg_run(const pl_arg *local);

/**
 * @brief check if argument has been run without PL_SUCCESS comparison 
 * this macro just removes the need to use the PL_SUCCESS comparison 
 * which removes clutter and line size.
 * @param arg pl_arg *  
 * @see pl_arg_run
 **/ 
#define PL_R(arg) pl_arg_run(arg) == PL_SUCCESS


/* returns PL_SUCCESS of if ALL arguments set where used 
 * else return PL_FALIURE */
pl_r pl_all_triggered(void); 


/* prints out all set arguments and their info,
 * very powerful and auto-adapting to whatever 
 * information is available or set in args, by 
 * this i mean if all of the arguments set do 
 * not specify a .type then no space or column 
 * will be printed for them but if one of the 
 * set args (or more) have the .type set then 
 * a column will be printed, all values printed 
 * are aligned and catagorized too. */ 
void pl_help(void);


/* when creating an argument a new slice of an 
 * array is allocated where the arguments data 
 * is held. also when an argument has a value 
 * parsed into it the .value of the arg has to 
 * be allocated too, the pl_exit function clears
 * all of the memory from everything. this function 
 * will be automatically called on exit if the 
 * pl_proc function has been run */
void pl_exit(void);


/* arguments can be run multiple times which results 
 * in multiple values, this function will get a value 
 * of a pl_arg using an index (i), to get the amount of 
 * values available (indexs) use pl_arg_value_count().*/ 
char* pl_arg_value(const pl_arg *local, const int i);

/* this is the main function used to create an argument 
 * there are many forms of this argument listed below.
 * this function simply takes in a pl_arg and writes it 
 * to an array, this array is used as reference when 
 * calling pl_proc, as noted at the very top in the 
 * definition of pl_arg there are many values, now the 
 * only value actually required is .title, all of the 
 * other arguments are not required or some default to 
 * values like so:
 * 
 * catagory = "Options:" 
 * takes_value = 0
 *
 * this function returns a integer index to the slice 
 * in the array at which the new argument has been 
 * made. 
 * */ 
int  pl_a(pl_arg in);
/* as mentioned there are other ways of using pl_a, 
 * they are as follows: */ 

/* PL_A is a wrapper that allows you to enter the struct 
 * data directly into a function without needing to define 
 * the type of struct, here is the before and after: 
 * 
 * int ret = pl_a((pl_arg){.name="test",.takes_value=1});
 * 
 * int ret = PL_A(.name="test",.takes_value=1);
 *
 * so much easier right?
 * */
#define PL_A(...) pl_a((pl_arg){__VA_ARGS__})

/* to understand why this macro exists along with its implicit 
 * pl_arg_global_ptr. originally pl_a returned a pointer pl_arg 
 * to the actual item in the arguments array, this was very bad 
 * as when the argument list reached capacity it would overwrite 
 * the pointers memory location which creates a dangling pointer.
 * this pointer can still be interacted with without error, the 
 * issue with this is that the pointer that we think is pointing 
 * to an argument in our argument list is actually not existant 
 * which means that from a debug point of view if you created an 
 * argument and the list was re-allocated then now for some 
 * reason regardless of how you trigger that argument it still 
 * comes up as false when using pl_arg_run this is of course 
 * because the pointer is refering to something other then what 
 * it is suppost too. to combat this i changed the code so an 
 * index is returned instead of a pointer, then after all of the 
 * args are set you can use either &PL_ARGS[index] or PL_GETI(i) 
 * to grab a pointer to an argument, now this was a good fix but 
 * now one line has turned into two lines, one for definition 
 * and a second for actually getting the pointer from the index. 
 * the way this is resolved is by using the following macro which 
 * is a copy of the PL_A macro but for the pl_arg_global_ptr func 
 * the pl_arg_global_ptr function will first check to see if the 
 * arg has been created if it has then it will search for it in 
 * the argument list and return a pointer to it if it finds it.
 * if the arg has not been made then the arg will be offloaded
 * into the pl_a function and then returned as a pointer. this 
 * means that if you initialize a var using the PL_P function 
 * every time you call the variable it will perform a search in 
 * the argument list for the argument which removes all risk of 
 * dangling pointers as the information is updated per-call and 
 * isent static and overwritable. */
#define PL_P(...) pl_arg_global_ptr((pl_arg){__VA_ARGS__})
pl_arg *pl_arg_global_ptr(pl_arg in);


/* takes in a string of a flag name and searches for it in the 
 * argument list, if found it will return the pl_arg as a pointer 
 * or if not found it will return nothing. */ 
pl_arg *pl_arg_by_name(const char *name);


// wrapper for pl_arg_get_name...
#define PL_GETN(name) pl_arg_by_name(name)


/* get a pointer to an arg using index 
 * see pl_a and PL_A. */ 
#define PL_GETI(idx) &PL_ARGS[idx]


// auto grab argv and argc if they are named as such.


/* get the last argument parsed by pl_proc, 
 * super useful in debugging and error output */
#define PL_LAST_ARG (PL_PROC_END_ARGC >= 0) ? \
										(PL_ARGV[PL_PROC_END_ARGC]) : \
										pl_s[PL_INVERT(PL_NO_ARGUMENTS_GIVEN)]


// convert a return code to a string using pl_r to pl_s array
#define PL_E(i) pl_s[PL_INVERT(i)]


/* arguments can be run using either their .name or .shorthand values,
 * this function returns the method used when the argument was parsed.
 * note this won't work if the the user uses the same flag multiple 
 * times and with both the shorthand and name */
#define pl_arg_sys(arg) (arg->triggered == 1) ? ((arg->shorthand_triggered == 1) ? arg->shorthand : arg->name) : NULL


// return the amount of values an argument was run with
#define pl_arg_value_count(parg) parg->value_idx
#endif // PLIB_H

