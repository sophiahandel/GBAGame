#include "graphics.h"
#include <stdlib.h>
#include <stdio.h>
#include "titlescreen.h"
#include "gameover.h"
#include "jacket_image.h"
#include "dawg_image.h"
#include "garbage.h"
#include "font.h"
#include "text.h"

struct jacket {
	volatile u16 jacket_x;
	volatile u16 jacket_y;
	int score;
	int jacket_speed;
};

struct dawg {
	volatile u16 dawg_x;
	volatile u16 dawg_y;
	int dawg_speed;
};

struct badG {
    volatile u16 badG_x;
    volatile u16 badG_y;
    int badG_horiz;
    int badG_vert;
};

u16 magenta = MAGENTA;
u16 white = WHITE;
int ended = FALSE;
int score = 0;
char str[12];

int main() {
    //setting up 4 falling dawgs
    struct dawg dawgs[4]; // create an array of 4 dawgs
    for (int i = 0; i < 4; i++) {
        dawgs[i].dawg_x = (70*i); // each dog is 70 px from the previous in the x direction
        dawgs[i].dawg_y = 0;      // initialized at y pos 0
        dawgs[i].dawg_speed = 1;
    }
    //setting up the moving around bad dawg
    struct badG bad;
	bad.badG_x = 0; // x pos of G
	bad.badG_y = 30; // y pos of G
	bad.badG_horiz = 2; // horizontal speed, faster than vertical so it moves diagonally
	bad.badG_vert = 1; // vertical speed
	
	//setting up your player
    struct jacket jacket;
    jacket.jacket_x = 120;  // starts in the middle of the screen in x direction
    jacket.jacket_y = 160 - JACKET_IMAGE_HEIGHT; // bottom of screen in y direction
	jacket.jacket_speed = 2;
	
	REG_DISPCNT = MODE3 | BG2_ENABLE;
	if (!ended) {
        drawImage3(0, 0, 240, 160, titlescreen); // if you are starting the game for the first time, draw title screen, skip from gameOver
    }
	while(TRUE) {
	    if (KEY_DOWN_NOW(BUTTON_START)) {
			break; // when start button is pressed, enter game loop
		}
	}
	for (int i = 0; i < 240; i++) {
	    for (int j = 0; j < 160; j++) {
	        setPixel(i, j, BLACK); // set black background
	    } 
	}
	sprintf(str, "%d", 0); 
	drawString(0, 170, "score:", WHITE);
	drawString(1, 210, str, WHITE); //draw initial score of 0
	while(TRUE) {
	    //return to home screen at any time if select is pressed
	    if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			main();
		}
		if (KEY_DOWN_NOW(BUTTON_LEFT) && (jacket.jacket_x >= (JACKET_IMAGE_HEIGHT))) {
			jacket.jacket_x -= jacket.jacket_speed; // decrease jacket x pos by speed
		}
		if (KEY_DOWN_NOW(BUTTON_RIGHT) && (jacket.jacket_x <= (240 - JACKET_IMAGE_WIDTH))) {
			jacket.jacket_x += jacket.jacket_speed; // increase jacket x pos by speed
		}
		if (KEY_DOWN_NOW(BUTTON_UP) && (jacket.jacket_y >= (JACKET_IMAGE_HEIGHT))) {
			jacket.jacket_y -= jacket.jacket_speed; // increase jacket y pos by speed
		}
		if (KEY_DOWN_NOW(BUTTON_DOWN) && (jacket.jacket_y <= (160 - JACKET_IMAGE_HEIGHT))) {
			jacket.jacket_y += jacket.jacket_speed; // decrease jacket y pos by speed
		}
		waitForVblank(); //after calculations, wait for previous screen to finish drawing before drawing updates to prevent tearing
		drawImage3(jacket.jacket_y, jacket.jacket_x, JACKET_IMAGE_WIDTH, JACKET_IMAGE_HEIGHT, jacket_image); 
		drawHollowRect(jacket.jacket_x - 2, jacket.jacket_y - 2, JACKET_IMAGE_WIDTH + 1, JACKET_IMAGE_HEIGHT + 1, BLACK); // cover previous image
		drawHollowRect(jacket.jacket_x + 1, jacket.jacket_y + 1, JACKET_IMAGE_WIDTH + 1, JACKET_IMAGE_HEIGHT + 1, BLACK); 
	    sprintf(str, "%d", score);
		drawString(1, 210, str, BLACK);
	    score++;
		sprintf(str, "%d", score);
		drawString(1, 210, str, WHITE);
	    //draw falling dawgs
		for (int i = 0; i < 4; i++) {
			dawgs[i].dawg_y += dawgs[i].dawg_speed;
			drawImage3(dawgs[i].dawg_y, dawgs[i].dawg_x, DAWG_IMAGE_WIDTH, DAWG_IMAGE_HEIGHT, dawg_image);
            drawHollowRect(dawgs[i].dawg_x - 1, dawgs[i].dawg_y - 1, DAWG_IMAGE_WIDTH + 1, DAWG_IMAGE_HEIGHT + 1, BLACK); //cover previous image
			//check for collisions between jacket and falling dawgs
			if (collision(dawgs[i].dawg_x, dawgs[i].dawg_y, DAWG_IMAGE_WIDTH, DAWG_IMAGE_HEIGHT,jacket.jacket_x, jacket.jacket_y, JACKET_IMAGE_WIDTH, JACKET_IMAGE_HEIGHT)) {
			    //if there is a collision, end the game
				gameOver();
			}
			//when dawgs reach bottom of screen, reset to top
			if (dawgs[i].dawg_y >= 160) {
				dawgs[i].dawg_y = 0;
			}		
		}
		//draw moving big G
		drawImage3(bad.badG_y, bad.badG_x, GARBAGE_WIDTH, GARBAGE_HEIGHT, garbage);
		drawHollowRect(bad.badG_x - 1, bad.badG_y - 1, GARBAGE_WIDTH + 1, GARBAGE_HEIGHT + 1, BLACK); //cover previous
		drawHollowRect(bad.badG_x - 2, bad.badG_y - 2, GARBAGE_WIDTH + 1, GARBAGE_HEIGHT + 1, BLACK); //cover previous
		//check for collisions between jacket and big G
		if(collision(bad.badG_x, bad.badG_y, GARBAGE_WIDTH, GARBAGE_HEIGHT,jacket.jacket_x, jacket.jacket_y, JACKET_IMAGE_WIDTH, JACKET_IMAGE_HEIGHT)) {
		    gameOver();
		}
		bad.badG_x += bad.badG_horiz; // move big G forward in x by its horizontal speed
		bad.badG_y += bad.badG_vert;  // move big G forward in y by its vertical speed
		// if big g reaches end of the screen, reset to front
		if (bad.badG_x >= 240) {
			bad.badG_x = 0;
		}
		// if big G reaches bottom of the screen, geneerate new random position
		if (bad.badG_y >= 165) {
			bad.badG_y = jacket.jacket_y - 40; //prevents big G from generating on top of you, always 40 away in y direction (unfair!!)
			bad.badG_x = rand() % 200 + 5; // big G generates at random x
		}
    }
}


