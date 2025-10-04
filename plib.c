#include "plib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  ERROR,
  VERBOSE,
} mode;

// external definintions
pl_arg *PL_ARGS;
int PL_ARGS_IDX      =  0;
int PL_ARGS_CAP      =  0;
int PL_PROC_END_ARGC = -1;
int PL_ARGC          = -1;
char **PL_ARGV       =  NULL;

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


pl_arg *pl_arg_global_ptr(pl_arg in){
	if(pl_arg_exist(in.name) != PL_SUCCESS){
		pl_a(in);
	}

	return pl_arg_by_name(in.name);
}


#define ph() printf("\x1b[36;49m%s -> %s@%d: \033[0m",__FILE__,__func__,__LINE__+1)

int validate_argument_list() {
  // init pl_arg list
  if (PL_ARGS_CAP == 0) {
    PL_ARGS_CAP = PL_INIT_ARG_ALLOC;
    PL_ARGS = malloc(PL_ARGS_CAP * sizeof(pl_arg));
    if (!PL_ARGS) {
			ph();
			printf("couldent initialize argument list memory\n");
      return PL_ARG_IS_NULL;
    }
		if(PL_VERBOSE){
			ph();
			printf("initialized argument_list\n");
		}
  }

  // re-allocate argument_list
  if (PL_ARGS_CAP == PL_ARGS_IDX) {
    PL_ARGS_CAP *= 2;
    pl_arg *temp = realloc(PL_ARGS, PL_ARGS_CAP * sizeof(pl_arg));
    
		if (!temp) {
      free(PL_ARGS);
			ph();
			printf("couldent reallocate memory for argument list\n");
      return PL_MEM_ALLOC_ERROR;
    } else PL_ARGS = temp;

		if(PL_VERBOSE){
			ph();
			printf("reallocated memory for argument_list\n");
		}
  }

  if (PL_ARGS == NULL){
		ph();
		printf("argument list is null after allocation/reallocation\n");
		return PL_ARG_IS_NULL;
	}

  return PL_SUCCESS;
}


int pl_a(pl_arg in) {
  if (validate_argument_list() != PL_SUCCESS){
		// no need for argument because issue occured
		return PL_FAILURE;
	}

	if(in.name == NULL){
		ph();
		printf("argument call missing .title, this is a required field\n");
		return PL_ARG_MISSING_PARAM;
	}

	// check argument dosent already exist 
	for(int i = 0; i < PL_ARGS_IDX ;i++){
		if(PL_ARGS[i].name != NULL)
			if(strcmp(PL_ARGS[i].name,in.name)==0){
				ph();
				printf("argument name '%s' already exists\n",in.name); 
				return PL_ARG_PRE_EXISTS;
			}

		if(PL_ARGS[i].shorthand != NULL && in.shorthand != NULL)
			if(strcmp(PL_ARGS[i].shorthand,in.shorthand) == 0){
				ph();
				printf("argument shorthand '%s' already exists\n",in.name);
				return PL_ARG_PRE_EXISTS;
			}
	}

	// preference, remove if wanted
	if(in.shorthand != NULL && in.takes_value == 1 && PL_VERBOSE){
		ph();
		printf("argument should not be shorthand and take value\n");
	}

	pl_arg *local =  &PL_ARGS[PL_ARGS_IDX]; 
	*local = in;

	// reset vars 
	local->triggered = 0;
	local->shorthand_triggered = 0;
	
	local->value_cap = 2;
	local->value_idx = 0;
  local->value = (char **)malloc(local->value_cap * sizeof(char *));

	if(local->catagory == NULL) local->catagory = "Options";
	if(local->takes_value < 1) local->takes_value = 0;

	if(PL_VERBOSE){
		ph();
		printf("argument created: '%s' at index: %d\n",in.name,PL_ARGS_IDX);
	}

	PL_ARGS_IDX++;	
	return PL_ARGS_IDX-1;
}

int list_contains(const char*item, char **list, const int list_size){
	if(list_size <= 0) return 1;
	for(int i=0;i<list_size;i++)
		if(strcmp(list[i],item)==0)
			return 0; // true 
	
	return 1;
}

void rep(const int l, const int c){
	for(int i = 0; i < l; i++)
		printf("%c",c);
}

