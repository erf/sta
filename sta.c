#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>

#include "sta.h"

#define MAX 1024*1024
int len = 0;
char buffer [MAX];

static struct termios orig_termios;

void disable_raw_mode() {
	tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_termios);
}

void at_exit(void) {
    disable_raw_mode();
}

void enable_raw_mode() {
    atexit(at_exit);
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void append(const char *str) {
	size_t str_len = strlen(str);
    memcpy(buffer+len, str, str_len);
    len += str_len;
}

void append_char(const char c) {
    buffer[len++] = c;
}

void apply() {
	write(STDOUT_FILENO, buffer, len);
    memset(buffer, 0, MAX);
    len = 0;
}

void move(int row, int col) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[%d;%dH", row, col);
	append(buf);
}

void cursor(int visible) {
	if (visible) 
		append("\x1b[?25h");	
	else 
		append("\x1b[?25l");	
}

void clear() {
	append("\x1b[H"); // Go home
	append("\x1b[J"); // erase down
}

void color_fg(int col) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[38;5;%dm", col);
	append(buf);
}

void color_bg(int col) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[48;5;%dm", col);
	append(buf);
}

void color_reset() {
	append("\x1b[0m");
}

void on_resize(resize_handler handler) {
	signal(SIGWINCH, handler);
}

int window_size(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == 0) {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
    return -1;
}

