#ifndef PLIB3_H
#define PLIB3_H


typedef struct pl_arg {
  const char *name;
  const char *description;
  const char *catagory;
  char *value;
  int takes_value;
  int triggered;
} pl_arg;

typedef struct pl_prog {
	const char *name;
	const char *version;
	const char *description;
} pl_prog;


typedef enum {
  ERROR,
  VERBOSE,
} mode;


typedef enum {
	NO_VALUE = 1,
  TAKES_VALUE = 2,
} set_argument_enum_values;

/** 
 * @breif process system arguments AND set program settings 
 * @return int 
 * @param argc: amount of items in argv 
 * @param argv: array of arguments 
 * @details pl_proc_s_i is a wrapper for pl_proc_i, this function takes 
 * 					in an array of strings, the amount of those strings and a
 * 					struct that is set as the program settings, the program 
 * 					settings are used in places like pl_help and other outputs.
 **/
int pl_proc_s_i(const int argc, const char *argv[], const pl_prog opt);

/** 
 * @breif verbose print function 
 * @return int 
 * @param mode mode of print see mode enum 
 * @param LINE line number function called from 
 * @param FILE file function called from 
 * @param format print message
 * @param ... other values 
 * @details this is a wrapper for printf that also allows the user to 
 * 					pre-load styles depending on the type of output. The two 
 * 					outputs available are ERROR and VERBOSE, LINE and FILE can 
 * 					be optained using __LINE__ and __FILE__. see pl_v()
 **/
int pl_v_i(mode mode, const int LINE, const char *FILE, const char *format, ...);

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
int pl_proc_i(const int argc, const char *argv[]);

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

extern pl_arg *argument_list;
extern int argument_list_index;
extern int argument_list_capacity;


#define USE_FNAME 0
#define pl_v(a, c, ...) pl_v_i(a, __LINE__, __FILE__, c, ##__VA_ARGS__)
#define pl_proc() pl_proc_i(argc,argv)
#define pl_proc_s(opt) pl_proc_s_i(argc,argv,opt)

#endif // PLIB3_H
