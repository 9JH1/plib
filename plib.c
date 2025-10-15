#include "plib.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define print(a, ...)                                                          \
  print_imp(__TIME__, __FILE__, __LINE__, __func__, a, __VA_ARGS__)
void print_imp(const char *time, const char *file, const int line,
               const char *func, const char *format, ...) {
  if (PL_VERBOSE) {
    va_list args;
    va_start(args, format);
    printf("%s (%s@%d) %s: ", time, file, line, func);
    vprintf(format, args);
    va_end(args);
  }
}

int PL_MEM_USE = 1;

void *MALLOC(size_t in) {
  PL_MEM_USE += in;
  print("pl memory changed to %d (+%d)\n", PL_MEM_USE, in);
  return malloc(in);
}

void *REALLOC(void *in, size_t new, size_t old) {
  PL_MEM_USE -= old;
  printf("pl memory changed to %d (-%lu)\n", PL_MEM_USE, old);
  PL_MEM_USE += new;
  printf("pl memory changed to %d (+%lu)\n", PL_MEM_USE, new);
  return realloc(in, new);
}

void FREE(void *in, size_t size) {
  if (in == NULL)
    return;
  PL_MEM_USE -= size;
  print("pl memory changed to %d (-%lu)\n", PL_MEM_USE, sizeof(in));
  free(in);
}

int split_arg(char **key, char **val, char *in, char s) {
  int part_val = 0;
  int part_key = 0;
  *key = MALLOC(strlen(in) * sizeof(char) + 1);
  *val = NULL;

  for (size_t i = 0; i < strlen(in); i++) {

    // flip the part if =
    if (in[i] == s && part_val == 0) {
      part_val = i;
      *val = MALLOC(strlen(in) * sizeof(char) + 1);
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
        print("node found at index %d\n", idx);
        return PL_SUCCESS;
      }
      if ((*buf)->arg.short_flag != NULL &&
          strcmp((*buf)->arg.short_flag, name) == 0) {
        print("node found at index %d using shorthand\n", idx);
        return PL_SUCCESS;
      }

      print("node was not at recurse level %d\n", idx);
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

  print("returning node at index %d\n", idx);

  return cur;
}

int get_next_node_i(void) {
  node *cur = &PL_ARGS;
  int recurse_count = 0;
  while (cur->init != 0) {
    cur = cur->next;
    recurse_count++;
  }
  printf("returning node at index %d\n", recurse_count);
  return recurse_count;
}

pl_arg *pl_a(pl_arg in) {
  node *cur = get_next_node();

  cur->init = 1;
  cur->arg = in;
  cur->arg._value.capacity = 2;
  cur->arg._value.index = 0;
  cur->arg._short_run = 0;
  if (cur->arg.takes_value) {
    cur->arg._value.array = MALLOC(sizeof(char *) * 2);
    print("allocated %lu bytes for new value array\n", sizeof(char *) * 2);
  }

  if (cur->arg.cat == NULL)
    cur->arg.cat = "Options:";

  int node_size = sizeof(node);
  cur->next = MALLOC(node_size);
  print("allocated %lu bytes for new node\n", node_size);
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
              FREE(value->array, sizeof(value->array));
              break;
            }
            value->array = tmp;
          }

          value->array[value->index] = MALLOC(sizeof(val));
          strcpy(value->array[value->index], val);
          value->index++;
        }
      }
    } else if (PL_ARG_NOT_FOUND_ERROR) {
      return_code = PL_ARG_NOT_FOUND;
    }
    FREE(key, sizeof(key));
    FREE(val, sizeof(key));
    if (return_code != PL_SUCCESS)
      break;
  }

  return return_code;
}

