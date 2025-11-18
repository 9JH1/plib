#ifndef IN_2
#define IN_2 
/* 
#include <stdio.h>
#include "in.h"

// callbacks 
void mycallback1(){ printf("b pressed\n"); }
void mycallback2(){ printf("B pressed\n"); }
void mycallback3(){ printf("Control+g pressed\n"); }


int main() {
	// global input
  input globby;

	// set triggers
  in_key('b', mycallback1);
  in_key('B', mycallback2);
  in_spec_key('G', mycallback3);

	// loop 
  in_loop() {
    in_catch(&globby);

		// check triggers
    in_update(&globby);
    fflush(stdout);
  }

	// exit cleanly
  in_clean();
  return 1;
}
*/


#include <unistd.h>
#include <stdlib.h>

typedef enum { MOVE, CLICK, RELEASE, SCROLL, SPECIAL, ERR, KEY } _in_ret;

static const char *_in_ret_s[8] = {
    [MOVE] = "MOVE",     [CLICK] = "CLICK",     [RELEASE] = "RELEASE",
    [SCROLL] = "SCROLL", [SPECIAL] = "SPECIAL", [ERR] = "ERR",
    [KEY] = "KEY"};

typedef void (*func)();

#define in_loop()                                                              \
	_in_raw();                                                                   \
  char buf[64]; /* string key buffer */                                        \
  int n;        /* input code int */                                           \
  while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0)

typedef struct {
  char buf;
  int len;
  _in_ret type;
  union {
    struct {
      int m;   // raw mouse value
      int x;   // x position
      int y;   // y position
      int s;   // mouse scroll dir -1 | 1
      char et; // event type
      int btn; // event code
    } mouse;

    struct {
      char key;
    } special;
  };
} input;

typedef struct {
  func call;
  _in_ret type;
  char buf;
  union {
    struct {
      int x1;
      int y1;
      int x2;
      int y2;
    } mouse;
    struct {
      char key;
    } special;
  };
} trigger;

void _in_catch(char*, int, input*);
void _in_app(trigger);
void in_update(input*);
void in_clean(void);
void _in_raw(void);
void _in_no_raw(void);
void _print_input(input in);

extern trigger *in_trig_stack;

#define in_app(...) _in_app((trigger){__VA_ARGS__})
#define in_key(key, callback) in_app(.type = KEY, .buf = key, .call = callback)
#define in_spec_key(key, callback)                                             \
  in_app(.type = SPECIAL, .buf = key, .call = callback)	
#define in_catch(l) _in_catch(buf, n, l);
#endif // !DEBUG
