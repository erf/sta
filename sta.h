/* 
 * sta - simple vt100 terminal abstraction 
 *
 * - buffer common terminal escape codes for rendering
 * - window size and resize event 
 *
 * LINKS
 * https://github.com/antirez/kilo
 * http://www.termsys.demon.co.uk/vtansi.htm 
 * 
 */
 
typedef void (*resize_handler)(int);

/* init given an resize handler. */
void init(resize_handler);

/* append string to buffer. */
void append(const char *str);

/* write command buffer to STDOUT and reset buffer. */
void apply();

/* try to get the number of columns in the current terminal. */
int get_window_size(int *rows, int *cols);

/* sets cursor position where subsequent text will begin. */
void move(int row, int col);

/* set cursor visibility */
void cursor(int visible);

/* clear the screen and moves the cursor home. */
void clear();

/* clear line right of cursor. */
void clear_line();

