/**
* @file graphics.c
* @author Sophie Handel
* @date 3/12/18
* @brief A graphics library for drawing geometry, for Homework 8 of Georgia Tech
*        CS 2110, Spring 2018.
*/

// Please take a look at the header file below to understand what's required for
// each of the functions.
#include <stddef.h>
#include "graphics.h"
#include "font.h"
#include "text.h"

// TODO: Complete according to the prototype in graphics.h

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;

void setPixel(int x, int y, u16 color) {
	videoBuffer[x + 240 * y] = color;
}

void drawHollowRect(int x, int y, int width, int height, u16 color) {
	for (int i = 0; i < width; i++) {
		setPixel(x + i, y, color);
		setPixel(x + i, y + (height - 1), color);
	}
	for (int j = 0; j < height; j++) {
		setPixel(x, y + j, color);
		setPixel(x + (width - 1), y + j, color);
	}
}

void drawRectangle(int row, int col, int width, int height, volatile unsigned int color) {
    	DMA[3].src = &color;
	for (int r = 0; r < height; r++) {
		DMA[3].dst = videoBuffer + (row + r) * 240 + col;
		DMA[3].cnt = width | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
    }
}


void drawImage3(int row, int col, int width, int height, const u16* image) {
	for (int r = 0; r < height; r++) { //iterate through each row
	 	DMA[3].src = image + r * width; //read from source row starting at first index
		DMA[3].dst = videoBuffer + (row + r) * 240 + col; //copy into destination videoBuffer at row 
		DMA[3].cnt = (240 < width ? 240 : width) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT; //increment each time you reach end of image row (width)
	}
}

void waitForVblank() {
	while(*SCANLINECOUNTER > 160)
		;
	while(*SCANLINECOUNTER < 160)
		;
}

