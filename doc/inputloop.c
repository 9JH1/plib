#include "../lib/input.h"
#include "../lib/ansi.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

void quit(int code){
	disable_raw_mode();
	disable_mouse_reporting();
	clear();
	printf("\rexiting...\n");
	exit(code);
}
const char *exit_message = "Press this button or use 'q' to exit";

int dragging = -1;
int drag_x = 0;
int drag_y = 0;

int main(){
	signal(SIGINT,quit);

	clear();
	while(1){
		// capture event
		InputEvent loc; 
		gotoxy(1,1);
		int ret = input(&loc);
		//clear();
		
		// handle button hover
		if(
				loc.type == MOUSE_MOVE && // see if event was a mouse movement
				loc.data.mouse.y == 1 &&  // check if the current y pos of the mouse is on the message 
				loc.data.mouse.x <= strlen(exit_message)){ // check if the current x pos is on the message
			printf("\033[31;42;1m%s\033[0m\n",exit_message);
		} else printf("%s\n",exit_message);
		
		// print the event values
		log_InputEvent(loc);

		if(loc.type == MOUSE_CLICK && dragging == 0){
			dragging = 1;
			drag_x = loc.data.mouse.x;
			drag_y = loc.data.mouse.y;
		} else if (loc.type == MOUSE_CLICK && dragging == 1){
			printf("Currently dragging from (%d,%d)\n",drag_x,drag_y);
			gotoxy(loc.data.mouse.x,loc.data.mouse.y);
			printf("D");

		} else if (loc.type == MOUSE_RELEASE){
			dragging = 0;
			printf("Mouse dragged from (%d,%d) to (%d,%d)\n",drag_x,drag_y,loc.data.mouse.x,loc.data.mouse.y);
		}

		fflush(stdout);
		
		// handle exits
		if(loc.type == MOUSE_CLICK && loc.data.mouse.y == 1){
			break;
		}
	
		if(loc.type == KEY && loc.data.key.code == 'q'){
			break;
		}

	}
	quit(0);
}
