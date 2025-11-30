#ifndef IN_2
#define IN_2 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum { MOVE, CLICK, RELEASE, SCROLL, SPECIAL, ERR, KEY } _in_ret;

static const char *_in_ret_s[8] = {
    [MOVE] = "MOVE",     [CLICK] = "CLICK",     [RELEASE] = "RELEASE",
    [SCROLL] = "SCROLL", [SPECIAL] = "SPECIAL", [ERR] = "ERR",
    [KEY] = "KEY"};

typedef void (*func)(void *);

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
	void *self;
  union {
    struct {
			int x;
			int y;
      int x1;
      int y1;
    } mouse;
    struct {
      char key;
    } special;
  };
} trigger;

struct vec2 {
  int x, y;
};

// Border options
typedef struct border {
  char *top;
  char *bottom;
  char *right;
  char *left;
  char *top_left;
  char *top_right;
  char *bottom_left;
  char *bottom_right;
  char *ansi;
} border;

// Box options
typedef struct box {
  struct vec2 size;
  struct vec2 pos;
  char *ansi;
  char fill;     // fill char
  border border; // border
  char *_r;      // Rendered
	char *_r2;		 // Extra render slot
	char *_r3;     // Extra extra render slot
  int pr_idx;    // PRINT-IN index
  int pr_inherit_ansi;
} box;

void _in_catch(char*, int, input*);
void _in_app(trigger);
void in_update(input*);
void in_clean(void);
void _in_raw(void);
void _in_no_raw(void);
void _print_input(input in);
void UI_CLEAR(box *o);
box _init_box(box o);
border _init_border(char*, char*, char*);
void UI_PRINTIN(box *o, const char *fmt, ...);
char *_box_render(box *o);

extern trigger *in_trig_stack;

#define in_app(...) _in_app((trigger){__VA_ARGS__})
#define in_key(key, callback) in_app(.type = KEY, .buf = key, .call = callback)
#define in_spec_key(key, callback)                                             \
  in_app(.type = SPECIAL, .buf = key, .call = callback)	
#define in_hov_key(vec1, vec2, callback) in_app(.type = MOVE, .call = callback, .mouse.x = vec1.x, .mouse.y = vec1.y, .mouse.x1 = vec2.x, .mouse.y1 = vec2.x)
#define in_box_hov_key(box, callback) in_app(.type = MOVE, .call = callback, .mouse.x = box.pos.x, .mouse.y = box.pos.x, .mouse.x1 = box.size.x, .mouse.y1 = box.size.y)
#define in_box_clk_key(box, callback) in_app(.type = CLICK, .call = callback, .mouse.x = box.pos.x, .mouse.y = box.pos.x, .mouse.x1 = box.size.x, .mouse.y1 = box.size.y)
#define in_catch(l) _in_catch(buf, n, l);
#define UI_BOX(...) _init_box((box){__VA_ARGS__})
#define UI_BOX_DRAW(o) printf("%s", o._r)
#define VEC(x, y) (struct vec2) { x, y }
#define UI_BORDER(a,b,c) _init_border(a,b,c)

#endif // !DEBUG
