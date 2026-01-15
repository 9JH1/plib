// `plib 6.0.1`
#ifndef PLIB_6
#define PLIB_6


#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdbool.h>

// Maximum amount of 
// values that can be 
// all allocated to an
// argument.
#define PL_MAX_ARG_V 64

// Amount of bytes an argument
// can be at a maximum. If an
// argument string parsed in
// exceeds this then plib will
// crash.
#define PL_MAX_VALUE_S 512

// Printf function type used in the plib_HelpMenu 
// function.
typedef int (*plib_PrintFuncType)(const char *, ...);

// Return codes.
enum {
	PL_SUCCESS, PL_ARG_NONE,
	PL_ARG_NOT_FOUND, PL_ARG_VALUE, PL_ARG_NO_VALUE,
	PL_TO_MANY_VALUES, PL_NO_REQUIRED_ARG,
    PL_INTERNAL_ERROR,
	__PL_RETURN_COUNT__
};

// Array of strings representing the 
// various error codes, makes the 
// display part of error handling 
// easier on the development end.
static const char *_plib_return_strings[__PL_RETURN_COUNT__] = {
	[PL_SUCCESS]         = "SUCCESS",
	[PL_ARG_NONE]        = "ARG_NONE",
	[PL_ARG_NOT_FOUND]   = "ARG_NOT_FOUND",
	[PL_ARG_VALUE]       = "ARG_VALUE",
	[PL_ARG_NO_VALUE]    = "ARG_NO_VALUE",
	[PL_TO_MANY_VALUES]  = "TO_MANY_VALUES",
	[PL_NO_REQUIRED_ARG] = "NO_REQUIRED_ARG",
	[PL_INTERNAL_ERROR]  = "INTERNAL_ERROR"
};

// Structure of an 
// argument.
struct plib_Argument {
	char *flag;
	char *shrt;
	char *desc;
	char *catg; 
	
	uint8_t opt; 
	int idx;
	int  vals[PL_MAX_ARG_V];
};

// Struture of a 
// return value from
// the parsing 
// function.
struct plib_Return {
	int index;
	int code;

};

// Count of initialized
// arguments, used for 
// loops as otherwise the
// user would need to 
// give the count of
// arguments manually.
static int PL_ARG_IDX = 0;

// Local copy of argv 
// for use in error 
// handling.
static char **PL_ARGV;

// Pointer argument used for iteration
// loops. 
static struct plib_Argument *plib_Arg;

// Global return code of plib_Parse, used for error handling.
static struct plib_Return PL_RETURN = (struct plib_Return){0};

// Read the state of a property on an argument.
#define plib_ReadProperty(word, n) (word & (1u << n))

// Property list.
enum { 
	PLIB_TAKESVALUE, PLIB_ENABLED, 
	PLIB_TAKESVALUES, PLIB_REQUIRED, 
	PLIB_ENDARGUMENTLIST 
};

// strcmp implementation.
static int
_plib_strcmp (char *s1, char *s2)
{if (!s1 || !s2) return -1;
    while (*s1 && !(*s1 - *s2))
	  {
        s1++;
        s2++;
      }
    return *s1 - *s2;
}

// strlen implementation.
static int
_plib_strlen (char *str)
{if (!str) return -1;
	int str_s = 0;
	while(str[str_s])
		str_s++;

	return str_s+1;
}

#define undo_return(e, v, u) \
	do { \
		v[0] = '\0'; \
		const int u_s = _plib_strlen(u); \
		for (int j = 0; j < u_s; j++) \
			v[j] = u[j]; \
		return e; \
	} while(0)

