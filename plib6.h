// `plib 6.0.1`
#ifndef PLIB_6
#define PLIB_6

#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdbool.h>

/**
 * @brief maximum amount of values an argument can hold.
 **/
#define PL_MAX_ARG_V 64


/**
 * @brief return codes
 **/
enum {
	PL_SUCCESS, PL_RETURN_TYPE_END, PL_ARG_NONE,
	PL_ARG_NOT_FOUND, PL_ARG_VALUE, PL_ARG_NO_VALUE,
	PL_TO_MANY_VALUES, PL_NO_REQUIRED_ARG
};

/**
 * @brief contains all arguemnt information
 *
 * @param flag flagname like --flag
 * @param shrt short flag name like -f
 * @param desc description
 * @param opt uint8_t byte holding toggleable propertys.
 * @param idx amount of values the argument has
 * @param vals array of indexes pointing to values in argv.
 **/
struct plib_Argument {
	char *flag;
	char *shrt;
	char *desc;
	
	uint8_t opt; 
	int idx;
	int  vals[PL_MAX_ARG_V];
};

/**
 * @brief return struct for parse function
 **/
struct plib_Return {
	int index;
	int code;
};

/**
 * @brief amount of defined arguments updated through use of @ref plib_CreateArgCount
 **/
static int PL_ARG_IDX = 0;

/**
 * @brief global-local argument pointer used in loops.
 *
 * when in a for loop of any kind the variable plib_Arg
 * is assigned the pointer value of the current iterated
 * argument in the for loop.
 **/
static struct plib_Argument *plib_Arg;

/**
 * @brief global return struct containing last parse output.
 **/
static struct plib_Return PL_RETURN = (struct plib_Return){0};

/**
 * @brief returns the value of a bit at a certain index.
 **/
#define _rb(word, n) (((word) >> (n)) & 1u)

/**
 * @brief options for arguments
 **/
enum { PLIB_SETTAKESVALUE, PLIB_SETENABLED, 
	PLIB_SETTAKESVALUES, PLIB_SETREQUIRED, 
	PLIB_SETENDARGUMENTLIST 
};

/**
 * @brief we have strcmp at home!
 * @param s1 string one
 * @param s2 string two
 * @return int
 **/
static int
comp(char *s1, char *s2)
{
	if(!s1 || !s2) return -1;
    while (*s1 && !(*s1 - *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/**
 * @brief parse system arguments
 **/
static int
plib_Parse(int c, char *v[], struct plib_Argument *ar, char split_char)
{
	struct plib_Return *out = &PL_RETURN;
	
	// No arguments where provided
	if(c <= 1)
		return out->code = PL_ARG_NONE;
	
	// Loop through arguments 
	for(out->index = 1; out->index < c; ++out->index)
	{
		int arg_s = 0,
			split = 0;

		// `strstr` without the `strstr` 
		while(v[out->index][arg_s++])
			if(v[out->index][arg_s] == split_char)
				split = arg_s;

		char key[arg_s-split];

		// Separate key and value
		for(int i = 0; i < (split ? split : arg_s)+1; i++){
			if(i <= (split ? split : arg_s)){
				key[i] = v[out->index][i];
				if(split && i == split) 
					key[i] = '\0';
				else if (i == arg_s)
					v[out->index][0] = '\0';
			}
			
			if (split)
				v[out->index][i] = v[out->index][i+split+1];
		}

		arg_s = -1;

		// Search for argument in argument list
		for(int i = 0; i < PL_ARG_IDX; i++){
			if(comp(ar[i].flag, key) == 0 || comp(ar[i].shrt, key) == 0)
				arg_s = i;
		}

		// Argument was not found
		if(arg_s == -1) 
			return out->code = PL_ARG_NOT_FOUND;

		// Argument is not enabled
		else if(!_rb(ar[arg_s].opt, PLIB_SETENABLED)) 
			continue;
		
		// Argument takes a value but none was provided
		else if (_rb(ar[arg_s].opt, PLIB_SETTAKESVALUE) && (key[0] == '\0' || comp(v[out->index], key) == 0)) 
			return out->code = PL_ARG_NO_VALUE;

		// Argument does not take a value but one was provided.
		else if (!_rb(ar[arg_s].opt, PLIB_SETTAKESVALUE) && v[out->index][0] != '\0') 
			return out->code = PL_ARG_VALUE;

		// Check if argument can hold any more values
		else if((ar[arg_s].idx >= 1 && !_rb(ar[arg_s].opt, PLIB_SETTAKESVALUES)))// || ar[arg_s].idx >= PL_MAX_ARG_V)
			return out->code = PL_TO_MANY_VALUES;

		// Assign the value
		ar[arg_s].vals[ar[arg_s].idx++] = out->index;
	}

	// Check for required arguments 
	for(int i=0; i < PL_ARG_IDX; i++)
		if(_rb(ar[i].opt, PLIB_SETREQUIRED) && !ar[i].idx){
			out->index = 0;
			return out->code = PL_NO_REQUIRED_ARG;
		}

	return out->code = PL_SUCCESS;
}

/**
 * @brief toggles a bit at a certain index. use the argument enum for more
 **/
#define plib_ToggleProperty(a,p) \
	a->opt = a->opt ^ (1u << p)


/**
 * @brief iterate through a range of arguments
 **/
#define plib_ForEach(n1, n2, ar) \
	plib_Arg = &(ar[n1]); \
	for(int i = n1; i < n2;i++, \
			plib_Arg = &(ar[i]))

/**
 * @brief initialize arguments
 **/
#define plib_CreateArgCount(ar) \
	PL_ARG_IDX += sizeof(ar) / sizeof(struct plib_Argument); \
	plib_ForEach(0, sizeof(ar) / sizeof(struct plib_Argument), ar){ \
		plib_Arg->opt = (1u << PLIB_SETENABLED); \
		plib_Arg->idx = 0; \
	}

/**
 * @brief loop through all arguments
 **/
#define plib_ForAll(ar) \
	plib_ForEach(0, PL_ARG_IDX, ar)

/**
 * @brief use a single argument
 **/
#define plib_For(a) \
	plib_Arg = &(a); \
	for(int i = 0; i < 1; i++)

/**
 * @brief initialize arguments and start a forall loop
 **/
#define plib_CreateArgAndForAll(ar) \
	plib_CreateArgCount(ar) \
	plib_ForAll(ar) 

/**
 * @brief conditional simplicity function
 **/
#define ifnot_plib_Parse(ar) \
	if (plib_Parse(argc, argv, ar, '=') != PL_SUCCESS)

/**
 * @brief conditional if argument was run.
 **/
#define plib_WasRun(a) (a->idx > 0)

/**
 * @brief get argument count
 **/
#define plib_ArgValueCount(a) a->idx

/**
 * @brief get an arguments value at a certain index (with basic error checking)
 **/
#define plib_GetArgValue(a, n, arv) (n <= a->idx) ? arv[a->vals[n]] : NULL

#endif
