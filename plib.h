#ifndef PLIB_H
#define PLIB_H

/* This is the generic argument structure,
 * it contains all of the options that you 
 * may need or want idk. */
typedef struct pl_arg {
  // basic options
	const char *name;
  const char *description;
	int takes_value;

	// extended options
	const char *catagory; 
	const char *shorthand;
	const char *type;	

	// metadata ( this is overwritten )
  char *value;
  int triggered;
	int shorthand_triggered;
} pl_arg;


// PLib return codes
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

// PLib string map for return codes
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

// returns bool of if flag exists
int pl_arg_exist(const char *name);

// parses arguments
int pl_proc(const int argc, const char *argv[]);

// returns bool of if arg run 
int pl_arg_run(const pl_arg *local);

// returns bool of if ALL args have been run 
int pl_all_triggered(void); 

// prints out argument info 
void pl_help(void);

// de-allocates PL_ARGS 
void pl_exit(void);

// return the value of an arg 
char* pl_arg_value(const pl_arg *local);

// create an argument 
pl_arg *pl_a(pl_arg in);

// get the pl_a struct of an arg by flag name 
pl_arg *pl_arg_by_name(const char *name);

extern pl_arg *PL_ARGS;
extern int PL_ARGS_IDX;
extern int PL_ARGS_CAP;
extern int PL_PROC_END_ARGC;
extern char** PL_ARGV;
extern int PL_ARGC;

#define PL_VERBOSE 1
#define PL_VERSION "4.21" 
#define pl_proc_at_i(i,argv) pl_proc_i(i,argv)
#define PL_PROC() pl_proc(argc,argv)
#define PL_A(...) pl_a((pl_arg){__VA_ARGS__})
#define pl_arg_get(name) &PL_ARGS[pl_arg_exist(name)]
#define PL_LAST_ARG (PL_PROC_END_ARGC >= 0) ? \
										(PL_ARGV[PL_PROC_END_ARGC]) : \
										pl_return_type_string[PL_NO_ARGUMENTS_GIVEN]
#define PL_E(i) pl_return_type_string[i]
#endif // PLIB_H
