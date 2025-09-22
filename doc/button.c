#include <stdio.h> 
#include <string.h>
#include "../lib/gc.h"
#include "../lib/input.h" 
#include "../lib/ansi.h"

typedef struct {
	int x;
	int y;
	char *text;
	char *hover;
	char *ansi;
	char *ansihover;
} button;

button *buttons;
int button_cap = 0;
int button_idx;

// basic DMA for buttons
int create_button(const button opt){
	if(button_cap == 0){ 
		// initialize button list 
		button_cap = 2;
		button_idx = 0;
		buttons = malloc(button_cap * sizeof(button));
		if(!buttons){
			printf("error occured in init\n");
			return -1;
		}
	}

	if(button_cap == button_idx){
		button_cap *= 2;
		button *temp = realloc(buttons,button_cap * sizeof(button));
		if(!temp){
			printf("error in realloc\n");
			return -2;
		}
	}

	buttons[button_idx] = opt;
	button_idx++;
	return 0;
}

// renders butons from static opt
int render_buttons(InputEvent opt){
	for(int i = 0; i < button_idx; i++){
		button local = buttons[i];
		gotoxy(local.x,local.y);
		
		if(
				opt.data.mouse.x >= local.x &&
				opt.data.mouse.y >= local.y && 
				opt.data.mouse.x <= (local.x + strlen(local.text)-1) && 
				opt.data.mouse.y <= local.y) {
			if(opt.type == MOUSE_MOVE) printf("%s",local.hover);
			else if(opt.type == MOUSE_CLICK) return 0;
		} else printf("%s",local.text);
	}

	return 1;
}

int render_dropdown(InputEvent opt, const int argc, char **argv){
	for(int i = 0;i < argc; i++){
		create_button((button){
				.x = 1,
				.y = i,
				.text = argv[i],
				.hover = "TEST",
				});
	}

	render_buttons(opt);
	return 0;
}

int main(int argc, char *argv[]){
	while(1){
		InputEvent event;
		input(&event);
		render_dropdown(event,argc,argv);
	}
}
