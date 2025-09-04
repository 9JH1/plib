#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plib.h"

/*  TODO:
 *  1. add verbose mode 
 */


pl_prog PL_PROG_SET = {
	.name = "plib3",
	.version = "3.4",
	.description = "basic argument parsing library",
};



pl_arg *argument_list;
int argument_list_index = 0;
int argument_list_capacity = 0;
const int noquiet = 1;


int pl_v_i(mode mode, const int LINE, const char *FILE, const char *format, ...) {
  if (noquiet == 0) return 0;
  if (noquiet == 1 && mode == 1) return 0;

  va_list args;
	if(USE_FNAME) printf("%s] %s@%d: ", mode ? "\033[31;249m[Verbose]" : "\033[31;49m[Error]", FILE, LINE);
	else printf("%s %s@%d: ", mode ? "\033[31;249m[Verbose]" : "\033[31;49m[Error]", PL_PROG_SET.name, LINE);

  va_start(args, format);
  vprintf(format, args);
	printf("\033[0m");
  va_end(args);
  return 0;
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
  if (argument_list_capacity == 0) {
    argument_list_capacity = 2;
    argument_list = malloc(argument_list_capacity * sizeof(pl_arg));
    if (!argument_list) {
      pl_v(ERROR, "unable to initialize argument_list (malloc)\n");
      return 1;
    }
  }

  // re-allocate argument_list
  if (argument_list_capacity == argument_list_index) {
    argument_list_capacity *= 2;
    pl_arg *temp =
        realloc(argument_list, argument_list_capacity * sizeof(pl_arg));
    if (!temp) {
      pl_v(ERROR, "unable to re-allocate argument_list (malloc)\n");
      free(argument_list);
      return 1;
    } else
      argument_list = temp;
  }

  if (argument_list == NULL)
    return 1;

  return 0;
}




pl_arg *pl_a(pl_arg in) {
  if (validate_argument_list() != 0){
    pl_v(ERROR,"validate_argument_list\n");
		return (pl_arg *){0};
	}

	if(in.name == NULL){
		pl_v(ERROR,".title is a required field\n");
		return (pl_arg *){0};
	}

	pl_arg *local =  &argument_list[argument_list_index]; 
	*local = in;

	// safegaurd vars 
	local->triggered = 0;
  local->value = NULL;

	if(local->catagory == NULL){
		local->catagory = "base";
	}

	if(local->takes_value < 1){
		local->takes_value = NO_VALUE;
	}

	argument_list_index++;
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
	int catagorys_capacity = 10;
	char **catagorys = malloc(sizeof(char)*catagorys_capacity);
	int catagorys_index = 0;

	// for each argument: 
	for(int i=0;i<argument_list_index;i++){
		if(list_contains(argument_list[i].catagory,catagorys,catagorys_index)==1){
			if(catagorys_index == catagorys_capacity){
				catagorys_capacity+=2;
				char **temp = realloc(catagorys,sizeof(char)*catagorys_capacity);
				if(!temp){
					pl_v(ERROR,"Couldent re-allocate catagorys list.\n");
					return;
				}
				catagorys = temp;
			}

			catagorys[catagorys_index] = (char *)argument_list[i].catagory;
			catagorys_index++;
		}
	}

  if (argument_list == NULL) return;	
	if(PL_PROG_SET.name != NULL) printf("%s",PL_PROG_SET.name);
	if(PL_PROG_SET.version != NULL) printf("%s\n",PL_PROG_SET.version);
	if(PL_PROG_SET.description != NULL) printf("%s",PL_PROG_SET.description);

  for (int i = 0; i < argument_list_index; i++) {
    printf("%s", argument_list[i].name);
		if(argument_list[i].description != NULL)
			printf(" | %s",argument_list[i].description);
		
		printf("\n"); 
  }

	free(catagorys);
}

void pl_exit(void) {
  if (validate_argument_list() == 0) {
    for (int i = 0; i < argument_list_index; i++) {
      if (argument_list[i].value) {
        pl_v(VERBOSE, "free'd %lu bytes\n", strlen(argument_list[i].value));
        free(argument_list[i].value);
      }
    }
    pl_v(VERBOSE, "free'd argument_list\n");
    free(argument_list);
  }
}

int pl_arg_exist(const char *name) {
  for (int i = 0; i < argument_list_index; i++)
    if (strcmp(argument_list[i].name, name) == 0)
      return i; // pl_arg name found in argument_list

  return -1; // pl_arg was not found in argument_list
}

pl_arg *pl_arg_by_name(const char *name){
	int index = pl_arg_exist(name);
	if(index == -1) return (pl_arg *){0};
	return &argument_list[index];
}

