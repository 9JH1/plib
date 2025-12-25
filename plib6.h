// `plib 6.0.1`
#ifndef PLIB_6
#define PLIB_6

#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdbool.h>

#define PL_MAX_ARG_V 64


struct plib_Argument {
	char *flag;
	char *shrt;
	char *desc;
	
	uint8_t opt; 
	int idx;
	int  vals[PL_MAX_ARG_V];
};


struct plib_Return {
	int index;
	int code;
};


static int PL_ARG_IDX = 0;
static struct plib_Argument *plib_Arg;
static struct plib_Argument plib_Create_Arg;
static struct plib_Return PL_RETURN;


enum {
	PL_SUCCESS, PL_RETURN_TYPE_END, PL_ARG_NONE,
	PL_ARG_NOT_FOUND, PL_ARG_VALUE, PL_ARG_NO_VALUE,
	PL_TO_MANY_VALUES
};


#define plib_SetRequired(a)            a->opt = a->opt ^ (1u << 3);
#define plib_SetEnabled(a)             a->opt = a->opt ^ (1u << 2);
#define plib_SetTakesMultipleValues(a) a->opt = a->opt ^ (1u << 1);
#define plib_SetTakesValue(a)          a->opt = a->opt ^ (1u << 0);

#define plib_ArgValueCount(a) a->idx
#define plib_GetArgValue(a, n, arv) (n <= a->idx) ? arv[a->vals[n]] : NULL

#define plib_ForEach(n1, n2, ar) \
	plib_Arg = &(ar[n1]);         \
	for(int i = n1; i < n2;i++,    \
			plib_Arg = &(ar[i]))

#define plib_CreateArgCount(ar,c) \
	PL_ARG_IDX += c;               \
	plib_ForEach(0, c, ar){         \
		plib_Arg->opt = (1u << 2);   \
		plib_Arg->idx = 0;            \
		if (plib_Arg->flag == NULL) \
			plib_SetEnabled(plib_Arg) \
	}

#define plib_ForAll(ar) \
	plib_ForEach(0, PL_ARG_IDX, ar)


#define plib_InitArgForAll(ar)                      \
	plib_CreateArgCount(ar, sizeof(ar) / sizeof(struct plib_Argument)) \
	plib_ForAll(ar) 


#define plib_WasRun(a) \
	(a->idx > 0)

#define plib_ArgEnabled(a) \
	(a->opt & (1u << 2))

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

#include <stdio.h>

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
		char key[arg_s-split];

		// `strstr` without the `strstr` 
		while(v[out->index][arg_s++])
			if(v[out->index][arg_s] == split_char)
				split = arg_s;

		// Separate key and value
		for(int i = 0; i < split+1; i++){
			if(i <= split){
				key[i] = v[out->index][i];
				if(i == split) 
					key[i] = '\0';
			}
			
			if (split)
				v[out->index][i] = v[out->index][i+split+1];
		}

		printf("%s - %s\n", key, v[out->index]);
		
		arg_s = -1;

		// Search for argument in argument list
		for(int i = 0; i < PL_ARG_IDX; i++)
			if(comp(ar[i].flag, key) == 0 || comp(ar[i].shrt, key) == 0) 
				arg_s = i;

		// Argument was not found
		if(arg_s == -1) 
			return out->code = PL_ARG_NOT_FOUND;

		// Argument is not enabled
		else if(!(ar[arg_s].opt & (1u << 2))) 
			continue;
		
		// Argument takes a value but none was provided
		else if (ar[arg_s].opt & 1u && (key[0] == '\0' || comp(v[out->index], key) == 0)) 
			return out->code = PL_ARG_NO_VALUE;

		// Argument does not take a value but one was provided.
		else if (!(ar[arg_s].opt & 1u) && v[out->index][0] != '\0') 
			return out->code = PL_ARG_VALUE;

		// Check if argument can hold any more values
		else if((ar[arg_s].idx == 1 && (ar[arg_s].opt & (1u << 1))) || ar[arg_s].idx == PL_MAX_ARG_V)
			return out->code = PL_TO_MANY_VALUES;

		// Assign the value
		ar[arg_s].vals[ar[arg_s].idx++] = out->index;
	}

	return out->code = PL_SUCCESS;
}
#endif
