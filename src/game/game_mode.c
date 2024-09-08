#include"../../include/image-renderer/image.h"
#include "../../include/lib/framebf.h"
#include "../../include/game/screen/game-mode/welcome_screen_easy.h"
#include "../../include/game/screen/game-mode/welcome_screen_medium.h"
#include "../../include/game/screen/game-mode/welcome_screen_hard.h"
#include "../../include/game/screen/game-mode/background.h"
//unsigned long *mode_menu;
char easy_mode []={"CURRENT DIFFICULTY: EASY"};
char medium_mode []={"CURRENT DIFFICULTY: MEDIUM"};
char hard_mode []={"CURRENT DIFFICULTY: HARD"};
//char *title_ptr = &title;
void hard_mode_screen(){
    drawImage(welcome_screen_hard,0,0,image_width,image_height);
}
void easy_mode_screen(){
    drawImage(welcome_screen_easy,0,0,image_width,image_height);
}
void medium_mode_screen(){
    drawImage(welcome_screen_medium,0,0,image_width,image_height);
}

void draw_new_game_highlight(){
    drawLineWithThicknessARGB32(430,355,430,405,0xFFFFFF,1);//left line of new game, added 30 pixel to x coors compare to canva soruce
    drawLineWithThicknessARGB32(610,355,610,405,0xFFFFFF,1);//right line of new game
}
void draw_difficulty_highlight(){
    drawString(265,440,easy_mode,0xFFFFFF,1);
    drawLineWithThicknessARGB32(779,430,779,480,0xFFFFFF,1);// right line of difficulty
    drawLineWithThicknessARGB32(254,430,254,480,0xFFFFFF,1);//left line of difficulty
}
void draw_exit_highlight(){
    drawLineWithThicknessARGB32(430,510,430,560,0xFFFFFF,1);//left line of new game, added 30 pixel to x coors compare to canva soruce
    drawLineWithThicknessARGB32(610,510,610,560,0xFFFFFF,1);//right line of new game
}
void draw_general_menu(){
    drawString(380,240,"TETRIS",0xff4D3D,3);
    drawString(450,520,"EXIT GAME",0xFFFFFF,1);
    drawString(450,365,"NEW GAME",0xFFFFFF,1);
    
    draw_difficulty_highlight();
    
    //drawRectARGB32(212,33,801,734,0x0000FF,0);
    //drawImage(background,212,34,600,700);
    drawRectARGB32(240,184,785,584,0x0000FF,0); // draw outer rectangle

}
void draw_nav_exit(){
    draw_general_menu();
    draw_exit_highlight();
}
void draw_choose_new_game(){
    draw_general_menu();
    draw_new_game_highlight();
}
void draw_nav_diffuclty(){
    draw_general_menu();
    draw_exit_highlight();
}
void displayGamePlayScreen();
/*
void read_from_screen(){
    int array_length = sizeof(mode_menu) / sizeof(unsigned long);
    mode_menu = (unsigned long *)
    
    for(int i =0;i<array_length;i++){
        mode_menu[i] = 
    }
}*/