int pl_proc_s_i(const int argc, const char *argv[],const pl_prog opt){
	PL_PROG_SET = opt;
	return pl_proc_i(argc,argv);
}

int pl_proc_i(const int argc, const char *argv[]) {
  if (argc == 1) {
    pl_v(ERROR, "No arguments given\n");
    return 1;
  }
  for (int i = 1; i < argc; i++) {
    if (!argv[i]) {
      pl_v(ERROR, "NULL pl_arg at index %d\n", i);
      continue;
    }

    // handle -- flag
    if (strcmp(argv[i], "--") == 0) {
      const int idx = pl_arg_exist("--");
      if (idx != -1) {
        // the -- flag has been defined by user
        pl_arg *local = &argument_list[idx];

        // Start collecting arguments after "--"
        size_t totalLen = 0;
        for (int j = i + 1; j < argc; j++)
          totalLen += strlen(argv[j]) + 1; // space or null terminator

        local->value = malloc(totalLen + 1);
        if (!local->value) {
          pl_v(ERROR, "Couldn't allocate memory for -- flag\n");
          return -1;
        }

        local->triggered = 1;
        local->value[0] = '\0';
        for (int j = i + 1; j < argc; j++) {
          strcat(local->value, argv[j]);
          if (j < argc - 1)
            strcat(local->value, " ");
        }

        return 0;
      }
    }

    char *str, *to_free, *token;
    const char *arg = argv[i];
    char *key = NULL, *value = NULL;
    int token_count = 0;
    int return_code = 0;

    to_free = str = strdup(arg);
    if (!str) {
      pl_v(ERROR, "Memory allocation failed for pl_arg '%s'\n", arg);
      return 1;
    }

    while ((token = strsep(&str, "="))) {
      if (strlen(token) == 0)
        continue;

      if (token_count == 0)
        key = token;
      else if (token_count == 1)
        value = token;
      else {
        return_code = 1;
        break;
      }
      token_count++;
    }

    // handle the arguments
    if (return_code == 1)
      pl_v(ERROR, "too many '=' in argument: '%s'\n", arg);
    else if (!key || strlen(key) == 0) {
      return_code = 1;
      pl_v(ERROR, "invalid or empty key in '%s'\n", arg);
    } else {
      const int argument_index = pl_arg_exist(key);
      if (argument_index != -1) {
        pl_arg *local_argument = &argument_list[argument_index];
        if (!value || strlen(value) == 0) {
          /* a key has been provided but it has
           * no value, this is good for if you have
           * void flags like --help. */
          if (local_argument->takes_value == TAKES_VALUE) {
            pl_v(ERROR, "pl_arg '%s' takes a value but none was provided\n",key);
            return_code = 1;
          }

          local_argument->triggered = 1;
        } else {

          /* a key has been provided and a value
           * has also been provided. */
          if (local_argument->takes_value == NO_VALUE) {
            pl_v(ERROR,"argument '%s' does not take a value but '%s' was provided\n",key, value);
            return_code = 1;
          }

          // set the value if needed
          local_argument->value = malloc(strlen(value) + 1);
          if (local_argument->value) {
            strcpy(local_argument->value, value);
            local_argument->triggered = 1;
          } else
            pl_v(ERROR, "couldent allocate memory for pl_arg '%s' value\n",
                   key);
        }
      } else
        pl_v(ERROR, "argument '%s' not found\n", key);
    }

    free(to_free);
    if (return_code != 0)
      return return_code;
  }

  return 0;
}

// get bool if pl_arg has been run or not,
// useful for void pl_arg flags like --help
int pl_arg_run(const pl_arg *local) {
  if (validate_argument_list() != 0)
    return -1;
  if (!local)
    return 1;

  if (local->triggered)
    return 0;
  return 1;
}

char *pl_arg_value(const pl_arg *local) {
  if (validate_argument_list() != 0) {
    pl_v(ERROR, "argument_list validation failed\n");
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
    pl_v(ERROR, "pl_arg '%s' does not have a value\n", local->name);
    return "";
  }
  pl_v(ERROR, "pl_arg '%s' has not been run\n", local->name);
  pl_v(ERROR, "^--> use '*argument_run(pl_arg *)'\n");
  return "";
}

// EXPERIMENTAL
int pl_all_triggered() {
  if (validate_argument_list() != 0)
    return -1;
  for (int i = 0; i < argument_list_index; i++) {
    const pl_arg *local = &argument_list[i];
    if (pl_arg_run(local) != 0) {
      pl_v(VERBOSE, "%s flag not triggered", local->name);
      return 1;
    }
  }
  return 0;
}