void pl_help(void) {
	if (PL_ARGS == NULL) return;	
	
	int catagorys_capacity = 2;
	char **catagorys = malloc(catagorys_capacity*sizeof(char *));
	int catagorys_index = 0;
	int longest_name = 0;
	int longest_shorthand = 0;
	int longest_type = 0;

	// for each argument: 
	for(int i=0;i<PL_ARGS_IDX;i++){
		const pl_arg loc = PL_ARGS[i];

		// check catagory 
		if(list_contains(loc.catagory,catagorys,catagorys_index)==1){
			if(catagorys_index == catagorys_capacity){
				catagorys_capacity *= 2;
				char **temp = realloc(catagorys,catagorys_capacity * sizeof(char *));
				if(!temp){
					ph();
					printf("Couldent re-allocate catagorys list\n");
					return;
				}
				catagorys = temp;
			}

			catagorys[catagorys_index] = (char *)PL_ARGS[i].catagory;
			catagorys_index++;
		}

		// get longest values 
		if(loc.name != NULL)
			if(strlen(loc.name) > longest_name)
				longest_name = strlen(loc.name);
		
		if(loc.shorthand != NULL)
			if(strlen(loc.shorthand) > longest_shorthand)
				longest_shorthand = strlen(loc.shorthand);
		
		if(loc.type != NULL)
			if(strlen(loc.type) > longest_type)
				longest_type = strlen(loc.type);
	}
	
	// for each catagory 
	for(int i = 0; i < catagorys_index; i++){
		const char* cat = catagorys[i]; // :3	
		printf("\033[1m%s:\033[0m\n",cat);

		// for each argument
		for(int ii = 0; ii < PL_ARGS_IDX; ii++){
			const pl_arg loc = PL_ARGS[ii];

			if(loc.catagory != NULL){
				// skip argument if it dosent match catagory 
				if(strcmp(loc.catagory,cat)!=0) continue;
			}

			// print name 
			if(loc.name != NULL && longest_name != 0){
				printf("%s",loc.name);
				rep(strlen(loc.name) - longest_name,' ');
			}
			
			// print shorthand
			if(loc.shorthand != NULL && longest_shorthand != 0){
				printf(", %s",loc.shorthand);
				rep(longest_shorthand - strlen(loc.shorthand), ' ');
			} else if (longest_shorthand != 0)
				rep(longest_shorthand + 2,' ');

			// print type 
			if(loc.type != NULL && longest_type != 0){
				printf(" | %s",loc.type);
				rep(longest_type - strlen(loc.type),' ');
			} else if (longest_type != 0){
				printf(" | ");
				rep(longest_type, ' ');
			}

			// print description 
			if(loc.description) printf(" | %s",loc.description);

			printf("\n");
		}
		// extra newline for visuals
		printf("\n");
	}

	free(catagorys);
}

void pl_exit(void) {
	if(PL_VERBOSE){
		ph();
		printf("Deallocating argument list\n");
	}
  if (validate_argument_list() == PL_SUCCESS) {
    for (int i = 0; i < PL_ARGS_IDX; i++) {
			pl_arg *loc = &PL_ARGS[i];
      if (loc->value_idx > 0) {
				for(int ii = 0; ii < loc->value_idx; ii++){
					if(PL_VERBOSE){
						ph();
						printf("free'd %lu bytes from '%s'.value[%d] '%s'\n",
								strlen(loc->value[i]),
								loc->name,
								ii,
								loc->value[ii]);
					}
					free(loc->value[ii]);
				}
				if(PL_VERBOSE){
					ph();
        	printf("free'd '%s'.value\n", loc->name);
				}
				free(loc->value);
      }
    }
    if(PL_VERBOSE){
			ph();
			printf("free'd argument_list\n");
		}

    free(PL_ARGS);
  }
	if(PL_VERBOSE){
		ph();
		printf("PLib is not active now..\n");
	}
}

int pl_arg_exist(const char *name) {
  for (int i = 0; i < PL_ARGS_IDX; i++){
    if (strcmp(PL_ARGS[i].name, name) == 0)
      return i; // pl_arg name found

		if(PL_ARGS[i].shorthand != NULL)
			if (strcmp(PL_ARGS[i].shorthand,name) == 0)
				return i; // pl_arg shorthand found 
	}
	if(PL_VERBOSE){
		ph();
		printf("key: '%s' was not found in PL_ARGS\n",name);
	}

  return PL_ARG_NOT_FOUND; // pl_arg was not found in argument_list
}

pl_arg *pl_arg_by_name(const char *name){
	int index = pl_arg_exist(name);
	if(index == -1) return (pl_arg *){0};
	return &PL_ARGS[index];
}

