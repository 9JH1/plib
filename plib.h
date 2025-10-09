#ifndef PLIB_H
#define PLIB_H
#ifndef PL_INIT_ARG_ALLOC 
#define PL_INIT_ARG_ALLOC 2 // initial amount of arguments
#endif

#ifdef PL_VERBOSE_ENABLE
#define PL_VERBOSE 1 
#else 
#define PL_VERBOSE 0
#endif
#define PL_VERSION "4.7" 

/* This is the generic argument structure,
 * it contains all of the options that you 
 * may need or want idk. */
typedef struct pl_arg {
	const char *name;        // the flag name eg --test or --help 
  const char *description; // flag description (used in help page) 
	int takes_value;         // tell parser if flag takes arg, 0 = false, 1 = true

	// used in the help page 
	const char *catagory; 

  /* shorthand of the same arg eg if the arg is
	 * --help maybe the sorthand is -h */
	const char *shorthand; 
	
	/* if you have takes_arg set to true you can add a 
	 * type string to the flag eg "void" or "char *". 
	 * this is used in the help page */
	const char *type;	

	// other misc metadata (overwritten on pl_a)
  char **value;
	int value_cap;
	int value_idx;

  int triggered;
	int shorthand_triggered;
} pl_arg;

// array of arguments 
extern pl_arg *PL_ARGS;

// amount of items in PL_ARGS 
extern int PL_ARGS_IDX;

// capacity of items in PL_ARGS 
extern int PL_ARGS_CAP;

// a local copy of whatever is parsed into pl_proc  
extern char** PL_ARGV;
extern int PL_ARGC;

// idx of last parsed argument
extern int PL_PROC_END_ARGC; 


/* most of the PLib functions use these codes 
 * as return values */
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


/* PL_INVERT is a silly function used to convert a negative 
 * number above and not equal to zero into a positive num - 1 */
#define PL_INVERT(n) ((n * -1) - 1)


/* list of pl_r codes and a string version, this is used in error 
 * handling as you can call PL_E to get a return code as a string */
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


/* takes in a string name and searches PL_ARGS to check if 
 * the parsed string exists as a argument flag in the args 
 * array. if it does return PL_SUCCESS, if not return 
 * PL_FALIURE */
pl_r pl_arg_exist(const char *name);


/* takes in an array of strings and a count and parses 
 * them in comparison to the arguments set using pl_a */ 
pl_r pl_proc(const int argc, const char *argv[]);


/* see if an pl_arg has been triggered (run) */
pl_r pl_arg_run(const pl_arg *local);


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


/* basic optimize macro for pl_arg_run, 
 * before after:
 * 
 * if(pl_arg_run(arg) == PL_SUCCESS){
 * ... 
 * }
 *
 * if(PL_R(arg)){
 * ...
 * }
 * */
#define PL_R(arg) (pl_arg_run(arg) == PL_SUCCESS) ? 1 : 0 


// wrapper for pl_arg_get_name...
#define PL_GETN(name) pl_arg_by_name(name)


/* get a pointer to an arg using index 
 * see pl_a and PL_A. */ 
#define PL_GETI(idx) &PL_ARGS[idx]


// auto grab argv and argc if they are named as such.
#define PL_PROC() pl_proc(argc,argv)


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

