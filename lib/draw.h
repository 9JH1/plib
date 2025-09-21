#ifndef BOX_H
#define BOX_H 
#define ANSI_BYTE_SIZE 35 

typedef struct {
  const int width;
  const int height;
  const int x;
  const int y;
  const char fill; // char
  const char *ansi;
	char *string;
} solid_opt;

typedef enum {
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT,
} title_opt_position;

typedef struct {
  const char *title;
  const char *suffix;
  const char *prefix;
	const int padding;
  const char *ansi;
	char *string;
  const title_opt_position pos;
} title_opt;

typedef struct {
  const char *top;
  const char *top_left_corner;
  const char *top_right_corner;
  const char *bottom;
  const char *bottom_left_corner;
  const char *bottom_right_corner;
  const char *left;
  const char *right;
	char *string;
  const char *ansi;
} border_opt;

typedef struct {
	solid_opt solid;
	border_opt border;
	title_opt title;
	char* string;
} box_opt;

typedef struct {
	const int x;
	const int y;
	const char *ansi;
	const int meta_width;
	const int meta_height;
	char *string;
} sprite_opt;

int solid(solid_opt *opt);
int border(const solid_opt box_opt, border_opt *opt);
int title(const solid_opt box, title_opt *opt);
void box(box_opt *opt);
void free_box_opt(box_opt *opt);
int sprite(const char *array[], sprite_opt *opt);
#endif
