// `plib 6.0.1`
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>


/**
 * @brief Bytes allocated for max argument value size.
 *
 * The max size for input arguments, keep this high.
 **/
#define PL_ARGM_S  512


/**
 * @brief Bytes allocated for the description of arguments.
 *
 * Bytes allocated for the `.desc` value of an argument.
 * @see `plib_Argument`
 **/
#define PL_DESC_S  128


/**
 * @brief Bytes for the flag name of an argument.
 *
 * Bytes allocated for the `.flag` value of an argument.
 * @see `plib_Argument`
 **/
#define PL_FLAG_S   16


/**
 * @brief Bytes allocated for the short flag name of an argument 
 *
 * Bytes allocated for the `.shrt` value of an argument 
 * @see `plib_Argument`
 **/
#define PL_SHRT_S    16


/** 
 * @brief Amount of max values an argument can take.
 *
 * The argument holds values by keeping an array of the 
 * indexes at which values are held in `argv`. This value 
 * is the MAX amount of indexes that an argument could 
 * hold.
 **/
#define PL_MAX_ARG_V 5


/** 
 * @brief Argument struct.
 *
 * This is the data-type that holds all information on 
 * an argument. This struct uses the defined values above 
 * to determine how much memory should be statically 
 * allocated.
 * @see `PL_MAX_ARG_V`
 * @see `PL_SHRT_S`
 * @see `PL_FLAG_S`
 * @see `PL_DESC_S`
 *
 * @param opt `uint8_t` holding all set values.
 * @param `idx` Index at which this argument was initialized.
 * @param *values Array of indexes pointing to string values in `argv`
 * @param flag Flag name of argument for example; `--help`.
 * @param `shrt` Short flag name of argument for example; `-h`.
 * @param `desc` Description of argument.
 **/
struct plib_Argument {
	uint8_t opt; 
	int idx;
	int  vals[PL_MAX_ARG_V];
	char flag[PL_FLAG_S];
	char shrt[PL_SHRT_S];
	char desc[PL_DESC_S];
};


/**
 * @brief General use `plib` return codes.
 *
 * These are returned from `plib_Parse`,
 * the names are quite self explanatory.
 *
 * @see `plib_Return`
 **/
enum {
	PL_SUCCESS,
	PL_RETURN_TYPE_END,
	PL_ARG_NONE,
	PL_ARG_NOT_FOUND,
	PL_ARG_VALUE,
	PL_ARG_NO_VALUE,
	PL_TO_MANY_VALUES
};


/** 
 * @brief return struct of `plib_Parse`.
 **/
struct plib_Return {
	int index;
	int code;
};


/**
 * @brief Amount of arguments.
 *
 * This value is incremented with each 
 * call of @ref `pl_a` and reflects the
 * amount of arguments created. Also 
 * the @ref `plib_Arguement` struct has 
 * the index of itself in the `.idx` 
 * property.
 **/
static int PL_ARG_IDX = 0;


/*
 * @brief Returns an initialized `plib_Argument`
 * @see `plib_Argument`
 **/
struct plib_Argument
pl_a()
{
	PL_ARG_IDX++;
	return (struct plib_Argument){.opt = 0u, .idx = 0};
}


// These macros all just modify a particular bit which is read as a boolean later on.
#define plib_SetRequired(a)            do { a->opt = a->opt ^ (1u << 3); } while(0)
#define plib_SetEnabled(a)             do { a->opt = a->opt ^ (1u << 2); } while(0)
#define plib_SetTakesMultipleValues(a) do { a->opt = a->opt ^ (1u << 1); } while(0)
#define plib_SetTakesValue(a)          do { a->opt = a->opt ^ (1u << 0); } while(0)


/**
 * @brief Loops through a range in argument array.
 *
 * This macro loops over each item inside an array
 * from indexes `n1` to `n2`. On each iteration the 
 * `in` variable will have the current iterated 
 * argument assigned to it as a pointer. Dissimilar to 
 * @ref `plib_CreateArgCount` this function does not 
 * need the size of the array. This is because in 
 * the function @ref `pl_a()` we increment the 
 * `PL_ARG_IDX` value each time when assigning each 
 * arguments `.idx` value. This means that the 
 * `PL_ARG_IDX` value holds the amount of total 
 * arguments that have been initialized and therefore 
 * we can just use that instead of needing an explicit 
 * array size variable.
 *
 * If you want to iterate through EVERY argument in 
 * an array then use @ref `plib_ForAll`.
 *
 * @param `n1` First index in range (start index).
 * @param `n2` Last index in range (end index).
 * @param `in` Input variable of type `struct plib_Argument *`.
 * @param `ar` Array of arguments.
 *
 * @see `plib_ForAll`
 **/
struct plib_Argument *PL_TMP_ARG;
#define plib_ForEach(n1, n2, ar) \
	PL_TMP_ARG = &(ar[n1]); \
	for(int i = n1; i < n2;i++, PL_TMP_ARG = &(ar[i]))

#define plib_Arg PL_TMP_ARG



