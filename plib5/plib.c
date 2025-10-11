#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#define PL_HELP_CAT_INDENT 2
#define PL_HELP_SEP_ANSI "\033[34m"
#define PL_HELP_SEL_ANSI "\033[32m"

typedef enum {
  PL_NO_ARGS_GIVEN,
  PL_NO_ARGS_DEFINED,
  PL_SUCCESS,
  PL_FAIL,
  PL_ARG_NOT_FOUND,
  PL_ARG_REQUIRES_VALUE,
  PL_ARG_NO_REQUIRE_VALUE,
  PL_MEM_ALLOC_ERROR,
} pl_r;

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

struct dma {
  char **array;
  int capacity;
  int index;
};

typedef struct {
  char *flag;
  char *desc;
  char *cat;

  // extra
  char *type;
  char *short_flag;

  // features
  int takes_value;

  // cannot be changed
  int _short_run;
  int _run;
  struct dma _value;
} pl_arg;

typedef struct node {
  pl_arg arg;
  int recurse;
  int skip;
  int init;
  struct node *next;
} node;

node PL_ARGS;
int PL_ARG_LAST_INDEX = 0;
int PL_ARGC;
char **PL_ARGV;
char PL_SPLITCHAR = '=';
int PL_ARG_NOT_FOUND_ERROR = 1;

int pl_arg_exist(node **buf, const char *name) {
  *buf = &PL_ARGS;

  while (*buf != NULL) {
    if (name != NULL && (*buf)->init == 1) {
      if ((*buf)->arg.flag != NULL && strcmp((*buf)->arg.flag, name) == 0) {
        return PL_SUCCESS;
      }
      if ((*buf)->arg.short_flag != NULL &&
          strcmp((*buf)->arg.short_flag, name) == 0) {
        return PL_SUCCESS;
      }
    }
    *buf = (*buf)->next;
  }

  return PL_FAIL;
}

node *get_next_node() {
  node *cur = &PL_ARGS;
  while (cur->init != 0) {
    cur = cur->next;
  }

  return cur;
}

int get_next_node_i() {
  node *cur = &PL_ARGS;
  int recurse_count = 0;
  while (cur->init != 0) {
    cur = cur->next;
    recurse_count++;
  }

  return recurse_count;
}

pl_arg *pl_a(pl_arg in) {
  node *cur = get_next_node();

  cur->init = 1;
  cur->arg = in;
  cur->arg._value.capacity = 2;
  cur->arg._value.index = 0;
  cur->arg._run = 0;
  cur->arg._short_run = 0;
  cur->arg._value.array = malloc(sizeof(char *) * 2);

  if (cur->arg.cat == NULL)
    cur->arg.cat = "Options:";

  cur->next = malloc(sizeof(node));
  return &cur->arg;
}

int pl_proc(const int c, char *v[]) {
  if (c <= 1)
    return PL_NO_ARGS_GIVEN;

  int return_code = PL_SUCCESS;
	
	// set locals
	PL_ARGV=v;
	PL_ARGC=c;

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
        // parsed perfectly
        arg_node->arg._run++;

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
    } else if (PL_ARG_NOT_FOUND_ERROR){
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
    for (int i = 0; i < cur->arg._value.index; i++)
      free(cur->arg._value.array[cur->arg._value.index]);

    free(cur->arg._value.array);
    if (cur != &PL_ARGS)
      free(cur);

    cur = next;
  }
}

// macro for arg def
#define PL_A(...) pl_a((pl_arg){__VA_ARGS__})

// macro for was run or not
#define PL_R(arg) (arg->_run || arg->_short_run)

// get the value at an index
char *pl_get_value(const pl_arg *arg, const int i) {
  if (PL_R(arg)) {
    if (i > arg->_value.index)
      return NULL;
    return arg->_value.array[i];
  } else
    return NULL;
}

// get latest value of arg
#define PL_G(arg) pl_get_value(arg, arg->_value.index)

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

        printf("%s%s",PL_HELP_SEL_ANSI, arg.flag);
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

int main(const int c, char *argv[]) {
	pl_arg *test1 = PL_A(.flag = "--test", "basic test arg",.takes_value=1);
  
	PL_ARG_NOT_FOUND_ERROR = 0;

	pl_proc(c, argv);
  if (PL_R(test1))
    for (int i = 0; i < test1->_value.index; i++)
      printf("value: %s\n", pl_get_value(test1, i));
	
	pl_arg *test2 = PL_A(.flag = "-", .takes_value = 1);
	PL_SPLITCHAR = 'D';
  pl_proc(c, argv);

  if (PL_R(test2))
    for (int i = 0; i < test2->_value.index; i++)
      printf("value: %s\n", pl_get_value(test2, i));
  return 0;
}
