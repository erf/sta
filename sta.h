/* 
 * sta - simple vt100 terminal abstraction 
 *
 * - buffer common terminal escape codes for rendering
 * - window size and resize event 
 *
 * LINKS
 * https://github.com/antirez/kilo
 * http://vt100.net/docs/vt100-ug/contents.html
 * http://www.termsys.demon.co.uk/vtansi.htm 
 * http://ascii-table.com/ansi-escape-sequences-vt-100.php
 * 
 */
 
typedef void (*resize_handler)(int);

/* init given an resize handler */
void init(resize_handler);

/* append string to buffer */
void append(const char *str);

/* write command buffer to STDOUT and reset buffer */
void apply();

/* try to get the number of columns in the current terminal. */
int window_size(int *rows, int *cols);

/* VT-100 */

/* sets the cursor position where subsequent text will begin. */
void cursor_move(int row, int col);

/* show / hide cursor */
void cursor_show(int show);

/* erases the screen with the background colour and moves the cursor home. */
void clear();

/* clear line right of cursor */
void clear_line();

/* Enable scrolling from row {start} to row {end}. */
void scroll(int start, int end);

