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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
//#include <termios.h>
//#include <errno.h>
//#include <ctype.h>
//#include <sys/time.h>
//#include <stdarg.h>
//#include <fcntl.h>

/* buffer */

typedef struct {
    char *data;
    int len;
} Buffer;

static Buffer buffer = { NULL, 0 };

void append(const char *str) {
	size_t len = strlen(str);
    char *new = realloc(buffer.data, buffer.len+len);
    if (new == NULL) return;
    memcpy(new+buffer.len, str, len);
    buffer.data = new;
    buffer.len += len;
}

/* vt100 escape codes */

void cursor_position() {
	append("\x1b[6n");
	// response: EscLine;ColumnR 
}

/* Sets the cursor position where subsequent text will begin. 
 * If no row/column parameters are provided (ie. <ESC>[H), the cursor will move
 * to the home position, at the upper left of the screen. */
void cursor_move(int row, int col) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[%d;%dH", row, col);
	append(buf);
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
	append("\x1b[K");
}

/* Enable scrolling from row {start} to row {end}. */
void scroll(int start, int end) {
	char buf[32];
    snprintf(buf, sizeof(buf),"\x1b[%d;%dr", start, end);
	append(buf);
}

void apply() {
	write(STDOUT_FILENO, buffer.data, buffer.len);
    memset(buffer.data, 0, buffer.len);
    buffer.data = NULL;
    buffer.len = 0;
}

/* window handling */

/* Try to get the number of columns in the current terminal.
 * Returns 0 on success, -1 on error. */
int getWindowSize(int ifd, int ofd, int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == 0) {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
    return -1;
}


void lines(int num, int row) {
	for(int i=0; i<num; i++) {
		cursor_move(1+row+i, 1);
		char buf[32];
    	snprintf(buf, sizeof(buf),"line %d", i);
		append(buf);
	}
}

void refresh() {
	cursor_hide();
	clear();
	lines(10, 0);
	lines(5, 10);
	apply();
	cursor_show();
}

void handle_winch(int sig){

	refresh();
	printf("resize");

 	signal(SIGWINCH, SIG_IGN);	
 	
 	// Reinitialize the window to update data structures.
 	//endwin();
 	//initscr();
 	//refresh();
 	//clear();	
 	//char tmp[128];
 	//sprintf(tmp, "%dx%d", COLS, LINES);	
 	//// Approximate the center
 	//int x = COLS / 2 - strlen(tmp) / 2;
 	//int y = LINES / 2 - 1;	
 	//mvaddstr(y, x, tmp);
 	//refresh();	
 	
 	signal(SIGWINCH, handle_winch);
}

int main(int argc, char *argv[]) {

	// how to
	// 1. append commands to buffer
	// 2. apply
	// 3. wait for input
	refresh();
	
	signal(SIGWINCH, handle_winch);
	
	while(getchar() != 27) {}
	
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