int pl_proc(const int argc, const char *argv[]) {
  atexit(pl_exit);
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

				// sneaky dma 
				if(local->value_idx == local->value_cap){
					// realloc that shit
					local->value_cap *= 2;
					char ** tmp = realloc(local->value,local->value_cap * sizeof (char *));
					if(!tmp){
						printf("couldent reallocate memory for values\n");
						pl_exit(); // NOTE: not sure if I need this or not..
						return PL_MEM_ALLOC_ERROR;
					}

					local->value = tmp;
				}

        local->value[local->value_idx][0] = '\0';
        for (int j = i + 1; j < argc; j++) {
          strcat(local->value[local->value_idx], argv[j]);
          if (j < argc - 1)
            strcat(local->value[local->value_idx], " ");
        }
				local->value_idx++;
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
			if(PL_VERBOSE){
				ph();
				printf("key: '%s', val: '%s'\n",key,value);
				ph();
				printf("argument_index initialized with arg number %d\n",argument_index);
			}

			if (argument_index > -1){
        pl_arg *local_argument = &PL_ARGS[argument_index];
				if(PL_VERBOSE){
					ph();
					printf("local arg found '%s'\n",local_argument->name);
				}


				if (!value || strlen(value) == 0) {
          if (local_argument->takes_value == 1){
          	if(PL_VERBOSE){
							ph();
							printf("Returning PL_ARG_REQUIRES_VALUE for key '%s'\n",key);
						}

						return PL_ARG_REQUIRES_VALUE;
					}

				// sys arg does not have value	
        } else {
          if (local_argument->takes_value == 0){
          	if(PL_VERBOSE){
							ph();
							printf("returning PL_ARG_NO_REQUIRES_VALUE for key '%s'\n",key);
						}
						return PL_ARG_NO_REQUIRES_VALUE;

					}

          // set the value
					if(local_argument->value_idx == local_argument->value_cap){
						// realloc that shit
						local_argument->value_cap *= 2;
						char ** tmp = realloc(local_argument->value,local_argument->value_cap * sizeof (char *));
						if(!tmp){
							printf("couldent reallocate memory for values\n");
							pl_exit(); // NOTE: not sure if I need this or not..
							return PL_MEM_ALLOC_ERROR;
						}

						local_argument->value = tmp;
					}

          local_argument->value[local_argument->value_idx] = malloc(strlen(value) + 1);
          if (local_argument->value){
            strcpy(local_argument->value[local_argument->value_idx], value);
						local_argument->value_idx++;
					}

          else return PL_MEM_ALLOC_ERROR;
        }
				local_argument->triggered = 1;
				

				if(PL_VERBOSE){
					ph();
					printf("key: '%s' has been triggered\n",key);
				}

				if(local_argument->shorthand != NULL){	
					if(strcmp(key,local_argument->shorthand) == 0){
						if(PL_VERBOSE){
							ph();
							printf("key: '%s' detected as shorthand\n",key);
						}
						local_argument->shorthand_triggered = 1;
					}
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
    return PL_FAILURE;
	}

  if (!local)
    return PL_ARG_IS_NULL;

  if (local->triggered || local->shorthand_triggered)
    return PL_SUCCESS;
  
	return PL_FAILURE;
}


char *pl_arg_value(const pl_arg *local, const int i) {
  if (validate_argument_list() != PL_SUCCESS)
    return NULL;

  if (local->triggered) {
    if (local->value[i])
      return local->value[i];
    else {
			ph();
      printf("pl_arg '%s' is triggered but has no value\n", local->name);
      return NULL;
    }
  } else {
		ph();
    printf("pl_arg '%s' does not have a value\n", local->name);
    return NULL;
  }

	ph();
  printf("pl_arg '%s' has not been run\n", local->name);
  printf("^--> use '*argument_run(pl_arg *)'\n");
  return NULL;
}

// EXPERIMENTAL
int pl_all_triggered() {
  if (validate_argument_list() != PL_SUCCESS)
    return PL_FAILURE;
  for (int i = 0; i < PL_ARGS_IDX; i++) {
    const pl_arg *local = &PL_ARGS[i];
    if (pl_arg_run(local) != 0) {
      if(PL_VERBOSE){
				ph();
				printf("%s flag not triggered\n", local->name);
			}
			return PL_FAILURE;
    }
  }
  return PL_SUCCESS;
}