//collision function
int collision(u16 dawg_x, u16 dawg_y, u16 dawg_width, u16 dawg_height, u16 jacket_x, u16 jacket_y, u16 jacket_width, u16 jacket_height) {
	//check if bottom right corner of dawg/G is within jacket
	if (((dawg_y + dawg_width) > jacket_y) && ((dawg_x + dawg_height) 
		> jacket_x ) &&  ((dawg_y + dawg_width) < (jacket_y + jacket_width)) 
		&& ((dawg_x + dawg_width) < (jacket_x + jacket_height))) {
		return TRUE;
	} 
	//check bottom left corner of dawg/G
	if ( (dawg_y < (jacket_y + jacket_width)) 
		&& (dawg_y > jacket_y)
		&& ((dawg_x + dawg_height) > jacket_x)
		&& ((dawg_x + dawg_height) < (jacket_x + jacket_height))
		) {
		return TRUE;
	}
	//check top left corner
	if ( (dawg_y < (jacket_y + jacket_width)) 
		&& (dawg_y > jacket_y)
		&& (dawg_x > jacket_x)
		&& (dawg_x < (jacket_x + jacket_height))
		) {
		return TRUE;
	}	
	//check top right corner
	if ( ((dawg_y + dawg_width) < (jacket_y + jacket_width)) 
		&& ((dawg_y + dawg_width) > jacket_y)
		&& (dawg_x > jacket_x)
		&& (dawg_x < (jacket_x + jacket_height))
		) {
		return TRUE;
	}	
	return FALSE;
}


        
void gameOver() {
	//start game over (failed)
	drawImage3(0, 0, 240, 160, gameover);
	sprintf(str, "%d", score);
	drawString(0, 170, "score:", WHITE);
	drawString(1, 210, str, WHITE); // displat final score
	score = 0;
	while(1) {
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
		    ended = FALSE; // set ended to false so title screen is loaded
			main();
		}
		if (KEY_DOWN_NOW(BUTTON_START))	{
		    ended = TRUE; // title screen should not be reloaded, so ended is set to true
			main();
		}
    }
}

