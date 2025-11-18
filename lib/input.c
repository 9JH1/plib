#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

termsize get_term_size(void) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return (termsize){.WIDTH = w.ws_col, .HEIGHT = w.ws_row};
}

termsize await_resize() {
  termsize glob = get_term_size();
  int old = glob.HEIGHT * glob.WIDTH;
  while (old == glob.HEIGHT * glob.WIDTH) {
    glob = get_term_size();
    usleep(50);
  }

  // termsize changed
  return glob;
}

int await_scroll() {
  const int old = MOUSE_SCROLL_Y;
  InputEvent buff;
  while (old == MOUSE_SCROLL_Y)
    input(&buff);
  return MOUSE_SCROLL_Y;
}

int MOUSE_SCROLL_Y = 0;
int MOUSE_X = 0;
int MOUSE_Y = 0;

struct termios orig_termios;

// === Terminal Setup ===
void disable_raw_mode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
  printf("\033[?1000l\033[?1003l\033[?1006l");
  printf("\033[?25h");
  fflush(stdout);
}

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void enable_mouse_reporting() {
  printf("\033[?1000h\033[?1003h\033[?1006h");
  printf("\033[?25l");
  fflush(stdout);
}

void disable_mouse_reporting() {
  printf("\033[?1000l\033[?1003l\033[?1006l");
  printf("\033[25h");
  fflush(stdout);
}

void log_InputEvent(InputEvent local) {
  printf("returnc: %d\n", local.type);
  switch (local.type) {
  case KEY:
    printf("keybuf: %s\n", local.data.key.buf);
    printf("keycode: %c (%d)\n", local.data.key.code, local.data.key.code);
    break;

  case SPECIAL_KEY:
    printf("keycode: %d\n", local.data.special_key);
    break;

  case MOUSE_SCROLL:
    printf("scrolly: %d\n", local.data.scroll_y);
    break;

  case MOUSE_MOVE:
    printf("mouse x: %d\n", local.data.mouse.x);
    printf("mouse y: %d\n", local.data.mouse.y);
    break;

  case MOUSE_CLICK:
    printf("mouse m: %d\n", local.data.mouse.m);
    printf("mouse x: %d\n", local.data.mouse.x);
    printf("mouse y: %d\n", local.data.mouse.y);
    break;

  case MOUSE_RELEASE:
    printf("mouse was released\n");
    printf("mouse x: %d\n", local.data.mouse.x);
    printf("mouse y: %d\n", local.data.mouse.y);
    break;

  case ERROR:
    printf("Error Input\n");
    break;
  }
}

// multi-purpose event listener
// issues:
// cannot detect F keys or any other non-regular characters
// cannot detect use of alt keys
// pros:
// detects all mouse input perfectly
// detects arrow keys
// detects control+key keys
int input(InputEvent *local) {
  enable_raw_mode();
  enable_mouse_reporting();

  *local = (InputEvent){.type = ERROR};
  char buf[32] = {0};
  ssize_t len = read(STDIN_FILENO, buf, sizeof(buf) - 1);

  disable_mouse_reporting();
  disable_raw_mode();

  if (len <= 0)
    return ERROR;

  // Mouse input: "\033[<btn;x;yM" or "\033[<btn;x;ym"
  if (len >= 6 && buf[0] == '\033' && buf[1] == '[' && buf[2] == '<') {
    int btn, x, y;
    char event_type;
    if (sscanf(buf, "\033[<%d;%d;%d%c", &btn, &x, &y, &event_type) != 4)
      return ERROR;

    local->data.mouse.x = MOUSE_X = x;
    local->data.mouse.y = MOUSE_Y = y;
    local->data.mouse.m = btn & 0b11;

    if (btn == 64 || btn == 65) {
      MOUSE_SCROLL_Y += (btn == 64) ? 1 : -1;
      local->data.scroll_y = MOUSE_SCROLL_Y;
      return (local->type = MOUSE_SCROLL);
    }

    local->type = (event_type == 'M')
                      ? (btn & 0b11) == 3 ? MOUSE_MOVE : MOUSE_CLICK
                      : MOUSE_RELEASE;
    return local->type;
  }

  if (len == 1 && buf[0] <= 26) { // Ctrl+A to Ctrl+Z
    local->data.key.buf[0] = buf[0] + 'A' - 1;
    local->data.key.buf[1] = '\0';
    return (local->type = SPECIAL_KEY);
  }

  if (len == 1) {
    local->data.key.buf[0] = buf[0];
    local->data.key.buf[1] = '\0';
    local->data.key.code = buf[0];
    return (local->type = KEY);
  }

  strncpy(local->data.key.buf, buf, len);
  local->data.key.buf[len] = '\0';
  return ERROR;
}

// if p return o 
// if not p return c
#define take_pr_ch(p, c, o) ((p) ? (o) : (c))

char *take(const char *prompt, const char cover) {
  int out_idx = 0;
  int out_cap = 4; // Start with slightly larger capacity
  char *out = malloc(out_cap * sizeof(char));
  if (!out) {
    return NULL; // Check for malloc failure
  }
  int print = (cover == '\0'); // Print actual input if cover is '\0'

  enable_raw_mode();
	printf("%s",prompt);

  while (1) {
    int ch = getchar(); // Read input as int to handle EOF

    if (ch == EOF || ch == 3) { // Handle Ctrl+D or Ctrl+C
      free(out);
      disable_raw_mode();
      return NULL;
    }

    if (ch == '\n') { // Enter key
      out[out_idx] = '\0';
      break;
    } else if (ch == 127 && out_idx > 0) { // Backspace
      out_idx--;
      out[out_idx] = '\0'; // Clear the last character
      printf("\b \b");     // Move back, overwrite with space, move back again
    } else if (isprint(ch)) { // Only accept printable characters
      out[out_idx] = (char)ch;
      printf("%c", take_pr_ch(print, cover, out[out_idx]));
      out_idx++;
    } else {
      continue; // Ignore non-printable characters
    }

    // Dynamic memory allocation
    if (out_idx >= out_cap - 1) { // Leave space for null terminator
      out_cap *= 2;
      char *temp = realloc(out, out_cap * sizeof(char));
      if (!temp) {
        free(out);
        disable_raw_mode();
        return NULL;
      }
      out = temp;
    }
  }

  disable_raw_mode();
  putchar('\n');

  if (out_idx == 0) {
    free(out);
    return NULL;
  }

  return out;
}
