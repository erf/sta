#include "sta.c"

int rows, cols;

void draw() {

	cursor(0);
	color_bg(0);
	clear();
	
	int colors = 0;
	for(int row=0; row<rows; row++)
	for(int col=0; col<cols; col++) {
		move(row + 1, col + 1);
		color_bg(colors);
		append(" ");
		++colors;
		if(colors >= 256) {
			break;
		}
	}
	
	apply();
}

void on_resize_handler(int sig){
	window_size(&rows, &cols);
	draw();
}

void input(int fd) {
	int nread;
	char c;
	while ((nread = read(fd,&c,1)) == 0);
	if (nread == -1) exit(1);
	switch(c) {
	
		case 'q': 
		{
			cursor(1);
			move(1, 1);
			color_reset();
			clear();
			apply();
			exit(0);
			break;
		}
	}
}

int main(int argc, char *argv[]) {
    enable_raw_mode();
	on_resize(on_resize_handler);
	window_size(&rows, &cols);
	draw();
	while(1) {
		input(STDIN_FILENO);
	}
	return 0;
}

