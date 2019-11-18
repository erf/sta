#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "sta.c"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX_ROWS 9
#define MAX_COLS 24
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
int paused = 0;

int r(int min, int max) {
	double one = (double) rand() / (double) RAND_MAX;
	int num = min + (int)(one * ( max - min ));
	return num;
}

Pos r_inside() {
	int x = r(1, MAX_COLS-1);
	int y = r(1, MAX_ROWS-1);
	return (Pos) { .x = x, .y = y };
}

Pos add(Pos p1, Pos p2) {
	return (Pos) { 
		.x = p1.x + p2.x, 
		.y = p1.y + p2.y 
	};
}

int is_zero(Pos p) {
	return p.x == 0 && p.y == 0;
}

int is_same(Pos p1, Pos p2) {
	return (p1.x == p2.x && p1.y == p2.y);
}

Pos create_food() {
	Pos p;
	while(1) {
		int food_collide = 0;
		p = r_inside();
		for(int i=0; i<snake_len; i++) {
			if(is_same(snake[i], p)) {
				food_collide = 1;
				break;
			}
		}
		if(!food_collide)
			break;
	}
	return p;
}

void init_game() {

	// init snake
	snake_len = 1;
	snake[0] = (Pos) { .x = MAX_COLS/2, .y = MAX_ROWS/2 };
	dir = (Pos) { .x = 1, .y = 0 };
	
	// init food
	food = create_food();
	
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
	
	// check if snake hit itself
	for(int i=0; i<snake_len-1; i++) {
		Pos p = snake[i];
		for(int j=i+1; j<snake_len; j++) {
			Pos p1 = snake[j];
			if(p.x == p1.x && p.y == p1.y) {
				game_over = 1;
				return;
			}
		}
	}
	
	// check if food was eaten and create new food
	if(is_same(p, food)) {
	
		// more snake
		++snake_len;
		snake[snake_len-1] = snake[snake_len-2];
		
		// create until food is not on snake
		food = create_food();
	}
	
}

void draw() {

	cursor(0);
	color_bg(0);
	clear();
	
	// draw wall
	color_fg(7);
	color_bg(238);
	for(int col=0; col<MAX_COLS; col++) {
		for(int row=0; row<MAX_ROWS; row++) {
			move(row + 1, col + 1);
			if(row==0 || col==0 || row==MAX_ROWS-1 || col==MAX_COLS-1)
				append("#");
			else 
				append(" ");
		}
	}
	
	// draw food
	color_fg(9);
	move(food.y + 1, food.x + 1);
	append("o");
	
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
	} 
	
	color_fg(226);
	
	const char * instructions [] = {
		"'hjkl' - move",
		"'p'    - pause",
		"'r'    - restart",
		"'q'    - quit"
	};
	
	for(int i=0; i<4; i++) {
		move(MAX_ROWS + 2 + i, 1);
		append(instructions[i]);
	}
	
	apply();
}

void on_resize_handler(int sig){
	window_size(&rows, &cols);
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
		{
			quit = 1;
			cursor(1);
			move(1, 1);
			color_reset();
			clear();
			apply();
			exit(0);
			break;
		}
		
		case 'p':
		{
			if(game_over) 
				return;
			paused = !paused;
			alert(1);
			break;
		}
		
		case 'r':
		{
			init_game();
			game_over = 0;
			alert(1);
			break;
		}
		
		case 'h': 
		{
			Pos d = { .x = -1, .y = 0 };
			if(!is_zero(add(dir, d))) 
				dir = d;
			break;
		}
			
		case 'j': 
		{
			Pos d = { .x = 0,  .y = 1 };  
			if(!is_zero(add(dir, d))) 
				dir = d;
			break;
		}
			
		case 'k': 
		{
			Pos d = { .x = 0,  .y = -1 }; 
			if(!is_zero(add(dir, d))) 
				dir = d;
			break;
		}
		
		case 'l': 
		{
			Pos d = { .x = 1,  .y = 0 };
			if(!is_zero(add(dir, d))) 
				dir = d;
			break;
		}
		
	}
}

void on_alarm(int sig) {
	update();
	draw();
	if(quit || game_over || paused)
		return;
	alert(UPDATE_TIME);
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	enable_raw_mode();
	on_resize(on_resize_handler);
	init_game();
	window_size(&rows, &cols);
	signal(SIGALRM, on_alarm);
	alert(1);
	while(1) {
		input(STDIN_FILENO);
	}
	return 0;
}

