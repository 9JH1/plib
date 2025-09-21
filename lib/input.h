#ifndef UILIB_H
#define UILIB_H

typedef struct {
  int WIDTH;
  int HEIGHT;
} termsize;

typedef enum {
  ERROR = -1,
  MOUSE_MOVE = 0,
  MOUSE_CLICK = 1,
  MOUSE_SCROLL = 2,
  KEY = 3,
  SPECIAL_KEY = 4,
  MOUSE_RELEASE = 5,
} EventType;

typedef struct {
  EventType type;
  union {
    struct {
      int x, y, m;
    } mouse;
    int scroll_y;
		struct {
			int code;
    	char buf[2];
		} key;
    int special_key;
  } data;
} InputEvent;

void enable_mouse_reporting(void);
void disable_mouse_reporting(void);
int await_scroll(void);
int input(InputEvent *local);
termsize get_term_size(void);
void log_InputEvent(InputEvent local);
void disable_raw_mode(void);
extern int MOUSE_SCROLL_Y;
extern int MOUSE_X;
extern int MOUSE_Y;
#endif // UILIB_H
