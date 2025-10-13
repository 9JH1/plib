#include "plib.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define print(a,...) print_imp(__TIME__,__FILE__,__LINE__,__func__,a,__VA_ARGS__)
void print_imp(const char *time, const char *file, const int line, const char * func, const char *format, ...) {
	if(PL_VERBOSE){
		va_list args;
  	va_start(args,format);
		printf("%s (%s@%d) %s: ",time,file,line,func);
  	vprintf(format, args);
  	va_end(args);
	}
}

int split_arg(char **key, char **val, char *in, char s) {
  int part_val = 0;
  int part_key = 0;
  *key = malloc(strlen(in) * sizeof(char) + 1);
  *val = NULL;

  for (size_t i = 0; i < strlen(in); i++) {

    // flip the part if =
    if (in[i] == s && part_val == 0) {
      part_val = i;
      *val = malloc(strlen(in) * sizeof(char) + 1);
      continue;
    }

    // assign key
    if (part_val == 0) {
      (*key)[i] = in[i];
      part_key++;
      // assign value
    } else {
      (*val)[i - part_val - 1] = in[i];
    }
  }

  // end strings and return
  (*key)[part_key] = '\0';
  if (part_val)
    (*val)[strlen(in) - part_val - 1] = '\0';

  return 0;
}

node PL_ARGS;
int PL_ARG_LAST_INDEX = -1;
int PL_ARGC;
char **PL_ARGV;
char PL_SPLITCHAR = '=';
int PL_ARG_NOT_FOUND_ERROR = 1;

int pl_arg_exist(node **buf, const char *name) {
  *buf = &PL_ARGS;
	int idx;

  while (*buf != NULL) {
    if (name != NULL && (*buf)->init == 1) {
      if ((*buf)->arg.flag != NULL && strcmp((*buf)->arg.flag, name) == 0) {
				print("node found at index %d\n",idx);
        return PL_SUCCESS;
      }
      if ((*buf)->arg.short_flag != NULL &&
          strcmp((*buf)->arg.short_flag, name) == 0) {
				print("node found at index %d using shorthand\n",idx);
        return PL_SUCCESS;
      }

      print("node was not at recurse level %d\n",idx);
    }
    *buf = (*buf)->next;
		idx++;
  }

  return PL_FAIL;
}

node *get_next_node(void) {
  node *cur = &PL_ARGS;
	int idx = 0;
  while (cur->init != 0) {
    cur = cur->next;
		idx++;
  }

	printf("returning node at index %d\n",idx);

  return cur;
}

int get_next_node_i(void) {
  node *cur = &PL_ARGS;
  int recurse_count = 0;
  while (cur->init != 0) {
    cur = cur->next;
    recurse_count++;
  }
	printf("returning node at index %d\n",recurse_count);
  return recurse_count;
}

pl_arg *pl_a(pl_arg in) {
  node *cur = get_next_node();

  cur->init = 1;
  cur->arg = in;
  cur->arg._value.capacity = 2;
  cur->arg._value.index = 0;
  cur->arg._short_run = 0;
	if(cur->arg.takes_value){
  	cur->arg._value.array = malloc(sizeof(char *) * 2);
		printf("allocated %lu bytes for new value array\n",sizeof(char *) * 2);
	}

  if (cur->arg.cat == NULL)
    cur->arg.cat = "Options:";

	int node_size = sizeof(node);
  cur->next = malloc(node_size);
	print("allocated %lu bytes for new node\n",node_size);
  return &cur->arg;
}

