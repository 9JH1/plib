#ifndef ANSI_H
#define ANSI_H
#define ESCAPE_CODE "\033"
#define ESC ESCAPE_CODE
#define ansi(seq) printf("%s%s", ESC, seq);
#define activate_terminal_buffer() ansi("[?1049h");
#define deactivate_terminal_buffer() ansi("[?1049l");
#define enable_mouse_reporting_ansi() ansi("[?1003h");
#define disable_mouse_reporting_ansi() ansi("[?1003l");
#define hide_cursor() ansi("[?25l");
#define show_cursor() ansi("[?25h");
#define clear() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH",y,x)
#define cursor_home() ansi("[H")
#endif
