#include "sta.c"
#include <unistd.h>
#include <signal.h>

volatile int num_lines = 1;

void draw() {
	cursor(0);
	apply();
	clear();
	int rows, cols;
	get_window_size(&rows, &cols);
	move(rows/2-1, cols/2 - 1);
	append("sta");
	move(rows/2 + 1, cols/2 - 8);
	append("Press 'q' to quit");
	for(int i=0; i<num_lines; i++) {
		move(rows/2 + 3 + i, cols/2 - 8);
		char buf[32];
	 	snprintf(buf, sizeof(buf),"line %d", i);
	 	append(buf);
	}
	apply();
}

void on_resize(int sig){
	draw();
}

void input(int fd) {
	int nread;
    char c;
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

void handle(int sig) {
    num_lines++;
    alarm(1);
    draw();
}

int main(int argc, char *argv[]) {
	init(on_resize);
	signal(SIGALRM, handle);
	alarm(1);
	while(1) {
		draw();
		input(STDIN_FILENO);
	}
	return 0;
}

