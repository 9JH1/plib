#ifndef COLOR_LIB
#define COLOR_LIB
#include <stdio.h>

static inline int _slen(const char* str){
    int i = 0;
    for(; str[i] != 0; i++);
    return i;
} 


static inline char *COLOR(const char *fg, const char *bg) {
  static char out[64];
  int pos = 0;
	out[0] = '\0';

  if (fg && _slen(fg) >= (fg[0] == '#' ? 7 : 6)) {
  	if (fg[0] == '#') fg++;
    	int r = ((fg[0] >= '0' && fg[0] <= '9') ? fg[0] - '0' : (fg[0] >= 'A' && fg[0] <= 'F') ? fg[0] - 'A' + 10 : (fg[0] >= 'a' && fg[0] <= 'f') ? fg[0] - 'a' + 10 : 0) * 16 +
              ((fg[1] >= '0' && fg[1] <= '9') ? fg[1] - '0' : (fg[1] >= 'A' && fg[1] <= 'F') ? fg[1] - 'A' + 10 : (fg[1] >= 'a' && fg[1] <= 'f') ? fg[1] - 'a' + 10 : 0);
      int g = ((fg[2] >= '0' && fg[2] <= '9') ? fg[2] - '0' : (fg[2] >= 'A' && fg[2] <= 'F') ? fg[2] - 'A' + 10 : (fg[2] >= 'a' && fg[2] <= 'f') ? fg[2] - 'a' + 10 : 0) * 16 +
              ((fg[3] >= '0' && fg[3] <= '9') ? fg[3] - '0' : (fg[3] >= 'A' && fg[3] <= 'F') ? fg[3] - 'A' + 10 : (fg[3] >= 'a' && fg[3] <= 'f') ? fg[3] - 'a' + 10 : 0);
      int b = ((fg[4] >= '0' && fg[4] <= '9') ? fg[4] - '0' : (fg[4] >= 'A' && fg[4] <= 'F') ? fg[4] - 'A' + 10 : (fg[4] >= 'a' && fg[4] <= 'f') ? fg[4] - 'a' + 10 : 0) * 16 +
              ((fg[5] >= '0' && fg[5] <= '9') ? fg[5] - '0' : (fg[5] >= 'A' && fg[5] <= 'F') ? fg[5] - 'A' + 10 : (fg[5] >= 'a' && fg[5] <= 'f') ? fg[5] - 'a' + 10 : 0);
      pos += snprintf(out + pos, sizeof(out) - pos, "\033[38;2;%d;%d;%dm", r, g, b);
    } else pos += snprintf(out + pos, sizeof(out) - pos, "\033[39m");

    if (bg && _slen(bg) >= (bg[0] == '#' ? 7 : 6)) {
      if (bg[0] == '#') bg++;
      int r = ((bg[0] >= '0' && bg[0] <= '9') ? bg[0] - '0' : (bg[0] >= 'A' && bg[0] <= 'F') ? bg[0] - 'A' + 10 : (bg[0] >= 'a' && bg[0] <= 'f') ? bg[0] - 'a' + 10 : 0) * 16 +
              ((bg[1] >= '0' && bg[1] <= '9') ? bg[1] - '0' : (bg[1] >= 'A' && bg[1] <= 'F') ? bg[1] - 'A' + 10 : (bg[1] >= 'a' && bg[1] <= 'f') ? bg[1] - 'a' + 10 : 0);
      int g = ((bg[2] >= '0' && bg[2] <= '9') ? bg[2] - '0' : (bg[2] >= 'A' && bg[2] <= 'F') ? bg[2] - 'A' + 10 : (bg[2] >= 'a' && bg[2] <= 'f') ? bg[2] - 'a' + 10 : 0) * 16 +
              ((bg[3] >= '0' && bg[3] <= '9') ? bg[3] - '0' : (bg[3] >= 'A' && bg[3] <= 'F') ? bg[3] - 'A' + 10 : (bg[3] >= 'a' && bg[3] <= 'f') ? bg[3] - 'a' + 10 : 0);
      int b = ((bg[4] >= '0' && bg[4] <= '9') ? bg[4] - '0' : (bg[4] >= 'A' && bg[4] <= 'F') ? bg[4] - 'A' + 10 : (bg[4] >= 'a' && bg[4] <= 'f') ? bg[4] - 'a' + 10 : 0) * 16 +
              ((bg[5] >= '0' && bg[5] <= '9') ? bg[5] - '0' : (bg[5] >= 'A' && bg[5] <= 'F') ? bg[5] - 'A' + 10 : (bg[5] >= 'a' && bg[5] <= 'f') ? bg[5] - 'a' + 10 : 0);
      pos += snprintf(out + pos, sizeof(out) - pos, "\033[48;2;%d;%d;%dm", r, g, b);
    } else pos += snprintf(out + pos, sizeof(out) - pos, "\033[49m");

    return out;
}

#define BG(c) COLOR(NULL, c)
#define FG(c) COLOR(c, NULL)

static inline char *RGB_ANSI_BG(const int r, const int g, const int b){
	static char out[32];
	const int pos = snprintf(out, sizeof(out), "\033[48;2;%d;%d;%dm",r,g,b);
	out[pos] = '\0';
	return out;
}

#endif
