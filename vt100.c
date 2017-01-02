#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "vt100.h"

typedef struct {
    char *data;
    int len;
} Buffer;

static Buffer buffer = { NULL, 0 };

void init(resize_handler handler) {
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

void append(const char *str) {
	size_t len = strlen(str);
    char *new = realloc(buffer.data, buffer.len+len);
    if (new == NULL) return;
    memcpy(new+buffer.len, str, len);
    buffer.data = new;
    buffer.len += len;
}

void apply() {
	write(STDOUT_FILENO, buffer.data, buffer.len);
    memset(buffer.data, 0, buffer.len);
    buffer.data = NULL;
    buffer.len = 0;
}

void cursor_move(int row, int col) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[%d;%dH", row, col);
	append(buf);
}

void cursor_show(int show) {
	if (show) 
		append("\x1b[?25h");	
	else 
		append("\x1b[?25l");	
}

void clear() {
	append("\x1b[2J");
}

void clear_line() {
	append("\x1b[K");
}

void scroll(int start, int end) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[%d;%dr", start, end);
	append(buf);
}

/* TODO support multiple terminals */
/*
typedef struct {
	void (*clear) ();
} Terminal;

enum {
	TERM_VT100
};

static const Terminal terminals [] = {
	[TERM_VT100] = {
		clear,
	}
};
*/
