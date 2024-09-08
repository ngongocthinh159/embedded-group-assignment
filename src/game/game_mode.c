#include"../../include/image-renderer/image.h"
#include "../../include/lib/framebf.h"
#include "../../include/image-renderer/game-mode/welcome_screen_easy.h"
#include "../../include/image-renderer/game-mode/welcome_screen_medium.h"
#include "../../include/image-renderer/game-mode/welcome_screen_hard.h"

void hard_mode_screen(){
    drawImage(welcome_screen_hard,0,0,image_width,image_height);
}
void easy_mode_screen(){
    drawImage(welcome_screen_easy,0,0,image_width,image_height);
}
void medium_mode_screen(){
    drawImage(welcome_screen_medium,0,0,image_width,image_height);
}

void navigate_game_mode_screen(){
    
}