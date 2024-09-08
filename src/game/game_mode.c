#include"../../include/image-renderer/image.h"
#include "../../include/lib/framebf.h"
#include "../../include/image-renderer/game-mode/welcome_screen_easy.h"
#include "../../include/image-renderer/game-mode/welcome_screen_medium.h"
#include "../../include/image-renderer/game-mode/welcome_screen_hard.h"

unsigned long *mode_menu;

void hard_mode_screen(){
    drawImage(welcome_screen_hard,0,0,image_width,image_height);
}
void easy_mode_screen(){
    drawImage(welcome_screen_easy,0,0,image_width,image_height);
}
void medium_mode_screen(){
    drawImage(welcome_screen_medium,0,0,image_width,image_height);
}

void read_from_screen(){
    int array_length = sizeof(mode_menu) / sizeof(unsigned long);
    mode_menu = (unsigned long *)
    
    for(int i =0;i<array_length;i++){
        mode_menu[i] = 
    }
}
