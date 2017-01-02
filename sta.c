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

typedef struct {
    char *data;
    int len;
} Buffer;

static Buffer buffer = { NULL, 0 };
static int rawmode = 0;
static struct termios orig_termios; /* In order to restore at exit.*/

void disable_raw_mode(int fd) {
    /* Don't even check the return value as it's too late. */
    if (rawmode) {
        tcsetattr(fd,TCSAFLUSH,&orig_termios);
        rawmode = 0;
    }
}

/* Called at exit to avoid remaining in raw mode. */
void editor_at_exit(void) {
    disable_raw_mode(STDIN_FILENO);
}

/* Raw mode: 1960 magic shit. */
int enable_raw_mode(int fd) {
    struct termios raw;

    if (rawmode) return 0; /* Already enabled. */
    if (!isatty(STDIN_FILENO)) goto fatal;
    atexit(editor_at_exit);
    if (tcgetattr(fd,&orig_termios) == -1) goto fatal;

    raw = orig_termios;  /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer. */
    raw.c_cc[VMIN] = 0; /* Return each byte, or zero for timeout. */
    raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(fd,TCSAFLUSH,&raw) < 0) goto fatal;
    rawmode = 1;
    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}

void init(resize_handler handler) {
	signal(SIGWINCH, handler);
	enable_raw_mode(STDIN_FILENO);
}

int get_window_size(int *rows, int *cols) {
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
	append("\x1b[2J");
}

void clear_line() {
	append("\x1b[K");
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
