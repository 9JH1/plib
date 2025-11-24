// advanced-box-renderer

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

//-----------------------------------------------
// HEADER FILE
// ----------------------------------------------

// Basic Vector 2
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
} box;

// Macro Definitions
#define vec(x, y)                                                              \
  (struct vec2) { x, y }

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

size_t get_ansi_size(box *o){
    const size_t border_ansi = snprintf(NULL, 0, "%s",o->border.ansi);
    const size_t ansi = snprintf(NULL, 0, "%s", o->ansi);
    const int fill_height = o->size.y-2;
    return (2*border_ansi) + (ansi*fill_height) + (border_ansi*fill_height);
}

struct dma {
  int cap;
  int idx;
  char *str;
};

void gen_pos_ansi(struct dma *o, int y, int x) {
  o->idx += snprintf(o->str + o->idx, o->cap - o->idx, "\033[%d;%dH", y + 1, x + 1);
}

char *_box_render(box *o) 
{
  size_t pos_b = get_pos_size(o); 
  size_t fill_b = (o->size.x - 2) * (o->size.y - 2);
  size_t border_b = get_border_size(o);
	size_t ansi_b = get_ansi_size(o);

	struct dma out;
  out.cap = pos_b + border_b + fill_b + ansi_b + 1;
  printf("Using %d bytes\n", out.cap);

  out.idx = 0;
  out.str = malloc(out.cap);

  // Render fill
  for (int y = 0; y < o->size.y; ++y) 
	{
    gen_pos_ansi(&out, o->pos.y + y, o->pos.x);
    for (int x = 0; x < o->size.x; ++x) 
		{
      char *border = is_border(o, y, x);
			if (!border)
			{
				if(x == 1){
					out.idx += snprintf(out.str + out.idx, out.cap - out.idx, "%s", o->ansi);	
				}
        // Write fill
        out.str[out.idx++] = o->fill;
      }
			else 
			{
				if((x == 0 && y == 0) || (x == o->size.x - 1 && y != o->size.y - 1 && y != 0))
				{
					out.idx += snprintf(out.str + out.idx, out.cap - out.idx, "%s", o->border.ansi);
				}
        // Write border character
        out.idx += snprintf(out.str + out.idx, out.cap - out.idx, "%s", border);
      }
    }
  }

  out.str[out.idx++] = '\0';

  printf("remaining bytes: %d/%d\n", out.cap - out.idx, out.cap);
  return out.str;
}

#define UI_BOX(...) _init_box((box){__VA_ARGS__})
#define UI_BOX_DRAW(o) printf("%s", o._r)
box _init_box(box o) {
  o._r = _box_render(&o);
  return o;
}


int main() {

  struct vec2 box_size = vec(20, 10);

  border b = {
      .top = "━",
      .bottom = "━",
      .left = "┃",
      .right = "┃",
      .top_right = "┓",
      .top_left = "┏",
      .bottom_left = "┗",
      .bottom_right = "┛",
      .ansi = "\033[48;2;255;0;0m",
  };

  box one = UI_BOX(box_size, vec(0, 3), .ansi = "\033[48;2;0;255;0m", .fill = '#', .border = b);

  // draw boxes
  UI_BOX_DRAW(one);

  // clean up
  free(one._r);
}
