/ advanced-box-renderer

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------
// HEADER FILE
// ----------------------------------------------

// function type
typedef void (*func)();

// basic Vector 2
struct vec2 {
  int x, y;
};

// Box options
typedef struct box {
  struct vec2 size;
  struct vec2 pos;
  char *ansi;
  char fill; // fill char

  char *_r; // Rendered
  int _d;   // Disable
} box;

// Function Declarations
char *_box_render(box *);
box _init_box(box);

// Macro Definitions
#define vec(x, y)                                                              \
  (struct vec2) { x, y }

#define UI_BOX(...) _init_box((box){__VA_ARGS__})
#define UI_BOX_DRAW(o) printf("%s", o._r);
#define _UI_RESET_CODE "\033[0m"
//-----------------------------------------------

int digits(int n) {
  if (n == 0) {
    return 1;
  }

  int count = 0;
  if (n < 0) {
    n = -n;
  }
  while (n > 0) {
    n /= 10;
    count++;
  }
  return count;
}

// render box
char *_box_render(box *o) { // clang-format off
	const size_t block_start = strlen(o->ansi);
	const size_t block_end   = strlen(_UI_RESET_CODE);

  const size_t line_max    =
		+ strlen("\033[;H")
		+ snprintf(NULL, 0,"\033[%d;%dH",
			+ digits(o->size.x + o->pos.x),
			+ digits(o->size.y + o->pos.y))
		+ o->size.x;
	//  ^ Size of the fill char
	
	const size_t out_max = 
		+ block_start 
		+ line_max * o->size.y 
		+ block_end 
		+ 1;
	//  ^ String terminator

	int out_i = 0;
	char *out_s = malloc(out_max);
	if(!out_s) return NULL;	
	
	// copy ANSI bytes
	out_i += snprintf(out_s + out_i, out_max - out_i, "%s", o->ansi);

	// Compute box data
	for(int h = 0; h < o->size.y; ++h){
		int line_i = 0;
		char line_s[line_max];

		// Copy position bytes to line
		line_i += snprintf(line_s + line_i, line_max - line_i, "\033[%d;%dH",
				o->pos.y + 1 + h,
				o->pos.x + 1);
		//             ^ Terminals index co-ordinates at 1

		// Generate and append line/row bytes
		for(int line = 0; line < o->size.x; ++line)
			line_i += snprintf(line_s + line_i, line_max - line_i, "%c", o->fill);
		line_s[line_i] = '\0';
		out_i += snprintf(out_s + out_i, out_max - out_i, "%s", line_s);
	}

	// Final terminators
	out_i += snprintf(out_s + out_i, out_max - out_i, "%s", _UI_RESET_CODE);
	out_s[out_i] = '\0';

  return out_s;
} // clang-format off

/*
 * Called as a wrapper for box that initiates values
 * and returns the new initialized box.
 */
box _init_box(box o) {

	// Pre-render
  o._r = _box_render(&o);
  return o;
}

#include "color.h"


int main() {

	struct vec2 box_size = vec(200,100);

  box one = UI_BOX(
			box_size,
			vec(0, 0),
			.ansi = color(NULL, "#ff0000"), 
			.fill = 'A');

	box two = UI_BOX(
			box_size,
			vec(box_size.x, 0),
			.ansi = color(NULL, "#00ff00"),
			.fill = 'B');

	box three = UI_BOX(
			box_size,
			vec(0, box_size.y),
			.ansi = color(NULL, "#0000ff"),
			.fill = 'C');

	// draw boxes
	UI_BOX_DRAW(one);
	UI_BOX_DRAW(two);
	UI_BOX_DRAW(three);

	// clean up
	free(one._r);
	free(two._r);
	free(three._r);
}
