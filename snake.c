#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "sta.c"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX_ROWS 16
#define MAX_COLS 32
#define MAX_SNAKE MAX_ROWS*MAX_COLS
#define UPDATE_TIME 150

typedef struct { int x, y; } Pos;

int rows, cols;
Pos snake [MAX_SNAKE];
int snake_len = 1;
Pos food;
Pos dir;
int quit = 0;
int game_over = 0;

int r(int min, int max) {
	double one = (double) rand() / (double) RAND_MAX;
	int num = min + (int)(one * ( max - min ));
	return num;
}

Pos r2() {
	int x = r(1, MAX_COLS-1);
	int y = r(1, MAX_ROWS-1);
	return (Pos) { .x = x, .y = y };
}

void init_game() {

	// init snake
	snake_len = 1;
	snake[0] = (Pos) { .x = MAX_COLS/2, .y = MAX_ROWS/2 };
	dir = (Pos) { .x = 1, .y = 0 };
	
	// init food
	food = r2();
	
}

void update() {

	// update snake tail
	for(int i=snake_len-1; i>0; i--) {
		snake[i] = snake[i-1];
	}

	// update snake head
	snake[0].x += dir.x;
	snake[0].y += dir.y;
		
	// check if snake hit wall
	Pos p = snake[0];
	if (p.x==0 || p.y==0 || p.y==MAX_ROWS-1 || p.x==MAX_COLS-1) {
		game_over = 1;
	}
	
	// TODO check if snake hit itself
	
	// check if food was eaten and create new food
	if(p.x == food.x && p.y == food.y) {
		++snake_len;
		snake[snake_len-1] = snake[snake_len-2];
		food = r2();
	}
	
}

void draw() {

	cursor(0);
	color_bg(0);
	clear();
	
	// draw wall
	color_fg(7);
	color_bg(238);
	for(int row=0; row<MAX_ROWS; row++)
	for(int col=0; col<MAX_COLS; col++) {
		move(row + 1, col + 1);
		if(row==0 || col==0 || row==MAX_ROWS-1 || col==MAX_COLS-1)
			append("#");
		else 
			append(" ");
	}
	
	// draw food
	color_fg(9);
	move(food.y + 1, food.x + 1);
	append("&");
	
	// draw snake
	color_fg(11);
	for(int i=0; i<snake_len; i++) {
		Pos p = snake[i];
		move(p.y + 1, p.x + 1);
		append("s");
	}
		
	if(game_over) {
		color_fg(226);
		const char * gameOver = "Game Over";
		move(rows/2, (cols/2) - (strlen(gameOver)/2));
		append(gameOver);
		
		const char * restart = "Press 'r' to restart or 'q' to quit.";
		move(rows, (cols / 2) - strlen(restart) / 2);
		append(restart);
	} else {
		color_fg(226);
		const char * pressEscapeToQuit = "Press 'q' to quit";
		move(rows, (cols / 2) - strlen(pressEscapeToQuit) / 2);
		append(pressEscapeToQuit);
	}
	
	apply();
}

void on_resize(int sig){
	get_window_size(&rows, &cols);
	draw();
}

void alert(unsigned int ms)
{
	struct itimerval t;
	t.it_interval.tv_sec = 0;
	t.it_interval.tv_usec = 0;
	t.it_value.tv_sec = 0;
	t.it_value.tv_usec = ms * 1000;
	setitimer(ITIMER_REAL, &t, NULL);
}

void input(int fd) {
	int nread;
	char c;
	while ((nread = read(fd,&c,1)) == 0);
	if (nread == -1) exit(1);
	switch(c) {
	
		case 'q': 
			quit = 1;
			move(1, 1);
			color_reset();
			clear();
			apply();
			exit(0);
		break;
		
		case 'r':
			init_game();
			game_over = 0;
			alert(1);
		break;
		
		case 'h': dir = (Pos) { .x = -1, .y = 0 };  break;
		case 'j': dir = (Pos) { .x = 0,  .y = 1 };  break;
		case 'k': dir = (Pos) { .x = 0,  .y = -1 }; break;
		case 'l': dir = (Pos) { .x = 1,  .y = 0 };  break;
		
	}
}

void on_alarm(int sig) {
	update();
	draw();
	if(quit || game_over)
		return;
	alert(UPDATE_TIME);
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	init(on_resize);
	init_game();
	get_window_size(&rows, &cols);
	signal(SIGALRM, on_alarm);
	alert(1);
	while(1) {
		input(STDIN_FILENO);
	}
	return 0;
}

