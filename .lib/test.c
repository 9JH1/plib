#include "input.h"

box new;

// change fill char and color
void on_click(void *self){
	printf("%s", new._r2);
}

void on_hover(void *self){
	printf("%s", new._r3);
}

int main()
{
	input glob;	
	border b = UI_BORDER("-","|","+");
	b.ansi = "\033[0m";

	// init box 
	new = UI_BOX(VEC(5,5), VEC(10,10), .ansi = "", .border = b, .fill = ' ');
	printf("%s",new._r);

	// set click event 
	new.fill = 'C';
	new.ansi = "\033[32m";
	new._r2 = _box_render(&new);
	in_box_clk_key(new,on_click);

	// set hover event 
	new.fill = 'H';
	new.ansi = "\033[36m";
	new._r3 = _box_render(&new);
	in_box_hov_key(new, on_hover);


	// start input loop
	in_loop()
	{
		in_catch(&glob);
		in_update(&glob);
		UI_BOX_DRAW(new);
		fflush(stdout);
	}
}