void pl_free() {
  node *cur = &PL_ARGS;
  int rec_lev = 0;

  while (cur != NULL) {
    print("starting de-alloc on node index %d\n", rec_lev);
    node *next = cur->next;
    if (cur->arg.takes_value && cur->arg._value.array != NULL) {
      for (int i = 0; i < cur->arg._value.index; i++) {
        if (cur->arg._value.array[i] != NULL) {
          size_t val_size = strlen(cur->arg._value.array[i]) + 1;
          print("free'd %lu bytes of mem for value[%d]\n", val_size, i);
          FREE(cur->arg._value.array[i], val_size);
          cur->arg._value.array[i] = NULL;
        }
      }
      print("free'd %lu bytes of mem for value array\n",
            cur->arg._value.capacity * sizeof(char *));
      FREE(cur->arg._value.array, cur->arg._value.capacity * sizeof(char *));
      cur->arg._value.array = NULL;
    }
    if (cur != &PL_ARGS) {
      print("free'd %lu bytes of mem for node\n", sizeof(node));
      FREE(cur, sizeof(node));
    }
    cur = next;
    rec_lev++;
  }

  print("exiting plib with %d bytes of memory held\n", PL_MEM_USE);
}

// get the value at an index
char *pl_get_value(const pl_arg *arg, const int i) {
  print("retrieving value of arg %s at index %d\n", arg->flag, i);
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

int in(const char *in, char **arr, const size_t size) {
  if (arr == NULL || in == NULL || size < 0)
    return 0;
  for (int i = 0; i < size; i++) {
    if (strcmp(in, arr[i]) == 0)
      return 1;
  }

  return 0;
}

// arg count squared recurse model
void pl_help() {
  node *cur = &PL_ARGS;

  size_t l_flag = 0, l_type = 0, l_short = 0;

  int cat_idx = 0;
  int cat_cap = 2;
  char **cats = MALLOC(cat_cap * sizeof(char *));

  // pre-run
  while (cur->init == 1) {
    pl_arg a = cur->arg;

    if (a.flag != NULL && strlen(a.flag) > l_flag)
      l_flag = strlen(a.flag);

    if (a.type != NULL && strlen(a.type) > l_type)
      l_type = strlen(a.type);

    if (a.short_flag != NULL && strlen(a.short_flag) > l_short)
      l_short = strlen(a.short_flag);

    if (!in(a.cat, cats, cat_idx)) {
      if (cat_idx == cat_cap) {
        cat_cap *= 2;
        REALLOC(cats, cat_cap * sizeof(char *), (cat_cap/2) * sizeof(char *));

        if (!cats) {
          printf("error in mem alloc\n");
        }
      }

      cats[cat_idx] = a.cat;
      cat_idx++;
    }

    cur = cur->next;
  }

  for (int i = 0; i < cat_idx; i++) {
    char *cur_cat = cats[i];
    cur = &PL_ARGS;
    printf("\033[1m%s\033[0m\n", cur_cat);

    while (cur->init == 1) {
      if (strcmp(cur->arg.cat, cur_cat) == 0) {

        // print arguments in cat
        pl_arg arg = cur->arg;
        rep(PL_HELP_CAT_INDENT, ' ');

        printf("%s%s", PL_HELP_SEL_ANSI, arg.flag);
        rep(l_flag - strlen(arg.flag), ' ');

        if (l_short != 0) {
          if (arg.short_flag != NULL) {
            printf("%s%s",PL_HELP_SEP, arg.short_flag);
            rep(l_short - strlen(arg.short_flag), ' ');
          } else
            rep(l_short + 2, ' ');
        }

        if (arg.takes_value)
          printf("%s%s\033[0m value   ",PL_HELP_SEP, PL_HELP_SEP_ANSI);

        else
          printf("%s%s\033[0m no value",PL_HELP_SEP,PL_HELP_SEP_ANSI);

        if (l_type != 0) {
          if (arg.type != NULL) {
            printf("%s%s\033[0m %s",PL_HELP_SEP, PL_HELP_SEP_ANSI, arg.type);
            rep(l_type - strlen(arg.type), ' ');
          } else
            rep(l_type + 4, ' ');
        }

        printf("%s%s\033[0m %s\n",PL_HELP_SEP, PL_HELP_SEP_ANSI, arg.desc);
      }
      cur = cur->next;
    }
    printf("\n");
  }

  FREE(cats, cat_idx * sizeof(char *));
}
