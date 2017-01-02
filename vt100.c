/* 
 * vt100 - simple vt100 terminal abstraction 
 *
 * - buffer common terminal escape codes for rendering
 * - vt 100
 *
 * LINKS
 * https://github.com/antirez/kilo
 * http://www.termsys.demon.co.uk/vtansi.htm 
 * 
 */
 
//#define _BSD_SOURCE
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <termios.h>
//#include <errno.h>
//#include <ctype.h>
//#include <sys/types.h>
//#include <sys/ioctl.h>
//#include <sys/time.h>
//#include <stdarg.h>
//#include <fcntl.h>

typedef struct {
    char *data;
    int len;
} Buffer;

static Buffer buffer;

void append(const char *str) {
	size_t len = strlen(str);
    char *new = realloc(buffer.data, buffer.len+len);
    if (new == NULL) return;
    memcpy(new+buffer.len, str, len);
    buffer.data = new;
    buffer.len += len;
}

/* vt100 escape codes */

/* Requests a Report Device Code response from the device. */
void device_query() {
	append("\x1b[c");
}

void cursor_query() {
	append("\x1b[6n");
}

void cursor_hide() {
	append("\x1b[?25l");	
}

void cursor_show() {
	append("\x1b[?25h");	
}

/* Erases the screen with the background colour and moves the cursor home. */
void clear() {
	append("\x1b[2J");
}

void clear_line() {
	append("\x1b[0K");
}
/* Sets the cursor position where subsequent text will begin. 
 * If no row/column parameters are provided (ie. <ESC>[H), the cursor will move
 * to the home position, at the upper left of the screen. */
void move(int row, int col) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[%d;%dH", row, col);
	append(buf);
}

/* Enable scrolling from row {start} to row {end}. */
void scroll(int start, int end) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[%d;%dr", start, end);
	append(buf);
}

void apply() {
	write(STDOUT_FILENO, buffer.data, buffer.len);
    free(buffer.data);
}

int main(int argc, char *argv[]) {

	// how to
	// 1. append commands to buffer
	// 2. apply
	// 3. wait for input
	
	cursor_hide();
	clear();
	char buf[32];
	for(int i=0; i<3; i++) {
		move(1+i, 1);
    	snprintf(buf, sizeof(buf),"line %d", i);
		append(buf);
	}
	cursor_show();
	apply();
	
	return 0;
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
