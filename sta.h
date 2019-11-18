/* 
 * sta - simple vt100 terminal abstraction 
 *
 * - buffer common terminal escape codes for rendering
 * - window size 
 * - window resize event 
 *
 * LINKS
 * https://github.com/antirez/kilo
 * http://www.termsys.demon.co.uk/vtansi.htm 
 * http://misc.flogisoft.com/bash/tip_colors_and_formatting
 * https://gist.github.com/XVilka/8346728
 */
 
typedef void (*resize_handler)(int);

/* enable raw mode */
void enable_raw_mode();

/* disable raw mode */
void disable_raw_mode();

/* append to buffer */
void append(const char *str);

/* write buffer to STDOUT and reset */
void apply();

/* append cursor position */
void move(int row, int col);

/* set cursor visibility */
void cursor(int visible);

/* move home and clear down */
void clear();

/* append foreground color */
void color_fg(int col);

/* append background color */
void color_bg(int col);

/* append reset color */
void color_reset();

/* init with a given resize handler */
void on_resize(resize_handler);

/* get window rows and cols */
int window_size(int *rows, int *cols);

