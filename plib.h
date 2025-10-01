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
} pl_return_type;

#define PL_INVERT(n) ((n * -1) - 1)

// PLib string map for return codes
static const char *pl_return_type_string[12] = {
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
int pl_a(pl_arg in);

// get the pl_a struct of an arg by flag name 
pl_arg *pl_arg_by_name(const char *name);

// global pl_arg macro/function 
pl_arg *pl_arg_global_ptr(pl_arg in);

extern pl_arg *PL_ARGS;
extern int PL_ARGS_IDX;
extern int PL_ARGS_CAP;
extern int PL_PROC_END_ARGC;
extern char** PL_ARGV;
extern int PL_ARGC;

#define PL_GETN(name) pl_arg_by_name(name)
#define PL_GETI(idx) &PL_ARGS[idx]
#define PL_VERSION "4.7" 
#define pl_proc_at_i(i,argv) pl_proc_i(i,argv)
#define PL_PROC() pl_proc(argc,argv)
#define PL_A(...) pl_a((pl_arg){__VA_ARGS__})
#define pl_arg_get(name) &PL_ARGS[pl_arg_exist(name)]
#define PL_LAST_ARG (PL_PROC_END_ARGC >= 0) ? \
										(PL_ARGV[PL_PROC_END_ARGC]) : \
										pl_return_type_string[PL_INVERT(PL_NO_ARGUMENTS_GIVEN)]
#define PL_E(i) pl_return_type_string[PL_INVERT(i)]
#define pl_arg_sys(arg) (arg->triggered == 1) ? ((arg->shorthand_triggered == 1) ? arg->shorthand : arg->name) : NULL
#define PL_P(...) pl_arg_global_ptr((pl_arg){__VA_ARGS__})
#endif // PLIB_H

