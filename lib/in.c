#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_term;

typedef enum { MOVE, CLICK, RELEASE, SCROLL, SPECIAL, ERR, KEY} _in_ret;

void disable_raw_mode();
void enable_raw_mode();

typedef void (*func)();

#define in_loop()                                                              \
  char buf[64]; /* string key buffer */                                        \
  int n;        /* input code int */                                           \
  while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0)

typedef struct {
  char buf[2];
  int len;
  _in_ret type;
  union {
    struct {
      int m;   // raw mouse value
      int x;   // x position
      int y;   // y position
			int s;   // mouse scroll dir -1 | 1
      char et;  // event type
      int btn; // event code
    } mouse;

    struct {
      char key[2];
    } special;
  };
} input;

void _in_catch(char *buf, int len, input * l) {
  if (len <= 0) {
    printf("error\n");
		l->type = ERR;
    return;
  }

  if (len >= 6 && buf[0] == '\033' && buf[1] == '[' && buf[2] == '<') {
    if (sscanf(buf, "\033[<%d;%d;%d%c", &l->mouse.btn, &l->mouse.x, &l->mouse.y, &l->mouse.et) != 4) {
      printf("error\n");
      return;
    }

    // set type
    l->type = (l->mouse.et == 'M') ? (l->mouse.btn & 0b11) == 3 ? MOVE : CLICK : RELEASE;

    if (l->mouse.btn == 64 || l->mouse.btn == 65) {
      l->mouse.s = (l->mouse.btn == 64) ? 1 : -1;
      l->type = SCROLL;
    }

    // handle control+_ keys
  } else if (len == 1 && buf[0] <= 26) {
    l->special.key[0] = buf[0] + 'A' - 1;
  	l->special.key[1] = '\0';
		l->type = SPECIAL;

  } else if (len == 1) {
		l->buf[0] = buf[0];
		l->len = len;
		l->type = KEY;
  }

  // key not supported
	l->type = ERR;
}


int main() {
  enable_raw_mode();
	input globby;

  in_loop() {
		_in_catch(buf, n, &globby);
	}

  disable_raw_mode();
  return 1;
}

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &orig_term);
  struct termios term = orig_term;
  printf("\033[?1000h\033[?1003h\033[?1006h");

  term.c_lflag &= ~(ECHO | ICANON);
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
  fflush(stdout);
}

void disable_raw_mode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
  printf("\033[?1000l\033[?1003l\033[?1006l");
  fflush(stdout);
}
