#include "sta.c"

void lines(int num, int row, int col) {
	for(int i=0; i<num; i++) {
		move(1+row+i, 1+col);
		char buf[32];
    	snprintf(buf, sizeof(buf),"line %d", i);
		append(buf);
	}
}

void draw() {
	cursor(0);
	clear();
	int rows, cols;
	get_window_size(&rows, &cols);
	move(rows/2, cols/2 - 1);
	append("STA");
	move(rows/2 + 2, cols/2 - 8);
	append("Press 'q' to quit");
	//lines(3, (rows/2) + 4, (cols/2) - 3);
	apply();
}

void on_resize(int sig){
	draw();
}

void process_input(int fd) {
	int nread;
    char c, seq[3];
    while ((nread = read(fd,&c,1)) == 0);
    if (nread == -1) exit(1);
    
    switch(c) {
    	case 'q': 
    	move(0, 0);
    	clear();
    	apply();
    	exit(0);
    	break;
    }
}

int main(int argc, char *argv[]) {

	// how to
	// 1. append commands to buffer
	// 2. apply
	// 3. wait for input
	init(on_resize);
	while(1) {
		draw();
		process_input(STDIN_FILENO);
	}
	return 0;
}

