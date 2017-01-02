#include "sta.c"

void lines(int num, int row, int col) {
	for(int i=0; i<num; i++) {
		cursor_move(1+row+i, 1+col);
		char buf[32];
    	snprintf(buf, sizeof(buf),"line %d", i);
		append(buf);
	}
}

void refresh() {
	cursor_show(0);
	clear();
	int rows, cols;
	window_size(&rows, &cols);
	cursor_move(rows/2, cols/2);
	append("HELLO");
	lines(3, (rows/2) + 1, (cols/2) - 1);
	apply();
	cursor_show(1);
}

void on_resize(int sig){
	refresh();
}

int main(int argc, char *argv[]) {

	// how to
	// 1. append commands to buffer
	// 2. apply
	// 3. wait for input
	init(on_resize);
	
	refresh();
	
	while(getchar() != '\n') {}
	
	return 0;
}