pl_r pl_proc(const int c, char *v[]) {
  if (c <= 1)
    return PL_NO_ARGS_GIVEN;

  int return_code = PL_SUCCESS;

  // set locals
  PL_ARGV = v;
  PL_ARGC = c;

  for (int i = 1; i < c; i++) {
    PL_ARG_LAST_INDEX = i;

    char *arg = v[i];
    char *key, *val = NULL;

    split_arg(&key, &val, arg, PL_SPLITCHAR);

    node *arg_node = NULL;
    if (pl_arg_exist(&arg_node, key) == PL_SUCCESS && arg_node != NULL &&
        arg_node->init == 1) {

      // check if arg needs value
      if (arg_node->arg.takes_value == 1 && val == NULL) {
        return_code = PL_ARG_REQUIRES_VALUE;

        // check if arg does not need value
      } else if (arg_node->arg.takes_value == 0 && val != NULL) {
        return_code = PL_ARG_NO_REQUIRE_VALUE;
      } else {

        // check if argument was shorthand
        if (arg_node->arg.short_flag != NULL)
          if (strcmp(key, arg_node->arg.short_flag) == 0)
            arg_node->arg._short_run++;

        // handle value dma
        if (arg_node->arg.takes_value == 1) {
          struct dma *value = &arg_node->arg._value;

          // dma
          if (value->index == value->capacity) {
            value->capacity *= 2;
            char **tmp =
                realloc(value->array, value->capacity * sizeof(char *));
            if (!tmp) {
              return_code = PL_MEM_ALLOC_ERROR;
              free(value->array);
              break;
            }
            value->array = tmp;
          }

          value->array[value->index] = malloc(sizeof(val));
          strcpy(value->array[value->index], val);
          value->index++;
        }
      }
    } else if (PL_ARG_NOT_FOUND_ERROR) {
      return_code = PL_ARG_NOT_FOUND;
    }
    free(key);
    free(val);
    if (return_code != PL_SUCCESS)
      break;
  }

  return return_code;
}

void free_nodes() {
  node *cur = &PL_ARGS;
  while (cur != NULL) {
    node *next = cur->next;
		if(cur->arg._value.index > 0 && cur->arg.takes_value){
    	for (int i = 0; i < cur->arg._value.index; i++){
    	  free(cur->arg._value.array[cur->arg._value.index]);
			}

			print("free'd");
		}

    free(cur->arg._value.array);
    if (cur != &PL_ARGS)
      free(cur);

    cur = next;
  }
}

// get the value at an index
char *pl_get_value(const pl_arg *arg, const int i) {
  if (PL_R(arg)) {
    if (i > arg->_value.index)
      return NULL;
    return arg->_value.array[i];
  } else
    return NULL;
}

void rep(const int l, const char c) {
  for (int i = 0; i < l; i++)
    putchar(c);
}

// arg count squared recurse model
void pl_help() {
  node *cur = &PL_ARGS;
  char *cur_cat;

  size_t l_flag = 0, l_type = 0, l_short = 0;

  // pre-run
  while (cur->init == 1) {
    pl_arg a = cur->arg;

    if (a.flag != NULL && strlen(a.flag) > l_flag)
      l_flag = strlen(a.flag);

    if (a.type != NULL && strlen(a.type) > l_type)
      l_type = strlen(a.type);

    if (a.short_flag != NULL && strlen(a.short_flag) > l_short)
      l_short = strlen(a.short_flag);

    cur = cur->next;
  }

  // real run
  cur = &PL_ARGS;
  while (cur->init == 1) {
    if (cur_cat == cur->arg.cat)
      continue;

    if (cur->arg.cat == NULL)
      continue;

    cur_cat = cur->arg.cat;

    // print category
    printf("\033[1m%s\033[0m\n", cur_cat);

    // second recurse
    node *cur2 = &PL_ARGS;
    while (cur2->init == 1) {
      if (strcmp(cur2->arg.cat, cur_cat) == 0) {

        // print arguments in cat
        pl_arg arg = cur2->arg;
        rep(PL_HELP_CAT_INDENT, ' ');

        printf("%s%s", PL_HELP_SEL_ANSI, arg.flag);
        rep(l_flag - strlen(arg.flag), ' ');

        if (l_short != 0) {
          if (arg.short_flag != NULL) {
            printf(", %s", arg.short_flag);
            rep(l_short - strlen(arg.short_flag), ' ');
          } else
            rep(l_short + 2, ' ');
        }

        if (arg.takes_value)
          printf(" %s|\033[0m value   ", PL_HELP_SEP_ANSI);
        else
          printf(" %s|\033[0m no value", PL_HELP_SEP_ANSI);

        if (l_type != 0) {
          if (arg.type != NULL) {
            printf(" %s|\033[0m %s", PL_HELP_SEP_ANSI, arg.type);
            rep(l_type - strlen(arg.type), ' ');
          } else
            rep(l_type + 2, ' ');
        }

        printf(" %s|\033[0m %s\n", PL_HELP_SEP_ANSI, arg.desc);
      }
      cur2 = cur2->next;
    }

    printf("\n");
    cur = cur->next;
  }
}
