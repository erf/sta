/* 
 * sta - simple vt100 terminal abstraction 
 *
 * - buffer common terminal escape codes for rendering
 * - window size and resize event 
 *
 * LINKS
 * https://github.com/antirez/kilo
 * http://www.termsys.demon.co.uk/vtansi.htm 
 * http://misc.flogisoft.com/bash/tip_colors_and_formatting
 * https://gist.github.com/XVilka/8346728
 */
 
typedef void (*resize_handler)(int);

/* init given resize handler */
void init(resize_handler);

/* append to buffer */
void append(const char *str);

/* write buffer to STDOUT and reset */
void apply();

/* get window rows and cols */
int get_window_size(int *rows, int *cols);

/* sets cursor position */
void move(int row, int col);

/* set cursor visibility */
void cursor(int visible);

/* move home and clear down */
void clear();

/* set foreground color */
void color_fg(int col);

/* set background color */
void color_bg(int col);

/* resset color */
void color_reset();

