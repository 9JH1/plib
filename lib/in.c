// in_ui_hover_box(func, x1,y1,x2,y2);
// in_ui_click_box(func, x1,y1,x2,y2)

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "in.h"

struct termios orig_term;

trigger *in_trig_stack;
int in_trig_idx = 0;
int in_trig_cap = 2;

void _in_app(trigger in) {

  // Initialize
  if (in_trig_idx == 0) {
    in_trig_stack = malloc(in_trig_cap * sizeof(trigger));

    // RE-allocate for DMA
  } else if (in_trig_idx == in_trig_cap) {
    in_trig_cap *= 2;
    trigger *tmp = realloc(in_trig_stack, in_trig_cap * sizeof(trigger));
    if (!tmp) {
      printf("memory error while re-allocating in_trig_stack\n");
      free(in_trig_stack);
      return;
    }

    in_trig_stack = tmp;
  }

  in_trig_stack[in_trig_idx] = in;
  in_trig_idx++;
}

void in_update(input *in) {
  for (int i = 0; i < in_trig_idx; i++) {
    const trigger loc = in_trig_stack[i];
    if (loc.type != in->type)
      continue;

    switch (loc.type) {
    	case KEY:
      	if (in->buf == loc.buf && loc.call != NULL)
        	loc.call();
      	break;

    	case SPECIAL:
      	if (in->special.key == loc.buf && loc.call != NULL)
      		loc.call();
      	break;
    }
  }
}

void _print_input(input in) {
  printf("type: %s:\nbuf: %c\nlen: %d\nmouse:\nm: %d\nx: %d\ny: %d\ns: %d\net: "
         "%c\nbtn: %d\nspecial:\nkey: %c\n",
         _in_ret_s[in.type], in.buf, in.len, in.mouse.m, in.mouse.x, in.mouse.y,
         in.mouse.s, in.mouse.et, in.mouse.btn, in.special.key);
}

void _in_catch(char *buf, int len, input *l) {
  if (len <= 0) {
    l->type = ERR;
    return;
  }

  // handle mouse
  if (len >= 6 && buf[0] == '\033' && buf[1] == '[' && buf[2] == '<') {
    if (sscanf(buf, "\033[<%d;%d;%d%c", &l->mouse.btn, &l->mouse.x, &l->mouse.y,
               &l->mouse.et) != 4) {
      l->type = ERR;
      return;
    }

    // set type
    l->type = (l->mouse.et == 'M') ? (l->mouse.btn & 0b11) == 3 ? MOVE : CLICK
                                   : RELEASE;

    if (l->mouse.btn == 64 || l->mouse.btn == 65) {
      l->mouse.s = (l->mouse.btn == 64) ? 1 : -1;
      l->type = SCROLL;
    }

    return;

    // handle control+_ keys
  } else if (len == 1 && buf[0] <= 26) {
    l->special.key = buf[0] + 'A' - 1;
    l->type = SPECIAL;
    return;

    // handle regular keys
  } else if (len == 1) {
    l->buf = buf[0];
    l->len = len;
    l->type = KEY;
    return;
  }

  // key not supported
  l->type = ERR;
}
void _in_raw(void) {
  tcgetattr(STDIN_FILENO, &orig_term);
  struct termios term = orig_term;
  printf("\033[?1000h\033[?1003h\033[?1006h");

  term.c_lflag &= ~(ECHO | ICANON);
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
  fflush(stdout);
}

void _in_no_raw(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
  printf("\033[?1000l\033[?1003l\033[?1006l");
  fflush(stdout);
}

void in_clean(void){
	free(in_trig_stack);
	_in_no_raw();
}
