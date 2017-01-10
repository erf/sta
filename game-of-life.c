#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "sta.c"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int rows, cols;

const int MAX_ROWS = 64;
const int MAX_COLS = 256;
int curr [MAX_ROWS][MAX_COLS];
int next [MAX_ROWS][MAX_COLS];

const int neighbours[8][2] = {
	{ -1, -1 }, {  0, -1 }, {  1, -1 },
	{ -1,  0 },             {  1,  0 },
	{ -1,  1 }, {  0,  1 }, {  1,  1 },
};

void init_grid() {
	for (int row=0; row<MAX_ROWS; row++)
		for (int col=0; col<MAX_COLS; col++)
			curr[row][col] = floor(rand() / (RAND_MAX/2));
}

int live_neighbours(int row, int col, int grid[][MAX_COLS]) {
	int sum = 0;
	for(int i=0; i<8; i++) {
		int x = col + neighbours[i][0];
		if(x < 0 || x >= MAX_COLS)
			continue;
		int y = row + neighbours[i][1];
		if(y < 0 || y >= MAX_ROWS)
			continue;
		sum += grid[y][x];
	}
	return sum;
}

/*
 * 1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
 * 2. Any live cell with two or three live neighbours lives on to the next generation.
 * 3. Any live cell with more than three live neighbours dies, as if by overpopulation.
 * 4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 */
void update() {

	memset(next, 0, sizeof(int) * MAX_ROWS * MAX_COLS);

	for (int row=0; row<MAX_ROWS; row++)
		for (int col=0; col<MAX_COLS; col++) {
			int n = live_neighbours(row, col, curr);
			int v = curr[row][col];
			next[row][col] = 
				(v == 1 && (n == 2 || n == 3)) || 
				(v == 0 && n == 3);
		}
	
	memcpy(curr, next, sizeof(next));
}

void draw() {
	cursor(0);
	clear();
	for (int row=0; row<MIN(rows, MAX_ROWS); row++)
		for (int col=0; col<MIN(cols, MAX_COLS); col++) {
			move(row + 1, col + 1);
			if(curr[row][col]) 
				append("#");
		}
		
	const char * pressEscapeToQuit = "Press 'q' to quit";
	move(rows, cols - strlen(pressEscapeToQuit));
	append(pressEscapeToQuit);
	
	move(rows, 1);
	char buf[32];
	snprintf(buf, sizeof(buf), "rows %d cols %d", rows, cols);
	append(buf);
	
	apply();
}

void on_resize(int sig){
	get_window_size(&rows, &cols);
	draw();
}

void input(int fd) {
	int nread;
    char c;
    while ((nread = read(fd,&c,1)) == 0);
    if (nread == -1) exit(1);
    switch(c) {
    	case 'q': 
    	move(1, 1);
    	clear();
    	apply();
    	exit(0);
    	break;
    }
}

void alert(unsigned int ms)
{
  struct itimerval old, new;
  new.it_interval.tv_sec = 0;
  new.it_interval.tv_usec = 0;
  new.it_value.tv_sec = 0;
  new.it_value.tv_usec = ms;
  setitimer(ITIMER_REAL, &new, NULL);
}

void on_alarm(int sig) {
    draw();
    update();
    alert(200*1000);
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	init(on_resize);
	init_grid();
	get_window_size(&rows, &cols);
	signal(SIGALRM, on_alarm);
	alert(200*1000);
	while(1) {
		input(STDIN_FILENO);
	}
	return 0;
}