/** 
 * @brief initializes a list of arguments 
 *
 * Simply loops through an array of arguments 
 * and uses @ref `pl_a()` to initialize each 
 * argument.
 *
 * @param `ar` Array of arguments.
 * @param `c` Amount of items in array.
 **/
#define plib_CreateArgCount(ar,c) \
	for(int i = 0; i < c;i++) \
		ar[i] = pl_a();


/**
 * @brief loop through all arguments in array.
 *
 * This is just @ref `plib_ForEach` but called 
 * with the size of the array given to it.
 *
 * @param `in` Input variable of type `struct plib_Argument *`.
 * @param `ar` Array of arguments.
 *
 * @see `plib_ForEach`
 **/
#define plib_ForAll(ar) \
	plib_ForEach(0, PL_ARG_IDX, ar)


/** 
 * @brief set the string values of an argument.
 * 
 * @param `a` Argument pointer.
 * @param `b` Flag name.
 * @param `c` Short flag name.
 * @param `e` Description.
 *
 * @see `plib_Argument`
 **/
#define plib_SetStringValues(a, b, c, e)   \
	do { \
		if(b) snprintf(a.flag, PL_FLAG_S, "--%s",b); \
		if(c) snprintf(a.shrt, PL_SHRT_S, "-%s",c);  \
		if(e) snprintf(a.desc, PL_DESC_S, "%s", e);  \
	} while (0)


int
comp(char *s1, char *s2)
{
    while (*s1 && !(*s1 - *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

static int pl_uint8_t_read(uint8_t *word, uint8_t n){return (*word & (1u << n));}

// Parse an array for arguments
struct plib_Return
plib_Parse(int c, char *v[], struct plib_Argument *ar)
{
	struct plib_Return out;
	
	// No arguments where provided
	if(c <= 1){
		out.index = 0;
		out.code = PL_ARG_NONE;
		return out;
	}
	
	// Loop through arguments 
	for(out.index = 1; out.index < c; ++out.index)
	{
		// Part one: Get the values of an argument.
		// ----------------------------------------
		char *arg = v[out.index];
		int arg_s = 0;
		int has_split = 0;

		// `strstr` without the `strstr` 
		while(arg[arg_s]){
			if(arg[arg_s] == '=')
				has_split = 1;
			arg_s++; 
		}

		char key[PL_ARGM_S];
		char val[PL_ARGM_S];
		int key_idx = 0;
		int val_idx = 0;

		bool toggle = false;

		// `strtok` without the `strtok`
		// Loop through letters of argument
		for(int i = 0; i < arg_s; i++){
			if(has_split && arg[i] == '='){
				toggle = true;
				continue;
			}

			// Separate values for the key and value.
			if(toggle) val[val_idx++] = arg[i];
			else       key[key_idx++] = arg[i];
			
			// Null-terminate strings
			if((i+1) == arg_s) 
				key[key_idx] = val[val_idx] = '\0';
		}
		
		// Part two: search for argument. 
		// ------------------------------
		struct plib_Argument *loc;	
		bool argument_found = false;


		// Search for argument in argument list
		for(int i = 0; i < PL_ARG_IDX; i++)
			if(comp(ar[i].flag, key) == 0){
				loc = &ar[i];
				argument_found = true;
			}
		
		// Argument was not found
		if(!argument_found){
			out.code = PL_ARG_NOT_FOUND;
			return out;
		}


		// Part three: Handle conditionals.
		// --------------------------------
		if (pl_uint8_t_read(&loc->opt, 0)){
			if(val[0] == '\0'){
				// Argument takes a value but none was provided
				out.code = PL_ARG_NO_VALUE;
				return out;
			}
		} else if (val[0] != '\0'){
			// Argument does not take a value but one was provided.
			out.code = PL_ARG_VALUE;
			return out;
		}

		// Part four: assignment.

		if((loc->idx == 1 && pl_uint8_t_read(&loc->opt, 1)) || loc->idx == PL_MAX_ARG_V){
			out.code = PL_TO_MANY_VALUES;
			return out;
		}

		loc->vals[loc->idx++] = out.index;
		printf("assigned value %d\n", loc->idx);
	}

	out.code = PL_SUCCESS;
	return out;
}


void
print_bits(uint8_t v)
{
    for (int i = 7; i >= 0; i--) 
        printf("%u", (v >> i) & 1);
    printf("\n");
}


int 
main(int argc, char *argv[])
{
	// Create argument list 
	struct plib_Argument pl[3];
	plib_CreateArgCount(pl, 3);
	
	plib_ForAll(pl){
		plib_SetTakesValue(plib_Arg);
	}
	
	plib_SetStringValues(pl[0], "test", "t", "test");
	
	
	plib_ForAll(pl){
		printf("%s - %d\n", plib_Arg->flag, plib_Arg->idx);
	}


	// Parse arguments
	struct plib_Return ret = 
		plib_Parse(argc, argv, pl);

	// Catch errors?
	if(ret.code != PL_SUCCESS){
		printf("Error occured with argument: %s (%d)", argv[ret.index], ret.code); 
	}

	printf("value: %d\n", pl[0].idx);


	// Done!

	return 0;
}