// Iterates through all string arguments
// and parses them with the provided 
// plib_Argument struct array and then 
// handles the arguments accordingly. 
static int
plib_Parse (int c, char *v[], struct plib_Argument *ar, char split_char)
{
	struct plib_Return *out = &PL_RETURN;
	
	// No arguments where provided
	if (c <= 1)
		return out->code = PL_ARG_NONE;

	// Loop through argument strings
	for (out->index = 1; out->index < c; ++out->index)
	  {
		char undo[PL_MAX_VALUE_S];
		undo[0] = '\0';

		// Undo file is used to remove edits to argv if 
		// an error occurs. if the arguments length is
		// more then PL_MAX_VALUE_S then we won't have 
		// enough space to hold the original version 
		// of the string argument. We throw this error
		// to ensure nothing bad occurs.
		if (_plib_strlen(v[out->index]) > PL_MAX_VALUE_S)
			return out->code = PL_INTERNAL_ERROR;

		// Copy argument over 
		for( int i = 0; i < _plib_strlen(v[out->index]); i++)
			undo[i] = v[out->index][i];

		int arg_s = 0,
			split = 0;

		// locate occurances of split_char in argument
		while (v[out->index][arg_s++])
			if (v[out->index][arg_s] == split_char)
				split = arg_s;


		// Separate key and value from argument string
		char key[arg_s-split];
		for (int i = 0; i < arg_s; i++)
		  {
			if (i <= (split ? split : arg_s))
			  {
				key[i] = v[out->index][i];
				if (split && i == split) 
					key[i] = '\0';
				
				else if (i == arg_s) 
					v[out->index][0] = '\0';
			  }
			
			if (split) 
				v[out->index][i] = v[out->index][i+split+1];
		}

		// Search for argument in plib_Argument list
		arg_s = -1;
		for (int i = 0; i < PL_ARG_IDX; i++)
			if (_plib_strcmp(ar[i].flag, key) == 0 || _plib_strcmp(ar[i].shrt, key) == 0)
				arg_s = i;
		
		// Argument was not found
		if (arg_s == -1)
			undo_return(out->code = PL_ARG_NOT_FOUND, v[out->index], undo);

		// Argument propertys
		const int val_provided    = (split != 0 || v[out->index][0] != '\0');
		const int arg_takes_val   = (plib_ReadProperty(ar[arg_s].opt, PLIB_TAKESVALUE) == 1);
		const int arg_enabled     = plib_ReadProperty(ar[arg_s].opt, PLIB_ENABLED);
		const int arg_value_count = ar[arg_s].idx; 
		const int arg_multi_val   = plib_ReadProperty(ar[arg_s].opt, PLIB_TAKESVALUES);

		// Conditionals
		if (!arg_enabled) continue;

		else if (arg_takes_val && !val_provided)
			undo_return(out->code = PL_ARG_NO_VALUE, v[out->index], undo);

		else if (!arg_takes_val && val_provided)
			undo_return(out->code = PL_ARG_VALUE, v[out->index],  undo);

		else if ((arg_value_count && !arg_multi_val) || ar[arg_s].idx >= PL_MAX_ARG_V)
			undo_return(out->code = PL_TO_MANY_VALUES, v[out->index], undo);

		// Assign the value
		ar[arg_s].vals[ar[arg_s].idx++] = out->index;
	}

	// Check for required arguments 
	for (int i=0; i < PL_ARG_IDX; i++)
		if (plib_ReadProperty(ar[i].opt, PLIB_REQUIRED) && !ar[i].idx)
		  {
			out->index = 0;
			return out->code = PL_NO_REQUIRED_ARG;
		  }

	// Exit successfully
	return out->code = PL_SUCCESS;
}

// Prints a basic help menu
static void
_plib_HelpMenu(struct plib_Argument *ar, plib_PrintFuncType print)
{ if (PL_ARG_IDX <= 0)
	  {
		print("No arguments set\n"); 
		return;
	  }

	// Print string
	#define print_space(str, len) \
		do { \
			print("%s", str); \
			int c_s = 0; while(str[c_s++]); \
			for(int i = 0; i < len - c_s; i++) print(" "); \
			print(" | "); \
		} while(0)

	int flag_s = -1;
	int shrt_s = -1;

	// TODO: FIX THIS CRAP
	// Get spacing values:
	for (int i = 0; i < PL_ARG_IDX; i++)
	  {
		if (flag_s == -1)
		  {
			for (int j = 0;j < PL_ARG_IDX; j++)
			  {
				if (shrt_s == -1)
				  {
					int loc_s = 0;
					while (ar[j].shrt[loc_s++]);
				
					if (loc_s > flag_s)
						shrt_s = loc_s;
				  }

				int loc_s = 0;

				while (ar[j].flag[loc_s++]);
				if (loc_s > flag_s)
					flag_s = loc_s;
			  }
		  }

		// Print argument data 
		struct plib_Argument loc = ar[i];
		print_space (loc.flag, flag_s);
		print_space (loc.shrt, shrt_s);
		print ("%s\n", loc.desc);
	  }
	#undef print_space
}

