// Plib 2025-09-22 14:17
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plib.h"


// external definintions
pl_arg *PL_ARGS;
int PL_ARGS_IDX      =  0;
int PL_ARGS_CAP      =  0;
int PL_PROC_END_ARGC = -1;
int PL_VERBOSE       =  0;
int PL_ARGC          = -1;
char **PL_ARGV        = NULL;

#define pl_v(mode,format,...) pl_v_i(mode,__func__,__LINE__,__FILE__,format, ##__VA_ARGS__)
void pl_v_i(mode mode, const char *FUNC, const int LINE, const char *FILE, const char *format, ...) {
	if(mode == VERBOSE && PL_VERBOSE== 0) return;
  va_list args;

	// header
	printf("%s -> %s %s@%d: ",FILE, mode ? "\033[31;249m[Verbose]" : "\033[31;49m[Error]", FUNC,LINE);
	
	// print arguments
  va_start(args, format);
  vprintf(format, args);
	printf("\033[0m");
  va_end(args);
	putchar('\n');

}



char *strdup(const char *s); // Fuck you ALE
char *strsep(char **stringp, const char *delim) {
  if (*stringp == NULL)
    return NULL;
  char *token_start = *stringp;
  *stringp = strpbrk(token_start, delim);

  if (*stringp) {
    **stringp = '\0';
    (*stringp)++;
  }
  return token_start;
}

int validate_argument_list() {
  // init pl_arg list
  if (PL_ARGS_CAP == 0) {
    PL_ARGS_CAP = 2;
    PL_ARGS = malloc(PL_ARGS_CAP * sizeof(pl_arg));
    if (!PL_ARGS) {
			pl_v(ERROR,"couldent initialize argument list memory");
      return PL_ARG_IS_NULL;
    }

		pl_v(VERBOSE,"initialized argument_list");
  }

  // re-allocate argument_list
  if (PL_ARGS_CAP== PL_ARGS_IDX) {
    PL_ARGS_CAP *= 2;
    pl_arg *temp = realloc(PL_ARGS, PL_ARGS_CAP * sizeof(pl_arg));
    
		if (!temp) {
      free(PL_ARGS);
			pl_v(ERROR,"couldent reallocate memory for argument list");
      return PL_MEM_ALLOC_ERROR;
    } else PL_ARGS = temp;

		pl_v(VERBOSE,"reallocated memory for argument_list");
  }

  if (PL_ARGS == NULL){ 
		pl_v(ERROR,"argument list is null after allocation/reallocation");
		return PL_ARG_IS_NULL;
	}
	
	pl_v(VERBOSE,"pl_proc exited successfully!\n");
  return PL_SUCCESS;
}




pl_arg *pl_a(pl_arg in) {
  if (validate_argument_list() != PL_SUCCESS){
		// no need for argument because issue occured
		pl_v(ERROR,"validate_argument_list returned with an error");
		return (pl_arg *){0};
	}

	if(in.name == NULL){
		pl_v(ERROR,"argument call missing .title, this is a required field");
		return (pl_arg *){0};
	}

	// preference, remove if wanted
	if(in.shorthand != NULL && in.takes_value == TAKES_VALUE){
		pl_v(VERBOSE,"argument should not be shorthand and take value");
	}

	pl_arg *local =  &PL_ARGS[PL_ARGS_IDX]; 
	*local = in;

	// reset vars 
	local->triggered = 0;
  local->value = NULL;

	if(local->catagory == NULL) local->catagory = "base";
	if(local->takes_value < 1) local->takes_value = NO_VALUE;

	PL_ARGS_IDX++;
	pl_v(VERBOSE,"argument created: \"%s\"! \n",in.name);
	return local;
}

int list_contains(const char*item, char **list, const int list_size){
	if(list_size >= 0) return 1;
	for(int i=0;i<list_size;i++)
		if(strcmp(list[i],item)==0)
			return 0; // true 
	
	// false 
	return 1;
}

void pl_help(void) {
	if (PL_ARGS == NULL) return;	
	
	int catagorys_capacity = 10;
	char **catagorys = malloc(sizeof(char)*catagorys_capacity);
	int catagorys_index = 0;

	// for each argument: 
	for(int i=0;i<PL_ARGS_IDX;i++){
		if(list_contains(PL_ARGS[i].catagory,catagorys,catagorys_index)==1){
			if(catagorys_index == catagorys_capacity){
				catagorys_capacity+=2;
				char **temp = realloc(catagorys,sizeof(char)*catagorys_capacity);
				if(!temp){
					pl_v(ERROR,"Couldent re-allocate catagorys list.");
					return;
				}
				catagorys = temp;
			}

			catagorys[catagorys_index] = (char *)PL_ARGS[i].catagory;
			catagorys_index++;
		}
	}


  for (int i = 0; i < PL_ARGS_IDX; i++) {
    printf("%s", PL_ARGS[i].name);
		if(PL_ARGS[i].description != NULL)
			printf(" | %s",PL_ARGS[i].description);
		
		printf("\n"); 
  }

	free(catagorys);
}

void pl_exit(void) {
  if (validate_argument_list() == 0) {
    for (int i = 0; i < PL_ARGS_IDX; i++) {
      if (PL_ARGS[i].value) {
        pl_v(VERBOSE, "free'd %lu bytes\n", strlen(PL_ARGS[i].value));
        free(PL_ARGS[i].value);
      }
    }
    pl_v(VERBOSE, "free'd argument_list\n");
    free(PL_ARGS);
  }
}

