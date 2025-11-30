// in_ui_hover_box(func, x1,y1,x2,y2);
// in_ui_click_box(func, x1,y1,x2,y2)
// pmoddr'
// r

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include "input.h"

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
		const int width = loc.mouse.x + loc.mouse.x1;
		const int height = loc.mouse.y + loc.mouse.y1;
    
		if (loc.type != in->type)
      continue;

    switch (loc.type) {
    	case KEY:
      	if (in->buf == loc.buf && loc.call != NULL)
        	loc.call(" ");
      	break;

    	case SPECIAL:
      	if (in->special.key == loc.buf && loc.call != NULL)
      		loc.call(" ");
      	break;

			case MOVE:
				if (in->mouse.x >= width + 1)
					break;

				if(in->mouse.y >= height + 1)
					break;

				if(in->mouse.x <= loc.mouse.x)
					break;

				if(in->mouse.y <= loc.mouse.y)
					break;

				loc.call(loc.self);
				break;
				
			case CLICK:
				if (in->mouse.x >= width + 1)
					break;

				if(in->mouse.y >= height + 1)
					break;

				if(in->mouse.x <= loc.mouse.x)
					break;

				if(in->mouse.y <= loc.mouse.y)
					break;

				loc.call(loc.self);
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


char *is_border(box *o, int row, int col) {
  if (row == 0 && col == 0)
    return o->border.top_left;
  if (row == 0 && col == o->size.x - 1)
    return o->border.top_right;
  if (row == o->size.y - 1 && col == 0)
    return o->border.bottom_left;
  if (row == o->size.y - 1 && col == o->size.x - 1)
    return o->border.bottom_right;
  if (row == 0)
    return o->border.top;
  if (row == o->size.y - 1)
    return o->border.bottom;
  if (col == 0)
    return o->border.left;
  if (col == o->size.x - 1)
    return o->border.right;

  return NULL;
}

size_t get_border_size(box *o) {
  size_t bytes = 0;
  for (int y = 0; y < o->size.y; ++y) {
    for (int x = 0; x < o->size.x; ++x) {
      char *b = is_border(o, y, x);
      if (b)
        bytes += strlen(b);
    }
  }
  return bytes;
}

size_t get_pos_size(box *o) {
  size_t bytes = 0;
  for (int h = 0; h < o->size.y; ++h) {
    bytes += snprintf(NULL, 0, "\033[%d;%dH", o->pos.y + h + 1, o->pos.x + 1);
  }
  return bytes;
}

size_t get_ansi_size(box *o) {
  size_t total_ansi = 0;
  size_t border_ansi_len = strlen(o->border.ansi);
  size_t ansi_len = strlen(o->ansi);

  total_ansi += 2 * border_ansi_len;

  int interior_rows = o->size.y - 2;
  if (interior_rows > 0) {
    total_ansi += border_ansi_len;            // left border
    total_ansi += ansi_len * (o->size.x - 2); // fill characters
    total_ansi += border_ansi_len;            // right border
    total_ansi *= interior_rows;
  }

  return total_ansi;
}

struct dma {
  int cap;
  int idx;
  char *str;
};

void gen_pos_ansi(struct dma *o, int y, int x) {
  o->idx +=
      snprintf(o->str + o->idx, o->cap - o->idx, "\033[%d;%dH", y + 1, x + 1);
}

void UI_PRINTIN(box *o, const char *fmt, ...) {
  if (o->pr_idx >= o->size.y - 2) {
    return;
  }

  printf("\033[%d;%dH", o->pos.y + 2 + o->pr_idx, o->pos.x + 2);

  if (!o->pr_inherit_ansi) {
    printf("%s", o->ansi);
  }

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  o->pr_idx++;
}

char *_box_render(box *o) {
  size_t pos_b = get_pos_size(o);
  size_t fill_b = (o->size.x - 2) * (o->size.y - 2);
  size_t border_b = get_border_size(o);
  size_t ansi_b = get_ansi_size(o);

  struct dma out;
  out.cap = pos_b + border_b + fill_b + ansi_b + 1;
  // printf("Using %d bytes\n", out.cap);

  out.idx = 0;
  out.str = malloc(out.cap);
  memset(out.str, 0, out.cap);

  // Render fill
  for (int y = 0; y < o->size.y; ++y) {
    gen_pos_ansi(&out, o->pos.y + y, o->pos.x);
    for (int x = 0; x < o->size.x; ++x) {
      char *border = is_border(o, y, x);
      if (!border) {
        if (x == 1) {
          out.idx +=
              snprintf(out.str + out.idx, out.cap - out.idx, "%s", o->ansi);
        }
        // Write fill
        out.str[out.idx++] = o->fill;
      } else {
        if ((x == 0 && y == 0) ||
            (x == o->size.x - 1 && y != o->size.y - 1 && y != 0)) {
          out.idx += snprintf(out.str + out.idx, out.cap - out.idx, "%s",
                              o->border.ansi);
        }
        // Write border character
        out.idx += snprintf(out.str + out.idx, out.cap - out.idx, "%s", border);
      }
    }
  }

  out.str[out.idx++] = '\0';

  // printf("remaining bytes: %d/%d\n", out.cap - out.idx, out.cap);
  return out.str;
}


void UI_CLEAR(box *o) {
  o->pr_idx = 0;
  printf("%s", o->_r);
}

box _init_box(box o) {
  o.pr_idx = 0;
  o.pr_inherit_ansi = 0;
  o._r = _box_render(&o);
  return o;
}

border _init_border(char *a, char*b, char*c){
	return (border){
		a,
		a,
		b,
		b,
		c,
		c,
		c,
		c,
		.ansi = ""
	};
}
