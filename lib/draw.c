#include "draw.h"
#include <stdio.h>
#include <stdlib.h>

// draw a solid box on screen
int solid(solid_opt *opt) {
  const int max_len = opt->width + ANSI_BYTE_SIZE;
  const int max_out =
      max_len * opt->height + (8 * ANSI_BYTE_SIZE) + ANSI_BYTE_SIZE;
  char *box = malloc(max_out);
  if (!box) {
    printf("Failed to allocate %d bytes of memory for solid\n", max_out);
    return 1;
  }

  int idx = 0;
  box[0] = '\0';
  idx = snprintf(box, max_out, "%s", opt->ansi);

  for (int h = 0; h < opt->height; h++) {
    char line[max_len];
    int pos = 0;
    int cur = opt->y + h;
    pos += snprintf(line + pos, max_len - pos, "\033[%d;%dH", cur, opt->x);
    for (int w = 0; w < opt->width; w++) {
      if (pos < max_len - 1) {
        line[pos++] = opt->fill;
      }
    }

    line[pos] = '\0';
    idx += snprintf(box + idx, max_out - idx, "%s", line);
  }
  opt->string = box;
  return 0;
}

int slen(const char *str) {
  size_t length = 0;
  while (*str != '\0') {
    length++;
    str++;
  }
  return length;
}

int border(const solid_opt box_opt, border_opt *opt) {
  const int max_len = box_opt.width * (ANSI_BYTE_SIZE * 2);
  const int y = box_opt.y;
  const int x = box_opt.x;
  const int h = box_opt.height;
  const int w = box_opt.width;
  const int max_out = h * (ANSI_BYTE_SIZE * 2);
  const char *ansi = (opt->ansi && slen(opt->ansi) > 0)
                         ? opt->ansi
                         : (box_opt.ansi ? box_opt.ansi : "");

  char *box = malloc(max_out);
  if (!box) {
    printf("Failed to allocate %d bytes of memory for border\n", max_out);
    return 1;
  }

  box[0] = '\0';
  snprintf(box, max_out, "%s", ansi);
  int pos = 0;

  // top
  pos += snprintf(box, max_len, "\033[%d;%dH%s", y, x, opt->top_left_corner);
  for (int i = 0; i < w - 2; i++)
    pos += snprintf(box + pos, max_len - pos, "%s", opt->top);
  pos += snprintf(box + pos, max_len - pos, "%s", opt->top_right_corner);

  // bottom
  pos += snprintf(box + pos, max_len - pos, "\033[%d;%dH%s", y + h - 1, x,
                  opt->bottom_left_corner);
  for (int i = 0; i < w - 2; i++)
    pos += snprintf(box + pos, max_len - pos, "%s", opt->bottom);
  pos += snprintf(box + pos, max_len - pos, "%s", opt->bottom_right_corner);

  // left / right
  for (int i = 1; i < h - 1; i++) {
    pos += snprintf(box + pos, max_len - pos, "\033[%d;%dH%s", y + i, x,
                    opt->left);
    pos += snprintf(box + pos, max_len - pos, "\033[%d;%dH%s", y + i, x + w - 1,
                    opt->right);
  }

  opt->string = box;
  return 0;
}

int title(const solid_opt box, title_opt *opt) {
  const title_opt_position pos = opt->pos;
  const int title_len =
      slen(opt->title) + slen(opt->suffix) + slen(opt->prefix);
  const char *ansi = (opt->ansi && slen(opt->ansi) > 0)
                         ? opt->ansi
                         : (box.ansi ? box.ansi : "");
  int y = box.y, x = box.x;

  if (pos == TOP_LEFT)
    x += opt->padding + 1;
  else if (pos == TOP_RIGHT) {
    x += box.width - title_len;
    x -= opt->padding + 1;
  } else if (pos == BOTTOM_LEFT) {
    y += box.height - 1;
    x += opt->padding + 1;
  } else if (pos == BOTTOM_RIGHT) {
    y += box.height - 1;
    x += box.width - title_len;
    x -= opt->padding + 1;
  }

  int max_size = 2 * ANSI_BYTE_SIZE + slen(opt->prefix) + slen(opt->title) +
                 slen(opt->suffix);
  char *out = malloc(max_size);
  if (!out) {
    printf("failed to allocate %d bytes for title\n", max_size);
    return 1;
  }
  out[0] = '\0';

  snprintf(out, max_size, "\033[%d;%dH%s%s%s%s", y, x, ansi, opt->prefix,
           opt->title, opt->suffix);
  opt->string = out;
  return 0;
}

void box(box_opt *opt) {
  int ret = 0;
  // draw solid (1 print call)
  ret = solid(&opt->solid);
  if (ret)
    return;

  // draw border (1 print call)
  ret = border(opt->solid, &opt->border);
  if (ret)
    return;

  // draw title (1 print call)
  ret = title(opt->solid, &opt->title);
  if (ret)
    return;

  int max_size = slen(opt->solid.string) + slen(opt->border.string) +
                 slen(opt->title.string) + ANSI_BYTE_SIZE;
  char *out = malloc(max_size);
  out[0] = '\0';

  snprintf(out, max_size, "%s%s%s", opt->solid.string, opt->border.string,
           opt->title.string);
  opt->string = out;
}

#define f(opt) free(opt.string);
void free_box_opt(box_opt *opt) {
  f(opt->solid);
  f(opt->border);
  f(opt->title);
  free(opt->string);
}

int sprite(const char *array[], sprite_opt *opt) {
  int max_size = (opt->ansi ? slen(opt->ansi) : 0) + 1;
  for (int i = 0; i < opt->meta_height; i++)
    max_size += (array[i] ? slen(array[i]) : 0) + ANSI_BYTE_SIZE + 1;

  char *out = malloc(max_size);
  if (!out) {
    printf("Failed to allocate %d bytes for sprite\n", max_size);
    return 1;
  }

  out[0] = '\0';
  int pos = snprintf(out, max_size, "%s", opt->ansi ? opt->ansi : "");
  for (int i = 0; i < opt->meta_height; i++) {
    if (array[i] && slen(array[i]) > 0) {
      int written = snprintf(out + pos, max_size - pos, "\033[%d;%dH%s",
                             opt->y + i, opt->x, array[i]);
      if (written >= max_size - pos) {
        printf("Buffer overflow in sprite at line %d\n", i);
        free(out);
        return 1;
      }
      pos += written;
    }
  }
  opt->string = out;
  return 0;
}
