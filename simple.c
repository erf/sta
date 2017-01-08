#include <unistd.h>
#include <signal.h>

#include "sta.c"

int rows, cols;

void draw() {
	cursor(0);
	apply();
	clear();
	get_window_size(&rows, &cols);
	move(rows/2-1, cols/2 - 1);
	append("sta");
	move(rows/2 + 1, cols/2 - 8);
	append("Press 'q' to quit");
	char buf[32];
	snprintf(buf, sizeof(buf), "rows %d cols %d", rows, cols);
	move(rows + 1, cols - strlen(buf));
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
    	move(0, 0);
    	clear();
    	apply();
    	exit(0);
    	break;
    }
}

int main(int argc, char *argv[]) {
	init(on_resize);
	signal(SIGALRM, handle);
	while(1) {
		draw();
		input(STDIN_FILENO);
	}
	return 0;
}