// Toggles a property on an argument.
#define plib_ToggleProperty(a,p) \
	a->opt = a->opt ^ (1u << p)

// Iterate through each argument in 
// a range.
#define plib_ForEach(n1, n2, ar) \
	plib_Arg = &(ar[n1]); \
	for (int i = n1; i < n2;i++, \
			plib_Arg = &(ar[i]))

// Intialize arguments.
#define plib_CreateArgCount(ar) \
	PL_ARG_IDX = sizeof (ar) / sizeof (struct plib_Argument); \
	plib_ForEach (0, PL_ARG_IDX, ar){ \
		plib_Arg->opt = 0u; \
		plib_Arg->idx = 0; \
	}

// Iterate through ALL arguments.
#define plib_ForAll(ar) \
	plib_ForEach (0, PL_ARG_IDX, ar)

// Enter a single loop for an
// individual argument.
#define plib_For(a) \
	plib_Arg = &(a); \
	for (int i = 0; i < 1; i++)

// Intiailizes arguments and acts as 
// a for all loop after.
#define plib_CreateArgAndForAll(ar) \
	plib_CreateArgCount (ar) \
	plib_ForAll (ar) 

// Display help menu of 
// arguments in argument 
// list, this macro takes
// in printf automatically 
// which is used for printing
// values, you can customize
// the print function in this
// case using either a 
// wrapper around sprintf
// and a filter to customize
// the colors of the help
// menu, OR you can of course
// just re-write your own help
// function.
 #define plib_HelpMenu(ar) \
 	_plib_HelpMenu (ar, printf)

// Returns string version
// of an error code.
#define plib_Error \
   	_plib_return_strings[PL_RETURN.code]

// Parses arguments and acts as an if statement, the 
// scope will run if plib_Parse fails to run. This 
// macro takes in argc and argv automatically, but 
// it will fail to function if argc and argv are not 
// defined. also note that the spit_char value '=' 
// has been provided. If you want to change this char
// you will need to use the implicit function. 
#define ifnot_plib_Parse(ar) \
	if (plib_Parse(argc, argv, ar, '=') != PL_SUCCESS)

// returns argv value of last 
// parsed string argument
#define plib_ErrorArgument \
	argv[PL_RETURN.index]

// Conditional for if an argument was
// run.
// #define plib_ArgWasRun(a) (a->idx > 0)

// Get the amount of values assigned 
// to an argument.
// #define plib_ArgValueCount(a) a->idx

// Returns a value at a certain index,
// has error checking for null values.
// assumes pointer a.
// #define plib_ArgGetValue(a, n) \
//	(n <= a->idx) ? argv[a->vals[n]] : NULL

// Returns the first value of an 
//  argument. assumes static a.
//#define plib_ArgGetFirstValue(a) \
//	argv[a.vals[0]]

#define plib_ArgRun(a) (a->idx > 0)
#define plib_SArgRun(a) (a.idx > 0)

#define plib_ArgValueCount(a) a->idx
#define plib_SArgValueCount(a) a.idx

#define plib_ArgValue(a, n) \
	(n < a->idx) ? argv[a->vals[n]] : NULL
#define plib_SArgValue(a, n) \
	(n < a.idx) ? argv[a.vals[n]] : NULL

#define plib_ArgGetFirstValue(a) \
	(a->idx > 0) ? argv[a->vals[0]] : NULL
#define plib_SArgGetFirstValue(a) \
	(a.idx > 0) ? argv[a.vals[0]] : NULL

#endif
