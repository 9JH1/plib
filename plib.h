#ifndef PLIB3_H
#define PLIB3_H

typedef struct pl_arg {
  const char *name;
  const char *description;
  const char *catagory;

	// NEW:
	const char *shorthand;
	const char *type;

	// metadata: 
  char *value;
  int takes_value;
  int triggered;
} pl_arg;

typedef enum {
  ERROR,
  VERBOSE,
} mode;

typedef enum {
	PL_ARG_NOT_FOUND,
	PL_ARG_REQUIRES_VALUE,
	PL_ARG_NO_REQUIRES_VALUE,
	PL_NO_ARGUMENTS_GIVEN,
	PL_MEM_ALLOC_ERROR,
	PL_SUCCESS,
	PL_ARG_INVALID_FORMAT,
	PL_ARG_IS_NULL,
	PL_FAILURE
} pl_return_type;

static const char *pl_return_type_string[9] = {
    [PL_ARG_NOT_FOUND] = "ARG_NOT_FOUND",
    [PL_ARG_REQUIRES_VALUE] = "ARG_REQUIRES_VALUE",
    [PL_ARG_NO_REQUIRES_VALUE] = "ARG_NO_REQUIRES_VALUE",
		[PL_NO_ARGUMENTS_GIVEN] = "NO_ARGUMENTS_GIVEN",
		[PL_MEM_ALLOC_ERROR] = "MEM_ALLOC_ERROR",
		[PL_SUCCESS] = "SUCCESS",
		[PL_ARG_INVALID_FORMAT] = "ARG_INVALID_FORMAT",
		[PL_ARG_IS_NULL] = "ARG_IS_NULL",
		[PL_FAILURE] = "FAILURE"
};



typedef enum {
	NO_VALUE = 1,
  TAKES_VALUE = 2,
} set_argument_enum_values;


/** 
 * @breif return bool if arg exist or not 
 * @return int 
 * @param name of argument flag eg --test
 * @details this function takes in a name of a argument flag eg --test 
 * 					and will return a 0 if the argument exists or a 1 if the 
 * 					argument is not yet defined.
 */
int pl_arg_exist(const char *name);


/** 
 * @breif process arguments 
 * @return int 
 * @param argc amount of items in argv 
 * @param argv array of strings 
 * @details processes all arguments set before call, does not set 
 * 					program config, see pl_proc_s_i().
 **/
int pl_proc(const int argc, const char *argv[]);

/** 
 * @breif return bool if argument has been run 
 * @return int 
 * @param local local argument to check
 * @details takes in a local arguement and returns 1 if the argument 
 * 					has been run and returns a 0 if the argument has not been 
 * 					run.
 **/
int pl_arg_run(const pl_arg *local);

/** 
 * @breif returns bool if ALL defined arguments have been run 
 * @returns int 
 * @details checks if every single argument defined before call has been 
 * 					run, if so then return 1 if not return 0.
 **/
int pl_all_triggered(void); 

/**
 * @breif basic built in help dialog 
 * @details basic build in help dialog. 
 **/
void pl_help(void);

/**
 * @breif de-initialize the argument_list 
 * @details every time pl_a is run the arguement settings parsed into it 
 * 					are appended to an array that contains all of the other arguments 
 * 					of the program. This list is dynamically allocated meaning that we 
 * 					have to free that memory some way, calling this function will free
 * 					all of the memory held in argument_list.
 **/
void pl_exit(void);

/**
 * @breif get value of an argument 
 * @returns value of an argument 
 * @param local argument to get value of
 * @details this function takes in an argument and returns the value run with the 
 * 					argument, eg if --test=123 was run getting the pl_arg_value of that 
 * 					argument would return "123".
 **/
char* pl_arg_value(const pl_arg *local);

/**
 * @breif create an argument 
 * @return pointer to arguemnt in argument_list 
 * @param in shallow copy of an argument. 
 * @details This function is used to append a new argument to argument_list, this 
 * 					function can be used in shorthand like so: 
 * 					pl_arg *myArg = pl_a((pl_arg){.name = "--my-arg"});
 * 					see pl_exit().
 **/
pl_arg *pl_a(pl_arg in);

/** 
 * @breif get an argument pointer by flag name.
 * @return pointer to argument in argument_list 
 * @param name name of the flag you want to fetch 
 * @details use the name of a flag eg --help and get the pl_arg pointer from 
 * 					argument_list. this is useful if you dident take the pointer when using 
 * 					pl_a() or if you are in another scope and want to fetch the argument.
 **/
pl_arg *pl_arg_by_name(const char *name);


extern pl_arg *PL_ARGS;
extern int PL_ARGS_IDX;
extern int PL_ARGS_CAP;
extern int PL_PROC_END_ARGC;
extern int PL_VERBOSE;
extern char** PL_ARGV;
extern int  PL_ARGC;


#define pl_proc_at_i(i,argv) pl_proc_i(i,argv)
#define PL_PROC() pl_proc(argc,argv)
#define PL_A(...) pl_a((pl_arg){__VA_ARGS__})

/* issue occured when PL_PROC_END_ARGC was below 
 * 0, and pl_proc returned a PL_NO_ARGUMENTS_GIVEN 
 * core. this case caused a segfault, this happened 
 * because in plib3.c PL_PROC_END_ARGC is defined 
 * as -1, when PL_NO_ARGUMENTS_GIVEN returns it means 
 * that pl_proc has skipped looping through the arguments 
 * all togeather which means that when PL_LAST_ARG is called 
 * it trys to access the -1 index of PL_ARGV which 
 * is what causes the seg fault, this has been fixed 
 * by running an inline if statement to check if 
 * PL_PROC_END_ARG is defined. 2025-09-19 18:11 */
#define PL_LAST_ARG (PL_PROC_END_ARGC >= 0) ? \
										(PL_ARGV[PL_PROC_END_ARGC]) : \
										pl_return_type_string[PL_NO_ARGUMENTS_GIVEN]


#endif // PLIB3_H