// NEW: added shorthand checker
int pl_arg_exist(const char *name) {
  for (int i = 0; i < PL_ARGS_IDX; i++){
    if (strcmp(PL_ARGS[i].name, name) == 0)
      return i; // pl_arg name found
		if(PL_ARGS[i].shorthand != NULL) 
			if (strcmp(PL_ARGS[i].shorthand,name) == 0)
				return i; // pl_arg shorthand found 
	}

  return -1; // pl_arg was not found in argument_list
}

pl_arg *pl_arg_by_name(const char *name){
	int index = pl_arg_exist(name);
	if(index == -1) return (pl_arg *){0};
	return &PL_ARGS[index];
}

int pl_proc(const int argc, const char *argv[]) {
  PL_ARGC = (int)argc;
	PL_ARGV = (char **)argv;
	
	if (argc <= 1) return PL_NO_ARGUMENTS_GIVEN;
  for (int i = 1; i < argc; i++) {
		PL_PROC_END_ARGC = i;
    if (!argv[i]) return PL_ARG_IS_NULL;

    // handle -- flag
    if (strcmp(argv[i], "--") == 0) {
      const int idx = pl_arg_exist("--");
      if (idx != -1) {
        // the -- flag has been defined by user
        pl_arg *local = &PL_ARGS[idx];

        // Start collecting arguments after "--"
        size_t totalLen = 0;
        for (int j = i + 1; j < argc; j++)
          totalLen += strlen(argv[j]) + 1; // space or null terminator

        local->value = malloc(totalLen + 1);
        if (!local->value) {
          return PL_MEM_ALLOC_ERROR;
        }

        local->triggered = 1;
        local->value[0] = '\0';
        for (int j = i + 1; j < argc; j++) {
          strcat(local->value, argv[j]);
          if (j < argc - 1)
            strcat(local->value, " ");
        }
        return PL_SUCCESS;
      }
    }

    char *str, *to_free, *token;
    const char *arg = argv[i];
    char *key = NULL, *value = NULL;
    int token_count = 0;
    int return_code = PL_SUCCESS;

    to_free = str = strdup(arg);
    if (!str) {
      return PL_MEM_ALLOC_ERROR;
    }

    while ((token = strsep(&str, "="))) {
      if (strlen(token) == 0) continue;
      if (token_count == 0) key = token;
      else if (token_count == 1) value = token;
      else {
				// double '=' wrong format
        return PL_ARG_INVALID_FORMAT;
        break;
      }

      token_count++;
    }

    // handle the arguments
    if (!key || strlen(key) == 0) {
      // invalid or empty key
			return PL_ARG_INVALID_FORMAT;
    
		} else {
      const int argument_index = pl_arg_exist(key);
      if (argument_index != -1) {
        pl_arg *local_argument = &PL_ARGS[argument_index];
        if (!value || strlen(value) == 0) {
          /* a key has been provided but it has
           * no value, this is good for if you have
           * void flags like --help. */
          if (local_argument->takes_value == TAKES_VALUE)
          	return PL_ARG_REQUIRES_VALUE;

          local_argument->triggered = 1;
        } else {

          /* a key has been provided and a value
           * has also been provided. */
          if (local_argument->takes_value == NO_VALUE)
            return PL_ARG_NO_REQUIRES_VALUE;

          // set the value if needed
          local_argument->value = malloc(strlen(value) + 1);
          if (local_argument->value) {
            strcpy(local_argument->value, value);
            local_argument->triggered = 1;
          } else return PL_MEM_ALLOC_ERROR;
        }
      } else return PL_ARG_NOT_FOUND;
    }

    free(to_free);
  }

  return PL_SUCCESS;
}

// get bool if pl_arg has been run or not,
// useful for void pl_arg flags like --help
int pl_arg_run(const pl_arg *local) {
  if (validate_argument_list() != PL_SUCCESS){
    pl_v(ERROR, "argument_list validation failed");
    return -1;
	}

  if (!local)
    return PL_ARG_IS_NULL;

  if (local->triggered)
    return PL_SUCCESS;
  return PL_FAILURE;
}

char *pl_arg_value(const pl_arg *local) {
  if (validate_argument_list() != PL_SUCCESS) {
    pl_v(ERROR, "argument_list validation failed");
    return "";
  }

  if (local->triggered) {
    if (local->value)
      return local->value;
    else {
      pl_v(ERROR, "pl_arg '%s' is triggered but has no value\n",
             local->name);
      return "";
    }
  } else {
    pl_v(ERROR, "pl_arg '%s' does not have a value", local->name);
    return "";
  }
  pl_v(ERROR, "pl_arg '%s' has not been run", local->name);
  pl_v(ERROR, "^--> use '*argument_run(pl_arg *)'");
  return "";
}

// EXPERIMENTAL
int pl_all_triggered() {
  if (validate_argument_list() != PL_SUCCESS)
    return -1;
  for (int i = 0; i < PL_ARGS_IDX; i++) {
    const pl_arg *local = &PL_ARGS[i];
    if (pl_arg_run(local) != 0) {
      pl_v(VERBOSE, "%s flag not triggered", local->name);
      return 1;
    }
  }
  return 0;
}
